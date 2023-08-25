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
        // 第一个参数：目标内存块的指针，表示数据将被移动到的位置。
        // 第二个参数：源内存块的指针，表示要被移动的数据的起始位置。
        // 第三个参数：要移动的字节数。
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


// 5.copy_backward：将 [first, last)区间内的元素拷贝到 [result - (last - first), result)内
// bidirectional_iterator_tag（针对bidirectional_iterator_tag及以上的迭代器）
template <class BidirectionalIter1, class BidirectionalIter2>
BidirectionalIter2 unchecked_copy_backward_cat(BidirectionalIter1 first, BidirectionalIter1 last, BidirectionalIter2 result, mystl::bidirectional_iterator_tag){
    while(first!=last){
        // 前--是因为右开
        *--result=*--last;
    }
    return result;
}

template <class BidirectionalIter1, class BidirectionalIter2>
BidirectionalIter2 unchecked_copy_backward(BidirectionalIter1 first, BidirectionalIter1 last, BidirectionalIter2 result){
    return unchecked_copy_backward_cat(first,last,result,iterator_category(first));
}

// trivially_copy_assignable特化版本
template <class Tp, class Up>
typename std::enable_if<
    std::is_same<typename std::remove_const<Tp>::type,Up>::value &&
    std::is_trivially_copy_assignable<Up>::value, Up*>::type
    unchecked_copy_backward(Tp* first, Tp* last, Up* result){
    const auto n=static_cast<size_t>(last-first);
    if(n!=0){
        result-=n; //起点
        std::memmove(result, first, n*sizeof(Up));
    }
    return result;
}

template <class BidirectionalIter1, class BidirectionalIter2>
BidirectionalIter2 copy_backward(BidirectionalIter1 first, BidirectionalIter1 last, BidirectionalIter2 result){
    return unchecked_copy_backward(first,last,result);
}

// 6.copy_if：把[first, last)内满足一元操作 unary_pred 的元素拷贝到以 result 为起始的位置上
template <class InputIter, class OutputIter, class UnaryPredicate>
OutputIter copy_if(InputIter first, InputIter last, OutputIter result, UnaryPredicate unary_pred){
    for(; first!=last; ++first){
        if(unary_pred(*first)){
            // 注意result++不能放在for循环中，因为可能存在不满足条件的元素
            *result++=*first;
        }
    }
    return result;
}

// 7.copy_n：把 [first, first + n)区间上的元素拷贝到 [result, result + n)上，返回一个 pair 分别指向拷贝结束的尾部
template <class InputIter, class Size, class OutputIter>
mystl::pair<InputIter, OutputIter> unchecked_copy_n(InputIter fisr, Size n, OutputIter result, mystl::input_iterator_tag){
    for(; n>0;--n, ++first, ++result){
        *result=*first;
    }
    return mystl::pair<InputIter, OutputIter>(fisrt,result);
}

// random_access_iterator_tag特化版本
template <class RandomIter, class Size, class OutputIter>
mystl::pair<RandomIter, OutputIter> unchecked_copy_n(RandomIter first, Size n, OutputIter result, mystl::random_access_iterator_tag){
    auto last=first+n;
    return mystl::pair<RandomIter, OutputIter>(last, mystl::copy(first, last, result));
}

template <class InputIter, class Size, class OutputIter>
mystl::pair<InputIter, OutputIter> copy_n(InputIter first, Size n, OutputIter result){
    return unchecked_copy_n(first, n,result,iterator_category(first));
}

// 8.move：把 [first, last)区间内的元素移动到 [result, result + (last - first))内
// input_iterator_tag
template <class InputIter, class OutputIter>
OutputIter unchecked_move_cat(InputIter first, InputIter last, OutputIter result, mystl::input_iterator_tag){
    for(; first!=last; ++first, ++result){
        *result=mystl::move(*first);
    }
    return result;
}

// random_access_iterator_tag特化版本
template <class RandomIter, class OutputIter>
OutputIter unchecked_move_cat(RandomIter first, RandomIter last, OutputIter result, mystl::random_access_iterator_tag){
    for(auto n=last-first; n>0; --n,++fisrt,++result){
        *result=mystl::move(*first);
    }
    return result;
}

template <class InputIter, class OutputIter>
OutputIter unchecked_move(InputIter first, InputIter last, OutputIter result){
    return unchecked_move_cat(first, last,result,iterator_category(first));
}

// trivially_copy_assignable特化版本
template <class Tp, class Up>
typename std::enable_if<
    std::is_same<typename std::remove_const<Tp>::type, Up>::value &&
    std::is_trivially_move_assignable<Up>::value,Up*>::type
    unchecked_move(Tp* first, Tp* last, Up* result){
    const size_t n=static_cast<size_t>(last-first);
    if(n!=0){
        std::memmove(result,first,n*sizeof(Up));
    }
    return result+n;
}

template <class InputIter, class OutputIter>
OutputIter move(InputIter first, InputIter last, OutputIter result){
    return unchecked_move(first, last, result);
}


// 9.move_backward：将 [first, last)区间内的元素移动到 [result - (last - first), result)内
// bidirectional_iterator_tag 版本
template <class BidirectionalIter1, class BidirectionalIter2>
BidirectionalIter2 unchecked_move_backward_cat(BidirectionalIter1 first, BidirectionalIter1 last,BidirectionalIter2 result, mystl::bidirectional_iterator_tag){
    while (first != last)
        *--result = mystl::move(*--last);
    return result;
}

// random_access_iterator_tag 版本
template <class RandomIter1, class RandomIter2> 
RandomIter2 unchecked_move_backward_cat(RandomIter1 first, RandomIter1 last,RandomIter2 result, mystl::random_access_iterator_tag){
    for (auto n = last - first; n > 0; --n)
        *--result = mystl::move(*--last);
    return result;
}

template <class BidirectionalIter1, class BidirectionalIter2>
BidirectionalIter2 unchecked_move_backward(BidirectionalIter1 first, BidirectionalIter1 last, BidirectionalIter2 result){
  return unchecked_move_backward_cat(first, last, result,iterator_category(first));
}

// 为 trivially_copy_assignable 类型提供特化版本
template <class Tp, class Up>
typename std::enable_if<
    std::is_same<typename std::remove_const<Tp>::type, Up>::value &&
    std::is_trivially_move_assignable<Up>::value,Up*>::type
    unchecked_move_backward(Tp* first, Tp* last, Up* result){
    const size_t n = static_cast<size_t>(last - first);
    if (n != 0)
    {
        result -= n;
        std::memmove(result, first, n * sizeof(Up));
    }
    return result;
}

template <class BidirectionalIter1, class BidirectionalIter2>
BidirectionalIter2 move_backward(BidirectionalIter1 first, BidirectionalIter1 last, BidirectionalIter2 result){
  return unchecked_move_backward(first, last, result);
}

// 10.equal：比较第一序列在 [first, last)区间上的元素值是否和第二序列相等
template <class InputIter1, class InputIter2>
bool equal(InputIter1 first1, InputIter1 last1, InputIter2 first2){
    for(; first1!=last1; ++first1, ++first2){
        if(*first1!=*first2){
            return false;
        }
    }
    return true;
}

// 重载版本使用函数对象 comp 代替比较操作
template <class InputIter1, class InputIter2, class Compared>
bool equal(InputIter1 first1, InputIter1 last1, InputIter2 first2, Compared comp){
    for(; first1!=last1; ++first1, ++first2){
        if(!comp(*first1,*first2)){
            return false;
        }
    }
    return true;
}

// 11.fill_n：从first位置开始填充n个相同值
template <class OutputIter, class Size, class T>
OutputIter unchecked_fill_n(OutputIter first, Size n, const T& value){
    for(; n>0; n--,++first){
        *first=value;
    }
    return first;
}

// 为 one-byte 类型提供特化版本，提高效率
template <class Tp, class Size, class Up>
typename std::enable_if<
    std::is_integral<Tp>::value && sizeof(Tp)==1 &&
    !std::is_same<Tp, bool>::value &&
    std::is_integral<Up>::value && sizeof(Up)==1, Tp*>::type
    // 是整数类型，并且大小为 1 字节，不是 bool 类型（如 char、signed char、unsigned char）
    unchecked_fill_n(Tp* first, Size n, Up value){
    if(n>0){
        // 对于某些操作，如初始化大型内存块，使用 memset 可能会比逐个赋值更高效，因为 memset 是高度优化的库函数。
        // 第一个参数指向要填充的内存块的起始地址，第二个参数是要填充的值，第三个参数是要填充的字节数。
        std::memset(first, (unsigned char)value, (size_t)(n));
    }
    return first+n;
}

template <class OutputIter, class Size, class T>
OutputIter fill_n(OutputIter first, Size n, const T& value){
    return unchecked_fill_n(first, n, value);
}

// 12.fill：为 [first, last)区间内的所有元素填充新值
template <class ForwardIter, class T>
void fill_cat(ForwardIter first, ForwardIter last, const T& value,mystl::forward_iterator_tag){
    for (; first != last; ++first){
        *first = value;
    }
}

template <class RandomIter, class T>
void fill_cat(RandomIter first, RandomIter last, const T& value,mystl::random_access_iterator_tag){
    fill_n(first, last - first, value);
}

template <class ForwardIter, class T>
void fill(ForwardIter first, ForwardIter last, const T& value){
    fill_cat(first, last, value, iterator_category(first));
}

// 13.lexicographical_compare: 字典序比较
// (1)如果第一序列的元素较小，返回 true ，否则返回 false
// (2)如果到达 last1 而尚未到达 last2 返回 true
// (3)如果到达 last2 而尚未到达 last1 返回 false
// (4)如果同时到达 last1 和 last2 返回 false
template <class InputIter1, class InputIter2>
bool lexicographical_compare(InputIter1 first1, InputIter1 last1,InputIter2 first2, InputIter2 last2){
    for (; first1 != last1 && first2 != last2; ++first1, ++first2)
    {
        // 等于就继续比较
        if (*first1 < *first2)
            return true;
        if (*first2 < *first1)
            return false;
    }
    return first1 == last1 && first2 != last2;
}

// 重载版本使用函数对象 comp 代替比较操作
template <class InputIter1, class InputIter2, class Compred>
bool lexicographical_compare(InputIter1 first1, InputIter1 last1,
    InputIter2 first2, InputIter2 last2, Compred comp){
    for (; first1 != last1 && first2 != last2; ++first1, ++first2)
    {
        if (comp(*first1, *first2))
            return true;
        if (comp(*first2, *first1))
            return false;
    }
    return first1 == last1 && first2 != last2;
}

// 针对 const unsigned char* 的特化版本
bool lexicographical_compare(const unsigned char* first1,const unsigned char* last1,
    const unsigned char* first2,const unsigned char* last2){
    const auto len1 = last1 - first1;
    const auto len2 = last2 - first2;
    // 先比较相同长度的部分
    const auto result = std::memcmp(first1, first2, mystl::min(len1, len2));
    // 若相等，长度较长的比较大
    return result != 0 ? result < 0 : len1 < len2;
}

// 14.mismatch：平行比较两个序列，找到第一处失配的元素，返回一对迭代器，分别指向两个序列中失配的元素
template <class InputIter1, class InputIter2>
mystl::pair<InputIter1, InputIter2> mismatch(InputIter1 first1, InputIter1 last1, InputIter2 first2){
    while (first1 != last1 && *first1 == *first2){
        ++first1;
        ++first2;
    }
    return mystl::pair<InputIter1, InputIter2>(first1, first2);
}

// 重载版本使用函数对象 comp 代替比较操作
template <class InputIter1, class InputIter2, class Compred>
mystl::pair<InputIter1, InputIter2> mismatch(InputIter1 first1, InputIter1 last1, InputIter2 first2, Compred comp){
    while (first1 != last1 && comp(*first1, *first2)){
        ++first1;
        ++first2;
    }
    return mystl::pair<InputIter1, InputIter2>(first1, first2);
}

} // namespace mystl

#endif
