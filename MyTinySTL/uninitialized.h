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

// uninitialized_copy把 [first, last) 上的内容复制到以 result 为起始处的空间，返回复制结束的位置
template <class InputIter, class ForwardIter>
ForwardIter unchecked_uninit_copy(InputIter first, InputIter last, ForwardIter result, std::true_type){
    return mystl::copy(first,last,result);
}

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


// uninitialized_copy_n 把 [first, first + n) 上的内容复制到以 result 为起始处的空间，返回复制结束的位置
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


} // namespace mystl


#endif