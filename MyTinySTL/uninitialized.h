#ifndef MYTINYSTL_UNINITIALIZED_H_
#define MYTINYSTL_UNINITIALIZED_H_

// 这个文件用于对为初始化空间构造元素

#include "algorithm_base.h"
#include "construct.h"
#include "iterator.h"
#include "type_traits.h"
#include "util.h"

namespace mystl
{

// 1.uninitialized_copy把 [first, last) 上的内容复制到以 result 为起始处的空间，返回复制结束的位置
// 迭代器为可平凡拷贝赋值时，调用copy函数
template <class InputIter, class ForwardIter>
ForwardIter unchecked_uninit_copy(InputIter first, InputIter last, ForwardIter result, std::true_type){
    return mystl::copy(first,last,result);
}

// 迭代器不能平凡拷贝赋值时，挨个进行深拷贝
template <class InputIter, class ForwardIter>
ForwardIter unchecked_uninit_copy(InputIter first, InputIter last, ForwardIter result, std::false_type){
    auto cur=result;
    try{
        for(;first!=last; ++first, ++cur){
            mystl::construct(&*cur, *first);
        }
    }
    catch(...){ //发生异常时的回滚过程，遍历从已构造位置 cur 到初始位置 result，对已构造的对象执行析构操作
        for(;result!=cur; --cur){
            mystl::destroy(&*cur);
        }
    }
    return cur;
}

template <class InputIter, class ForwardIter>
ForwardIter unintialized_copy(InputIter first, InputIter last, ForwardIter result){
    return mystl::unchecked_uninit_copy(first,last,result,
        std::is_trivially_copy_assignable<typename iterator_traits<ForwardIter>::value_type>{});
    // {}创建一个类型为 bool 的临时对象，传递给unchecked_uninit_copy模板函数
}


// 2.uninitialized_copy_n 把 [first, first + n) 上的内容复制到以 result 为起始处的空间，返回复制结束的位置
template <class InputIter, class Size, class ForwardIter>
ForwardIter unchecked_uninit_copy_n(InputIter first, Size n, ForwardIter result, std::true_type){
    return mystl::copy_n(first, n, result).second;
}

template <class InputIter, class Size, class ForwardIter>
ForwardIter unchecked_uninit_copy_n(InputIter first, Size n, ForwardIter result, std::false_type){
    auto cur=result;
    try{
        for(; n>0; --n,++cur,++first){
            mystl::construct(&*cur, *first);
        }
    }
    catch(...){
        for(; result!=cur; --cur){
            mystl::destroy(&*cur);
        }
    }
}

template <class InputIter, class Size, class ForwardIter>
ForwardIter uninitialized_copy_n(InputIter first, Size n, ForwardIter result){
    return mystl::unchecked_uninit_copy_n(first, n, result, 
        std::is_trivially_copy_assignable<typename iterator_traits<InputIter>::value_type{});
}

// 3.uninitialized_fill：在 [first, last) 区间内填充元素值
template <class ForwardIter, class T>
void unchecked_uninit_fill(ForwardIter first, ForwardIter last, const T& value, std::true_type){
    mystl::fill(first,last,value);
}

template <class ForwardIter, class T>
void unchecked_uninit_fill(ForwardIter first, ForwardIter last, const T& value, std::false_type){
    auto cur=first;
    try(
        for(; cur!=last; ++cur){
            mystl::construct(&*cur,value);
        }
    )
    catch(...){
        for(; first!=cur;++first){
            mystl::destroy(&*fisrt);
        }
    }
}

template <class ForwardIter, class T>
void uninitialized_fill(ForwardIter first, ForwardIter last, const T& value){
    mystl::unchecked_uninit_fill(first,last,value,
        std::is_trivially_copy_assignable<typename iterator_traits<ForwardIter>::value_type{});
}

// 4.uninitialized_fill_n：从 first 位置开始，填充 n 个元素值，返回填充结束的位置
template <class ForwardIter, class Size, class T>
ForwardIter unchecked_uninit_fill_n(ForwardIter first, Size n, const T& value, std::true_type){
    return mystl::fill_n(first, n, value);
}

template <class ForwardIter, class Size, class T>
ForwardIter unchecked_uninit_fill_n(ForwardIter first, Size n, const T& value, std::false_type){
    auto cur = first;
    try{
        for (; n > 0; --n, ++cur){
        mystl::construct(&*cur, value);
        }
    }
    catch (...){
        for (; first != cur; ++first)
        mystl::destroy(&*first);
    }
    return cur;
}

template <class ForwardIter, class Size, class T>
ForwardIter uninitialized_fill_n(ForwardIter first, Size n, const T& value){
    return mystl::unchecked_uninit_fill_n(first, n, value, 
        std::is_trivially_copy_assignable<typename iterator_traits<ForwardIter>::value_type>{});
}

// 5.uninitialized_move：把[first, last)上的内容移动到以 result 为起始处的空间，返回移动结束的位置
template <class InputIter, class ForwardIter>
ForwardIter unchecked_uninit_move(InputIter first, InputIter last, ForwardIter result, std::true_type){
    return mystl::move(first, last, result);
}

template <class InputIter, class ForwardIter> 
ForwardIter unchecked_uninit_move(InputIter first, InputIter last, ForwardIter result, std::false_type){
    ForwardIter cur = result;
    try{
        for (; first != last; ++first, ++cur){
        mystl::construct(&*cur, mystl::move(*first));
        }
    }
    catch (...){
        mystl::destroy(result, cur);
    }
    return cur;
}

template <class InputIter, class ForwardIter>
ForwardIter uninitialized_move(InputIter first, InputIter last, ForwardIter result){
    return mystl::unchecked_uninit_move(first, last, result,
        std::is_trivially_move_assignable<typename iterator_traits<InputIter>::value_type>{});
}


// 6.uninitialized_move_n：把[first, first + n)上的内容移动到以 result 为起始处的空间，返回移动结束的位置
template <class InputIter, class Size, class ForwardIter>
ForwardIter unchecked_uninit_move_n(InputIter first, Size n, ForwardIter result, std::true_type){
    return mystl::move(first, first + n, result);
}

template <class InputIter, class Size, class ForwardIter>
ForwardIter unchecked_uninit_move_n(InputIter first, Size n, ForwardIter result, std::false_type){
    auto cur = result;
    try{
        for (; n > 0; --n, ++first, ++cur){
        mystl::construct(&*cur, mystl::move(*first));
        }
    }
    catch (...){
        for (; result != cur; ++result)
        mystl::destroy(&*result);
        throw;
    }
    return cur;
}

template <class InputIter, class Size, class ForwardIter>
ForwardIter uninitialized_move_n(InputIter first, Size n, ForwardIter result){
    return mystl::unchecked_uninit_move_n(first, n, result,
        std::is_trivially_move_assignable<typename iterator_traits<InputIter>::value_type>{});
}

} // namespace mystl

#endif