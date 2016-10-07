/*
 * BigInt.h
 *
 *  Created on: Apr 18, 2013
 *      Author: etremel
 */

#ifndef BIGINT_H_
#define BIGINT_H_

#include <string>
#include <iostream>

#include <flint/fmpz.h>

namespace flint {

class BigInt {
    public:
        /**
         * Constructs a BigInt initialized to 0.
         */
        BigInt();
        ~BigInt();
        /**
         * Copy constructor.
         * @param other The BigInt to copy into this one.
         */
        BigInt(const BigInt& other);
        /**
         * Move constructor; takes ownership of other's value.
         * @param other The BigInt to move into this one.
         */
        BigInt(BigInt&& other);
        /**
         * Constructs a BigInt initialized to the value of the long.
         * @param value the long integer value that the BigInt should have.
         */
        BigInt(const long value);
        /**
         * Constructs a BigInt to wrap the given fmpz value. This copies the
         * value of the given fmpz, so changes to the resulting BigInt will not
         * modify the fmpz used to initialize it.
         * @param fmpz The fmpz_t that should be used to initialize the BigInt
         */
        BigInt(const fmpz_t& fmpz);
        /**
         * Move-constructs a BigInt from an fmpz value. This takes ownership of
         * the given fmpz and frees it, so the parameter will no longer be a
         * valid fmpz after initializing the BigInt. Note that this violates the
         * standard move-constructor contract that the parameter be left in a
         * valid state, but it's the only way to allow move-constructing from
         * an fmpz without leaking the fmpz (since temporary fmpzs are not
         * automatically freed).
         *
         * @param fmpz The fmpz_t that should be used to initialize the BigInt
         */
        BigInt(fmpz_t&& fmpz);

        /**
         * Constructs a BigInt from a string representation of a number in a
         * given base. The string is a C-style string because that's what is
         * required by the FLINT library.
         * @param string A null-terminated string representing a number
         * @param base The base (radix) of the number represented in the string
         */
        BigInt(const char* string, const int base);

        /** @return true if this has the same value as other, false otherwise */
        bool equals(const BigInt& other) const;
        //Mutative arithmetic methods. Return this to allow for chaining.
        /** Adds another BigInt to this one */
        BigInt& add(const BigInt& rhs);
        /** Subtracts another BigInt from this one */
        BigInt& subtract(const BigInt& rhs);
        /** Multiplies this BigInt by another BigInt */
        BigInt& multiply(const BigInt& rhs);
        /** Divides this BigInt by another BigInt, rounding down */
        BigInt& divide(const BigInt& rhs);
        /** Raises this BigInt to an unsigned long exponent
         * (this operation isn't supported for BigInts)*/
        BigInt& power(const unsigned long exponent);
        /** Sets this BigInt to its absolute value */
        BigInt& abs();

        /**
         * Sets this BigInt's value to the base-10 number represented by the
         * given C-string.
         *
         * @param string A C-string representing a base-10 number.
         * @return true if the assignment was successful, false if the string
         *         did not contain valid input
         */
        bool assign(const char* string);
        /**
         * Sets this BigInt's value to the base-10 number represented by the
         * given string.
         *
         * @param string A string containing a base-10 representation of a number
         * @return true if the assignment was successful, false if the string
         *         did not contain valid input
         */
        bool assign(const std::string& string);

        /** @return a string containing the base-10 representation of this BigInt */
        std::string toString() const;
        /** @return a string containing the base-16 representation of this BigInt */
        std::string toHex() const;
        /** @return the number of bits needed to represent this BigInt */
        unsigned long bitLength() const;

        /**
         * Finds and returns the smallest probable prime greater than this
         * BigInt. Important: Assumes that FLINT was shipped/linked with GMP,
         * rather than MPIR, since FLINT doesn't provide its own next-prime
         * function.
         * @return A BigInt containing a (likely) prime number at least as
         * large as this BigInt.
         */
        BigInt nextPrime() const;

        //Unified assignment operator
        BigInt& operator=(BigInt rhs);
        //Assignment from string
        BigInt& operator=(const std::string rhs);
        //Arithmetic operator overloading...
        BigInt& operator+=(const BigInt& rhs);
        BigInt& operator-=(const BigInt& rhs);
        BigInt& operator*=(const BigInt& rhs);
        BigInt& operator/=(const BigInt& rhs);
        BigInt& operator^=(const unsigned long rhs);

        bool operator==(const BigInt& rhs) const;
        bool operator!=(const BigInt& rhs) const;
        bool operator<(const BigInt& rhs) const;
        bool operator>(const BigInt& rhs) const;
        bool operator<=(const BigInt& rhs) const;
        bool operator>=(const BigInt& rhs) const;
        //Bit shifts
        BigInt& operator<<=(const unsigned long numBits);
        BigInt& operator>>=(const unsigned long numBits);
        const BigInt operator<<(const unsigned long numBits) const;
        const BigInt operator>>(const unsigned long numBits) const;

        const fmpz* getUnderlyingObject() const;

    private:
        fmpz_t value;
        friend void swap(BigInt& first, BigInt& second) noexcept;
        friend void add(const BigInt& lhs, const BigInt& rhs, BigInt& result);
        friend void subtract(const BigInt& lhs, const BigInt& rhs, BigInt& result);
        friend void multiply(const BigInt& lhs, const BigInt& rhs, BigInt& result);
        friend void divide(const BigInt& lhs, const BigInt& rhs, BigInt& result);
        friend void power(const BigInt& base, const unsigned long exponent, BigInt& result);
        friend void abs(const BigInt& input, BigInt& output);
};


BigInt operator+(const BigInt& lhs, const BigInt& rhs);
BigInt operator-(const BigInt& lhs, const BigInt& rhs);
BigInt operator*(const BigInt& lhs, const BigInt& rhs);
BigInt operator/(const BigInt& lhs, const BigInt& rhs);
BigInt operator^(const BigInt& base, const unsigned long exp);

/**
 * Swaps two BigInts efficiently, by swapping pointers.
 * @param first The first BigInt, which will have the value of second after
 *        calling this function.
 * @param second The second BigInt, which will have the value of first after
 *        calling this function.
 */
void swap(BigInt& first, BigInt& second) noexcept;

/** Sets result to lhs + rhs */
void add(const BigInt& lhs, const BigInt& rhs, BigInt& result);
/** Sets result to lhs - rhs */
void subtract(const BigInt& lhs, const BigInt& rhs, BigInt& result);
/** Sets result to lhs * rhs */
void multiply(const BigInt& lhs, const BigInt& rhs, BigInt& result);
/** Sets result to lhs / rhs, rounded down */
void divide(const BigInt& lhs, const BigInt& rhs, BigInt& result);
/** Sets result to base ^ exponent */
void power(const BigInt& base, const unsigned long exponent, BigInt& result);
/** Sets output to the absolute value of input */
void abs(const BigInt& input, BigInt& output);

} //end namespace flint

//Global namespace
std::ostream& operator<<(std::ostream& os, const flint::BigInt& obj);
std::istream& operator>>(std::istream& is, flint::BigInt& obj);

#endif /* BIGINT_H_ */
