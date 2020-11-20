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

namespace testutils {

//Dumps an array of bytes to cout as hex, followed by an endline (signed char version)
inline void print_hex(const char* arr, const int size){
    for (int i = 0; i < size; ++i)
        std::cout << std::hex << static_cast<int>(static_cast<unsigned char>(arr[i]));
    std::cout << std::endl;
    std::dec(std::cout);
}
//Dumps an array of bytes to cout as hex, followed by an endline
inline void print_hex(const unsigned char* arr, const int size){
    for (int i = 0; i < size; ++i)
        std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(arr[i]);
    std::cout << std::endl;
    std::dec(std::cout);
}

}



#endif /* TESTUTILS_H_ */
