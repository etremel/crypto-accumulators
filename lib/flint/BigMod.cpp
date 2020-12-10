/*
 * BigMod.cpp
 *
 *  Created on: Apr 18, 2013
 *      Author: etremel
 */

#include <sstream>

#include <flint/ArithmeticException.hpp>
#include <flint/BigMod.hpp>

namespace flint {

BigMod::BigMod() {
    fmpz_init(value);
    fmpz_init(modulus);
}

BigMod::BigMod(const BigMod& other) {
    fmpz_init_set(value, other.value);
    fmpz_init_set(modulus, other.modulus);
}

BigMod::BigMod(BigMod&& other) {
    fmpz_init(value);
    fmpz_init(modulus);
    //Steal other's values and leave it with initialized but useless ones
    swap(*this, other);
}

BigMod::BigMod(const BigInt& modulus) {
    fmpz_init(value);
    fmpz_init_set(this->modulus, modulus.getUnderlyingObject());
}

BigMod::BigMod(const fmpz_t& value, const fmpz_t& modulus) {
    fmpz_init_set(this->value, value);
    fmpz_init_set(this->modulus, modulus);
    fmpz_mod(this->value, this->value, this->modulus);
}

BigMod::BigMod(const BigInt& value, const BigInt& modulus) {
    fmpz_init_set(this->value, value.getUnderlyingObject());
    fmpz_init_set(this->modulus, modulus.getUnderlyingObject());
    fmpz_mod(this->value, this->value, this->modulus);
}

BigMod::BigMod(const unsigned long value, const unsigned long modulus) {
    fmpz_init_set_ui(this->value, value);
    fmpz_init_set_ui(this->modulus, modulus);
    fmpz_mod(this->value, this->value, this->modulus);
}

BigMod::~BigMod() {
    fmpz_clear(value);
    fmpz_clear(modulus);
}

BigMod& BigMod::add(const BigMod& rhs) {
    flint::add(*this, rhs, *this);
    return *this;
}
BigMod& BigMod::subtract(const BigMod& rhs) {
    flint::subtract(*this, rhs, *this);
    return *this;
}
BigMod& BigMod::multiply(const BigMod& rhs) {
    flint::multiply(*this, rhs, *this);
    return *this;
}
BigMod& BigMod::add(const BigInt& rhs) {
    flint::add(*this, rhs, *this);
    return *this;
}
BigMod& BigMod::subtract(const BigInt& rhs) {
    flint::subtract(*this, rhs, *this);
    return *this;
}
BigMod& BigMod::multiply(const BigInt& rhs) {
    flint::multiply(*this, rhs, *this);
    return *this;
}

BigMod& BigMod::power(const BigInt& exponent) {
    flint::power(*this, exponent, *this);
    return *this;
}

bool BigMod::equals(const BigMod& other) const {
    return fmpz_equal(modulus, other.modulus)
           && fmpz_equal(value, other.value);
}

bool BigMod::assign(const char* string) {
    int success = fmpz_set_str(value, const_cast<char*>(string), 10);
    fmpz_mod(value, value, modulus);
    return (success == 0);
}

bool BigMod::assign(const std::string& string) {
    return assign(string.c_str());
}

BigMod& BigMod::operator=(BigMod rhs) {
    swap(*this, rhs);
    return *this;
}

BigMod& BigMod::operator=(const BigInt& rhs) {
    fmpz_set(value, rhs.getUnderlyingObject());
    return *this;
}

BigMod& BigMod::operator=(const long& rhs) {
    fmpz_set_si(value, rhs);
    fmpz_mod(value, value, modulus);
    return *this;
}

BigMod& BigMod::operator+=(const BigMod& rhs) {
    this->add(rhs);
    return *this;
}

BigMod& BigMod::operator-=(const BigMod& rhs) {
    this->subtract(rhs);
    return *this;
}

BigMod& BigMod::operator*=(const BigMod& rhs) {
    this->multiply(rhs);
    return *this;
}

BigMod& BigMod::operator+=(const BigInt& rhs) {
    this->add(rhs);
    return *this;
}

BigMod& BigMod::operator-=(const BigInt& rhs) {
    this->subtract(rhs);
    return *this;
}

BigMod& BigMod::operator*=(const BigInt& rhs) {
    this->multiply(rhs);
    return *this;
}

BigMod& BigMod::operator^=(const BigInt& exp) {
    this->power(exp);
    return *this;
}

bool BigMod::operator==(const BigMod& rhs) const {
    return this->equals(rhs);
}

bool BigMod::operator!=(const BigMod& rhs) const {
    return !operator==(rhs);
}

bool BigMod::operator<(const BigMod& rhs) const {
    if(!fmpz_equal(modulus, rhs.modulus))
        throw ArithmeticException("Cannot compare operands with different moduli.");
    return fmpz_cmp(this->value, rhs.value) < 0;
}

bool BigMod::operator>(const BigMod& rhs) const {
    if(!fmpz_equal(modulus, rhs.modulus))
        throw ArithmeticException("Cannot compare operands with different moduli.");
    return fmpz_cmp(this->value, rhs.value) > 0;
}

bool BigMod::operator>=(const BigMod& rhs) const {
    return !operator<(rhs);
}

bool BigMod::operator<=(const BigMod& rhs) const {
    return !operator>(rhs);
}

std::string BigMod::toString() const {
    return std::string(fmpz_get_str(nullptr, 10, value));
}

BigInt BigMod::getMantissa() const {
    return BigInt(value);
}

BigInt BigMod::getModulus() const {
    return BigInt(modulus);
}

void BigMod::setModulus(const BigInt& modulus) {
    fmpz_set(this->modulus, modulus.getUnderlyingObject());
    fmpz_mod(value, value, this->modulus);
}

//const fmpz* BigMod::getUnderlyingValue() const {
//    return value;
//}
//
//const fmpz* BigMod::getUnderlyingModulus() const {
//    return modulus;
//}

/* -------------------------- End class BigMod ------------------------------ */

BigMod operator+(const BigMod& lhs, const BigMod& rhs) {
    return BigMod(lhs) += rhs;
}

BigMod operator-(const BigMod& lhs, const BigMod& rhs) {
    return BigMod(lhs) -= rhs;
}

BigMod operator*(const BigMod& lhs, const BigMod& rhs) {
    return BigMod(lhs) *= rhs;
}

BigMod operator^(const BigMod& base, const BigInt& exp) {
    return BigMod(base) ^= exp;
}

BigMod operator+(const BigMod& lhs, const BigInt& rhs) {
    return BigMod(lhs) += rhs;
}

BigMod operator-(const BigMod& lhs, const BigInt& rhs) {
    return BigMod(lhs) -= rhs;
}

BigMod operator*(const BigMod& lhs, const BigInt& rhs) {
    return BigMod(lhs) *= rhs;
}

BigMod operator+(const BigInt& lhs, const BigMod& rhs) {
    return BigMod(rhs) += lhs;
}

BigMod operator-(const BigInt& lhs, const BigMod& rhs) {
    BigMod result;
    subtract(lhs, rhs, result);
    return result;
}

BigMod operator*(const BigInt& lhs, const BigMod& rhs) {
    BigMod result;
    multiply(lhs, rhs, result);
    return result;
}

void swap(BigMod& first, BigMod& second) {
    //Just swap both fmpz members using FLINT's swap
    fmpz_swap(first.value, second.value);
    fmpz_swap(first.modulus, second.modulus);
}

void add(const BigMod& lhs, const BigMod& rhs, BigMod& result) {
    if(!fmpz_equal(lhs.modulus, rhs.modulus))
        throw ArithmeticException("Addition error: operands have different moduli.");
    fmpz_add(result.value, lhs.value, rhs.value);
    fmpz_mod(result.value, result.value, lhs.modulus);
    fmpz_set(result.modulus, lhs.modulus);
}

void subtract(const BigMod& lhs, const BigMod& rhs, BigMod& result) {
    if(!fmpz_equal(lhs.modulus, rhs.modulus))
        throw ArithmeticException("Subtraction error: operands have different moduli.");
    fmpz_sub(result.value, lhs.value, rhs.value);
    fmpz_mod(result.value, result.value, lhs.modulus);
    fmpz_set(result.modulus, lhs.modulus);
}

void multiply(const BigMod& lhs, const BigMod& rhs, BigMod& result) {
    if(!fmpz_equal(lhs.modulus, rhs.modulus))
        throw ArithmeticException("Multiplication error: operands have different moduli.");
    fmpz_mul(result.value, lhs.value, rhs.value);
    fmpz_mod(result.value, result.value, lhs.modulus);
    fmpz_set(result.modulus, lhs.modulus);
}

void add(const BigMod& lhs, const BigInt& rhs, BigMod& result) {
    fmpz_add(result.value, lhs.value, rhs.getUnderlyingObject());
    fmpz_mod(result.value, result.value, lhs.modulus);
    fmpz_set(result.modulus, lhs.modulus);
}

void add(const BigInt& lhs, const BigMod& rhs, BigMod& result) {
    add(rhs, lhs, result);
}

void subtract(const BigMod& lhs, const BigInt& rhs, BigMod& result) {
    fmpz_sub(result.value, lhs.value, rhs.getUnderlyingObject());
    fmpz_mod(result.value, result.value, lhs.modulus);
    fmpz_set(result.modulus, lhs.modulus);
}

void subtract(const BigInt& lhs, const BigMod& rhs, BigMod& result) {
    fmpz_sub(result.value, lhs.getUnderlyingObject(), rhs.value);
    fmpz_mod(result.value, result.value, rhs.modulus);
    fmpz_set(result.modulus, rhs.modulus);
}

void multiply(const BigMod& lhs, const BigInt& rhs, BigMod& result) {
    fmpz_mul(result.value, lhs.value, rhs.getUnderlyingObject());
    fmpz_mod(result.value, result.value, lhs.modulus);
    fmpz_set(result.modulus, lhs.modulus);
}

void multiply(const BigInt& lhs, const BigMod& rhs, BigMod& result) {
    multiply(rhs, lhs, result);
}

void power(const BigMod& base, const BigInt& exponent, BigMod& result) {
    fmpz_powm(result.value, base.value, exponent.getUnderlyingObject(), base.modulus);
    fmpz_set(result.modulus, base.modulus);
}

}  //end namespace flint

std::ostream& operator<<(std::ostream& os, const flint::BigMod& obj) {
    os << obj.toString();
    return os;
}

std::istream& operator>>(std::istream& is, flint::BigMod& obj) {
    std::string inString;
    is >> inString;
    if(!obj.assign(inString))
        is.setstate(std::ios::failbit);
    return is;
}
