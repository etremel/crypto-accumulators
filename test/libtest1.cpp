/*
 * libtest.cpp
 *
 *  Created on: Apr 3, 2013
 *      Author: etremel
 */

#include <LiDIA/bigint.h>
#include <LiDIA/bigmod.h>

#include <bilinear/G1_DCLXVI.h>
#include <bilinear/G2_DCLXVI.h>
#include <bilinear/Scalar_DCLXVI.h>

#include <utils/LibConversions.h>
#include <utils/testutils.h>

using namespace std;
/*
 * Tests some libraries that my project depends on.
 */
namespace libtest {


void multiplyIdentityWith(G1DCLXVI target, string targetName) {
    G1DCLXVI identity;
    identity.becomeIdentity();
    G1DCLXVI identityTimesTarget;
    identity.doMultiplication(target, identityTimesTarget);
    if(identityTimesTarget.isEqual(target)) {
        cout << "identity * " << targetName << " = " << targetName << endl;
    } else {
        cout << "identity * " << targetName << " != " << targetName << endl;
    }
}

void multiplyIdentityWith(G2DCLXVI target, string targetName) {
    G2DCLXVI identity;
    identity.becomeIdentity();
    G2DCLXVI identityTimesTarget;
    identity.doMultiplication(target, identityTimesTarget);
    if(identityTimesTarget.isEqual(target)) {
        cout << "identity * " << targetName << " = " << targetName << endl;
    } else {
        cout << "identity * " << targetName << " != " << targetName << endl;
    }
}

void groupOperationsTest() {
    //Initialize LiDIA's global modulus
    LiDIA::bigint modulus;
    LibConversions::getModulus(modulus);
    LiDIA::bigmod::set_modulus(modulus);

    G1DCLXVI g1Generator;
    LiDIA::bigmod lidiaZero;
    LiDIA::bigmod lidiaOne(1);
    G1DCLXVI randomElement;
    randomElement.generateRandom();
    G1DCLXVI identity;
    identity.becomeIdentity();
    //g^0, should be identity
    G1DCLXVI gToThe0;
    ScalarDCLXVI scalar0(lidiaZero);
    g1Generator.doPower(scalar0, gToThe0);

    // -- FIX
    G1DCLXVI g1To0;
    g1Generator.doPower(ScalarDCLXVI(lidiaZero), g1To0);

    identity.doMultiplication(g1To0, identity);
    // FIX --

    if(gToThe0.isEqual(identity)) {
        cout << "g^0 is the identity" << endl;
    } else {
        cout << "g^0 is not the identity" << endl;
    }
    //g^0 * g, should be g
    G1DCLXVI expectedGenerator;
    g1Generator.doMultiplication(gToThe0, expectedGenerator);
    if(expectedGenerator.isEqual(g1Generator)) {
        cout << "(g)(g^0) = g" << endl;
    } else {
        cout << "(g)(g^0) != g" << endl;
    }
    //(g^0)(g^0), should be g^0
    G1DCLXVI gToThe0Squared;
    gToThe0.doMultiplication(gToThe0, gToThe0Squared);
    if(gToThe0Squared.isEqual(gToThe0)) {
        cout << "(g^0)(g^0) = g^0" << endl;
    } else {
        cout << "(g^0)(g^0) != g^0" << endl;
    }
    //(g^0)(g^0) * g equals (g^0) * g?
    G1DCLXVI gDoubleGTo0;
    gToThe0Squared.doMultiplication(g1Generator, gDoubleGTo0);
    if(gDoubleGTo0.isEqual(expectedGenerator)) {
        cout << "(g^0)(g^0)(g) = (g^0)(g)" << endl;
    } else {
        cout << "(g^0)(g^0)(g) != (g^0)(g)" << endl;
    }
    //g^1, should be g
    G1DCLXVI gToThe1;
    ScalarDCLXVI scalar1(lidiaOne);
    g1Generator.doPower(scalar1, gToThe1);
    if(gToThe1.isEqual(g1Generator)) {
        cout << "g^1 = g" << endl;
    } else {
        cout << "g^1 != g" << endl;
    }
    //is g^1 = g^0?
    if(gToThe1.isEqual(gToThe0)) {
        cout << "g^1 = g^0" << endl;
    } else {
        cout << "g^1 != g^0" << endl;
    }
    //random^1, should be random
    G1DCLXVI randomTo1;
    randomElement.doPower(scalar1, randomTo1);
    if(randomTo1.isEqual(randomElement)) {
        cout << "random^1 = random" << endl;
    } else {
        cout << "random^1 != random" << endl;
    }
    //"identity" * g, should be g
    //WARNING: This multiplies g with the old, "un-fixed" value of identity
    multiplyIdentityWith(g1Generator, "g");
    //identity^1, should be identity
    G1DCLXVI identityTo1;
    identity.doPower(scalar1, identityTo1);
    if(identityTo1.isEqual(identity)) {
        cout << "identity^1 = identity" << endl;
    } else {
        cout << "identity^1 != identity" << endl;
    }
    //identity^0, should be identity
    G1DCLXVI identityTo0;
    identity.doPower(scalar0, identityTo0);
    if(identityTo0.isEqual(identity)) {
        cout << "identity^0 = identity" << endl;
    } else {
        cout << "identity^0 != identity" << endl;
    }
    if(identityTo0.isEqual(gToThe0)) {
        cout << "identity^0 = g^0" << endl;
    } else {
        cout << "identity^0 != g^0" << endl;
    }
    //identity * identity, should be identity
    multiplyIdentityWith(identity, "identity");
    //identity * identity * identity, should still be identity
    G1DCLXVI identityTimesSelf;
    identity.doMultiplication(identity, identityTimesSelf);
    multiplyIdentityWith(identityTimesSelf, "identity * identity");
    G1DCLXVI identityCubed;
    identityTimesSelf.doMultiplication(identity, identityCubed);
    if(identityCubed.isEqual(identity)) {
        cout << "identity * identity * identity = identity" << endl;
    } else {
        cout << "identity * identity * identity != identity" << endl;
    }
    //(random value) * identity, should be that value

    multiplyIdentityWith(randomElement, "random");
    //(random value)^0, see if it equals g^0
    G1DCLXVI randomTo0;
    randomElement.doPower(scalar0, randomTo0);
    if(randomTo0.isEqual(gToThe0)) {
        cout << "random^0 = g^0" << endl;
    } else {
        cout << "random^0 != g^0" << endl;
    }
    //(g^1)(g^-1), see if it equals identity
    G1DCLXVI gToMinus1;
    LiDIA::bigmod lidiaMinus1(-1);
    ScalarDCLXVI scalarMinus1(lidiaMinus1);
    g1Generator.doPower(scalarMinus1, gToMinus1);
    G1DCLXVI identityFromNegative;
    gToThe1.doMultiplication(gToMinus1, identityFromNegative);
    if(identityFromNegative.isEqual(identity)) {
        cout << "(g^1)(g^-1) = identity" << endl;
    } else {
        cout << "(g^1)(g^-1) != identity" << endl;
    }
    //Also see if it equals g^0
    if(identityFromNegative.isEqual(gToThe0)) {
        cout << "(g^1)(g^-1) = g^0" << endl;
    } else {
        cout << "(g^1)(g^-1) != g^0" << endl;
    }
    //g^(n-1), see if it equals g^(-1)
    G1DCLXVI gToNMinus1;
    ScalarDCLXVI scalarNMinus1(modulus-1);
    g1Generator.doPower(scalarNMinus1, gToNMinus1);
    if(gToNMinus1.isEqual(gToMinus1)) {
        cout << "g^(n-1) = g^-1" << endl;
    } else {
        cout << "g^(n-1) != g^-1" << endl;
    }
    //(g^1)(g^(n-1)), see if it equals identity
    G1DCLXVI identityFromWraparound;
    gToThe1.doMultiplication(gToNMinus1, identityFromWraparound);
    if(identityFromWraparound.isEqual(identity)) {
        cout << "(g^1)(g^(n-1)) = identity" << endl;
    } else {
        cout << "(g^1)(g^(n-1)) != identity" << endl;
    }
    //see if it equals g^0 (it does)
    if(identityFromWraparound.isEqual(gToThe0)) {
        cout << "(g^1)(g^(n-1)) = g^0" << endl;
    } else {
        cout << "(g^1)(g^(n-1)) != g^0" << endl;
    }
    //(g^1)(g^(n-1))(g^0), should still equal g^0
    G1DCLXVI wraparoundTimesG0;
    identityFromWraparound.doMultiplication(gToThe0, wraparoundTimesG0);
    if(wraparoundTimesG0.isEqual(gToThe0)) {
        cout << "(g^1)(g^(n-1))(g^0) = g^0" << endl;
    } else {
        cout << "(g^1)(g^(n-1))(g^0) != g^0" << endl;
    }

}

void groupG2OperationsTest() {
    //Initialize LiDIA's global modulus
    LiDIA::bigint modulus;
    LibConversions::getModulus(modulus);
    LiDIA::bigmod::set_modulus(modulus);

    G2DCLXVI g2Generator;
    LiDIA::bigmod lidiaZero;
    LiDIA::bigmod lidiaOne(1);
    G2DCLXVI randomElement;
    randomElement.generateRandom();
    G2DCLXVI identity;
    identity.becomeIdentity();
    //g^0, should be identity
    G2DCLXVI gToThe0;
    ScalarDCLXVI scalar0(lidiaZero);
    g2Generator.doPower(scalar0, gToThe0);
    if(gToThe0.isEqual(identity)) {
        cout << "g^0 is the identity" << endl;
    } else {
        cout << "g^0 is not the identity" << endl;
    }
    //g^0 * g, should be g
    G2DCLXVI expectedGenerator;
    g2Generator.doMultiplication(gToThe0, expectedGenerator);
    if(expectedGenerator.isEqual(g2Generator)) {
        cout << "(g)(g^0) = g" << endl;
    } else {
        cout << "(g)(g^0) != g" << endl;
    }
    //(g^0)(g^0), should be g^0
    G2DCLXVI gToThe0Squared;
    gToThe0.doMultiplication(gToThe0, gToThe0Squared);
    if(gToThe0Squared.isEqual(gToThe0)) {
        cout << "(g^0)(g^0) = g^0" << endl;
    } else {
        cout << "(g^0)(g^0) != g^0" << endl;
    }
    //(g^0)(g^0) * g equals (g^0) * g?
    G2DCLXVI gDoubleGTo0;
    gToThe0Squared.doMultiplication(g2Generator, gDoubleGTo0);
    if(gDoubleGTo0.isEqual(expectedGenerator)) {
        cout << "(g^0)(g^0)(g) = (g^0)(g)" << endl;
    } else {
        cout << "(g^0)(g^0)(g) != (g^0)(g)" << endl;
    }
    //g^1, should be g
    G2DCLXVI gToThe1;
    ScalarDCLXVI scalar1(lidiaOne);
    g2Generator.doPower(scalar1, gToThe1);
    if(gToThe1.isEqual(g2Generator)) {
        cout << "g^1 = g" << endl;
    } else {
        cout << "g^1 != g" << endl;
    }
    //is g^1 = g^0?
    if(gToThe1.isEqual(gToThe0)) {
        cout << "g^1 = g^0" << endl;
    } else {
        cout << "g^1 != g^0" << endl;
    }
    //random^1, should be random
    G2DCLXVI randomTo1;
    randomElement.doPower(scalar1, randomTo1);
    if(randomTo1.isEqual(randomElement)) {
        cout << "random^1 = random" << endl;
    } else {
        cout << "random^1 != random" << endl;
    }
    //"identity" * g, should be g
    multiplyIdentityWith(g2Generator, "g");
    //identity^1, should be identity
    G2DCLXVI identityTo1;
    identity.doPower(scalar1, identityTo1);
    if(identityTo1.isEqual(identity)) {
        cout << "identity^1 = identity" << endl;
    } else {
        cout << "identity^1 != identity" << endl;
    }
    //identity^0, should be identity
    G2DCLXVI identityTo0;
    identity.doPower(scalar0, identityTo0);
    if(identityTo0.isEqual(identity)) {
        cout << "identity^0 = identity" << endl;
    } else {
        cout << "identity^0 != identity" << endl;
    }
    if(identityTo0.isEqual(gToThe0)) {
        cout << "identity^0 = g^0" << endl;
    } else {
        cout << "identity^0 != g^0" << endl;
    }
    //identity * identity, should be identity
    multiplyIdentityWith(identity, "identity");
    //identity * identity * identity, should still be identity
    G2DCLXVI identityTimesSelf;
    identity.doMultiplication(identity, identityTimesSelf);
    multiplyIdentityWith(identityTimesSelf, "identity * identity");
    G2DCLXVI identityCubed;
    identityTimesSelf.doMultiplication(identity, identityCubed);
    if(identityCubed.isEqual(identity)) {
        cout << "identity * identity * identity = identity" << endl;
    } else {
        cout << "identity * identity * identity != identity" << endl;
    }
    //(random value) * identity, should be that value

    multiplyIdentityWith(randomElement, "random");
    //(random value)^0, see if it equals g^0
    G2DCLXVI randomTo0;
    randomElement.doPower(scalar0, randomTo0);
    if(randomTo0.isEqual(gToThe0)) {
        cout << "random^0 = g^0" << endl;
    } else {
        cout << "random^0 != g^0" << endl;
    }
    //(g^1)(g^-1), see if it equals identity
    G2DCLXVI gToMinus1;
    LiDIA::bigmod lidiaMinus1(-1);
    ScalarDCLXVI scalarMinus1(lidiaMinus1);
    g2Generator.doPower(scalarMinus1, gToMinus1);
    G2DCLXVI identityFromNegative;
    gToThe1.doMultiplication(gToMinus1, identityFromNegative);
    if(identityFromNegative.isEqual(identity)) {
        cout << "(g^1)(g^-1) = identity" << endl;
    } else {
        cout << "(g^1)(g^-1) != identity" << endl;
    }
    //Also see if it equals g^0
    if(identityFromNegative.isEqual(gToThe0)) {
        cout << "(g^1)(g^-1) = g^0" << endl;
    } else {
        cout << "(g^1)(g^-1) != g^0" << endl;
    }
    //g^(n-1), see if it equals g^(-1)
    G2DCLXVI gToNMinus1;
    ScalarDCLXVI scalarNMinus1(modulus-1);
    g2Generator.doPower(scalarNMinus1, gToNMinus1);
    if(gToNMinus1.isEqual(gToMinus1)) {
        cout << "g^(n-1) = g^-1" << endl;
    } else {
        cout << "g^(n-1) != g^-1" << endl;
    }
    //(g^1)(g^(n-1)), see if it equals identity
    G2DCLXVI identityFromWraparound;
    gToThe1.doMultiplication(gToNMinus1, identityFromWraparound);
    if(identityFromWraparound.isEqual(identity)) {
        cout << "(g^1)(g^(n-1)) = identity" << endl;
    } else {
        cout << "(g^1)(g^(n-1)) != identity" << endl;
    }
    //see if it equals g^0 (it does)
    if(identityFromWraparound.isEqual(gToThe0)) {
        cout << "(g^1)(g^(n-1)) = g^0" << endl;
    } else {
        cout << "(g^1)(g^(n-1)) != g^0" << endl;
    }
    //(g^1)(g^(n-1))(g^0), should still equal g^0
    G2DCLXVI wraparoundTimesG0;
    identityFromWraparound.doMultiplication(gToThe0, wraparoundTimesG0);
    if(wraparoundTimesG0.isEqual(gToThe0)) {
        cout << "(g^1)(g^(n-1))(g^0) = g^0" << endl;
    } else {
        cout << "(g^1)(g^(n-1))(g^0) != g^0" << endl;
    }
}

void conversionTest() {
    LiDIA::bigint LiDIAnum;
    char* decimalString = "13292278995784915872913837968287348576";
    LiDIA::string_to_bigint(decimalString, LiDIAnum);
    cout << "Explicit decimal function input - number is: " << LiDIAnum << endl;
    cout << "Number in hex is: " << hex << LiDIAnum << endl;
    dec(cout);
    size_t byteLength = LiDIAnum.bit_length()/8;
    if(LiDIAnum.bit_length() % 8 != 0)
        byteLength++;
    unsigned char byteArr[byteLength];
    LibConversions::lidiaToBytes(LiDIAnum, byteArr);
    cout << "Number as byte array: ";
    testutils::print_hex(byteArr, byteLength);
    LiDIA::bigint numCopy;
    LibConversions::bytesToLiDIA(byteArr, byteLength, numCopy);
    cout << "Number converted back to LiDIA: " << numCopy << endl;

}

}

int main(int argc, char **argv) {
    cout << "Group G1 test: " << endl;
    libtest::groupOperationsTest();
    cout << endl << "Group G2 test: " << endl;
    libtest::groupG2OperationsTest();

    return 0;
}

