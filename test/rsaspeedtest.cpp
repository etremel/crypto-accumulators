/*
 * rsaspeedtest.cpp
 *
 *  Created on: Apr 20, 2013
 *      Author: etremel
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <memory>

#include <algorithms/RSAAccumulator.h>
#include <algorithms/RSAKey.h>

#include <utils/Profiler.h>
#include <utils/Pointers.h>
#include <utils/ThreadPool.h>

#include <flint/BigInt.h>
#include <flint/BigMod.h>
#include <flint/Random.h>

using namespace std;

namespace speedtest {
void rsaTest(int setSize);
vector<flint::BigInt> readBigInts(string filename);
}

int main(int argc, char** argv) {
    int setSize;
    if(argc > 1) {
        setSize = atoi(argv[1]);
    } else {
        setSize = 1000;
    }
    speedtest::rsaTest(setSize);
    return 0;
}

namespace speedtest {


vector<flint::BigInt> readBigInts(string filename) {
    ifstream fileIn(filename);
    vector<flint::BigInt> elements;
    string fileLine;
    while(getline(fileIn, fileLine)) {
        flint::BigInt element;
        element.assign(fileLine);
        elements.push_back(move(element));
    }
    return elements;
}

void rsaTest(int setSize) {
//    cout << "RSA Accumulator test:" << endl;
    static const int THREAD_POOL_SIZE = 16;
    ThreadPool threadPool(THREAD_POOL_SIZE);

    vector<flint::BigInt> elements;
    //Read a random set from a file
    elements = readBigInts("randomBigInts" + to_string(setSize));
//    cout << "Generated " << elements.size() << " random elements." << endl;
//    for(auto& element : elements) {
//        cout << element << endl;
//    }
    cout << elements.size() << endl;

    //Generate the public/private key
    double keyGenStart = Profiler::getCurrentTime();
    RSAKey rsaKey;
    RSAAccumulator::genKey(0, 3072, rsaKey);
    double keyGenEnd = Profiler::getCurrentTime();
//    cout << "Key generation took " << (keyGenEnd-keyGenStart) << " seconds" << endl;
    cout << (keyGenEnd-keyGenStart) << endl;

    //Generate representatives for the elements
    vector<flint::BigInt> representatives(setSize);
    double repGenStart = Profiler::getCurrentTime();
    RSAAccumulator::genRepresentatives(elements, *(rsaKey.getPublicKey().primeRepGenerator), representatives, threadPool);
    double repGenEnd = Profiler::getCurrentTime();
//    cout << "Generated " << elements.size() << " prime representatives in " << (repGenEnd-repGenStart) << " seconds" << endl;
    cout << (repGenEnd-repGenStart) << endl;

    //Accumulate the representatives
    flint::BigMod accumulator;
    double accStart = Profiler::getCurrentTime();
    RSAAccumulator::accumulateSet(representatives, rsaKey, accumulator, threadPool);
    double accEnd = Profiler::getCurrentTime();
//    cout << "Accumulated " << representatives.size() << " prime representatives in " << (accEnd-accStart) << " seconds with private key" << endl;
    cout << (accEnd-accStart) << endl;

    //Accumulate again with only the public information
    flint::BigMod accPub;
    double pubAccStart = Profiler::getCurrentTime();
    RSAAccumulator::accumulateSet(representatives, rsaKey.getPublicKey(), accPub);
    double pubAccEnd = Profiler::getCurrentTime();
//    cout << "Accumulated " << representatives.size() << " prime representatives in " << (pubAccEnd-pubAccStart) << " seconds with public key" << endl;
    cout << (pubAccEnd-pubAccStart) << endl;

    if(accumulator != accPub) {
        cout << "Error! Public and private accumulation do not match!" << endl;
        cout << "Private: " << accumulator << endl;
        cout << "Public: " << accPub << endl;
    }

    //Generate witnesses for all of the representatives
    vector<flint::BigMod> witnesses(setSize);
    double witStart = Profiler::getCurrentTime();
    RSAAccumulator::witnessesForSet(representatives, rsaKey, witnesses, threadPool);
    double witEnd = Profiler::getCurrentTime();
//    cout << "Generated " << witnesses.size() << " witnesses in " << (witEnd-witStart) << " seconds with private key" << endl;
    cout << (witEnd-witStart) << endl;

    //Generate witnesses again with only the public information
    vector<flint::BigMod> witnessesPub(setSize);
    double witPubStart = Profiler::getCurrentTime();
    RSAAccumulator::witnessesForSet(representatives, rsaKey.getPublicKey(), witnessesPub, threadPool);
    double witPubEnd = Profiler::getCurrentTime();
//    cout << "Generated " << witnessesPub.size() << " witnesses in " << (witPubEnd-witPubStart) << " seconds with public key" << endl;
    cout << (witPubEnd-witPubStart) << endl;

    //Verify the values with the witnesses
    double verifyStart = Profiler::getCurrentTime();
    for(size_t i = 0; i < elements.size(); i++) {
        bool verifyPassed = RSAAccumulator::verify(elements.at(i), witnesses.at(i), accumulator, rsaKey.getPublicKey());
        if(!verifyPassed) {
            cout << "Witness for element " << i << " did not pass!" << endl;
        }
    }
    double verifyEnd = Profiler::getCurrentTime();
//    cout << "Verified " << elements.size() << " elements in " << (verifyEnd-verifyStart) << " seconds" << endl;
    cout << (verifyEnd-verifyStart) << endl;

    //Verify the public-key witnesses, just in case they're different
    double verifyPubStart = Profiler::getCurrentTime();
    for(size_t i = 0; i < elements.size(); i++) {
        bool verifyPassed = RSAAccumulator::verify(elements.at(i), witnessesPub.at(i), accPub, rsaKey.getPublicKey());
        if(!verifyPassed) {
            cout << "Witness for element " << i << " did not pass!" << endl;
        }
    }
    double verifyPubEnd = Profiler::getCurrentTime();
//    cout << "Verified " << witnessesPub.size() << " elements in " << (verifyPubEnd-verifyPubStart) << " seconds against public accumulator" << endl;
    //This actually doesn't need to get measured and logged, since the time will
    //be the same as for verification with the private-key witnesses. It only
    //needs to run to guarantee correctness.
}

}

