#ifndef MYTINYSTL_ALLOCATOR_H_
#define MYTINYSTL_ALLOCATOR_H_

#include "util.h"
#include "construct.h"

namespace mystl
{

// 模板类声明
template <class T>
class allocator
{
public:
    typedef T           value_type;
    typedef T*          pointer;
    typedef const T*    const_pointer;
    typedef T&          reference;
    typedef const T&    const_reference;
    typedef size_t      size_type;
    typedef ptrdiff_t   difference_type;

public:
    // 内存配置
    static T* allocate();
    // 存储n个T对象
    static T* allocate(size_type n);
    
    // 内存释放
    static void deallocate(T* ptr);
    static void deallocate(T* ptr, size_type n);

    // 对象构造
    static void construct(T* ptr);
    static void construct(T* ptr, const T& value);
    static void construct(T* ptr, T&& value);
    // 不定参数函数模板
    template <class... Args>
    static void construct(T* ptr, Args&& ...args);

    // 对象析构
    static void destroy(T* ptr);
    static void destroy(T* first, T* last);
};

template <class T>
T* allocator<T>::allocate(){
    return static_cast<T*>(::operator new(sizeof(T)));
}

template <class T>
T* allocator<T>::allocate(size_type n){
    if(n==0){
        return nullptr;
    }
    return static_cast<T*>(::operator new(n * sizeof(T)));
}

template <class T>
void allocator<T>::deallocate(T* ptr){
    if(ptr==nullptr){
        return;
    }
    ::operator delete(ptr);
}

template <class T>
void allocator<T>::deallocate(T* ptr, size_type){
    if(ptr==nullptr){
        return;
    }
    ::operator delete(ptr);
}

template <class T>
void allocator<T>::construct(T* ptr){
    mystl::construct(ptr);
}

template <class T>
void allocator<T>::construct(T* ptr, const T& value){
    mystl::construct(ptr,value);
}

template <class T>
void allocator<T>::construct(T* ptr, T&& value){
    mystl::construct(ptr, mystl::move(value));
}

template <class T>
template <class ...Args>
void allocator<T>::construct(T* ptr, Args&& ...args){
    mystl::construct(ptr, mystl::forward<Args>(args)...);
}

template <class T>
void allocator<T>::destroy(T* ptr){
    mystl::detroy(ptr);
}

template <class T>
void allocator<T>::destroy(T* first, T* last){
    mystl::destroy(first,last);
}
} // namespace mystl

#endif