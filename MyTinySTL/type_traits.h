#ifndef MYTINYSTL_TYPE_TRAITS_H_
#define MYTINYSTL_TYPE_TRAITS_H_

// 引入标准库的type_traits，用于操作类型
#include <type_traits>

namespace mystl
{

template <class T, T v>
struct m_integral_constant
{
    static constexpr T value=v;
    //编译时常量，而且在运行时具有不变性
};

// 用using关键字来定义类型模板的别名，同时保持可变性和固定性
template <bool b>
using m_bool_constant=m_integral_constant<bool,b>;

typedef m_bool_constant<true> m_true_type;
typedef m_bool_constant<false> m_false_type;

// type traits
template <class T1, class T2>
struct pair;

template <class T>
struct is_pair: mystl::m_false_type {};

template <class T1, class T2>
struct is_pair<mystl::pair<T1, T2>> : mystl::m_true_type {};

}

#endif
