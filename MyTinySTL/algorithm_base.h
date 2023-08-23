#ifndef MYTINYSTL_ALGOBASE_H_
#define MYTINYSTL_ALGOBASE_H_

#include <cstring>
#include "util.h"
#include "iterator.h"

namespace mystl
{
    
// 取消max和min的宏定义
#ifdef max
#pragma message("#undefing macro max") //发出提示信息“取消max的宏定义”
#undef max
#endif

#ifdef min
#pragma message("#undefing macro min") 
#undef min
#endif

// 1.max：比较两个参数的大小，返回较大值，相等时返回第一个值
template <class T>
const T& max(const T& lhs, const T& rhs){
    return lhs<rhs? rhs:lhs;
}

// 1.1自定义比较函数的max
    // 比较函数按小于来写
template <class T, class Compare>
const T& max(const T& lhs, const T& rhs, Compare comp){
    return comp(lhs,rhs)? rhs:lhs;
}

// 2.min：比较两个参数的大小，返回较小值，相等时返回第一个值
template <class T>
const T& min(const T& lhs, const T& rhs){
    return rhs<lhs? rhs:lhs;
}

// 2.1自定义比较函数的min
    // 比较函数也是小于
template <class T, class Compare>
const T& min(const T& lhs, const T& rhs, Compare comp){
    return comp(rhs,lhs)? rhs:lhs;
}

// 3.iter_swap：将两个迭代器所指对象对调
template <class FIter1, class FIter2>
void iter_swap(FIter1 lhs, FIter2 rhs){
    mystl::swap(*lhs,*rhs);
}

// 4.copy：把被拷贝容器[first,last)区间内的元素拷贝到指定容器中
// 针对input_iterator_tag的重载
template <class InputIter, class OutputIter>
OutputIter unchecked_copy_cat(InputIter first, InputIter last, OutputIter result, mystl::input_iterator_tag){
    for(;first!=last; first++,result++){
        *result=*first;
    }
    return result;
}
// 针对random_access_iterator_tag的重载
template <class RandomIter, class OutputIter>
OutputIter unchecked_copy_cat(RandomIter first, RandomIter last, OutputIter result, mystl::random_access_iterator_tag){
    // 速度快于input_iterator_tag的拷贝
    for(auto n=last-first; n>0; n--,first++,result++){
        *result=*first;
    }
    return result;
}
// 根据迭代器类型调用对应函数，复制迭代器位置上的值
template <class InputIter, class OutputIter>
OutputIter unchecked_copy(InputIter first, InputIter last, OutputIter result){
    return unchecked_copy_cat(first,last,result,iterator_category(first));
} 

// 为trivially_copy_assignable 类型提供特化版本
    // 使用类型特征萃取和 SFINAE 技术，利用了编译期进行类型检查，以便在符合条件的情况下使用更高效的 memmove 函数进行复制
    // SFINAE（Substitution Failure Is Not An Error）： 是一种模板元编程技术，允许在编译期根据类型的条件进行选择和重载。
        // 当在模板实例化时发生类型替换失败（例如，不满足某些条件时），编译器不会抛出错误，而是会继续寻找其他可行的模板实例化。
template <class Tp,class Up>
typename std::enable_if<
std::is_same<typename std::remove_const<Tp>::type, Up>::value && 
std::is_trivially_copy_assignable<Up>::value,Up*
>::type unchecked_copy(Tp* first,Tp* last, Up* result){
    // std::enable_if 是一个模板元函数，在编译时条件为真则返回第二个类型参数
    // 在这里，我们检查 Tp 是否去除了 const 修饰后和 Up 是否相同，以及 Up 是否可以进行平凡的拷贝赋值
    // 如果满足，则调用memmove函数，将一块内存从一个位置复制到另一个位置，速度最快
    const auto n=static_cast<size_t>(last-first);
    if(n!=0){
        std::memmove(result,first,n*sizeof(Up));
    }
    return result+n;
}

template <class InputIter, class OutputIter>
OutputIter copy(InputIter first, InputIter last, OutputIter result){
    return unchecked_copy(first,last,result);
}

} // namespace mystl

#endif
