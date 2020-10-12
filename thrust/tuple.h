/*
 *  Copyright 2008-2018 NVIDIA Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */


/*! \file tuple.h
 *  \brief A type encapsulating a heterogeneous collection of elements
 */

/*
 * Copyright (C) 1999, 2000 Jaakko Järvi (jaakko.jarvi@cs.utu.fi)
 * 
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying NOTICE file for the complete license)
 *
 * For more information, see http://www.boost.org
 */

#pragma once

#include <thrust/detail/config.h>
#include <thrust/detail/tuple/tuple.h>
#include <thrust/pair.h>
#include <thrust/detail/tuple/variadic_tuple.h>
#include <thrust/detail/tuple/tuple_io.h>

namespace thrust
{

/*! \addtogroup utility
 *  \{
 */

/*! \addtogroup tuple
*  \{
*/

/*! This version of \p tuple_cat creates a new \c tuple object from two
 *  tuples by concanenation
 *
 *  \param t1 The first object to concatenate.
 *  \param ts The other objects to concatenate.
 *  \return A \p tuple object which is a concatenation of \p t0 and \p ts... .
 */
#if 0
template<typename... Tuples>
inline __host__ __device__
tuple_cat_result<Tuples...>
  tuple_cat(const Tuples&... ts);
#endif

/*! tuple_manipulator for defining the character that is output before the first element of a tuple
 *
 *  \param c The character that is output before the first element of a tuple
 *  \return A tuple_manipulator which changes the default character output before the first element of a tuple
 */
template<class CharType>
inline detail::tuple_detail::tuple_manipulator<CharType>
  set_open(const CharType c);

/*! tuple_manipulator for defining the character that is output after the last element of a tuple
 *
 *  \param c The character that is output after the last element of a tuple
 *  \return A tuple_manipulator which changes the default character output after the last element of a tuple
 */
template<class CharType>
inline detail::tuple_detail::tuple_manipulator<CharType>
  set_close(const CharType c);

 /*! tuple_manipulator for defining the character that is output between tuple elements
  *
  *  \param c The character that is output between tuple elements
  *  \return A tuple_manipulator which changes the default character output between element
  */
template<class CharType>
inline detail::tuple_detail::tuple_manipulator<CharType>
  set_delimiter(const CharType c);

/*! Stream insertion operator<< overload for tuple_manipulator
 *
 *  \param o The stream to insert \p t into.
 *  \param t The tuple_manipulator object to insert into \p o.
 *  \return A reference to \p o.
 */
template<class CharType, class CharTrait>
inline std::basic_ostream<CharType, CharTrait>&
  operator<<(std::basic_ostream<CharType, CharTrait>& o, const detail::tuple_detail::tuple_manipulator<CharType>& m);

/*! Stream extraction operator<< overload for tuple_manipulator
 *
 *  \param o The stream to extract \p t from.
 *  \param t The tuple_manipulator object to extract from \p i.
 *  \return A reference to \p i.
 */
template<class CharType, class CharTrait>
inline std::basic_istream<CharType, CharTrait>&
  operator>>(std::basic_istream<CharType, CharTrait>& i, const detail::tuple_detail::tuple_manipulator<CharType>& m);

/*! Stream insertion operator<< overload for thrust::tuple
 *
 *  \param o The stream to insert \p t into.
 *  \param t The tuple object to insert into \p o.
 *  \return A reference to \p o.
 */
template<class CharType, class CharTrait, class... Types>
inline std::basic_ostream<CharType, CharTrait>& 
  operator<<(std::basic_ostream<CharType, CharTrait>& o, 
             const thrust::tuple<Types...>& t);

/*! Stream extraction operator>> overload for thrust::tuple
 *
 *  \param is The stream to extract \p t from.
 *  \param t The tuple object to extract from \p i.
 *  \return A reference to \p i.
 */
template<class CharType, class CharTrait, class... Types>
inline std::basic_istream<CharType, CharTrait>& 
  operator>>(std::basic_istream<CharType, CharTrait>& i,
             thrust::tuple<Types...>& t);

/*! \} // tuple
 */

/*! \} // utility
 */

} // end thrust

