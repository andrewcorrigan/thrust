/*
 *  Copyright 2008-2012 NVIDIA Corporation
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

#include <thrust/iterator/zip_iterator.h>
#include <thrust/detail/tuple_transform.h>

namespace thrust
{


template <typename IteratorTuple>
  zip_iterator<IteratorTuple>
    ::zip_iterator(void)
{
} // end zip_iterator::zip_iterator()


template <typename IteratorTuple>
  zip_iterator<IteratorTuple>
    ::zip_iterator(IteratorTuple iterator_tuple)
      :m_iterator_tuple(iterator_tuple)
{
} // end zip_iterator::zip_iterator()


template <typename IteratorTuple>
  template <typename OtherIteratorTuple>
    zip_iterator<IteratorTuple>
      ::zip_iterator(const zip_iterator<OtherIteratorTuple> &other,
                     typename thrust::detail::enable_if_convertible<
                       OtherIteratorTuple,
                       IteratorTuple
                     >::type *)
        :m_iterator_tuple(other.get_iterator_tuple())
{
} // end zip_iterator::zip_iterator()


template <typename IteratorTuple>
const IteratorTuple &zip_iterator<IteratorTuple>
  ::get_iterator_tuple(void) const
{
  return m_iterator_tuple;
} // end zip_iterator::get_iterator_tuple()


template <typename IteratorTuple>
  typename zip_iterator<IteratorTuple>::super_t::reference
    zip_iterator<IteratorTuple>
      ::dereference(void) const
{
  using namespace detail::tuple_impl_specific;

  return thrust::detail::tuple_host_device_transform<detail::dereference_iterator::template apply>(get_iterator_tuple(), detail::dereference_iterator());
} // end zip_iterator::dereference()


template <typename IteratorTuple>
  template <typename OtherIteratorTuple>
    bool zip_iterator<IteratorTuple>
      ::equal(const zip_iterator<OtherIteratorTuple> &other) const
{
  return get<0>(get_iterator_tuple()) == get<0>(other.get_iterator_tuple());
} // end zip_iterator::equal()


template <typename IteratorTuple>
  void zip_iterator<IteratorTuple>
    ::advance(typename super_t::difference_type n)
{
  using namespace detail::tuple_impl_specific;

  // XXX note that we use a pointer to System to dispatch to avoid
  //     default construction of a System
  typename thrust::iterator_system<zip_iterator>::type *use_me_to_dispatch = 0;

  // dispatch on system
  tuple_for_each(m_iterator_tuple,
                 detail::advance_iterator<typename super_t::difference_type>(n),
                 use_me_to_dispatch);
} // end zip_iterator::advance()


template <typename IteratorTuple>
  void zip_iterator<IteratorTuple>
    ::increment(void)
{
  using namespace detail::tuple_impl_specific;

  // XXX note that we use a pointer to System to dispatch to avoid
  //     default construction of a System
  typename thrust::iterator_system<zip_iterator>::type *use_me_to_dispatch = 0;

  // dispatch on system
  tuple_for_each(m_iterator_tuple, detail::increment_iterator(),
                 use_me_to_dispatch);
} // end zip_iterator::increment()


template <typename IteratorTuple>
  void zip_iterator<IteratorTuple>
    ::decrement(void)
{
  using namespace detail::tuple_impl_specific;

  // dispatch on system
  tuple_for_each(m_iterator_tuple, detail::decrement_iterator(),
                 typename thrust::iterator_system<zip_iterator>::type());
} // end zip_iterator::decrement()


template <typename IteratorTuple>
  template <typename OtherIteratorTuple>
    typename zip_iterator<IteratorTuple>::super_t::difference_type
      zip_iterator<IteratorTuple>
        ::distance_to(const zip_iterator<OtherIteratorTuple> &other) const
{
  return get<0>(other.get_iterator_tuple()) - get<0>(get_iterator_tuple());
} // end zip_iterator::distance_to()


template <typename IteratorTuple>
  zip_iterator<IteratorTuple> make_zip_iterator(IteratorTuple t)
{
  return zip_iterator<IteratorTuple>(t);
} // end make_zip_iterator()


} // end thrust

