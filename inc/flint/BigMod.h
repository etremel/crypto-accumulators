/*
 * BigMod.h
 *
 *  Created on: Apr 18, 2013
 *      Author: etremel
 */

#ifndef BIGMOD_H_
#define BIGMOD_H_

#include <string>
#include <iostream>

#include <flint/fmpz.h>
#include <flint/BigInt.h>

namespace flint {

class BigMod {
    public:
        /**
         * Constructs a BigMod with a value and modulus of 0. This is usually
         * useless, since with a modulus of 0 it can't have a nonzero value.
         */
        BigMod();
        ~BigMod();
        /**
         * Copy constructor.
         * @param other The BigMod to copy into this one.
         */
        BigMod(const BigMod& other);
        /**
         * Move constructor; takes ownership of other's value.
         * @param other The BigMod to move into this one.
         */
        BigMod(BigMod&& other);
        /**
         * Constructs a BigMod with a value of 0 and the given modulus
         * @param modulus A BigInt whose value should become the modulus
         */
        BigMod(const BigInt& modulus);
        /**
         * Constructs a BigMod with a value and modulus from BigInts. If value >
         * modulus, it will be reduced modulo modulus.
         * @param value A BigInt whose value should become the BigMod's value
         * @param modulus A BigInt whose value should become the BigMod's modulus
         */
        BigMod(const BigInt& value, const BigInt& modulus);
        /**
         * Constructs a BigMod to wrap the given fmpz values. This copies in the
         * values from the fmpz_ts, so changes to the resulting BigMod will not
         * change the objects used to initialize it.
         * @param value An fmpz_t representing the value (mantissa) of the BigMod
         * @param modulus An fmpz_t representing the modulus of the BigMod
         */
        BigMod(const fmpz_t& value, const fmpz_t& modulus);
        /**
         * Constructs a BigMod with a value and modulus from unsigned longs.
         * If value > modulus, it will be reduced modulo modulus.
         * @param value An unsigned long whose value should become the BigMod's
         *        value
         * @param modulus An unsigned long whose value should become the BigMod's
         *        modulus
         */
        BigMod(const unsigned long value, const unsigned long modulus);

        /**
         * Returns the mantissa (residue value) as a BigInt. Note that this
         * involves constructing a new object because the mantissa is not
         * stored internally as a BigInt.
         * @return a BigInt containing a copy of the mantissa value.
         */
        BigInt getMantissa() const;
        /**
         * Returns the modulus as a BigInt. Note that this involves
         * constructing a new object because the modulus is not stored
         * internally as a BigInt.
         * @return a BigInt containing a copy of the modulus value.
         */
        BigInt getModulus() const;

        /**
         * Sets the modulus to the value of the given BigInt, then reduces the
         * mantissa modulo the new modulus.
         * @param modulus a BigInt containing the desired modulus value.
         */
        void setModulus(const BigInt& modulus);

        //Mutative arithmetic methods. Return this to allow for chaining.
        /** Adds another BigMod to this one */
        BigMod& add(const BigMod& rhs);
        /** Subtracts another BigMod from this one */
        BigMod& subtract(const BigMod& rhs);
        /** Multiplies this BigMod by another BigMod */
        BigMod& multiply(const BigMod& rhs);
        //BigInt versions
        /** Adds a BigInt to this BigMod, reducing by the BigMod's modulus */
        BigMod& add(const BigInt& rhs);
        /** Subtracts a BigInt from this BigMod, reducing by the BigMod's modulus */
        BigMod& subtract(const BigInt& rhs);
        /** Multiplies this BigMod by a BigInt, reducing by the BigMod's modulus */
        BigMod& multiply(const BigInt& rhs);

        /** Raises this BigMod to the given exponent, which must be a BigInt
         *  (because if it was a BigMod it would need to have modulus phi(m)) */
        BigMod& power(const BigInt& exponent);

        /** @return true if this is equal to other and they have the same moduli */
        bool equals(const BigMod& other) const;
        /**
         * Sets this BigMod's value to the base-10 number represented by the
         * C-string, modulo this BigMod's existing modulus.
         *
         * @param string A C-string representing a base-10 number
         * @return true if the assignment was successful, false if the string
         *         did not contain valid input
         */
        bool assign(const char* string);
        /**
         * Sets this BigMod's value to the base-10 number represented by the
         * string, modulo this BigMod's existing modulus.
         *
         * @param string A string representing a base-10 number
         * @return true if the assignment was successful, false if the string
         *         did not contain valid input
         */
        bool assign(const std::string& string);

        /** @return a string containing the base-10 representation of the mantissa of this BigMod */
        std::string toString() const;

        //Unified assignment operator
        BigMod& operator=(BigMod rhs);
        //Conversion assignment
        BigMod& operator=(const BigInt& rhs);
        BigMod& operator=(const long& rhs);
        //Mutative arithmetic operators
        BigMod& operator+=(const BigMod& rhs);
        BigMod& operator-=(const BigMod& rhs);
        BigMod& operator*=(const BigMod& rhs);

        BigMod& operator+=(const BigInt& rhs);
        BigMod& operator-=(const BigInt& rhs);
        BigMod& operator*=(const BigInt& rhs);
        BigMod& operator^=(const BigInt& exp);

        bool operator==(const BigMod& rhs) const;
        bool operator!=(const BigMod& rhs) const;
        bool operator<(const BigMod& rhs) const;
        bool operator>(const BigMod& rhs) const;
        bool operator<=(const BigMod& rhs) const;
        bool operator>=(const BigMod& rhs) const;
//
//        const fmpz* getUnderlyingValue() const;
//        const fmpz* getUnderlyingModulus() const;

    private:
        fmpz_t value;
        fmpz_t modulus;

        friend void swap(BigMod& first, BigMod& second);

        friend void add(const BigMod& lhs, const BigMod& rhs, BigMod& result);
        friend void subtract(const BigMod& lhs, const BigMod& rhs, BigMod& result);
        friend void multiply(const BigMod& lhs, const BigMod& rhs, BigMod& result);

        friend void add(const BigMod& lhs, const BigInt& rhs, BigMod& result);
        friend void subtract(const BigMod& lhs, const BigInt& rhs, BigMod& result);
        friend void multiply(const BigMod& lhs, const BigInt& rhs, BigMod& result);

        friend void add(const BigInt& lhs, const BigMod& rhs, BigMod& result);
        friend void subtract(const BigInt& lhs, const BigMod& rhs, BigMod& result);
        friend void multiply(const BigInt& lhs, const BigMod& rhs, BigMod& result);

        friend void power(const BigMod& base, const BigInt& exponent, BigMod& result);

};

//Generative arithmetic operators as non-class members
BigMod operator+(const BigMod& lhs, const BigMod& rhs);
BigMod operator-(const BigMod& lhs, const BigMod& rhs);
BigMod operator*(const BigMod& lhs, const BigMod& rhs);
BigMod operator^(const BigMod& base, const BigInt& exp);
//Interoperability with BigInt arithemetic operators
BigMod operator+(const BigMod& lhs, const BigInt& rhs);
BigMod operator-(const BigMod& lhs, const BigInt& rhs);
BigMod operator*(const BigMod& lhs, const BigInt& rhs);
BigMod operator+(const BigInt& lhs, const BigMod& rhs);
BigMod operator-(const BigInt& lhs, const BigMod& rhs);
BigMod operator*(const BigInt& lhs, const BigMod& rhs);
/**
 * Swaps two BigMods efficiently, by swapping pointers.
 * @param first The first BigMod, which will have the value of second after
 *        calling this function.
 * @param second The second BigMod, which will have the value of first after
 *        calling this function.
 */
void swap(BigMod& first, BigMod& second);
/** Sets result to lhs + rhs */
void add(const BigMod& lhs, const BigMod& rhs, BigMod& result);
/** Sets result to lhs - rhs */
void subtract(const BigMod& lhs, const BigMod& rhs, BigMod& result);
/** Sets result to lhs * rhs */
void multiply(const BigMod& lhs, const BigMod& rhs, BigMod& result);
//BigInt versions - the result is assumed to take the modulus of the BigMod operand
/** Sets result to lhs + rhs, with lhs's modulus */
void add(const BigMod& lhs, const BigInt& rhs, BigMod& result);
/** Sets result to lhs + rhs, with rhs's modulus */
void add(const BigInt& lhs, const BigMod& rhs, BigMod& result);
/** Sets result to lhs - rhs, with lhs's modulus */
void subtract(const BigMod& lhs, const BigInt& rhs, BigMod& result);
/** Sets result to lhs - rhs, with rhs's modulus */
void subtract(const BigInt& lhs, const BigMod& rhs, BigMod& result);
/** Sets result to lhs * rhs, with lhs's modulus */
void multiply(const BigMod& lhs, const BigInt& rhs, BigMod& result);
/** Sets result to lhs * rhs, with rhs's modulus */
void multiply(const BigInt& lhs, const BigMod& rhs, BigMod& result);

/** Sets result to base ^ exponent. Note that exponent must be a BigInt, since
 *  if it were a BigMod we would need to check that it had modulus phi(m) */
void power(const BigMod& base, const BigInt& exponent, BigMod& result);

} //end namespace flint

std::ostream& operator<<(std::ostream& os, const flint::BigMod& obj);
std::istream& operator>>(std::istream& is, flint::BigMod& obj);

#endif /* BIGMOD_H_ */
