/*
 * ArithmeticException.h
 *
 *  Created on: Apr 19, 2013
 *      Author: etremel
 */

#ifndef ARITHMETICEXCEPTION_H_
#define ARITHMETICEXCEPTION_H_

#include <stdexcept>

namespace flint {

class ArithmeticException : public std::runtime_error {
public:
    explicit ArithmeticException(const std::string& what_msg)
            : std::runtime_error(what_msg) {}
};

}  // namespace flint

#endif /* ARITHMETICEXCEPTION_H_ */
