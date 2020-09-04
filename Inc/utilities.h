/*
 * utilities.h
 *
 *  Created on: 24.12.2019
 *      Author: Mati
 */

#ifndef UTILITIES_H_
#define UTILITIES_H_

template<class T>
constexpr const T& clamp( const T& v, const T& lo, const T& hi )
{
    assert( !(hi < lo) );
    return (v < lo) ? lo : (hi < v) ? hi : v;
}



#endif /* UTILITIES_H_ */
