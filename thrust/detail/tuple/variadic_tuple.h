#pragma once

#include <type_traits>
#include <stddef.h>
#include <utility>
//#include <tuple>

// allow the user to define an annotation to apply to these functions
// by default, it attempts to be constexpr
#ifndef __TUPLE_ANNOTATION
#  if __cplusplus <= 201103L
#    define __TUPLE_ANNOTATION __device__ __host__
#  else
#    define __TUPLE_ANNOTATION constexpr __device__ __host__
#  endif
#  define __TUPLE_ANNOTATION_NEEDS_UNDEF
#endif

// allow the user to define a namespace for these functions
#ifndef __TUPLE_NAMESPACE
#define __TUPLE_NAMESPACE thrust
#else
#error __TUPLE_NAMESPACE
#endif


namespace __TUPLE_NAMESPACE
{


template<class... Types> class tuple;


//} // end namespace


// specializations of stuff in std come before their use
//namespace std
//{


template<size_t, class> struct tuple_element;


template<size_t i>
struct tuple_element<i, __TUPLE_NAMESPACE::tuple<>> {};


template<class Type1, class... Types>
struct tuple_element<0, __TUPLE_NAMESPACE::tuple<Type1,Types...>>
{
  using type = Type1;
};


template<size_t i, class Type1, class... Types>
struct tuple_element<i, __TUPLE_NAMESPACE::tuple<Type1,Types...>>
{
  using type = typename tuple_element<i - 1, __TUPLE_NAMESPACE::tuple<Types...>>::type;
};


template<size_t i, class... Types>
using tuple_element_t = typename tuple_element<i,Types...>::type;


template<class> struct tuple_size;


template<class... Types>
struct tuple_size<__TUPLE_NAMESPACE::tuple<Types...>>
  : std::integral_constant<size_t, sizeof...(Types)>
{};


//} // end std


//namespace __TUPLE_NAMESPACE
//{
namespace __tuple_detail
{


// define index sequence in case it is missing
template<size_t... I> struct __index_sequence {};

template<size_t Start, typename Indices, size_t End>
struct __make_index_sequence_impl;

template<size_t Start, size_t... Indices, size_t End>
struct __make_index_sequence_impl<
  Start,
  __index_sequence<Indices...>, 
  End
>
{
  typedef typename __make_index_sequence_impl<
    Start + 1,
    __index_sequence<Indices..., Start>,
    End
  >::type type;
};

template<size_t End, size_t... Indices>
struct __make_index_sequence_impl<End, __index_sequence<Indices...>, End>
{
  typedef __index_sequence<Indices...> type;
};

template<size_t N>
using __make_index_sequence = typename __make_index_sequence_impl<0, __index_sequence<>, N>::type;


} // end __tuple_detail


template<class Type1, class... Types>
class tuple<Type1,Types...>
{
  public:
    __TUPLE_ANNOTATION
    tuple() : head_(), tail_() {}


    __TUPLE_ANNOTATION
    explicit tuple(const Type1& arg1, const Types&... args)
      : head_(arg1), tail_(args...)
    {}


    template<class UType1, class... UTypes,
             class = typename std::enable_if<
               (sizeof...(Types) == sizeof...(UTypes)) &&
               std::is_constructible<Type1,UType1&&>::value // XXX fill in the rest of these
             >::type>
    __TUPLE_ANNOTATION
    explicit tuple(UType1&& arg1, UTypes&&... args)
      : head_(std::forward<UType1>(arg1)),
        tail_(std::forward<UTypes>(args)...)
    {}


    template<class UType1, class... UTypes,
             class = typename std::enable_if<
               (sizeof...(Types) == sizeof...(UTypes)) &&
               std::is_constructible<Type1,const UType1&>::value // XXX fill in the rest of these
             >::type>
    __TUPLE_ANNOTATION
    tuple(const tuple<UType1,UTypes...>& other)
      : head_(other.head_),
        tail_(other.tail_)
    {}


    template<class UType1, class... UTypes,
             class = typename std::enable_if<
               (sizeof...(Types) == sizeof...(UTypes))
             >::type>
    __TUPLE_ANNOTATION
    tuple(tuple<UType1,UTypes...>&& other)
      : head_(std::move(other.head_)),
        tail_(std::move(other.tail_))
    {}


    template<class UType1, class UType2,
             class = typename std::enable_if<
               (sizeof...(Types) == 1) &&
               std::is_constructible<Type1,const UType1&>::value // XXX fill in the rest of these
             >::type>
    __TUPLE_ANNOTATION
    tuple(const __TUPLE_NAMESPACE::pair<UType1,UType2>& p)
      : head_(p.first),
        tail_(p.second)
    {}


    template<class UType1, class UType2,
             class = typename std::enable_if<
               (sizeof...(Types) == 1) &&
               std::is_constructible<Type1,UType1&&>::value // XXX fill in the rest of these
             >::type>
    __TUPLE_ANNOTATION
    tuple(__TUPLE_NAMESPACE::pair<UType1,UType2>&& p)
      : head_(std::move(p.first)),
        tail_(std::move(p.second))
    {}


    __TUPLE_ANNOTATION
    tuple(const tuple& other)
      : head_(other.head_),
        tail_(other.tail_)
    {}


    __TUPLE_ANNOTATION
    tuple(tuple&& other)
      : head_(std::forward<Type1>(other.head_)),
        tail_(std::move(other.tail_))
    {}


  private:
    template<class... UTypes>
    __TUPLE_ANNOTATION
    static tuple<UTypes&&...> forward_as_tuple(UTypes&&... args)
    {
      return tuple<UTypes&&...>(std::forward<UTypes>(args)...);
    }


public:
    __TUPLE_ANNOTATION
    tuple& operator=(const tuple& other)
    {
      head_ = other.head_;
      tail_ = other.tail_;
      return *this;
    }


    __TUPLE_ANNOTATION
    tuple& operator=(tuple&& other)
    {
      head_ = std::move(other.head_);
      tail_ = std::move(other.tail_);
      return *this;
    }


    template<class... UTypes>
    __TUPLE_ANNOTATION
    tuple& operator=(const tuple<UTypes...>& other)
    {
      head_ = other.head_;
      tail_ = other.tail_;
      return *this;
    }


    template<class... UTypes>
    __TUPLE_ANNOTATION
    tuple& operator=(tuple<UTypes...>&& other)
    {
      head_ = std::move(other.head_);
      tail_ = std::move(other.tail_);
      return *this;
    }


    template<class UType1, class UType2,
             class = typename std::enable_if<
               (sizeof...(Types) == 1) &&
               std::is_assignable<Type1,const UType1&>::value // XXX fill in the rest of these
             >::type>
    __TUPLE_ANNOTATION
    tuple& operator=(const __TUPLE_NAMESPACE::pair<UType1,UType2>& p)
    {
      head_ = p.first;
      tail_ = tuple<UType2>(p.second);
      return *this;
    }


    template<class UType1, class UType2,
             class = typename std::enable_if<
               (sizeof...(Types) == 1) &&
               std::is_assignable<Type1,UType1&&>::value // XXX fill in the rest of these
             >::type>
    __TUPLE_ANNOTATION
    tuple& operator=(__TUPLE_NAMESPACE::pair<UType1,UType2>&& p)
    {
      head_ = std::move(p.first);
      tail_ = std::move(tuple<UType2>(std::move(p.second)));
      return *this;
    }
      

    __TUPLE_ANNOTATION
    void swap(tuple& other)
    {
      using std::swap;

      swap(head_, other.head_);
      tail_.swap(other.tail_);
    }

    
    template<class... UTypes >
    __TUPLE_ANNOTATION
    bool operator==(const tuple<UTypes...>& rhs ) const
    {
        return head_ == rhs.head_ && tail_ == rhs.tail_;
    }


    template<class... UTypes >
    __TUPLE_ANNOTATION
    bool operator!=(const tuple<UTypes...>& rhs ) const
    {
        return head_ != rhs.head_ || tail_ != rhs.tail_;
    }


    template<class... UTypes >
    __TUPLE_ANNOTATION
    bool operator<(const tuple<UTypes...>& rhs ) const
    {
      return (head_ < rhs.head_)  ||
                (!(rhs.head_ < head_) &&
                 tail_ < rhs.tail_);

    }


  private:
    Type1 head_;
    tuple<Types...> tail_;


    template<class... UTypes>
    friend class tuple;


    // mutable get
    template<size_t i>
    __TUPLE_ANNOTATION
    typename __TUPLE_NAMESPACE::tuple_element<i, tuple>::type&
    mutable_get_impl(std::true_type)
    {
      return head_;
    }

    template<size_t i>
    __TUPLE_ANNOTATION
    typename __TUPLE_NAMESPACE::tuple_element<i,tuple>::type&
    mutable_get_impl(std::false_type)
    {
      return tail_.template mutable_get<i-1>();
    }


    // const get
    template<size_t i>
    __TUPLE_ANNOTATION
    const typename __TUPLE_NAMESPACE::tuple_element<i, tuple>::type&
    const_get_impl(std::true_type) const
    {
      return head_;
    }

    template<size_t i>
    __TUPLE_ANNOTATION
    const typename __TUPLE_NAMESPACE::tuple_element<i,tuple>::type&
    const_get_impl(std::false_type) const
    {
      return tail_.template const_get<i-1>();
    }


    // move get
    template<size_t i>
    __TUPLE_ANNOTATION
    typename __TUPLE_NAMESPACE::tuple_element<i, tuple>::type&&
    move_get_impl(std::true_type) &&
    {
      return std::forward<typename __TUPLE_NAMESPACE::tuple_element<i,tuple>::type&&>(head_);
    }

    template<size_t i>
    __TUPLE_ANNOTATION
    typename __TUPLE_NAMESPACE::tuple_element<i,tuple>::type&&
    move_get_impl(std::false_type) &&
    {
      return std::move(tail_).template move_get<i-1>();
    }

  protected:
    template<size_t i>
    __TUPLE_ANNOTATION
    typename __TUPLE_NAMESPACE::tuple_element<i, tuple>::type&
    mutable_get()
    {
      return mutable_get_impl<i>(std::integral_constant<bool, i == 0>());
    }


    template<size_t i>
    __TUPLE_ANNOTATION
    const typename __TUPLE_NAMESPACE::tuple_element<i, tuple>::type&
    const_get() const
    {
      return const_get_impl<i>(std::integral_constant<bool, i == 0>());
    }


    template<size_t i>
    __TUPLE_ANNOTATION
    typename __TUPLE_NAMESPACE::tuple_element<i, tuple>::type&&
    move_get() &&
    {
      return std::move(*this).template move_get_impl<i>(std::integral_constant<bool, i == 0>());
    }


  public:
    template<size_t i, class... UTypes>
    friend __TUPLE_ANNOTATION
    typename __TUPLE_NAMESPACE::tuple_element<i, __TUPLE_NAMESPACE::tuple<UTypes...>>::type &
    __TUPLE_NAMESPACE::get(__TUPLE_NAMESPACE::tuple<UTypes...>& t);


    template<size_t i, class... UTypes>
    friend __TUPLE_ANNOTATION
    const typename __TUPLE_NAMESPACE::tuple_element<i, __TUPLE_NAMESPACE::tuple<UTypes...>>::type &
    __TUPLE_NAMESPACE::get(const __TUPLE_NAMESPACE::tuple<UTypes...>& t);


    template<size_t i, class... UTypes>
    friend __TUPLE_ANNOTATION
    typename __TUPLE_NAMESPACE::tuple_element<i, __TUPLE_NAMESPACE::tuple<UTypes...>>::type &&
    __TUPLE_NAMESPACE::get(__TUPLE_NAMESPACE::tuple<UTypes...>&& t);
};


template<class Type1>
class tuple<Type1>
{
  public:
    __TUPLE_ANNOTATION
    tuple() : head_() {}


    __TUPLE_ANNOTATION
    explicit tuple(const Type1& arg1)
      : head_(arg1)
    {}


    template<class UType1,
             class = typename std::enable_if<
               std::is_constructible<Type1,UType1&&>::value // XXX fill in the rest of these
             >::type>
    __TUPLE_ANNOTATION
    explicit tuple(UType1&& arg1)
      : head_(std::forward<UType1>(arg1))
    {}


    template<class UType1,
             class = typename std::enable_if<
               std::is_constructible<Type1,const UType1&>::value // XXX fill in the rest of these
             >::type>
    __TUPLE_ANNOTATION
    tuple(const tuple<UType1>& other)
      : head_(other.head_)
    {}


    template<class UType1,
             class = typename std::enable_if<
               std::is_constructible<Type1,UType1&&>::value // XXX fill in the rest of these
             >::type>
    __TUPLE_ANNOTATION
    tuple(tuple<UType1>&& other)
      : head_(std::move(other.head_))
    {}


    __TUPLE_ANNOTATION
    tuple(const tuple& other)
      : head_(other.head_)
    {}


    __TUPLE_ANNOTATION
    tuple(tuple&& other)
      : head_(std::forward<Type1>(other.head_))
    {}


  private:
    template<class UType>
    __TUPLE_ANNOTATION
    static tuple<UType&&> forward_as_tuple(UType&& arg)
    {
      return tuple<UType&&>(std::forward<UType>(arg));
    }


  public:
    __TUPLE_ANNOTATION
    tuple& operator=(const tuple& other)
    {
      head_ = other.head_;
      return *this;
    }


    __TUPLE_ANNOTATION
    tuple& operator=(tuple&& other)
    {
      head_ = std::move(other.head_);
      return *this;
    }


    template<class UType>
    __TUPLE_ANNOTATION
    tuple& operator=(const tuple<UType>& other)
    {
      head_ = other.head_;
      return *this;
    }


    template<class UType>
    __TUPLE_ANNOTATION
    tuple& operator=(tuple<UType>&& other)
    {
      head_ = std::move(other.head_);
      return *this;
    }
      

    __TUPLE_ANNOTATION
    void swap(tuple& other)
    {
      using std::swap;

      swap(head_, other.head_);
    }

    
    template<class UType >
    __TUPLE_ANNOTATION
    bool operator==(const tuple<UType>& rhs ) const
    {
      return head_ == rhs.head_;
    }


    template<class UType >
    __TUPLE_ANNOTATION
    bool operator!=(const tuple<UType>& rhs ) const
    {
      return head_ != rhs.head_;
    }


    template<class UType >
    __TUPLE_ANNOTATION
    bool operator<(const tuple<UType>& rhs ) const
    {
      return (head_ < rhs.head_);
    }


  private:
    Type1 head_;

    template<class... UTypes>
    friend class tuple;


    // mutable get
    template<size_t i>
    __TUPLE_ANNOTATION
    typename __TUPLE_NAMESPACE::tuple_element<i, tuple>::type&
    mutable_get_impl(std::true_type)
    {
      return head_;
    }


    // const get
    template<size_t i>
    __TUPLE_ANNOTATION
    const typename __TUPLE_NAMESPACE::tuple_element<i, tuple>::type&
    const_get_impl(std::true_type) const
    {
      return head_;
    }


    // move get
    template<size_t i>
    __TUPLE_ANNOTATION
    typename __TUPLE_NAMESPACE::tuple_element<i, tuple>::type&&
    move_get_impl(std::true_type) &&
    {
      return std::forward<typename __TUPLE_NAMESPACE::tuple_element<i,tuple>::type&&>(head_);
    }


  protected:
    template<size_t i>
    __TUPLE_ANNOTATION
    typename __TUPLE_NAMESPACE::tuple_element<i, tuple>::type&
    mutable_get()
    {
      return mutable_get_impl<i>(std::integral_constant<bool, i == 0>());
    }


    template<size_t i>
    __TUPLE_ANNOTATION
    const typename __TUPLE_NAMESPACE::tuple_element<i, tuple>::type&
    const_get() const
    {
      return const_get_impl<i>(std::integral_constant<bool, i == 0>());
    }


    template<size_t i>
    __TUPLE_ANNOTATION
    typename __TUPLE_NAMESPACE::tuple_element<i, tuple>::type&&
    move_get() &&
    {
      return std::move(*this).template move_get_impl<i>(std::integral_constant<bool, i == 0>());
    }


  public:
    template<size_t i, class... UTypes>
    friend __TUPLE_ANNOTATION
    typename __TUPLE_NAMESPACE::tuple_element<i, __TUPLE_NAMESPACE::tuple<UTypes...>>::type &
    __TUPLE_NAMESPACE::get(__TUPLE_NAMESPACE::tuple<UTypes...>& t);


    template<size_t i, class... UTypes>
    friend __TUPLE_ANNOTATION
    const typename __TUPLE_NAMESPACE::tuple_element<i, __TUPLE_NAMESPACE::tuple<UTypes...>>::type &
    __TUPLE_NAMESPACE::get(const __TUPLE_NAMESPACE::tuple<UTypes...>& t);


    template<size_t i, class... UTypes>
    friend __TUPLE_ANNOTATION
    typename __TUPLE_NAMESPACE::tuple_element<i, __TUPLE_NAMESPACE::tuple<UTypes...>>::type &&
    __TUPLE_NAMESPACE::get(__TUPLE_NAMESPACE::tuple<UTypes...>&& t);
};


template<> class tuple<>
{
public:
  __TUPLE_ANNOTATION
  void swap(tuple& other)
  {
  }

   __TUPLE_ANNOTATION
  bool operator==(const tuple<>& rhs ) const
  {
    return true;
  }

  __TUPLE_ANNOTATION
  bool operator!=(const tuple<>& rhs ) const
  {
      return false;
  }

  __TUPLE_ANNOTATION
  bool operator<(const tuple<>& rhs ) const
  {
    return true;
  }
};


template<class... Types>
__TUPLE_ANNOTATION
void swap(__TUPLE_NAMESPACE::tuple<Types...>& lhs, __TUPLE_NAMESPACE::tuple<Types...>& rhs)
{
  lhs.swap(rhs);
}


template<class... Types>
__TUPLE_ANNOTATION
tuple<typename std::decay<Types>::type...> make_tuple(Types&&... args)
{
  return tuple<typename std::decay<Types>::type...>(std::forward<Types>(args)...);
}


template<class... Types>
__TUPLE_ANNOTATION
tuple<Types&...> tie(Types&... args)
{
  return tuple<Types&...>(args...);
}

//} // end namespace


// implement __TUPLE_NAMESPACE::get()
//namespace std
//{


template<size_t i, class... UTypes>
__TUPLE_ANNOTATION
typename __TUPLE_NAMESPACE::tuple_element<i, __TUPLE_NAMESPACE::tuple<UTypes...>>::type &
  get(__TUPLE_NAMESPACE::tuple<UTypes...>& t)
{
  return t.template mutable_get<i>();
}


template<size_t i, class... UTypes>
__TUPLE_ANNOTATION
const typename __TUPLE_NAMESPACE::tuple_element<i, __TUPLE_NAMESPACE::tuple<UTypes...>>::type &
  get(const __TUPLE_NAMESPACE::tuple<UTypes...>& t)
{
  return t.template const_get<i>();
}


template<size_t i, class... UTypes>
__TUPLE_ANNOTATION
typename __TUPLE_NAMESPACE::tuple_element<i, __TUPLE_NAMESPACE::tuple<UTypes...>>::type &&
  get(__TUPLE_NAMESPACE::tuple<UTypes...>&& t)
{
  return std::move(t).template move_get<i>();
}


} // end std

#include <thrust/detail/tuple/tuple_cat.h>

#ifdef __TUPLE_ANNOTATION_NEEDS_UNDEF
#undef __TUPLE_ANNOTATION
#undef __TUPLE_ANNOTATION_NEEDS_UNDEF
#endif

