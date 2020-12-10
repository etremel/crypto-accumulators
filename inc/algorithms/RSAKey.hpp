#ifndef RSAKEY_H
#define RSAKEY_H

#include <algorithms/PrimeRepGenerator.hpp>
#include <flint/BigInt.hpp>
#include <flint/BigMod.hpp>
#include <vector>
#include <memory>

class RSAKey {
public:
    /**
     * The public key consists of a base, the modulus, and the function for
     * generating prime representatives within that modulus. The base is
     * usually fixed to 65537, like in the standard RSA public-key system.
    */
    struct PublicKey {
        flint::BigInt rsaModulus;
        flint::BigMod base;
        std::unique_ptr<PrimeRepGenerator> primeRepGenerator;
    };
    /** The secret key consists of the modulus factors p and q. */
    struct SecretKey {
        flint::BigInt p;
        flint::BigInt q;
    };

    RSAKey();
    ~RSAKey();
    /**
     * The secret key consists of the modulus factors p and q. The first
     * item in the pair is p, and the second item is q.
     * @return a pair containing p and q, in that order.
     */
    SecretKey& getSecretKey() const;
    PublicKey& getPublicKey() const;

private:
    std::unique_ptr<SecretKey> _secretKey;
    std::shared_ptr<PublicKey> _publicKey;
};

#endif  // RSAKEY_H
