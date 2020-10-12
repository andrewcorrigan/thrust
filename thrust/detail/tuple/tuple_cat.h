/*
 *  Copyright 2008-2014 NVIDIA Corporation
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

#pragma once

#include <thrust/detail/config.h>
#include <thrust/tuple.h>
#include <thrust/detail/type_traits.h>
#include <thrust/type_traits/integer_sequence.h>


namespace thrust
{
template<class T>
using __decay_t = typename std::decay<T>::type;

template<class T>
struct tuple_traits
{
  using tuple_type = T;

  static const size_t size = thrust::tuple_size<tuple_type>::value; 

  template<size_t i>
  using element_type = typename thrust::tuple_element<i,tuple_type>::type;

  template<size_t i>
  __TUPLE_ANNOTATION
  static element_type<i>& get(tuple_type& t)
  {
    return thrust::get<i>(t);
  }

  template<size_t i>
  __TUPLE_ANNOTATION
  static const element_type<i>& get(const tuple_type& t)
  {
    return thrust::get<i>(t);
  }

  template<size_t i>
  __TUPLE_ANNOTATION
  static element_type<i>&& get(tuple_type&& t)
  {
    return thrust::get<i>(std::move(t));
  }
};


template<size_t i, class Tuple>
__TUPLE_ANNOTATION
auto __get(Tuple&& t)
  -> decltype(
       tuple_traits<__decay_t<Tuple>>::template get<i>(std::forward<Tuple>(t))
     )
{
  return tuple_traits<__decay_t<Tuple>>::template get<i>(std::forward<Tuple>(t));
}

template<bool b, class True, class False>
struct __lazy_conditional
{
  using type = typename True::type;
};


template<class True, class False>
struct __lazy_conditional<false, True, False>
{
  using type = typename False::type;
};


template<size_t I0, class Tuple1, class... Tuples>
struct __tuple_cat_get_result
{
  using tuple1_type = typename std::decay<Tuple1>::type;
  static const size_t size1 = thrust::tuple_size<typename std::decay<Tuple1>::type>::value;

  using type = typename __lazy_conditional<
    (I0 < size1),
    thrust::tuple_element<I0,tuple1_type>,
    __tuple_cat_get_result<I0 - size1, Tuples...>
  >::type;
};


template<size_t I0, class Tuple1>
struct __tuple_cat_get_result<I0,Tuple1>
  : thrust::tuple_element<I0, typename std::decay<Tuple1>::type>
{};


template<size_t I0, class Tuple1, class... Tuples>
__TUPLE_ANNOTATION
typename __tuple_cat_get_result<I0,Tuple1,Tuples...>::type
  __tuple_cat_get(Tuple1&& t, Tuples&&... ts);


template<size_t I0, class Tuple1, class... Tuples>
__TUPLE_ANNOTATION
typename __tuple_cat_get_result<I0,Tuple1,Tuples...>::type
  __tuple_cat_get_impl(std::false_type, Tuple1&& t, Tuples&&...)
{
  return __get<I0>(std::forward<Tuple1>(t));
}


template<size_t I0, class Tuple1, class... Tuples>
__TUPLE_ANNOTATION
typename __tuple_cat_get_result<I0,Tuple1,Tuples...>::type
  __tuple_cat_get_impl(std::true_type, Tuple1&&, Tuples&&... ts)
{
  const size_t J = I0 - thrust::tuple_size<typename std::decay<Tuple1>::type>::value;
  return __tuple_cat_get<J>(std::forward<Tuples>(ts)...);
}


template<size_t I0, class Tuple1, class... Tuples>
__TUPLE_ANNOTATION
typename __tuple_cat_get_result<I0,Tuple1,Tuples...>::type
  __tuple_cat_get(Tuple1&& t, Tuples&&... ts)
{
  auto recurse = typename std::conditional<
    I0 < thrust::tuple_size<typename std::decay<Tuple1>::type>::value,
    std::false_type,
    std::true_type
  >::type();

  return __tuple_cat_get_impl<I0>(recurse, std::forward<Tuple1>(t), std::forward<Tuples>(ts)...);
}


template<size_t... Is, class Function, class... Tuples>
__TUPLE_ANNOTATION
auto __tuple_cat_apply_impl(thrust::index_sequence<Is...>, Function f, Tuples&&... ts)
  -> decltype(
       f(__tuple_cat_get<Is>(std::forward<Tuples>(ts)...)...)
     )
{
  return f(__tuple_cat_get<Is>(std::forward<Tuples>(ts)...)...);
}


template<size_t Size, size_t... Sizes>
struct __sum
  : std::integral_constant<
      size_t,
      Size + __sum<Sizes...>::value
    >
{};


template<size_t Size> struct __sum<Size> : std::integral_constant<size_t, Size> {};


template<class Function, class... Tuples>
__TUPLE_ANNOTATION
auto tuple_cat_apply(Function f, Tuples&&... ts)
  -> decltype(
       __tuple_cat_apply_impl(
         thrust::make_index_sequence<
           __sum<
             0u,
             thrust::tuple_size<typename std::decay<Tuples>::type>::value...
           >::value
         >(),
         f,
         std::forward<Tuples>(ts)...
       )
     )
{
  const size_t N = __sum<0u, thrust::tuple_size<typename std::decay<Tuples>::type>::value...>::value;
  return __tuple_cat_apply_impl(thrust::make_index_sequence<N>(), f, std::forward<Tuples>(ts)...);
}


template<class Function, class Tuple, size_t... Is>
__TUPLE_ANNOTATION
auto __tuple_apply_impl(Function f, Tuple&& t, thrust::index_sequence<Is...>)
  -> decltype(
       f(__get<Is>(std::forward<Tuple>(t))...)
     )
{
  return f(__get<Is>(std::forward<Tuple>(t))...);
}



template<class Function, class Tuple>
__TUPLE_ANNOTATION
auto tuple_apply(Function f, Tuple&& t)
  -> decltype(
       tuple_cat_apply(f, std::forward<Tuple>(t))
     )
{
  return tuple_cat_apply(f, std::forward<Tuple>(t));
}
    
template<class IndexSequence, class... Tuples>
struct tuple_cat_result_impl_impl;


template<size_t... Is, class... Tuples>
struct tuple_cat_result_impl_impl<thrust::index_sequence<Is...>, Tuples...>
{
  using type = tuple<typename __tuple_cat_get_result<Is, Tuples...>::type...>;
};


template<class... Tuples>
struct tuple_cat_result_impl
{
  static const size_t result_size = __sum<0u, tuple_size<Tuples>::value...>::value;

  using type = typename tuple_cat_result_impl_impl<
    thrust::make_index_sequence<result_size>,
    Tuples...
  >::type;
};


template<class... Tuples>
using tuple_cat_result = typename tuple_cat_result_impl<typename std::decay<Tuples>::type...>::type;


template<class Tuple>
struct tuple_maker
{
  template<class... Args>
  __TUPLE_ANNOTATION
  Tuple operator()(Args&&... args)
  {
    return Tuple{std::forward<Args>(args)...};
  }
};


// XXX this doesn't forward tuple elements which are reference types correctly
//     because make_tuple() doesn't do that
template<class... Tuples>
__TUPLE_ANNOTATION
tuple_cat_result<Tuples...> tuple_cat(Tuples&&... tuples)
{
  return tuple_cat_apply(tuple_maker<tuple_cat_result<Tuples...>>{}, std::forward<Tuples>(tuples)...);
}
}
