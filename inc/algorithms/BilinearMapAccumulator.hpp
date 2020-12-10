/*
 * Duy Nguyen - duy@cs.brown.edu
 *         May 18, 2011
 */

#ifndef _BILINEAR_MAP_ACCUMULATOR_H_
#define _BILINEAR_MAP_ACCUMULATOR_H_

#include <iomanip>
#include <iostream>

#include <memory>
#include <vector>

#include <algorithms/BilinearMapKey.hpp>

#include <bilinear/G.hpp>
#include <bilinear/GT.hpp>
#include <bilinear/Scalar.hpp>

#include <utils/ThreadPool.hpp>

extern "C" {
#include <curvepoint_fp_multiscalar.h>
#include <twistpoint_fp2_multiscalar.h>
}

namespace BilinearMapAccumulator {
/**
 * Generates a private/public key pair for a bilinear-map accumulator,
 * given either the set of all elements that will be used as inputs to
 * the accumulator or an integer to use as the size of the public key.
 * The public key must have as many elements as the largest set that will
 * be accumulated with the accumulator, so if a set of sets is given the
 * size of the largest set will be used as the size of the public key.
 *
 * @param sets (optional) a vector of vectors, each of which represents
 *        a set of Scalars that will be accumulated with this key pair
 * @param maxPkSize (optional) the desired size of the public key,
 *        which represents the maximum number of elements that can be
 *        accumulated with this key pair
 * @param key the BilinearMapKey object in which the generated key pair
 *        will be placed.
 * @param threadPool The ThreadPool to use for concurrent computation of
 *        public key components.
 */
void genKey(const std::vector<std::vector<std::reference_wrapper<Scalar>>>& sets,
            const unsigned int maxPkSize, BilinearMapKey& key, ThreadPool& threadPool);

/**
 * Accumulates the given set of Scalars into the given group element,
 * using the given private key.
 *
 * @param set a vector of Scalars that should be accumulated
 * @param privKey the private key of this accumulator ("s")
 * @param acc the group element that will be used as the base of
 *        accumulation, and after execution of this function will
 *        contain the accumulated product. Should be an element of G1.
 */
void accumulateSet(const std::vector<std::reference_wrapper<Scalar>>& set,
                   const Scalar& privKey, G& acc);

/**
 * Accumulates the given set of Scalars into the given group element,
 * using the given public key.
 *
 * @param set a vector of Scalars that should be accumulated
 * @param publicKey the public key of this accumulator
 * @param acc the group element that will be used as the base of
 *        accumulation, and after execution of this function will
 *        contain the accumulated product. Should be an element of G1.
 * @param threadPool the ThreadPool to use for concurrent computation.
 */
void accumulateSet(const std::vector<std::reference_wrapper<Scalar>>& set,
                   const BilinearMapKey::PublicKey& publicKey, G& acc, ThreadPool& threadPool);

/**
 * Computes an accumulator from a set of polynomial coefficients, which
 * represent the set to be accumulated as the product in the exponent of
 * the accumulation equation. The variables in this polynomial are the
 * unknown secret key s, and the polynomial is derived from the fact
 * that each set element would be accumulated with the secret key by
 * multiplying (e + s) in the exponent of a group element.
 *
 * @param coeffs a vector of polynomial coefficients to accumulate
 * @param publicKey the public key of this accumulator
 * @param acc the group element that will be used as the base of
 *        accumulation, and after execution of this function will
 *        contain the accumulated product
 * @param inG2 true if the target accumulator acc is in G2, false if it
 *        is in G1. (Sets are accumulated in G1, witnesses in G2).
 * @param threadPool the ThreadPool to use for concurrent computation.
 */
void accumulateSetFromCoeffs(const std::vector<std::unique_ptr<Scalar>>& coeffs,
                             const BilinearMapKey::PublicKey& publicKey, G& acc, bool inG2, ThreadPool& threadPool);

/**
 * Computes a witness for each element of the given set of Scalars (with
 * respect to the entire set), placing the results in the given vector
 * in the same order. The results vector must be initialized to the same
 * size as the input vector before calling this method, and contain
 * pointers to (default-constructed) elements of G2.
 *
 * @param set a vector of Scalars whose witnesses should be computed
 * @param privKey the private key of this accumulator ("s")
 * @param base the element of G2 that will be used as the base of accumulation
 * @param witnesses the witnesses of the elements in set, where the ith
 *        witness in this vector is a witness for the ith element in set
 * @param threadPool the ThreadPool to use for concurrent computation.
 */
void witnessesForSet(const std::vector<std::reference_wrapper<Scalar>>& set,
                     const Scalar& privKey, G& base, std::vector<std::unique_ptr<G>>& witnesses,
                     ThreadPool& threadPool);

/**
 * Computes a witness for each element of the given set of Scalars (with
 * respsect to the entire set) using only the public key information,
 * placing the results in the given vector. The results vector must be
 * initialized to the same size as the input vector before calling this
 * method, and contain pointers to (default-constructed) elements of G2.
 *
 * @param set a vector of Scalars whose witnesses should be computed
 * @param publicKey the public key of this accumulator
 * @param witnesses the witnesses of the elements in set, where the ith
 *        witness in this vector is a witness for the ith element in set
 * @param threadPool the ThreadPool to use for concurrent computation.
 */
void witnessesForSet(const std::vector<std::reference_wrapper<Scalar>>& set,
                     const BilinearMapKey::PublicKey& publicKey, std::vector<std::unique_ptr<G>>& witnesses,
                     ThreadPool& threadPool);

/**
 * Computes the pairing function of group elements g1Element and
 * g2Element and stores the result in result, an element of the target
 * group. This is the bilinear map of bilinear-map accumulators.
 *
 * @param result an element of group GT that will contain the result of
 *        the pairing
 * @param g1Element an element of group G1 that will be the first input
 *        to the pairing function
 * @param g2Element an element of group G2 that will be the second input
 *        to the pairing function
 */
void pairing(GT& result, const G& g1Element, const G& g2Element);

/**
 * Verifies the given Scalar element as a member of the set represented
 * by the given accumulator, by using the given witness and public key.
 * @param element a Scalar that may have been previously accumulated
 *        with {@code accumulator}
 * @param witness an element of group G2 that is a witness for {@code
 *        element}'s membership in the set
 * @param accumulator an accumulator representing the set that {@code
 *        element} should be a member of
 * @param publicKey the public key for the accumulator
 * @return true if the witness verifies the element's membership with
 *         the accumulator, false otherwise
 */
bool verify(const Scalar& element, const G& witness, const G& accumulator, BilinearMapKey::PublicKey& publicKey);

};  // namespace BilinearMapAccumulator

#endif /* _BILINEAR_MAP_ACCUMULATOR_H_ */
