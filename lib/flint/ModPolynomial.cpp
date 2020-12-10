/*
 * ModPolynomial.cpp
 *
 *  Created on: Apr 19, 2013
 *      Author: etremel
 */

#include <flint/ArithmeticException.hpp>
#include <flint/ModPolynomial.hpp>

namespace flint {

//ModPolynomial::ModPolynomial() {
//    fmpz_t modulus;
//    fmpz_init_set_ui(modulus, 1);
//    fmpz_mod_poly_init(mod_poly, modulus);
//    fmpz_clear(modulus);
//}
ModPolynomial::~ModPolynomial() {
    fmpz_mod_poly_clear(mod_poly);
}
ModPolynomial::ModPolynomial(const ModPolynomial& other) {
    //Initialize to zero, with the same modulus as other
    fmpz_mod_poly_init(mod_poly, fmpz_mod_poly_modulus(other.mod_poly));
    //Set only sets the coefficients, not the modulus (bug?)
    fmpz_mod_poly_set(mod_poly, other.mod_poly);
}

ModPolynomial::ModPolynomial(ModPolynomial&& other) {
    fmpz_mod_poly_init(mod_poly, fmpz_mod_poly_modulus(other.mod_poly));
    //Steal other's value, leaving it with an initialized value so it can destruct safely
    fmpz_mod_poly_swap(mod_poly, other.mod_poly);
}

ModPolynomial::ModPolynomial(const BigInt& modulus) {
    fmpz_mod_poly_init(mod_poly, modulus.getUnderlyingObject());
}

ModPolynomial::ModPolynomial(const BigInt& constantValue, const BigInt& modulus) {
    fmpz_mod_poly_init(mod_poly, modulus.getUnderlyingObject());
    fmpz_mod_poly_set_fmpz(mod_poly, constantValue.getUnderlyingObject());
}

ModPolynomial& ModPolynomial::operator=(ModPolynomial rhs) {
    //Copy-and-swap algorithm; note the parameter^ is by copy.
    swap(*this, rhs);
    return *this;
}

ModPolynomial& ModPolynomial::operator+=(const ModPolynomial& rhs) {
    this->add(rhs);
    return *this;
}

ModPolynomial& ModPolynomial::operator-=(const ModPolynomial& rhs) {
    this->subtract(rhs);
    return *this;
}

ModPolynomial& ModPolynomial::operator*=(const ModPolynomial& rhs) {
    this->multiply(rhs);
    return *this;
}

ModPolynomial& ModPolynomial::operator^=(const unsigned long& exp) {
    this->power(exp);
    return *this;
}

bool ModPolynomial::operator==(const ModPolynomial& rhs) const {
    return this->equals(rhs);
}

bool ModPolynomial::operator!=(const ModPolynomial& rhs) const {
    return !operator==(rhs);
}

long ModPolynomial::getDegree() const {
    return fmpz_mod_poly_degree(mod_poly);
}

BigInt ModPolynomial::getModulus() const {
    //BigInt's constructor takes fmpz_t&, not fmpz*
    return BigInt(fmpz_t{*fmpz_mod_poly_modulus(mod_poly)});
}

const BigMod ModPolynomial::at(long i) const {
    fmpz_t coeff;
    fmpz_mod_poly_get_coeff_fmpz(coeff, mod_poly, i);
    return BigMod(coeff, fmpz_t{*fmpz_mod_poly_modulus(mod_poly)});
}

void ModPolynomial::set(long i, BigInt& value) {
    fmpz_mod_poly_set_coeff_fmpz(mod_poly, i, value.getUnderlyingObject());
}

void ModPolynomial::set(long i, BigInt&& value) {
    fmpz_mod_poly_set_coeff_fmpz(mod_poly, i, value.getUnderlyingObject());
}

void ModPolynomial::set(long i, unsigned long value) {
    fmpz_mod_poly_set_coeff_ui(mod_poly, i, value);
}

void ModPolynomial::setConstant(BigInt& constant) {
    fmpz_mod_poly_set_fmpz(mod_poly, constant.getUnderlyingObject());
}

void ModPolynomial::setConstant(unsigned long constant) {
    fmpz_mod_poly_set_ui(mod_poly, constant);
}

void ModPolynomial::add(const ModPolynomial& rhs) {
    flint::add(*this, rhs, *this);
}

void ModPolynomial::subtract(const ModPolynomial& rhs) {
    flint::subtract(*this, rhs, *this);
}

void ModPolynomial::multiply(const ModPolynomial& rhs) {
    flint::multiply(*this, rhs, *this);
}

void ModPolynomial::power(const unsigned long& exponent) {
    flint::power(*this, exponent, *this);
}

bool ModPolynomial::equals(const ModPolynomial& other) const {
    return fmpz_mod_poly_equal(this->mod_poly, other.mod_poly);
}

void ModPolynomial::print() const {
    fmpz_mod_poly_print(mod_poly);
}

void ModPolynomial::printPretty() const {
    fmpz_mod_poly_print_pretty(mod_poly, "x");
}

/* ----------------------- End class ModPolynomial -------------------------- */

void add(const ModPolynomial& lhs, const ModPolynomial& rhs, ModPolynomial& result) {
    if(!fmpz_equal(fmpz_mod_poly_modulus(lhs.mod_poly), fmpz_mod_poly_modulus(rhs.mod_poly)))
        throw ArithmeticException("Polynomial addition error: operands have different moduli.");
    fmpz_set(fmpz_mod_poly_modulus(result.mod_poly), fmpz_mod_poly_modulus(lhs.mod_poly));
    fmpz_mod_poly_add(result.mod_poly, lhs.mod_poly, rhs.mod_poly);
}

void subtract(const ModPolynomial& lhs, const ModPolynomial& rhs, ModPolynomial& result) {
    if(!fmpz_equal(fmpz_mod_poly_modulus(lhs.mod_poly), fmpz_mod_poly_modulus(rhs.mod_poly)))
        throw ArithmeticException("Polynomial subtraction error: operands have different moduli.");
    fmpz_set(fmpz_mod_poly_modulus(result.mod_poly), fmpz_mod_poly_modulus(lhs.mod_poly));
    fmpz_mod_poly_sub(result.mod_poly, lhs.mod_poly, rhs.mod_poly);
}

void multiply(const ModPolynomial& lhs, const ModPolynomial& rhs, ModPolynomial& result) {
    if(!fmpz_equal(fmpz_mod_poly_modulus(lhs.mod_poly), fmpz_mod_poly_modulus(rhs.mod_poly)))
        throw ArithmeticException("Polynomial multiplication error: operands have different moduli.");
    fmpz_set(fmpz_mod_poly_modulus(result.mod_poly), fmpz_mod_poly_modulus(lhs.mod_poly));
    fmpz_mod_poly_mul(result.mod_poly, lhs.mod_poly, rhs.mod_poly);
}

void power(const ModPolynomial& base, const unsigned long& exponent, ModPolynomial& result) {
    fmpz_set(fmpz_mod_poly_modulus(result.mod_poly), fmpz_mod_poly_modulus(base.mod_poly));
    fmpz_mod_poly_pow(result.mod_poly, base.mod_poly, exponent);
}

ModPolynomial operator+(ModPolynomial lhs, const ModPolynomial& rhs) {
    lhs += rhs;
    return lhs;
}

ModPolynomial operator-(ModPolynomial lhs, const ModPolynomial& rhs) {
    lhs -= rhs;
    return lhs;
}

ModPolynomial operator*(ModPolynomial lhs, const ModPolynomial& rhs) {
    lhs *= rhs;
    return lhs;
}

ModPolynomial operator^(ModPolynomial lhs, const unsigned long& rhs) {
    lhs ^= rhs;
    return lhs;
}

//Trivial because FLINT already implemented swap for fmpz_mod_polys
void swap(ModPolynomial& first, ModPolynomial& second) noexcept {
    fmpz_mod_poly_swap(first.mod_poly, second.mod_poly);
}

}  // namespace flint

//std::ostream& operator<<(std::ostream& os, const ModPolynomial& obj) {
//
//}
//
//std::istream& operator>>(std::istream& is, ModPolynomial& obj) {
//
//}
