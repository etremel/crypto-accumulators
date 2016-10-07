/*
 * libdirecttest.cpp
 *
 *  Created on: Apr 15, 2013
 *      Author: etremel
 */

#include <iostream>
#include <gmp.h>

extern "C"{
    #include <curvepoint_fp.h>
    #include <scalar.h>
}

extern const scalar_t bn_n;
extern const curvepoint_fp_t bn_curvegen;

using std::cout;
using std::endl;

void allReduced(curvepoint_fp_t curvepoint){
    fpe_isreduced(curvepoint->m_x);
    fpe_isreduced(curvepoint->m_y);
    fpe_isreduced(curvepoint->m_z);
    fpe_isreduced(curvepoint->m_t);
}

bool isEqual(curvepoint_fp_t& lhs, curvepoint_fp_t& rhs){
    curvepoint_fp_makeaffine(lhs);
    curvepoint_fp_makeaffine(rhs);
    return (fpe_iseq(lhs->m_x, rhs->m_x) &&
            fpe_iseq(lhs->m_y, rhs->m_y) &&
            fpe_iseq(lhs->m_z, rhs->m_z));
}

int main(int argc, char** argv) {
    curvepoint_fp_t randomElement;
    scalar_t rScalar;
    scalar_setrandom(rScalar, bn_n);
    curvepoint_fp_scalarmult_vartime(randomElement, randomElement, rScalar);
    curvepoint_fp_makeaffine(randomElement);
    allReduced(randomElement);
    curvepoint_fp_t g1Generator;
    curvepoint_fp_set(g1Generator, bn_curvegen);
    fpe_isreduced(g1Generator->m_x);
    fpe_isreduced(g1Generator->m_y);
    curvepoint_fp_t gToThe0;
    scalar_t scalar0;
    unsigned long long zeroArr[] = {0ULL, 0ULL, 0ULL, 0ULL};
    scalar_set_lluarray(scalar0, zeroArr);
    if(scalar_iszero_vartime(scalar0))
        cout << "Scalar is 0." << endl;
    //gToThe0 = g^0
    curvepoint_fp_scalarmult_vartime(gToThe0, g1Generator, scalar0);
    curvepoint_fp_t gToThe0Squared;
    //gToThe0Squared = g^0 * g^0
    curvepoint_fp_add_vartime(gToThe0Squared, gToThe0, gToThe0);
    curvepoint_fp_t randomTimesG0;
    //randomTimesG0 = random * g^0
    curvepoint_fp_add_vartime(randomTimesG0, randomElement, gToThe0);
    curvepoint_fp_t randomTimesG0Squared;
    //randomTimesG0Squared = random * (g^0 * g^0)
    curvepoint_fp_add_vartime(randomTimesG0Squared, randomElement, gToThe0Squared);
    curvepoint_fp_t randomTimesG0TimesG0;
    //randomTimesG0TimesG0 = (random * g^0) * g^0
    curvepoint_fp_add_vartime(randomTimesG0TimesG0, randomTimesG0, gToThe0);
    if(isEqual(randomTimesG0, randomElement)) {
        cout << "random * g^0 = random" << endl;
    } else {
        cout << "random * g^0 != random" << endl;
    }
    if(isEqual(randomTimesG0Squared, randomTimesG0TimesG0)) {
        cout << "random * (g^0 * g^0) = (random * g^0)(g^0)" << endl;
    } else {
        cout << "random * (g^0 * g^0) != (random * g^0)(g^0)" << endl;
    }
    if(isEqual(randomTimesG0Squared, randomTimesG0)) {
        cout << "random * (g^0 * g^0) = random * g^0" << endl;
    } else {
        cout << "random * (g^0 * g^0) != random * g^0" << endl;
    }
}


