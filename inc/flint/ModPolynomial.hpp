/*
 * ModPolynomial.h
 *
 *  Created on: Apr 19, 2013
 *      Author: etremel
 */

#ifndef MODPOLYNOMIAL_H_
#define MODPOLYNOMIAL_H_

#include <iostream>

#include <flint/fmpz_mod_poly.h>

#include <flint/BigInt.hpp>
#include <flint/BigMod.hpp>

namespace flint {

class ModPolynomial {
public:
    /**
     * Copy constructor.
     * @param other The ModPolynomial whose value should be copied
     */
    ModPolynomial(const ModPolynomial& other);
    /**
     * Move constructor; takes ownership of the mod_poly in other and leaves
     * other with a default-initialized mod_poly.
     * @param other The ModPolynomial to move into this one.
     */
    ModPolynomial(ModPolynomial&& other);
    /**
     * Constructs a ModPolynomial initialized to 0, with the given BigInt
     * as its modulus.
     * @param modulus The value to use for the polynomial's modulus
     */
    ModPolynomial(const BigInt& modulus);
    /**
     * Constructs a ModPolynomial initialized to the given constant, with
     * the given BigInt as its modulus.
     * @param constantValue The value to use for the polynomial's constant
     *        term
     * @param modulus The value to use for the polynomial's modulus
     */
    ModPolynomial(const BigInt& constantValue, const BigInt& modulus);
    ~ModPolynomial();

    /** @return the degree of the polynomial */
    long getDegree() const;
    /** @return a BigInt copy of the polynomial's modulus */
    BigInt getModulus() const;
    /** @return a BigMod containing the value of the ith coefficient, with
     *          its modulus set to the polynomial's modulus. Unlike vector's
     *          at(), this cannot be used to modify the coefficient's value. */
    const BigMod at(long i) const;
    /**
     * Sets the ith coefficient of the polynomial to the given value.
     * @param i the index of the coefficient to set
     * @param value the value to set it to
     */
    void set(long i, BigInt& value);
    void set(long i, BigInt&& value);  //rvalue version so you can use temporaries
    void set(long i, unsigned long value);

    /**
     * Sets the polynomial to a constant term with the given value, reduced
     * modulo the polynomial's modulus.
     *
     * @param constant the value for the constant term
     */
    void setConstant(BigInt& constant);
    void setConstant(unsigned long constant);

    /** @return true if this equals other, false otherwise */
    bool equals(const ModPolynomial& other) const;
    //Mutative arithmetic operations
    /** Adds another ModPolynomial to this ModPolynomial */
    void add(const ModPolynomial& rhs);
    /** Subtracts another ModPolynomial from this ModPolynomial */
    void subtract(const ModPolynomial& rhs);
    /** Multiplies this polynomial by another ModPolynomial */
    void multiply(const ModPolynomial& rhs);
    /** Raises this ModPolynomial to a long integer power */
    void power(const unsigned long& exponent);

    /**
     * Prints the polynomial to stdout, in the format <size> <modulus>
     * <coefficients in ascending order>
     */
    void print() const;
    /**
     * Prints a "pretty" representation of the polynomial to stdout, where
     * the powers of x are explicitly written out, with multiplication and
     * addition symbols.
     */
    void printPretty() const;

    //Unified assignment operator
    ModPolynomial& operator=(ModPolynomial rhs);
    //Mutative arithmetic operator overloading
    ModPolynomial& operator+=(const ModPolynomial& rhs);
    ModPolynomial& operator-=(const ModPolynomial& rhs);
    ModPolynomial& operator*=(const ModPolynomial& rhs);
    ModPolynomial& operator^=(const unsigned long& exp);

    bool operator==(const ModPolynomial& rhs) const;
    bool operator!=(const ModPolynomial& rhs) const;

private:
    fmpz_mod_poly_t mod_poly;

    friend void swap(ModPolynomial& first, ModPolynomial& second) noexcept;
    friend void add(const ModPolynomial& lhs, const ModPolynomial& rhs, ModPolynomial& result);
    friend void subtract(const ModPolynomial& lhs, const ModPolynomial& rhs, ModPolynomial& result);
    friend void multiply(const ModPolynomial& lhs, const ModPolynomial& rhs, ModPolynomial& result);
    friend void power(const ModPolynomial& base, const unsigned long& exponent, ModPolynomial& result);

    //        friend std::ostream& operator<<(std::ostream& os, const ModPolynomial& obj);
    //        friend std::istream& operator>>(std::istream& is, ModPolynomial& obj);
};

ModPolynomial operator+(ModPolynomial lhs, const ModPolynomial& rhs);
ModPolynomial operator-(ModPolynomial lhs, const ModPolynomial& rhs);
ModPolynomial operator*(ModPolynomial lhs, const ModPolynomial& rhs);
ModPolynomial operator^(ModPolynomial lhs, const unsigned long& rhs);

/** Sets result to lhs + rhs */
void add(const ModPolynomial& lhs, const ModPolynomial& rhs, ModPolynomial& result);
/** Sets result to lhs - rhs */
void subtract(const ModPolynomial& lhs, const ModPolynomial& rhs, ModPolynomial& result);
/** Sets result to lhs * rhs */
void multiply(const ModPolynomial& lhs, const ModPolynomial& rhs, ModPolynomial& result);
/** Sets result to base ^ exponent */
void power(const ModPolynomial& base, const unsigned long& exponent, ModPolynomial& result);

/**
 * Swaps two ModPolynomials efficiently, by swapping pointers.
 * @param first The first ModPolynomial, which will have the value of second
 *        after calling this function.
 * @param second The second ModPolynomial, which will have the value of first
 *        after calling this function.
 */
void swap(ModPolynomial& first, ModPolynomial& second) noexcept;

}  // namespace flint

//std::ostream& operator<<(std::ostream& os, const ModPolynomial& obj);
//std::istream& operator>>(std::istream& is, ModPolynomial& obj);

#endif /* MODPOLYNOMIAL_H_ */
