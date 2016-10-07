#ifndef RSAKEY_H
#define RSAKEY_H

#include <vector>
#include <flint/BigInt.h>
#include <flint/BigMod.h>
#include <algorithms/PrimeRepGenerator.h>


class RSAKey {
    public:
        typedef struct {
            flint::BigInt rsaModulus;
            flint::BigMod base;
            std::unique_ptr<PrimeRepGenerator> primeRepGenerator;
        } PublicKey;

        RSAKey();
        ~RSAKey();
        /**
         * The secret key consists of the modulus factors p and q. The first
         * item in the pair is p, and the second item is q.
         * @return a pair containing p and q, in that order.
         */
        std::pair<flint::BigInt, flint::BigInt>& getSecretKey() const;
        PublicKey& getPublicKey() const;

    private:
        std::unique_ptr<std::pair<flint::BigInt, flint::BigInt>> _secretKey;
        std::shared_ptr<PublicKey> _publicKey;
};

#endif // RSAKEY_H
