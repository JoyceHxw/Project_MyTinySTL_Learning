#ifndef MYTINYSTL_UTIL_H_
#define MYTINYSTL_UTIL_H_

// 引入标准库中的cstddef，用于内存操作、指针运算和数组处理等
#include <cstddef>
#include "type_traits.h"

namespace mystl{

// move：将左值转化为右值，即move(左值)是右值，进而支持移动语义（不分配新的内存，只是移动源对象，“窃取”，原来的指针不再使用）
    // 移动语义允许资源的所有权从一个对象转移到另一个对象，而无需进行深拷贝，提高性能和效率（将即将被销毁的右值保存下来，传递函数的返回值使用的就是移动语义）
template <class T>
typename std::remove_reference<T>::type&& move(T&& arg) noexcept{ 
    // typename是指定获取模板类的类型成员
    // remove_reference<T>::type 解除引用的模板元函数，得到原始类型
    // && 在返回类型中使用&&表示右值引用的返回类型
        // 区分左值和右值：左值持久，右值短暂；左值是被赋值的（变量是左值），右值是赋的值（如字面常量、表达式返回值、函数返回值），是临时的
    // T&& arg函数的参数是一个通用引用，T&& 表示可以同时匹配左值和右值（能够绑定到左值上），并根据传入的参数类型决定是左值引用还是右值引用（右值引用就接收为T，左值引用就接收为T&（引用折叠））。
    // noexcept：移动操作不应抛出任何异常
    return static_cast<typename std::remove_reference<T>::type&&>(arg);
    // 使用 static_cast 将 arg 转换为右值引用，并作为返回值返回
} 


// forward：实现完美转发
    // 根据引用折叠原理，将forward使用在右值引用函数参数上，转发保持被转发实参的所有性质（是否是const，是左值还是右值）
        // 右值引用函数参数接受实参（右值）后可能改变其性质（变成左值），因为函数参数的传递是左值表达式，左值和右值之间可能无法传递，需要用forward加上&&保持性质
template <class T>
T&& forward(typename std::remove_reference<T>::type& arg) noexcept{
    return static_cast<T&&>(arg);
}

template <class T>
T&& forward(typename std::remove_reference<T>::type&& arg) noexcept{
    static_assert(!std::is_lvalue_reference<T>::value, "bad forward");
    // 这个静态断言检查参数的值类别是否为左值引用。如果 T 是左值引用，就会触发断言
    return static_cast<T&&>(arg);
}


// swap：使用了move函数来实现移动语义，从而在交换对象时避免进行不必要的深拷贝
    // 只是所有权名字发生改变，不用实际交付资产
template <class T>
void swap(T& p1, T& p2){
    auto tmp(mystl::move(p1));
    p1=mystl::move(p2);
    p2=mystl::move(tmp);
}

// 交换某一范围内的数组元素
template <class ForwardIter1, class ForwardIter2>
ForwardIter2 swap_range(ForwardIter1 first1, ForwardIter1 last1, ForwardIter2 first2){
    for(; first1!=last1; ++first1, (void) ++first2){
        mystl::swap(*first1,*first2);
    }
    return first2;
}

template <class Tp, size_t N>
void swap(Tp(&a)[N], Tp(&b)[N]){
    mystl::swap_range(a,a+N,b);
}

// pair
template <class Ty1, class Ty2>
struct pair
{
    typedef Ty1 first_type;
    typedef Ty2 second_type;

    first_type first;
    second_type second;

    // 1.默认构造：在未提供任何参数的情况下创建一个空的pair对象，其中两个成员（first和second）也会被默认构造
    template <class Other1=Ty1, class Other2=Ty2,
        typename=typename std::enable_if<
            std::is_default_constructible<Other1>::value &&
            std::is_default_constructible<Other2>::value, void>::type>
    // 运用标准库的type_traits进行特性抽取，判断两种类型是否都是默认可构造的
    constexpr pair(): first(), second(){}
    // 使用 constexpr 的目的是将构造函数声明为在编译时可以确定其结果的函数，这是为了支持在编译期间进行条件检查。

    // 2.只接受 Ty1 和 Ty2 类型的参数，并进行值的拷贝构造
    // 隐式构造
    template <class U1=Ty1, class U2=Ty2,
        typename std::enable_if<
            std::is_copy_constructible<U1>::value &&
            std::is_copy_constructible<U2>::value &&
            std::is_convertible<const U1&, Ty1>::value &&
            std::is_convertible<const U2&, Ty2>::value, int>::type=0>
    // is_convertible检查是否可以将第一个类型转换为第二个类型
    constexpr pair(const Ty1& a, const Ty2& b):first(a),second(b){}
    // 参数类型为

    // 显式构造（不能发生类型隐式转换）
    template <class U1=Ty1, class U2=Ty2,
        typename std::enable_if<
            std::is_copy_constructible<U1>::value &&
            std::is_copy_constructible<U2>::vlaue &&
            (!std::is_convertible<const U1&, Ty1>::value ||
            !std::is_convertible<const U2&, Ty2>::value),int>::type=0>
    explicit constexpr pair(const Ty1& a, const Ty2& b):first(a),second(b){}
    // explicit的作用是表明该构造函数是显式的, 而非隐式的,防止类构造函数的隐式自动转换

    // 提供拷贝构造
    pair(const pair& rhs)=default;
    pair(pair& rhs)=default;
    
    // 3.支持从不同类型的参数构造对象，使用完美转发来保留参数的值类别
    // 隐式构造
    template <class Other1, class Other2,
        typename std::enable_if<
            std::is_constructible<Ty1, Other1>::value &&
            std::is_constructible<Ty2, Other2>::value &&
            std::is_convertible<Other1&&, Ty1>::value &&
            std::is_convertible<Other2&&, Ty2>::value, int>::type=0>
    // is_constructible检验是否可以通过将类型Other1作为参数传递给Ty1的构造函数来构造Ty1类型的对象
    constexpr pair(Other1&& a, Other2&& b):first(mystl::forward<Other1>(a)),second(mystl::forward<Other2>(b)){}

    // 显式构造
    template <class Other1, class Other2,
        typename std::enable_if<
            std::is_constructible<Ty1, Other1>::value &&
            std::is_constructible<Ty2, Other2>::value &&
            (!std::is_convertible<Other1, Ty1>::value ||
            !std::is_convertible<Other2, Ty2>::value), int>::type=0>
    explicit constexpr pair(Other1&& a, Other2&& b):first(mystl::forward<Other1>(a)),second(mystl::forward<Other2>(b)){}

    // 4.使用拷贝构造，支持从不同类型的 pair 对象构造当前的 pair 对象
    // 隐式构造
    template <class Other1, class Other2,
        typename std::enable_if<
            std::is_constructible<Ty1, const Other1&>::value &&
            std::is_constructible<Ty2, const Other2&>::value &&
            std::is_convertible<const Other1&, Ty1>::value &&
            std::is_convertible<const Other2&, Ty2>::value, int>::type=0>
    constexpr pair(const pair<Other1,Other2>& other): first(other.first),second(other.second){}

    // 显式构造
    template <class Other1, class Other2,
        typename std::enable_if<
            std::is_constructible<Ty1, const Other1&>::value &&
            std::is_constructible<Ty2, const Other2&>::value &&
            (!std::is_convertible<const Other1&, Ty1>::value ||
            !std::is_convertible<const Other2&, Ty2>::value), int>::type=0>
    explicit constexpr pair(const pair<Other1,Other2>& other):first(other.first),second(other.second){}

    // 5.使用完美转发
    // 隐式构造
    template <class Other1, class Other2,
        typename std::enable_if<
            std::is_constructible<Ty1, Other1>::value &&
            std::is_constructible<Ty2, Other2>::value &&
            std::is_convertible<Other1, Ty1>::value &&
            std::is_convertible<Other2, Ty2>::value, int>::type=0>
    constexpr pair(pair<Other1, Other2>&& other):first(mystl::forward<Other1>(other.first)),second(mystl::forward<Other2>(other.second)){}

    // 显式构造
    template <class Other1, class Other2,
        typename std::enable_if<
            std::is_constructible<Ty1, Other1>::value &&
            std::is_constructible<Ty2, Other2>::value &&
            (!std::is_convertible<Other1, Ty1>::value ||
            !std::is_convertible<Other2, Ty2>::value), int>::type=0>
    explicit constexpr pair(pair<Other1,Other2>&& other):first(mystl::forward<Other1>(other.first)),second(mystl::forward<Other2>(other.second)){}

    // 同类型赋值
    // 拷贝赋值（深拷贝）
    pair& operator=(const pair& rhs){
        if(this!=&rhs){
            first=rhs.first;
            second=rhs.second;
        }
        return *this;
    }

    // 移动赋值（浅拷贝）（将其他类型的对象转换成右值从而替代）
    pair& operator=(pair&& rhs){
        if(this!=&rhs){
            first=mystl::move(rhs.first);
            second=mystl::move(rhs.second);
        }
        return *this;
    }

    // 不同类型赋值
    // 拷贝赋值（深拷贝）
    template <class Other1, class Other2>
    pair& operator=(const pair<Other1,Other2>& other){
        first=other.first;
        second=other.second;
        return *this;
    }

    // 移动赋值（浅拷贝）（保留其他类型的对象）
    template <class Other1, class Other2>
    pair& operator=(pair<Other1,Other2>&& other){
        first=mystl::forward<Other1>(other.first);
        second=mystl::forward<Other2>(other.second);
        return *this;
    }
    
    ~pair()=default;

    // 提供swap函数
    void swap(pair& other){
        if(this!=&other){
            mystl::swap(first, other.first);
            mystl::swap(second,other.second);
        }
    }

};

// 重载比较操作符: 比较元素大小
template <class Ty1, class Ty2>
bool operator==(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs){
    return lhs.first==rhs.first && lhs.second==rhs.second;
}

template <class Ty1, class Ty2>
bool operator<(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs){
    return lhs.first<rhs.first || (lhs.first==rhs.first && lhs.second<rhs.second);
}

// 借助上面的函数取反
template <class Ty1, class Ty2>
bool operator!=(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs){
    return !(lhs==rhs);
}

template <class Ty1, class Ty2>
bool operator>(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs){
    return rhs<lhs;
}

template <class Ty1, class Ty2>
bool operator<=(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs){
    return !(rhs<lhs);
}

template <class Ty1, class Ty2>
bool operator>=(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs){
    return !(lhs<rhs);
}

// 重载swap
template <class Ty1, class Ty2>
void swap(pair<Ty1, Ty2>& lhs, pair<Ty1, Ty2>& rhs){
    lhs.swap(rhs);
}

// make_pair
template <class Ty1, class Ty2>
pair<Ty1, Ty2> make_pair(Ty1&& first, Ty2&& second){
    return pair<Ty1,Ty2>(mystl::forward<Ty1>(first), mystl::forward<Ty2>(second));
}

} // namespace mystl

#endif
