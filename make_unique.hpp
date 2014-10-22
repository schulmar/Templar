#ifndef MAKE_UNIQUE_HPP_
#define MAKE_UNIQUE_HPP_

#include <memory>
/**
 * @brief Create a new instance of T and wrap it in a unique_ptr
 *
 * Taken from GotW 102
 */
template<typename T, typename ...Args>
std::unique_ptr<T> make_unique( Args&& ...args )
{
    return std::unique_ptr<T>( new T( std::forward<Args>(args)... ) );
}

#endif /* MAKE_UNIQUE_HPP_ */
