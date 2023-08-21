// 实现迭代器
#include <cstddef>
#include "type_traits.h"

namespace mystl{
    
// 五种迭代器类型
// 输入迭代器：只读不写，单遍扫描，只能递增
struct input_iterator_tag{};
// 输出迭代器：只写不读，单遍扫描，只能递增
struct output_iterator_tag{};
// 前向迭代器：可读写，多遍扫描，只能递增
struct forward_iterator_tag: public input_iterator_tag{};
// 双向迭代器：可读写，多遍扫描，可递增递减
struct bidirectional_iterator_tag: public forward_iterator_tag{};
// 随机访问迭代器：可读写，多遍扫描，支持全部迭代器运算
struct random_access_iterator_tag: public bidirectional_iterator_tag{};

// iterator模板，提供5中类型
    // ptrdiff_t 是 C++ 标准库中定义的一个整数类型，用于表示两个指针之间的距离或差值。
template <class Category, class T, class Distance=ptrdiff_t, class Pointer=T*, class Reference=T&>
struct iterator
{
    typedef Category iterator_category;
    typedef T value_type;
    typedef Pointer pointer;
    typedef Reference reference;
    typedef Distance difference_type;
};

// iterator traits解决非泛化指针的特征获取问题，一个中间层
// 检测类型T是否具有iterator_category类型成员
template <class T>
struct has_iterator_car
{
private:
    struct two {char a; char b;};
    // 匹配任何参数类型
    template <class U> static two test(...);
    // 只有当U具有iterator_category成员类型时才能匹配，=0是用于触发函数匹配的一种技巧
    template <class U> static char test(typename U::iterator_category*=0);
public:
    // sizeof也可以对一个函数调用求值，其结果是函数返回类型的大小，函数并不会被调用
    // 0 可以隐式地转换为各种类型，所以它可以用来触发不同的函数模板匹配
    static const bool value=sizeof(test<T>(0))==sizeof(char);
};

template <class Iterator, bool>
struct iterator_traits_iml{};

}