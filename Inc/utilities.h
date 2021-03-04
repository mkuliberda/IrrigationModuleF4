/*
 * utilities.h
 *
 *  Created on: 24.12.2019
 *      Author: Mati
 */

#ifndef UTILITIES_H_
#define UTILITIES_H_

#include <string>
#include <sstream>

#define MINIMUM(a,b)            (((a) < (b)) ? (a) : (b))
#define MAXIMUM(a,b)            (((a) < (b)) ? (b) : (a))

template<class T>
constexpr const T& clamp( const T& v, const T& lo, const T& hi )
{
    assert( !(hi < lo) );
    return (v < lo) ? lo : (hi < v) ? hi : v;
}

namespace patch
{
    template < typename T > std::string to_string( const T& n )
    {
        std::ostringstream stm ;
        stm << n ;
        return stm.str() ;
    }
}

#endif /* UTILITIES_H_ */
