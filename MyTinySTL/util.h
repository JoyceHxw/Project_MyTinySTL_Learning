// 引入标准库中的cstddef，用于内存操作、指针运算和数组处理等
#include <cstddef>
#include "type_traits.h"

namespace mystl{

// move：将左值转化为右值，即move(左值)是右值，进而支持移动语义（不分配新的内存，只是移动源对象，“窃取”）
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

// swap：使用了move函数来实现移动语义，从而在交换对象时避免进行不必要的深拷贝
    // 只是所有权名字发生改变，不用实际交付资产
template <class T>
void swap(T& p1, T& p2){
    auto tmp(mystl::move(p1));
    p1=mystl::move(p2);
    p2=mystl::move(tmp);
}

} // namespace mystl
