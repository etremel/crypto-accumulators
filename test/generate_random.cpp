/*
 * generate_random.cpp
 *
 *  Created on: Apr 23, 2013
 *      Author: etremel
 */

#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include <flint/BigInt.h>
#include <flint/BigMod.h>
#include <flint/Random.h>

#include <bilinear/Scalar_DCLXVI.h>

using namespace std;

namespace speedtest {
void saveBigInts(int setSize, string filename);
void saveScalars(int setSize, string filename);
}

int main(int argc, char** argv) {
    if(argc < 2) {
        cout << "Usage: generate_random <mode> <number of elements>" << endl;
        return 1;
    }
    string mode = string(argv[1]);
    int setSize;
    if(argc > 2) {
        setSize = atoi(argv[2]);
    } else {
        setSize = 1000;
    }
    if(mode == "bilinear") {
        speedtest::saveScalars(setSize, "randomScalars" + to_string(setSize));
    } else if(mode == "rsa") {
        speedtest::saveBigInts(setSize, "randomBigInts" + to_string(setSize));
    }
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

void saveBigInts(int setSize, string filename) {
    ofstream fileOut(filename);
    //Generate random elements to accumulate, of size 256 bits so they're the
    //same as the random elements for the bilinear test
    //(I'm pretty sure ScalarDCLXVI's randomize makes numbers up to 256 bits)
    flint::Random rng;
    for(int c = 0; c < setSize; c++) {
        fileOut << rng.nextInt(256).abs() << endl;
    }
}

}

