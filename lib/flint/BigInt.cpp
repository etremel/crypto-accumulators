/*
 * BigInt.cpp
 *
 *  Created on: Apr 19, 2013
 *      Author: etremel
 */

#include <sstream>
#include <string>
#include <time.h>
#include <cstdio>
#include <flint/fmpz.h>

#include "flint/BigInt.h"

using std::string;

namespace flint {

BigInt::BigInt() {
    fmpz_init(value);
}

BigInt::BigInt(const long value) {
    fmpz_init(this->value);
    fmpz_set_si(this->value, value);
}

BigInt::BigInt(const fmpz_t& fmpz) {
    fmpz_init_set(value, fmpz);
}

BigInt::BigInt(fmpz_t&& fmpz) {
    fmpz_init_set(value, fmpz);
    fmpz_clear(fmpz);
}


BigInt::BigInt(const BigInt& other) {
    fmpz_init_set(value, other.value);
}

BigInt::BigInt(BigInt&& other) : BigInt() {
    //Default-construct this, then swap it with other to steal other's value
    swap(*this, other);
}

BigInt::BigInt(const char* string, const int base) {
    fmpz_init(value);
    //why doesn't this function take const char*? it's not going to modify the string
    fmpz_set_str(value, const_cast<char*>(string), base);
}

BigInt::~BigInt() {
    fmpz_clear(value);
}

BigInt& BigInt::add(const BigInt& rhs) {
    flint::add(*this, rhs, *this);
    return *this;
}

BigInt& BigInt::subtract(const BigInt& rhs) {
    flint::subtract(*this, rhs, *this);
    return *this;
}

BigInt& BigInt::multiply(const BigInt& rhs) {
    flint::multiply(*this, rhs, *this);
    return *this;
}

BigInt& BigInt::divide(const BigInt& rhs) {
    flint::divide(*this, rhs, *this);
    return *this;
}

BigInt& BigInt::power(const unsigned long exponent) {
    flint::power(*this, exponent, *this);
    return *this;
}

BigInt& BigInt::abs() {
    flint::abs(*this, *this);
    return *this;
}

BigInt& BigInt::operator=(BigInt rhs) {
    //Copy-and-swap algorithm
    swap(*this, rhs);
    return *this;
}

BigInt& BigInt::operator=(const std::string string) {
    assign(string);
    return *this;
}

BigInt& BigInt::operator+=(const BigInt& rhs) {
    this->add(rhs);
    return *this;
}

BigInt& BigInt::operator-=(const BigInt& rhs) {
    this->subtract(rhs);
    return *this;
}

BigInt& BigInt::operator*=(const BigInt& rhs) {
    this->multiply(rhs);
    return *this;
}

BigInt& BigInt::operator/=(const BigInt& rhs) {
    this->divide(rhs);
    return *this;
}

BigInt& BigInt::operator^=(const unsigned long exp) {
    this->power(exp);
    return *this;
}

BigInt& BigInt::operator<<=(const unsigned long numBits) {
    fmpz_mul_2exp(this->value, this->value, numBits);
    return *this;
}

BigInt& BigInt::operator>>=(const unsigned long numBits) {
    fmpz_fdiv_q_2exp(this->value, this->value, numBits);
    return *this;
}

const BigInt BigInt::operator<<(const unsigned long numBits) const {
    return BigInt(*this) <<= numBits;
}
const BigInt BigInt::operator>>(const unsigned long numBits) const {
    return BigInt(*this) >>= numBits;
}

bool BigInt::operator==(const BigInt& rhs) const {
    return this->equals(rhs);
}

bool BigInt::operator!=(const BigInt& rhs) const {
    return !operator==(rhs);
}

bool BigInt::operator<(const BigInt& rhs) const {
    return fmpz_cmp(this->value, rhs.value) < 0;
}

bool BigInt::operator>(const BigInt& rhs) const {
    return fmpz_cmp(this->value, rhs.value) > 0;
}

bool BigInt::operator>=(const BigInt& rhs) const {
    return !operator<(rhs);
}

bool BigInt::operator<=(const BigInt& rhs) const {
    return !operator>(rhs);
}

bool BigInt::equals(const BigInt& other) const {
    return fmpz_equal(value, other.value);
}

bool BigInt::assign(const char* string) {
    //fmpz_set_str returns 0 on success, but 0 = FALSE in C
    //Also, it does not modify its input string, but it doesn't declare it const
    return(!fmpz_set_str(value, const_cast<char*>(string), 10));
}

bool BigInt::assign(const std::string& string) {
    return assign(string.c_str());
}

string BigInt::toString() const {
    char* cString = fmpz_get_str(nullptr, 10, value);
    string str(cString);
    delete[] cString;
    return str;
}

string BigInt::toHex() const {
    char* cString = fmpz_get_str(nullptr, 16, value);
    string str(cString);
    delete[] cString;
    return str;
}

//I happen to know that mp_bitcnt_t is unsigned long, and I want to hide the library dependency from clients
unsigned long BigInt::bitLength() const {
    return fmpz_bits(value);
}

BigInt BigInt::nextPrime() const {
    //FLINT doesn't provide "next probable prime," but GMP does
    //This requires some boilerplate to convert between GMP values
    //("mpz_t") and FLINT values ("fmpz_t").
    mpz_t mpzValue;
    mpz_init(mpzValue);
    fmpz_get_mpz(mpzValue, value);
    mpz_t mpzResult;
    mpz_init(mpzResult);

    //If FLINT includes MPIR rather than GMP, this will fail because
    //MPIR calls this function "mpz_next_likely_prime"
    mpz_nextprime(mpzResult, mpzValue);

    fmpz_t fmpzResult;
    fmpz_init(fmpzResult);
    fmpz_set_mpz(fmpzResult, mpzResult);
    mpz_clear(mpzValue);
    mpz_clear(mpzResult);

    return BigInt(std::move(fmpzResult));
}

const fmpz* BigInt::getUnderlyingObject() const {
    return value;
}

/* -------------------------- End class BigInt ------------------------------ */

BigInt operator+(const BigInt& lhs, const BigInt& rhs) {
    return BigInt(lhs) += rhs;
}

BigInt operator-(const BigInt& lhs, const BigInt& rhs) {
    return BigInt(lhs) -= rhs;
}

BigInt operator*(const BigInt& lhs, const BigInt& rhs) {
    return BigInt(lhs) *= rhs;
}

BigInt operator/(const BigInt& lhs, const BigInt& rhs) {
    return BigInt(lhs) /= rhs;
}

BigInt operator^(const BigInt& base, const unsigned long exp) {
    return BigInt(base) ^= exp;
}

//Trivial because FLINT already implemented swap for fmpz's
void swap(BigInt& first, BigInt& second) noexcept {
    fmpz_swap(first.value, second.value);
}

void add(const BigInt& lhs, const BigInt& rhs, BigInt& result) {
    fmpz_add(result.value, lhs.value, rhs.value);
}

void subtract(const BigInt& lhs, const BigInt& rhs, BigInt& result) {
    fmpz_sub(result.value, lhs.value, rhs.value);
}

void multiply(const BigInt& lhs, const BigInt& rhs, BigInt& result) {
    fmpz_mul(result.value, lhs.value, rhs.value);
}

void divide(const BigInt& lhs, const BigInt& rhs, BigInt& result) {
    fmpz_fdiv_q(result.value, lhs.value, rhs.value);
}

void power(const BigInt& base, const unsigned long exponent, BigInt& result) {
    fmpz_pow_ui(result.value, base.value, exponent);
}

void abs(const BigInt& input, BigInt& output) {
    fmpz_abs(output.value, input.value);
}

} //end namespace flint

std::ostream& operator<<(std::ostream& os, const flint::BigInt& obj) {
  os << obj.toString();
  return os;
}

std::istream& operator>>(std::istream& is, flint::BigInt& obj) {
  std::string word;
  is >> word;
  if(!obj.assign(word))
    is.setstate(std::ios::failbit);
  return is;
}

