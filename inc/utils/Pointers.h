#ifndef POINTERS_H
#define POINTERS_H

#include <memory>
#include <cassert>


/**
 * @brief unique_new constructs an object and returns a unique pointer
 * @param args the arguments to the object's constructor
 * @return a unique pointer to the object
 */
template<typename T, typename... A>
std::unique_ptr<T> unique_new(A... args){
  return std::move(std::unique_ptr<T>(new T(args...)));
}


/**
 * @brief ref_cast casts a reference from source type U to target type T
 * @param source the source reference
 * @return a reference to the same object, as the target type
 */
template<typename T, typename U>
T& ref_cast(U& source){
    using namespace std;
    //will error with assertion fail if cast fails.
    if (auto* tmp = dynamic_cast<T*>(&source))
        return *tmp;
    else
        assert("Unable to convert between types." && 0);
}

/**
 * @brief ref_cast casts a const reference from source type U to target type T
 * @param source the source const reference
 * @return a const reference to the same object, as the target type
 */
template<typename T, typename U>
const T& ref_cast(const U& source){
    using namespace std;
    //will error with assertion fail if cast fails.
    if (auto* tmp = dynamic_cast<const T*>(&source))
        return *tmp;
    else
        assert("Unable to convert between types." && 0);
}

#endif // POINTERS_H
