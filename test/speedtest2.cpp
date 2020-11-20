#include <algorithm>
#include <ctime>
#include <iostream>
#include <iterator>
#include <sstream>
#include <vector>
#include <string>
#include <cstdio>

#include <LiDIA/bigint.h>
#include <LiDIA/multi_bigmod.h>

#include <bilinear/Scalar_DCLXVI.hpp>
#include <bilinear/G1_DCLXVI.hpp>
#include <bilinear/G2_DCLXVI.hpp>
#include <bilinear/GT_DCLXVI.hpp>

#include <utils/LibConversions.hpp>
#include <utils/Profiler.hpp>
#include <utils/Pointers.hpp>
#include <utils/ThreadPool.hpp>

#include <algorithms/BilinearMapAccumulator.hpp>
#include <algorithms/RSAAccumulator.hpp>
#include <algorithms/RSAKey.hpp>

#include <utils/testutils.hpp>

using testutils::print_hex;
using namespace std;

namespace speedtest {
//Forward declarations
void bilinearTest(int setSize);
void rsaTest(int setSize);
void bilinearPublicPrivateTest();
void saveScalars(int setSize, string filename);
void saveBigints(int setSize, string filename);
}

int main(int argc, char **argv) {
    const int SET_SIZE = 1000;

//    speedtest::saveElements(SET_SIZE, "randomSet" + to_string(SET_SIZE));
//    speedtest::conversionTest();
//    speedtest::bilinearPublicPrivateTest();
    speedtest::bilinearTest(SET_SIZE);
//    speedtest::saveBigints(SET_SIZE, "randomBigints" + to_string(SET_SIZE));
//    speedtest::rsaTest(SET_SIZE);

    return 0;
}

namespace speedtest {

void saveScalars(int setSize, string filename) {
    ofstream fileOut(filename, ios::binary);
    for(int c = 0; c < setSize; c++) {
        ScalarDCLXVI element;
        element.generateRandom();
        element.writeToFile(fileOut);
    }
    fileOut.close();
}

vector<unique_ptr<Scalar>> readScalars(string filename) {
    ifstream fileIn(filename, ios::binary);
    vector<unique_ptr<Scalar>> elements;
    while(fileIn.peek() != EOF) {
        unique_ptr<Scalar> element = unique_new<ScalarDCLXVI>();
        element->readFromFile(fileIn);
        elements.push_back(move(element));
    }
    return elements; //this looks like it will copy the whole vector, but the C++11 standard requires an implicit move constructor here (I think)
}

void saveBigints(int setSize, string filename) {
    ofstream fileOut(filename);
    LiDIA::bigint bound256bits;
    LiDIA::string_to_bigint("115792089237316195423570985008687907853269984665640564039457584007913129639936", bound256bits);
    for(int c = 0; c < setSize; c++) {
        LiDIA::bigint element;
        element.randomize(bound256bits);
        fileOut << element << endl;
    }
}

vector<LiDIA::bigint> readBigInts(string filename) {
    ifstream fileIn(filename);
    vector<LiDIA::bigint> elements;
    string fileLine;
    while(getline(fileIn, fileLine)) {
        LiDIA::bigint element;
        LiDIA::string_to_bigint(fileLine.c_str(), element);
        elements.push_back(move(element));
    }
    return elements;
}

void bilinearPublicPrivateTest() {
    //Initialize LiDIA's global modulus
    LiDIA::bigint modulus;
    LibConversions::getModulus(modulus);
    LiDIA::bigmod::set_modulus(modulus);

    //Accumulating this "set" will accumulate just the secret key s once
    vector<reference_wrapper<Scalar>> setZero;
    LiDIA::bigmod lidiaZero;
    ScalarDCLXVI scalarZero(lidiaZero);
    setZero.push_back(scalarZero);

    //Accumulating this "set" will accumulate just s^2
    vector<reference_wrapper<Scalar>> setTwoZeros;
    ScalarDCLXVI s1(lidiaZero);
    ScalarDCLXVI s2(lidiaZero);
    setTwoZeros.push_back(s1);
    setTwoZeros.push_back(s2);

    //Accumulating this "set" will accumulate s+1, the simplest polynomial
    vector<reference_wrapper<Scalar>> setOne;
    LiDIA::bigmod lidiaOne(1);
    ScalarDCLXVI scalarOne(lidiaOne);
    setOne.push_back(scalarOne);

    //Use the same key as the 100-element test I was doing earlier, just in case
    BilinearMapKey key;
    key.readPkFromFile("testPublicKey");
    key.readSkFromFile("testPrivateKey");
    //Sanity checks on public key
    G1DCLXVI g1Generator;
    G1DCLXVI gToS;
    G1DCLXVI gToS2;
    g1Generator.doPower(key.getSecretKey(), gToS);
    gToS.doPower(key.getSecretKey(), gToS2);
    if(!key.getPublicKey().first.at(0)->isEqual(g1Generator)) {
        cout << "Error! Public key element 0 is not the generator for G1!" << endl;
        cout << "PublicKey.first.at(0): ";
        print_hex(key.getPublicKey().first.at(0)->getByteBuffer(), key.getPublicKey().first.at(0)->getSize());
        cout << "G1 generator: ";
        print_hex(g1Generator.getByteBuffer(), g1Generator.getSize());
    }
    if(!key.getPublicKey().first.at(1)->isEqual(gToS)) {
        cout << "Error! Public key element 1 is not g^s !" << endl;
        cout << "PublicKey.first.at(1):";
        print_hex(key.getPublicKey().first.at(1)->getByteBuffer(), key.getPublicKey().first.at(1)->getSize());
        cout << "g^s: ";
        print_hex(gToS.getByteBuffer(), gToS.getSize());
    }
    if(!key.getPublicKey().first.at(2)->isEqual(gToS2)) {
        cout << "Error! Public key element 2 is not g^s^2 !" << endl;
        cout << "PublicKey.first.at(2):";
        print_hex(key.getPublicKey().first.at(2)->getByteBuffer(), key.getPublicKey().first.at(2)->getSize());
        cout << "g^s^s: ";
        print_hex(gToS2.getByteBuffer(), gToS2.getSize());
    }

    //Let's keep this test single-threaded
    ThreadPool threadPool;
    //Accumulate the {0} set, which should produce g^s
    G1DCLXVI privateAccS;
    BilinearMapAccumulator::accumulateSet(setZero, key.getSecretKey(), privateAccS);
    G1DCLXVI publicAccS;
    BilinearMapAccumulator::accumulateSet(setZero, key.getPublicKey(), publicAccS, threadPool);

    G1DCLXVI gToThe0;
    ScalarDCLXVI scalar0(lidiaZero);
    g1Generator.doPower(scalar0, gToThe0);
    G1DCLXVI privateSTimesG0;
    privateAccS.doMultiplication(gToThe0, privateSTimesG0);
    G1DCLXVI publicSTimesG0;
    publicAccS.doMultiplication(gToThe0, publicSTimesG0);

    if(privateAccS.isEqual(gToS)) {
        cout << "Private accumulator of just 's' equals g^s. " << endl;
    } else {
        cout << "Private accumulator of just 's' was not g^s" << endl;
    }
    if(publicAccS.isEqual(gToS)) {
        cout << "Public accumulator of just 's' equals g^s. " << endl;
    } else {
        cout << "Public accumulator of just 's' was not g^s" << endl;
    }
    if(privateAccS.isEqual(publicAccS)) {
        cout << "Private and public accumulations of just 's' match." << endl;
    } else {
        cout << "Private and public accumulations of just 's' do not match! " << endl;
    }
    if(privateSTimesG0.isEqual(privateAccS)) {
        cout << "private * g^0 = private" << endl;
    } else {
        cout << "private * g^0 != private" << endl;
    }
    if(publicSTimesG0.isEqual(publicAccS)) {
        cout << "public * g^0 = public" << endl;
    } else {
        cout << "public * g^0 != public" << endl;
    }
    if(privateSTimesG0.isEqual(publicSTimesG0)) {
        cout << "private * g^0 = public * g^0" << endl;
    } else {
        cout << "private * g^0 != public * g^0" << endl;
    }
    cout << "Private: ";
    print_hex(privateAccS.getByteBuffer(), privateAccS.getSize());
    cout << "Public: ";
    print_hex(publicAccS.getByteBuffer(), publicAccS.getSize());
    cout << "g^s: ";
    print_hex(gToS.getByteBuffer(), gToS.getSize());

    G1DCLXVI privateAccS2;
    BilinearMapAccumulator::accumulateSet(setTwoZeros, key.getSecretKey(), privateAccS2);
    G1DCLXVI publicAccS2;
    BilinearMapAccumulator::accumulateSet(setTwoZeros, key.getPublicKey(), publicAccS2, threadPool);
    if(privateAccS2.isEqual(gToS2)) {
        cout << "Private accumulator of 's^2' equals g^s^2." << endl;
    } else {
        cout << "Private accumulator of 's^2' was not g^s^2 !" << endl;
    }
    if(publicAccS2.isEqual(gToS2)) {
        cout << "Public accumulator of 's^2' was g^s^2." << endl;
    } else {
        cout << "Public accumulator of 's^2' was not g^s^2 !" << endl;
    }
    if(privateAccS2.isEqual(publicAccS2)) {
        cout << "Private and public accumulations of 's^2' match." << endl;
    } else {
        cout << "Private and public accumulations of 's^2' do not match! " << endl;
    }
    cout << "Private: ";
    print_hex(privateAccS2.getByteBuffer(), privateAccS2.getSize());
    cout << "Public: ";
    print_hex(publicAccS2.getByteBuffer(), publicAccS2.getSize());
    cout << "g^s^2: ";
    print_hex(gToS2.getByteBuffer(), gToS2.getSize());

    G1DCLXVI privateAccSp1;
    BilinearMapAccumulator::accumulateSet(setOne, key.getSecretKey(), privateAccSp1);
    G1DCLXVI publicAccSp1;
    BilinearMapAccumulator::accumulateSet(setOne, key.getPublicKey(), publicAccSp1, threadPool);
    G1DCLXVI gToSp1;
    gToSp1.doPower(key.getSecretKey(), gToSp1);
    gToSp1.doMultiplication(g1Generator, gToSp1);
    if(privateAccSp1.isEqual(gToSp1)) {
        cout << "Private accumulator of 's+1' equals g^(s+1)." << endl;
    } else {
        cout << "Private accumulator of 's+1' was not g^(s+1)." << endl;
    }
    if(privateAccSp1.isEqual(publicAccSp1)) {
        cout << "Private and public accumulations of 's+1' match." << endl;
    } else {
        cout << "Private and public accumulations of 's+1' do not match! " << endl;
    }
    cout << "Private: ";
    print_hex(privateAccSp1.getByteBuffer(), privateAccSp1.getSize());
    cout << "Public: ";
    print_hex(publicAccSp1.getByteBuffer(), publicAccSp1.getSize());
    cout << "g^(s+1): ";
    print_hex(gToSp1.getByteBuffer(), gToSp1.getSize());

}

void bilinearTest(int setSize) {
    cout << "Bilinear-Map Accumulator test:" << endl;
    //Initialize LiDIA's global modulus
    LiDIA::bigint modulus;
    LibConversions::getModulus(modulus);
    LiDIA::bigmod::set_modulus(modulus);
    //Initialize the thread pool for multithreading
    const int THREAD_POOL_SIZE = 16;
    ThreadPool threadPool(THREAD_POOL_SIZE);

    vector<unique_ptr<Scalar>> set; //Holds the actual set
    vector<reference_wrapper<Scalar>> setView; //A "view" of the set to pass to functions that observe it
    //Generate random numbers to accumulate
    for(int c = 0; c < setSize; c++) {
        unique_ptr<Scalar> element = unique_new<ScalarDCLXVI>();
        element->generateRandom();
        set.push_back(move(element));
        setView.push_back(*(set.back()));
    }
    cout << "Generated " << set.size() << " random elements." << endl;
//    set = readScalars("randomSet" + to_string(setSize));

    //Generate public and private keys
    G1DCLXVI acc;
    BilinearMapKey key;
    double keyStart = Profiler::getCurrentTime();
    BilinearMapAccumulator::genKey(vector<vector<reference_wrapper<Scalar>>>(), setSize, key, threadPool);
    double keyEnd = Profiler::getCurrentTime();
    cout << "Key generation took " << (keyEnd-keyStart) << " seconds" << endl;

    //Accumulate all the values
    double accStart = Profiler::getCurrentTime();
    BilinearMapAccumulator::accumulateSet(setView, key.getSecretKey(), acc);
    double accEnd = Profiler::getCurrentTime();

    cout << "Accumulated " << set.size() << " elements in " << (accEnd-accStart) << " seconds with private key" << endl;

    //Accumulate the same values using only the public information
    G1DCLXVI accPub;
    double accPubStart = Profiler::getCurrentTime();
    BilinearMapAccumulator::accumulateSet(setView, key.getPublicKey(), accPub, threadPool);
    double accPubEnd = Profiler::getCurrentTime();

    cout << "Accumulated " << set.size() << " elements in " << (accPubEnd-accPubStart) << " seconds with public key" << endl;

    if(!acc.isEqual(accPub)) {
        cout << "Error! Private and public accumulations do not match!" << endl;
        cout << "Private: ";
        print_hex(acc.getByteBuffer(), acc.getSize());
        cout << "Public: ";
        print_hex(accPub.getByteBuffer(), accPub.getSize());
    }

    //Generate witnesses for all the values
    G2DCLXVI witnessBase;
    vector<unique_ptr<G>> witnesses;
    for(size_t c = 0; c < set.size(); c++) {
        witnesses.emplace_back(new G2DCLXVI());
    }
    double witStart = Profiler::getCurrentTime();
    BilinearMapAccumulator::witnessesForSet(setView, key.getSecretKey(), witnessBase, witnesses, threadPool);
    double witEnd = Profiler::getCurrentTime();

    cout << "Generated " << witnesses.size() << " witnesses in " << (witEnd-witStart) << " seconds with private key" << endl;

    //Generate witnesses with only the public information
    vector<unique_ptr<G>> witnessesPublic;
    for(size_t c = 0; c < set.size(); c++) {
        witnessesPublic.emplace_back(new G2DCLXVI());
    }
    double witPubStart = Profiler::getCurrentTime();
    BilinearMapAccumulator::witnessesForSet(setView, key.getPublicKey(), witnessesPublic, threadPool);
    double witPubEnd = Profiler::getCurrentTime();

    cout << "Generated " << witnesses.size() << " witnesses in " << (witPubEnd-witPubStart) << " seconds with public key" << endl;


    //Verify the values with the witnesses
    double verifyStart = Profiler::getCurrentTime();
    for(size_t i = 0; i < setView.size(); i++) {
        bool verifyPassed = BilinearMapAccumulator::verify(setView.at(i), *(witnesses.at(i)), acc, key.getPublicKey());
        if(!verifyPassed) {
            cout << "Witness for element " << i << " did not pass!" << endl;
        }
    }
    double verifyEnd = Profiler::getCurrentTime();

    cout << "Verified " << witnesses.size() << " elements in " << (verifyEnd-verifyStart) << " seconds against private accumulator" << endl;

    double verifyPubStart = Profiler::getCurrentTime();
    for(size_t i = 0; i < setView.size(); i++) {
        bool verifyPassed = BilinearMapAccumulator::verify(setView.at(i), *(witnessesPublic.at(i)), accPub, key.getPublicKey());
        if(!verifyPassed) {
            cout << "Witness for element " << i << " did not pass!" << endl;
        }
    }
    double verifyPubEnd = Profiler::getCurrentTime();

    cout << "Verified " << witnessesPublic.size() << " elements in " << (verifyPubEnd-verifyPubStart) << " seconds against public accumulator" << endl;

}

void rsaTest(int setSize) {
    cout << "RSA Accumulator test:" << endl;

    //Generate random elements to accumulate, of size 256 bits so they're the
    //same as the random elements for the bilinear test
    //(I'm pretty sure ScalarDCLXVI's randomize makes numbers up to 256 bits)
    vector<LiDIA::bigint> elements;
    LiDIA::bigint bound256bits;
    LiDIA::string_to_bigint("115792089237316195423570985008687907853269984665640564039457584007913129639936", bound256bits);
    for(int c = 0; c < setSize; c++) {
        LiDIA::bigint element;
        element.randomize(bound256bits);
        elements.push_back(element);
    }
    cout << "Generated " << elements.size() << " random elements." << endl;
//    elements = readBigints("randomBigints" + to_string(setSize));

    //Generate the public/private key
    double rsaKeyStart = Profiler::getCurrentTime();
    RSAKey rsaKey;
    RSAAccumulator::genKey(0, 3072, rsaKey);
    double rsaKeyEnd = Profiler::getCurrentTime();
    cout << "Key generation took " << (rsaKeyEnd-rsaKeyStart) << " seconds" << endl;

    //Generate representatives for the elements
    vector<LiDIA::bigint> representatives(setSize);
    double repGenStart = Profiler::getCurrentTime();
    RSAAccumulator::genRepresentatives(elements, *(rsaKey.getPublicKey().primeRepGenerator), representatives);
    double repGenEnd = Profiler::getCurrentTime();
    cout << "Generated " << elements.size() << " prime representatives in " << (repGenEnd-repGenStart) << " seconds" << endl;

    //Accumulate the representatives
    LiDIA::multi_bigmod accumulator;
    double accStart = Profiler::getCurrentTime();
    RSAAccumulator::accumulateSet(representatives, rsaKey, accumulator);
    double accEnd = Profiler::getCurrentTime();
    cout << "Accumulated " << representatives.size() << " prime representatives in " << (accEnd-accStart) << " seconds with private key" << endl;

    //Accumulate again with only the public information
    LiDIA::multi_bigmod pubAcc;
    double pubAccStart = Profiler::getCurrentTime();
    RSAAccumulator::accumulateSetPublic(representatives, rsaKey.getPublicKey(), pubAcc);
    double pubAccEnd = Profiler::getCurrentTime();
    cout << "Accumulated " << representatives.size() << " prime representatives in " << (pubAccEnd-pubAccStart) << " seconds with public key" << endl;

    if(accumulator != pubAcc) {
        cout << "Error! Public and private accumulation do not match!" << endl;
        cout << "Private: " << accumulator << endl;
        cout << "Public: " << pubAcc << endl;
    }

    //Generate witnesses for all of the representatives
    vector<LiDIA::multi_bigmod> witnesses(setSize);
    double witStart = Profiler::getCurrentTime();
    RSAAccumulator::witnessesForSet(representatives, rsaKey, witnesses);
    double witEnd = Profiler::getCurrentTime();
    cout << "Generated " << witnesses.size() << " witnesses in " << (witEnd-witStart) << " seconds" << endl;

    //Verify the values with the witnesses
    double verifyStart = Profiler::getCurrentTime();
    for(size_t i = 0; i < elements.size(); i++) {
        bool verifyPassed = RSAAccumulator::verify(elements[i], witnesses[i], accumulator, rsaKey.getPublicKey());
        if(!verifyPassed) {
            cout << "Witness for element " << i << " did not pass!" << endl;
        }
    }
    double verifyEnd = Profiler::getCurrentTime();
    cout << "Verified " << elements.size() << " elements in " << (verifyEnd-verifyStart) << " seconds" << endl;
}

}

