/*
 * testutils.h
 *
 *  Created on: Mar 28, 2013
 *      Author: etremel
 */

#ifndef TESTUTILS_H_
#define TESTUTILS_H_

#include <iostream>
#include <iomanip>

using std::cout;
using std::endl;

namespace testutils {

//Dumps an array of bytes to cout as hex, followed by an endline (signed char version)
inline void print_hex(const char* arr, const int size){
    for (int i = 0; i < size; ++i)
        cout << std::hex << static_cast<int>(static_cast<unsigned char>(arr[i]));
    cout << endl;
    std::dec(cout);
}
//Dumps an array of bytes to cout as hex, followed by an endline
inline void print_hex(const unsigned char* arr, const int size){
    for (int i = 0; i < size; ++i)
        cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(arr[i]);
    cout << endl;
    std::dec(cout);
}

}



#endif /* TESTUTILS_H_ */
