/*
 * Edward Tremel - etremel@cs.brown.edu
 *         April 2013
 */

#include <algorithm>
#include <functional>
#include <future>
#include <math.h>
#include <memory>
#include <vector>

#include <utils/LibConversions.hpp>
#include <utils/Pointers.hpp>
#include <utils/Profiler.hpp>
#include <utils/ThreadPool.hpp>
#include <utils/testutils.hpp>

#include <bilinear/G1_DCLXVI.hpp>
#include <bilinear/G2_DCLXVI.hpp>
#include <bilinear/GT_DCLXVI.hpp>
#include <bilinear/Scalar_DCLXVI.hpp>

#include <flint/BigInt.hpp>
#include <flint/BigMod.hpp>
#include <flint/ModPolynomial.hpp>

#include <algorithms/BilinearMapAccumulator.hpp>

using std::cout;
using std::endl;

extern "C" {
#include <optate.h>
}
using std::reference_wrapper;
using std::unique_ptr;

namespace BilinearMapAccumulator {

typedef std::unique_lock<std::mutex> lock_t;

/*-------------------------------Key Generation-------------------------------*/
//Private helper method
void computeG1Powers(const Scalar& secretKey, const unsigned int numPowers, std::vector<unique_ptr<G>>& pk1) {
    pk1.push_back(std::make_unique<G1DCLXVI>());
    for(unsigned int i = 0; i < numPowers; i++) {
        unique_ptr<G> g1Pow = std::make_unique<G1DCLXVI>();
        pk1.back()->doPower(secretKey, *g1Pow);
        pk1.push_back(move(g1Pow));
    }
}

//Private helper method
void computeG2Powers(const Scalar& secretKey, const unsigned int numPowers, std::vector<unique_ptr<G>>& pk2) {
    pk2.push_back(std::make_unique<G2DCLXVI>());
    for(unsigned int i = 0; i < numPowers; i++) {
        unique_ptr<G> g2Pow = std::make_unique<G2DCLXVI>();
        pk2.back()->doPower(secretKey, *g2Pow);
        pk2.push_back(move(g2Pow));
    }
}

void genKey(const std::vector<std::vector<reference_wrapper<Scalar>>>& sets, const unsigned int maxPkSize, BilinearMapKey& key, ThreadPool& threadPool) {
    // cout<<"Generating key...";
    unsigned int q = 0;

    if(maxPkSize > 0) {
        q = maxPkSize;
    } else {
        //Find largest set in sets of input
        unsigned int m = sets.size();

        for(size_t i = 0; i < m; i++) {
            if(q < (sets[i]).size()) {
                q = (sets[i]).size();
            }
        }
        if(q < m) {
            q = m;
        }
    }

    // Secret key
    Scalar& sk = key.getSecretKey();
    sk.generateRandom();

    // Public key
    BilinearMapKey::PublicKey& pk = key.getPublicKey();
    std::vector<unique_ptr<G>>& pk1 = pk.first;
    std::vector<unique_ptr<G>>& pk2 = pk.second;

    //Each component of the public key (powers of G1 and powers of G2) can be computed separately
    std::future<void> pk1Future = threadPool.enqueue<void>([&]() {
        computeG1Powers(sk, q, pk1);
    });
    std::future<void> pk2Future = threadPool.enqueue<void>([&]() {
        computeG2Powers(sk, q, pk2);
    });

    pk1Future.get();
    pk2Future.get();

    // cout<<"done. Generated "<<q<<" elements in both G1/G2."<<endl;
}

/*--------------------------Private key accumulation--------------------------*/

void accumulateSet(const std::vector<reference_wrapper<Scalar>>& set, const Scalar& privKey, G& acc) {
    flint::BigInt modulus;
    LibConversions::getModulus(modulus);
    flint::BigMod sk(modulus);
    privKey.exportFlintObject(sk);

    flint::BigMod sum(modulus), power(modulus);
    flint::BigMod element(modulus);
    power = 1;
    for(Scalar& scalar : set) {
        scalar.exportFlintObject(element);
        sum = sk + element;
        power = power * sum;
    }

    ScalarDCLXVI pScalar;
    pScalar.importFlintObject(power);
    acc.doPower(pScalar, acc);
}

/*---------------------------Public key accumulation--------------------------*/
/**
 * Private (not in header) helper function, to be submitted as a thread task.
 * Computes the product of a range of public-key elements raised to the powers
 * of a corresponding range of coefficients. Since each public-key element is
 * g to a power of s, and each coefficient is the coefficient of a power of s,
 * this computes g^(c0 + c1*s + c2*s^2 + c3*s^3) by computing g * (g^s)^c1 *
 * (g^(s^2))^c2 * (g^(s^3))^c3.
 *
 * @param inG2 True if the value being accumulated is in G2, false if it is in
 *        G1. This affects whether the public-key elements for G1 or G2 are used.
 * @param publicKey The public key of the bilinear-map accumulator, containing
 *        all the s-powers of G1 and G2
 * @param coeffs The vector of coefficients, a subset of which should be multiplied
 * @param startOffset The index into coeffs and publicKey at which to start multiplication
 * @param endOffset The index into coeffs and publicKey at which multiplication
 *        ends, exclusive
 * @return The product of the range of key elements raised to the powers of the
 *         corresponding range of coefficients
 */
unique_ptr<G> computePower(const bool& inG2, const BilinearMapKey::PublicKey& publicKey,
                           const std::vector<unique_ptr<Scalar>>& coeffs, size_t startOffset, size_t endOffset) {
    size_t rangeLen = endOffset - startOffset;

    scalar_t coeffs_scalars[rangeLen];
    unique_ptr<curvepoint_fp_struct_t[]> pk_curvepoints;
    unique_ptr<twistpoint_fp2_struct_t[]> pk_twistpoints;

    if(inG2) {
        pk_twistpoints = unique_ptr<twistpoint_fp2_struct_t[]>(new twistpoint_fp2_struct_t[rangeLen]);
    } else {
        pk_curvepoints = unique_ptr<curvepoint_fp_struct_t[]>(new curvepoint_fp_struct_t[rangeLen]);
    }

    //Convert all the C++ objects (Scalar, G1, G2) to their underlying C objects (scalar_t, twistpoint, curvepoint)
    int arraysIndex = 0;
    for(size_t i = startOffset; i < endOffset; i++) {
        coeffs.at(i)->exportObject(&coeffs_scalars[arraysIndex]);
        if(inG2) {
            publicKey.second.at(i)->exportObject(&pk_twistpoints[arraysIndex]);
        } else {
            publicKey.first.at(i)->exportObject(&pk_curvepoints[arraysIndex]);
        }
        arraysIndex++;
    }

    //Using the magical batch-multiply function from DCLXVI, raise all the public
    //key elements to the power of the coefficients and multiply the powers together
    unique_ptr<G> tempProduct;
    if(inG2) {
        tempProduct = std::make_unique<G2DCLXVI>();
        twistpoint_fp2_multiscalarmult_vartime(((G2DCLXVI*)(tempProduct.get()))->getUnderlyingObj(), pk_twistpoints.get(), coeffs_scalars, arraysIndex);
    } else {
        tempProduct = std::make_unique<G1DCLXVI>();
        curvepoint_fp_multiscalarmult_vartime(((G1DCLXVI*)(tempProduct.get()))->getUnderlyingObj(), pk_curvepoints.get(), coeffs_scalars, arraysIndex);
    }

    // cout << "Computed powers from coeffs[" << startOffset << "] to coeffs[" << (endOffset - 1) << "]" << endl;
    // cout << "Result: ";
    // testutils::print_hex(tempProduct->getByteBuffer(), tempProduct->getSize());

    return tempProduct;
}

void accumulateSetFromCoeffs(const std::vector<unique_ptr<Scalar>>& coeffs, const BilinearMapKey::PublicKey& publicKey,
                             G& acc, bool inG2, ThreadPool& threadPool) {
    static const size_t MAX_TASKS = 50;
    static const size_t MIN_OPERATIONS_PER_TASK = 1000;
    const std::vector<unique_ptr<G>>& pk1 = publicKey.first;
    const std::vector<unique_ptr<G>>& pk2 = publicKey.second;

    //compute the first power, for the constant term, outside of threads
    //initialize acc to this value (disregard what it used to be)
    if(inG2) {
        pk2.at(0)->doPower(*(coeffs.at(0)), acc);
    } else {
        pk1.at(0)->doPower(*(coeffs.at(0)), acc);
    }
    // cout << "After raising publicKey element 0 to [";
    // coeffs.at(0)->printPretty();
    // cout << "], acc is: ";
    // testutils::print_hex(acc.getByteBuffer(), acc.getSize());

    //Given the size of the coefficients vector and target number of threads,
    //determine the number of coefficients each thread can work on, and how many
    //will be left over at the end after dividing evenly
    size_t size = coeffs.size() - 1, numOfThreads, rangeLen, leftItems;
    if(size <= MAX_TASKS) {
        rangeLen = 1;
        numOfThreads = size;
        leftItems = 0;
    } else if(size <= MIN_OPERATIONS_PER_TASK) {
        rangeLen = size;
        numOfThreads = 1;
        leftItems = 0;
    } else {
        rangeLen = size / MAX_TASKS;
        numOfThreads = MAX_TASKS;
        leftItems = size % MAX_TASKS;
    }
    //But there's also a target number of operations per thread, so if the
    //range per thread ended up less than that, increase it to the target and
    //decrease the number of threads
    if(rangeLen < MIN_OPERATIONS_PER_TASK && size > MIN_OPERATIONS_PER_TASK) {
        rangeLen = MIN_OPERATIONS_PER_TASK;
        numOfThreads = (int)ceil(size / (1.0 * rangeLen));
        leftItems = size % numOfThreads;
    }

    // cout << "Size: " << size << endl;
    // cout << "Threads: " << numOfThreads <<endl;
    // cout << "leftItems: " << leftItems << endl;

    std::vector<std::future<unique_ptr<G>>> workerFutures;
    //Starting at index 1, divide the coefficients vector up into rangeLen-size
    //chunks, and spawn a new thread to deal with each subset
    size_t offset = 1;
    size_t totalCoef = 0;
    for(size_t i = 0; i < numOfThreads; i++) {
        size_t startOffset = offset;
        size_t endOffset;
        //The first leftItems threads will get an extra coefficient each
        if(i < leftItems) {
            endOffset = startOffset + rangeLen + 1;
            totalCoef += rangeLen + 1;
        } else {
            if((size - totalCoef) < rangeLen) {
                endOffset = startOffset + (size - totalCoef);
                totalCoef += (size - totalCoef);
            } else {
                endOffset = startOffset + rangeLen;
                totalCoef += rangeLen;
            }
        }
        offset = endOffset;

        // cout << "Thread " << i << " will have offsets " << startOffset << " to " << endOffset << endl;

        //Note: the offsets must be passed by value, because they'll get cleaned
        //up once this scope ends, but the threads may still be running!
        workerFutures.push_back(
                threadPool.enqueue<unique_ptr<G>>([&, startOffset, endOffset]() {
                    return computePower(inG2, publicKey, coeffs, startOffset, endOffset);
                }));
    }

    //Set acc to the first non-null temporary product, then multiply into it
    // bool accInitialized = false;
    // int i = -1;
    for(std::future<unique_ptr<G>>& workerFuture : workerFutures) {
        // i++;
        unique_ptr<G> tempProduct = workerFuture.get();
        // if(tempProduct == nullptr)
        //     continue;
        // if(!accInitialized) {
        //     acc = *(tempProduct);
        //     accInitialized = true;
        // } else {
        // }
        acc.doMultiplication(*(tempProduct), acc);
        // cout << "Product from thread " << i << " is ";
        // testutils::print_hex(tempProduct->getByteBuffer(), tempProduct->getSize());
        // cout << "After including product from thread " << i << ", acc is: ";
        // testutils::print_hex(acc.getByteBuffer(), acc.getSize());
    }
}

//Private function - not declared in header
void recursivePolMult(int low, int high, const std::vector<reference_wrapper<Scalar>>& diff, flint::ModPolynomial* pPoly) {
    const flint::BigInt modulus = ([]() {flint::BigInt modulus;
                                        LibConversions::getModulus(modulus);
                                        return modulus; })();
    if(high - low == 1) {
        flint::ModPolynomial x(modulus);
        //Sets coefficient 1 to 1, and modulus to the global DCLXVI modulus
        x.set(1, 1);
        flint::BigMod diffLow(modulus);
        diff.at(low).get().exportFlintObject(diffLow);
        x.set(0L, diffLow.getMantissa());
        (*pPoly) = x;
    } else {
        int mid = (high - low) / 2;
        flint::ModPolynomial* pPoly1 = new flint::ModPolynomial(modulus);
        flint::ModPolynomial* pPoly2 = new flint::ModPolynomial(modulus);
        recursivePolMult(low, low + mid, diff, pPoly1);
        recursivePolMult(low + mid, high, diff, pPoly2);
        (*pPoly) = (*pPoly1) * (*pPoly2);
        delete pPoly1;
        delete pPoly2;
    }
}

//Private function - not declared in header
void computeCoefficients(const std::vector<reference_wrapper<Scalar>>& roots, std::vector<unique_ptr<Scalar>>& coeffs) {
    const flint::BigInt modulus = ([]() {flint::BigInt modulus;
                                        LibConversions::getModulus(modulus);
                                        return modulus; })();
    flint::ModPolynomial polynomial(modulus);
    if(roots.size()) {
        recursivePolMult(0, roots.size(), roots, &polynomial);
    } else {
        polynomial.setConstant(1);
    }

    long degree = polynomial.getDegree() + 1;
    for(long i = 0; i < degree; i++) {
        unique_ptr<Scalar> pScalar = std::make_unique<ScalarDCLXVI>();
        pScalar->importFlintObject(polynomial.at(i));
        coeffs.push_back(move(pScalar));
    }
}

//Private function - not declared in header
void accumulateSet(const std::vector<reference_wrapper<Scalar>>& set, const BilinearMapKey::PublicKey& publicKey,
                   G& acc, bool inG2, ThreadPool& threadPool) {
    std::vector<unique_ptr<Scalar>> coeffs;
    computeCoefficients(set, coeffs);
    // cout << "Set is: " << endl;
    // for(const Scalar& el : set) {
    //     el.printPretty();
    // }
    // cout << endl;
    // cout << "Coefficients are: " << endl;
    // for(const unique_ptr<Scalar>& c : coeffs) {
    //     c->printPretty();
    // }
    accumulateSetFromCoeffs(coeffs, publicKey, acc, inG2, threadPool);
}

//Just a wrapper to hide the "inG2" parameter from the client
void accumulateSet(const std::vector<reference_wrapper<Scalar>>& set, const BilinearMapKey::PublicKey& publicKey,
                   G& acc, ThreadPool& threadPool) {
    accumulateSet(set, publicKey, acc, false, threadPool);
}

/*-----------------------Private key witness generation-----------------------*/

std::vector<flint::BigMod> multiplyLeftProducts(const std::vector<reference_wrapper<Scalar>>& set, const flint::BigMod& secretKey, const flint::BigInt& modulus) {
    std::vector<flint::BigMod> leftProducts(set.size() + 1, flint::BigMod(1, modulus));
    //Compute products (x_i + s) for the exponent of g left-to-right, saving each partial product
    flint::BigMod sum(modulus), element(modulus);
    for(size_t i = 1; i <= set.size(); i++) {
        set.at(i - 1).get().exportFlintObject(element);
        sum = secretKey + element;
        leftProducts.at(i) = leftProducts.at(i - 1) * sum;
    }

    return leftProducts;
}

std::vector<flint::BigMod> multiplyRightProducts(const std::vector<reference_wrapper<Scalar>>& set, const flint::BigMod& secretKey, const flint::BigInt& modulus) {
    std::vector<flint::BigMod> rightProducts(set.size() + 1, flint::BigMod(1, modulus));
    //Compute products (x_i + s) for the exponent of g right-to-left, saving each partial product
    flint::BigMod sum(modulus), element(modulus);
    for(int i = set.size() - 1; i >= 0; i--) {  //must be int not size_t so it can stop at 0
        set.at(i).get().exportFlintObject(element);
        sum = secretKey + element;
        rightProducts.at(i) = rightProducts.at(i + 1) * sum;
    }
    return rightProducts;
}

void witnessesForSet(const std::vector<reference_wrapper<Scalar>>& set, const Scalar& privKey,
                     G& base, std::vector<unique_ptr<G>>& witnesses, ThreadPool& threadPool) {
    flint::BigInt modulus;
    LibConversions::getModulus(modulus);
    flint::BigMod sk(modulus);
    privKey.exportFlintObject(sk);

    //Compute products for the exponent separately in threads; the vectors will
    //also be initialized in the threads, since initialization is O(n)
    std::future<std::vector<flint::BigMod>> leftFuture = threadPool.enqueue<std::vector<flint::BigMod>>([&]() {
        return multiplyLeftProducts(set, sk, modulus);
    });
    std::future<std::vector<flint::BigMod>> rightFuture = threadPool.enqueue<std::vector<flint::BigMod>>([&]() {
        return multiplyRightProducts(set, sk, modulus);
    });

    std::vector<flint::BigMod> leftProducts = leftFuture.get();
    std::vector<flint::BigMod> rightProducts = rightFuture.get();
    //Generate exponent for element i's witness by multiplying left-product i with right-product i+1
    flint::BigMod power(modulus);
    for(size_t i = 0; i < set.size(); i++) {
        power = leftProducts.at(i) * rightProducts.at(i + 1);
        ScalarDCLXVI powerScalar;
        powerScalar.importFlintObject(power);
        base.doPower(powerScalar, *(witnesses.at(i)));
    }
}

/*------------------------Public key witness generation-----------------------*/

/* Unfortunately, the only way to compute witnesses with the public key is
 * brute force: by calling accumulateSet on each subset {set - set[i]}
 */
void witnessTask(const std::vector<reference_wrapper<Scalar>>& set, const BilinearMapKey::PublicKey& publicKey,
                 const int witnessIndex, unique_ptr<G>& witness) {
    //Don't use the thread pool this task is in to run accumulateSet, otherwise the tasks
    //created by accumulateSet might get blocked by the witnessTask itself
    static const size_t THREADS_IN_SUB_POOL = 16;
    static ThreadPool localPool(THREADS_IN_SUB_POOL);
    //Make a subset that excludes witnessIndex
    std::vector<reference_wrapper<Scalar>> subset = std::vector<reference_wrapper<Scalar>>(set.begin(), set.begin() + witnessIndex);
    subset.insert(subset.end(), set.begin() + witnessIndex + 1, set.end());
    accumulateSet(subset, publicKey, *(witness), true, localPool);
}
void witnessesForSet(const std::vector<reference_wrapper<Scalar>>& set, const BilinearMapKey::PublicKey& publicKey,
                     std::vector<unique_ptr<G>>& witnesses, ThreadPool& threadPool) {
    std::vector<std::future<void>> futures;
    for(size_t i = 0; i < set.size(); i++) {
        futures.push_back(threadPool.enqueue<void>([&, i]() {
            witnessTask(set, publicKey, i, witnesses.at(i));
        }));
    }
    for(auto& future : futures) {
        future.get();
    }
}

/*--------------------------------Verification--------------------------------*/

void pairing(GT& result, const G& g1Element, const G& g2Element) {
    fp12e_t rop;
    curvepoint_fp_t op1;
    twistpoint_fp2_t op2;
    const G1DCLXVI& pG1 = ref_cast<G1DCLXVI>(g1Element);
    const G2DCLXVI& pG2 = ref_cast<G2DCLXVI>(g2Element);
    pG1.exportObject(op1);
    pG2.exportObject(op2);
    optate(rop, op2, op1);  //wtf?
    GTDCLXVI& pGT = ref_cast<GTDCLXVI>(result);
    pGT.importObject(rop);
}

bool verify(const Scalar& element, const G& witness, const G& accumulator, BilinearMapKey::PublicKey& publicKey) {
    G1DCLXVI g1Generator;
    G2DCLXVI g2Generator;

    //Compute g^element
    G1DCLXVI elementAsG;
    g1Generator.doPower(element, elementAsG);
    //Compute g^element * g^s, retrieving g^s from the public key
    G1DCLXVI elementInAccumulator;
    G& gPower1 = *(publicKey.first.at(1));
    gPower1.doMultiplication(elementAsG, elementInAccumulator);
    //Pairing e1 is g^(element+s) with witness, where g^(element+s) is in G1 and witness is in G2
    GTDCLXVI e1;
    pairing(e1, elementInAccumulator, witness);
    //Pairing e2 is the accumulator (in G1) with the generator for G2
    GTDCLXVI e2;
    pairing(e2, accumulator, g2Generator);
    //The results of the two pairings should be equal
    return e1.isEqual(e2);
}

}  // namespace BilinearMapAccumulator
