
#include <algorithms/RSAKey.hpp>

RSAKey::RSAKey() : _secretKey(std::make_unique<SecretKey>()),
                   _publicKey(std::make_shared<PublicKey>()) {}

RSAKey::~RSAKey() {
}

RSAKey::SecretKey& RSAKey::getSecretKey() const {
    return *(_secretKey);
}

RSAKey::PublicKey& RSAKey::getPublicKey() const {
    return *(_publicKey);
}
