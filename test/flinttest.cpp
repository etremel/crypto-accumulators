/*
 * flinttest.cpp
 *
 *  Created on: Apr 19, 2013
 *      Author: etremel
 */

#include <flint/flint.h>
#include <flint/fmpz.h>
#include <flint/fmpz_mod_poly.h>


int main(int argc, char** argv) {
    fmpz_t value;
    fmpz_t result;
    fmpz_t copy;
    fmpz_init(value);
    fmpz_init(result);
    fmpz_set_ui(value, 666);
    fmpz_set(copy, value);
    printf("Copy of ");
    fmpz_print(value);
    printf(" is ");
    fmpz_print(copy);
    printf("\n");
    fmpz_set_ui(value, 777);
    printf("Value changed to ");
    fmpz_print(value);
    printf("\nCopy is: ");
    fmpz_print(copy);
    printf("\n");
    fmpz_mul(result, value, value);
    fmpz_print (value);
    printf ("^2 = ");
    fmpz_print(result);
    printf ("\n");
    fmpz_t y;
    fmpz_init_set_ui(y, 333);
    fmpz_add(result, value, y);
    fmpz_print(value);
    printf("+");
    fmpz_print(y);
    printf("=");
    fmpz_print(result);
    printf("\n");
    fmpz_clear(y);
    fmpz_clear (value);
    fmpz_clear (result);

    fmpz_t modulus;
    fmpz_t mod2;
    fmpz_init_set_ui(modulus, 30);
    fmpz_init_set_ui(mod2, 1);
    fmpz_mod_poly_t poly1;
    fmpz_mod_poly_t poly2;
    fmpz_mod_poly_t res;
    fmpz_mod_poly_init(poly1, modulus);
    fmpz_mod_poly_init(poly2, mod2);
    fmpz_init_set_ui(fmpz_mod_poly_modulus(poly2), 30);
    fmpz_mod_poly_init(res, fmpz_mod_poly_modulus(poly1));
    fmpz_clear(modulus);
    fmpz_clear(mod2);
    fmpz_mod_poly_set_coeff_ui(poly1, 2, 5);
    fmpz_mod_poly_set_coeff_ui(poly1, 1, 2);
    fmpz_mod_poly_set_coeff_ui(poly2, 3, 6);
    fmpz_mod_poly_set_coeff_ui(poly2, 1, 4);
    fmpz_mod_poly_mul(res, poly1, poly2);
    fmpz_mod_poly_print_pretty(poly1, "x");
    printf(" * ");
    fmpz_mod_poly_print_pretty(poly2, "x");
    printf(" = ");
    fmpz_mod_poly_print_pretty(res, "x");
    printf("\n");
    fmpz_mod_poly_clear(poly1);
    fmpz_mod_poly_clear(poly2);
    fmpz_mod_poly_clear(res);
}


