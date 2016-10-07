/*
 * Edward Tremel - etremel@cs.brown.edu
 * March 2013
 */

#include <thread>
#include <vector>
#include <functional>
#include <algorithm>

#include <utils/Pointers.h>
#include <utils/LibConversions.h>
#include <utils/ThreadPool.h>

#include <algorithms/PrimeRepGenerator.h>
#include <algorithms/OraclePrimeRep.h>
#include <algorithms/RSAAccumulator.h>

#include <cryptopp/rsa.h>
#include <cryptopp/osrng.h>

using std::max;
using std::vector;
using std::unique_ptr;
using std::ref;
using std::cref;
using std::future;
using std::bind;

namespace RSAAccumulator {

/*-------------------------------Key Generation-------------------------------*/

void genKey(const unsigned int elementBits, const unsigned int modulusBits, RSAKey &key) {
    unsigned int modBits;
    if (modulusBits == 0){
        modBits = 3*elementBits+1;
    } else if(elementBits == 0) {
        modBits = modulusBits;
    } else {
        modBits = max(3*elementBits+1, modulusBits);
    }
    //Use Crypto++ to generate an RSA modulus
    CryptoPP::AutoSeededRandomPool random;
    CryptoPP::RSA::PrivateKey rsaKey;
    rsaKey.GenerateRandomWithKeySize(random, modBits);
    LibConversions::CryptoPPToFlint(rsaKey.GetModulus(), key.getPublicKey().rsaModulus);
    LibConversions::CryptoPPToFlint(rsaKey.GetPrime1(), key.getSecretKey().first);
    LibConversions::CryptoPPToFlint(rsaKey.GetPrime2(), key.getSecretKey().second);
    //Apparently, this is a reasonable number to hard-code for the base
    key.getPublicKey().base = flint::BigMod(65537, key.getPublicKey().rsaModulus);
    //For now, just hard-code in which PrimeRepGenerator to instantiate...
    key.getPublicKey().primeRepGenerator = unique_new<OraclePrimeRep>();
}

/*-------------------------------Representatives------------------------------*/

void genRepresentatives(const vector<flint::BigInt> &set, PrimeRepGenerator &repGen, vector<flint::BigInt>& reps, ThreadPool& threadPool) {
    vector<future<void>> futures;
    for(vector<flint::BigMod>::size_type element = 0; element < set.size(); element++) {
        //Submit repGen.genRepresentative(set[element], reps[element]) to the thread pool
        futures.push_back(threadPool.enqueue<void>(
                bind(&PrimeRepGenerator::genRepresentative, &repGen,
                        ref(set.at(element)), ref(reps.at(element)))
        ));
    }
    for(auto& future : futures) {
        future.get();
    }
}

/*--------------------------Private key accumulation--------------------------*/

void accumulateSet(const vector<flint::BigInt>& reps, const RSAKey &key, flint::BigMod &accumulator, ThreadPool& threadPool) {
    flint::BigInt phiOfN = (key.getSecretKey().first-1) * (key.getSecretKey().second-1);
    //The accumulator's exponent is the product of all the representatives mod phi(N)
    flint::BigMod exponent(1, phiOfN);
    for(auto rep : reps) {
        exponent *= rep;
    }
    //Don't trust the caller to have initialized accumulator's modulus
    accumulator.setModulus(key.getPublicKey().rsaModulus);
    flint::power(key.getPublicKey().base, exponent.getMantissa(), accumulator);
}

/*---------------------------Public key accumulation--------------------------*/
/**
 * Private (not in header) helper method that accumulates some or all of a set
 * using only the public key. If a valid indexToSkip is provided, it will
 * accumulate only the elements excluding that index. If indexToSkip is >= reps.size(),
 * it will accumulate the entire set.
 * @param reps The set of representatives of elements to accumulate
 * @param indexToSkip The index of the set indicating the element not to accumulate
 * @param publicKey The public key for the RSA accumulator
 * @return A BigMod that containing the result of accumulation; this
 *        could be an accumulator, if the entire set is accumulated, or a
 *        witness, if an element is skipped.
 */
flint::BigMod accumulateSetHelper(const vector<flint::BigInt>& reps, const size_t indexToSkip,
        const RSAKey::PublicKey& publicKey) {
    flint::BigMod output;
    //Ensure the modulus is set correctly
    output.setModulus(publicKey.rsaModulus);
    output = publicKey.base;
    //Safely allow the client to set an invalid skip index
    size_t end = std::min(indexToSkip, reps.size());
    //Split the loop into two parts, before and after the skip index
    for(size_t i = 0; i < end; i++) {
        output ^= reps.at(i);
    }
    for(size_t i = indexToSkip + 1; i < reps.size(); i++) {
        output ^= reps.at(i);
    }
    //Unfortunately there's no way to parallelize this, because each exponentiation
    //depends on the previous result, and without phi(N) we can't combine exponents
    return output;
}

void accumulateSet(const vector<flint::BigInt>& reps, const RSAKey::PublicKey& publicKey, flint::BigMod &accumulator) {
    //Just wrap the helper, hiding the "indexToSkip" parameter
    accumulator = accumulateSetHelper(reps, reps.size(), publicKey);
}

/*-----------------------Private key witness generation-----------------------*/

//Compute products for the witness exponents left-to-right, saving each partial product
vector<flint::BigMod> multiplyLeftProducts(const vector<flint::BigInt>& reps, const flint::BigInt& phiOfN) {
    vector<flint::BigMod> leftProducts(reps.size()+1, flint::BigMod(1, phiOfN));
    for(vector<flint::BigMod>::size_type i = 1; i <= reps.size(); i++) {
        leftProducts.at(i) = leftProducts.at(i-1) * reps.at(i-1);
    }
    return leftProducts;
}

//Compute products for the witness exponents right-to-left, saving each partial product
vector<flint::BigMod> multiplyRightProducts(const vector<flint::BigInt>& reps, const flint::BigInt& phiOfN) {
    vector<flint::BigMod> rightProducts(reps.size()+1, flint::BigMod(1, phiOfN));
    for(vector<flint::BigMod>::size_type i = reps.size()-1; i != (vector<flint::BigMod>::size_type)-1; i--) {
        rightProducts.at(i) = rightProducts.at(i+1) * reps.at(i);
    }
    return rightProducts;
}

//Holds onto the temporary exponent so it doesn't go out of scope, and resolves the ambiguous overload to power
void powerWrapper(const flint::BigMod base, const flint::BigMod& leftProduct, const flint::BigMod& rightProduct, flint::BigMod& witness) {
    flint::BigMod exponent = leftProduct * rightProduct;
    flint::power(base, exponent.getMantissa(), witness);
}

void witnessesForSet(const vector<flint::BigInt> &reps, const RSAKey &key, vector<flint::BigMod> &witnesses, ThreadPool& threadPool) {
    flint::BigInt phiOfN = (key.getSecretKey().first-1) * (key.getSecretKey().second-1);
    //Compute left and right products in threads.
    //The vectors will be initialized in the threads,
    //since initialization is O(n) and can be done in parallel
    future<vector<flint::BigMod>> leftFuture = threadPool.enqueue<vector<flint::BigMod>>(bind(
            multiplyLeftProducts, cref(reps), cref(phiOfN)));
    future<vector<flint::BigMod>> rightFuture = threadPool.enqueue<vector<flint::BigMod>>(bind(
            multiplyRightProducts, cref(reps), cref(phiOfN)));
    //Wait for both threads to finish
    vector<flint::BigMod> leftProducts = leftFuture.get();
    vector<flint::BigMod> rightProducts = rightFuture.get();
    //Generate exponent for element i's witness by multiplying left-product i with right-product i+1
    vector<future<void>> powerResults;
    for(vector<flint::BigMod>::size_type i = 0; i < reps.size(); i++) {
        witnesses.at(i).setModulus(key.getPublicKey().rsaModulus);
        powerResults.push_back(threadPool.enqueue<void>(bind(
                powerWrapper, cref(key.getPublicKey().base), cref(leftProducts.at(i)), cref(rightProducts.at(i+1)), ref(witnesses.at(i))
                )));
    }
    for(auto& future : powerResults) {
        future.get();
    }

}

/*------------------------Public key witness generation-----------------------*/

void witnessesForSet(const std::vector<flint::BigInt>& reps, const RSAKey::PublicKey& publicKey,
        vector<flint::BigMod>& witnesses, ThreadPool& threadPool) {
    vector<std::future<flint::BigMod>> futures;
    //Submit a task for each witness
    for(size_t witnessIndex = 0; witnessIndex < reps.size(); witnessIndex++) {
        futures.push_back(threadPool.enqueue<flint::BigMod>(
                bind(accumulateSetHelper, cref(reps), witnessIndex,
                        cref(publicKey))
        ));
    }
    //Wait for them all to finish
    for(size_t witnessIndex = 0; witnessIndex < reps.size(); witnessIndex++) {
        witnesses.at(witnessIndex) = futures.at(witnessIndex).get();
    }

}

/*--------------------------------Verification--------------------------------*/

bool verify(const flint::BigInt& element, const flint::BigMod &witness, const flint::BigMod &accumulator, const RSAKey::PublicKey &pubKey) {
    if(witness.getModulus() != pubKey.rsaModulus || accumulator.getModulus() != pubKey.rsaModulus) {
        std::cout << "Verification failed due to modulus mismatch. Witness modulus was ";
        std::cout << witness.getModulus() << std::endl;
        std::cout << "Accumulator modulus was: " << accumulator.getModulus();
        std::cout << ". Public Key modulus was: " << pubKey.rsaModulus << std::endl;
        return false;
    }
    flint::BigInt elementRep;
    pubKey.primeRepGenerator->genRepresentative(element, elementRep);
    flint::BigMod accCandidate(pubKey.rsaModulus);
    flint::power(witness, elementRep, accCandidate);
    bool valid = accCandidate == accumulator;
    if(!valid) {
        std::cout << "Verification failed! Representative for element " << element << " was " << elementRep << std::endl;
    }
    return valid;
}



}
