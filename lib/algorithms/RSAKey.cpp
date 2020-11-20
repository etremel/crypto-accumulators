
#include <utils/Pointers.hpp>
#include <algorithms/RSAKey.hpp>

using std::pair;

RSAKey::RSAKey() {
    _secretKey = unique_new<pair<flint::BigInt,flint::BigInt>>();
    _publicKey = std::make_shared<PublicKey>();
}

RSAKey::~RSAKey() {

}

pair<flint::BigInt, flint::BigInt>& RSAKey::getSecretKey() const {
    return *(_secretKey);
}

RSAKey::PublicKey& RSAKey::getPublicKey() const {
    return *(_publicKey);
}
