/*
 * StandardHashes.h
 *
 *  Created on: Mar 10, 2013
 *      Author: etremel
 */

#ifndef STANDARDHASHES_H_
#define STANDARDHASHES_H_

#include <memory>

/**
 * Combines the hash of the given object with the "seed," updating seed to
 * contain a combined hash. Copied from boost, so I have no idea what the magic
 * number 0x9e3779b9 does.
 * @param seed a seed value for the hash (or an existing hash value) that will
 *        contain the new hash after running this function.
 * @param v the object that should be combined into the hash
 */
template <class T>
inline void hash_combine(std::size_t & seed, const T & v) {
        std::hash<T> hasher;
        seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

namespace std {
    /**
     * The authors of STL didn't bother to create a hash function for pair, so
     * I have to do it for them. This just recursively calls hash on each
     * element of the pair and combines the resulting hashes.
     */
    template<typename F, typename S>
    class hash<pair<F, S>> {
        public:
            size_t operator()(const pair<F, S>& pair_val) const {
                size_t seed = 0;
                ::hash_combine(seed, pair_val.first);
                ::hash_combine(seed, pair_val.second);
                return seed;
            }
    };

//    /**
//     * There's no default hash function for shared_ptr, but it should be the
//     * same as the hash function for regular pointers, which is just to return
//     * the address of the pointer.
//     */
//    template<typename T>
//    class hash<std::shared_ptr<T>> {
//        public:
//            size_t operator()(const std::shared_ptr<T>& ptr) const {
//                return (size_t)ptr.get();
//            }
//    };
};



#endif /* STANDARDHASHES_H_ */
