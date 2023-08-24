// 实现迭代器
#include <cstddef>
#include "type_traits.h"

namespace mystl{
    
// 五种迭代器类型，继承关系---------------------------------------------------------
struct input_iterator_tag{};
struct output_iterator_tag{};
struct forward_iterator_tag: public input_iterator_tag{};
struct bidirectional_iterator_tag: public forward_iterator_tag{};
struct random_access_iterator_tag: public bidirectional_iterator_tag{};

// iterator模板，提供5种类型
    // ptrdiff_t 是 C++ 标准库中定义的一个整数类型，用于表示两个指针之间的距离或差值。
template <class Category, class T, class Distance=ptrdiff_t, class Pointer=T*, class Reference=T&>
struct iterator
{
    typedef Category    iterator_category;
    typedef T           value_type;
    typedef Pointer     pointer;
    typedef Reference   reference;
    typedef Distance    difference_type;
};



// iterator traits解决非泛化指针的特征获取问题，一个中间层---------------------------------------------------------
// 注意理清泛化类型指针的判断和萃取逻辑顺序

// 1.2检测类型T是否具有iterator_category类型成员
template <class T>
struct has_iterator_cat
{
private:
    struct two {char a; char b;};
    // 匹配任何参数类型
    template <class U> static two test(...);
    // 只有当U具有iterator_category成员类型时才能匹配，=0是用于触发函数匹配的一种技巧
    template <class U> static char test(typename U::iterator_category* =0); //注意分开写* =
public:
    // sizeof也可以对一个函数调用求值，其结果是函数返回类型的大小，函数并不会被调用
    // 0 可以隐式地转换为各种类型，所以它可以用来触发不同的函数模板匹配
    static const bool value=sizeof(test<T>(0))==sizeof(char);
};

// 1.4iterator_traits_impl类型迭代器萃取接口
template <class Iterator, bool>
struct iterator_traits_impl{};

template <class Iterator>
struct iterator_traits_impl<Iterator, true>
{
    typedef typename Iterator::iterator_category iterator_catory;
    typedef typename Iterator::value_type        value_type;
    typedef typename Iterator::pointer           pointer;
    typedef typename Iterator::reference         reference;
    typedef typename Iterator::difference_type   difference_type;
};


// 1.3iterator_traits_helper
template <class Iterator, bool>
struct iterator_traits_helper{};

// 判断是否是input/output_iterator_tag(覆盖了所有迭代器类型)
// 调用iterator_traits_iml，对input/output_iterator_tag类型的迭代器进行萃取
template <class Iterator>
struct iterator_traits_helper<Iterator, true>: public iterator_traits_impl<Iterator,
    std::is_convertible<typename Iterator::iterator_category, input_iterator_tag>::value ||
    std::is_convertible<typename Iterator::iterator_category, output_iterator_tag>::value>{};


// 1.1iterator_traits从上述过程继承得到类型指针的迭代器
template <class Iterator>
struct iterator_traits: public iterator_traits_helper<Iterator, has_iterator_cat<Iterator>::value>{};

// 2针对原生指针的两个偏特化版本
// 将原生指针萃取为random_access_iterator_tag
template <class T>
struct iterator_traits<T*>
{
    typedef random_access_iterator_tag  iterator_category;
    typedef T value_type;
    typedef T* pointer;
    typedef T& reference;
    typedef ptrdiff_t difference_type;
};

template <class T>
struct iterator_traits<const T*>
{
    typedef random_access_iterator_tag iterator_category;
    typedef T value_type;
    typedef const T* pointer;
    typedef const T& reference;
    typedef ptrdiff_t difference_type;
};



// 特征萃取---------------------------------------------------------
// 判断迭代器类型的继承关系
template <class T, class U>
struct has_iterator_cat_of<T,U,false>:public m_false_type{};

template <class T, class U, bool=has_iterator_cat<iterator_traits<T>>::value>
struct has_iterator_cat_of: public m_bool_constant<std::is_convertible<
    typename iterator_traits<T>::iterator_category,U>::value{};

// 判断五种类型
template <class Iter>
struct is_input_iterator: public has_iterator_cat_of<Iter, input_iterator_tag>{};

template <class Iter>
struct is_output_iterator: public has_iterator_cat_of<Iter, output_iterator_tag>{};

template <class Iter>
struct is_forward_iterator: public has_iterator_cat_of<Iter, forward_iterator_tag>{};

template <class Iter>
struct is_bidirectional_iterator: public has_iterator_cat_of<Iter, bidirectional_iterator_tag>{};

template <class Iter>
struct is_random_access_iterator: public has_iterator_cat_of<Iter, random_access_iterator_tag>{};

// 判断是否是迭代器（两种父类型之一）
template <class Iterator>
struct is_iterator: public m_bool_constant<is_input_iterator<Iterator>::value ||
    is_output_iterator<Iterator>::value>{};

// 萃取迭代器类型
template <class Iterator>
typename iterator_traits<Iterator>::iterator_category iterator_category(const Iterator&){
    typedef typename iterator_traits<Iterator>::iterator_category Category;
    return Category();
}

// 萃取distance_type
template <class Iterator>
typename iterator_traits<Iterator>::difference_type* distance_type(const Iterator&){
    return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
}

// 萃取value_type
template <class Iterator>
typename iterator_traits<Iterator>::value_type* value_type(const Iterator&){
    return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
}

// distance计算迭代器之间的距离
// input_iterator_tag版本（只能逐步向前累加）
template <class InputIterator>
typename iterator_traits<InputIterator>::difference_type 
    distance_dispatch(InputIterator first, InputIterator last, input_iterator_tag){
    typename iterator_traits<InputIterator>::difference_type n=0;
    while(first!=last){
        ++first;
        ++n;
    }
    return n;
}

// random_access_iterator_tag版本（可以直接作差得到，效率更高）
template <class RandomIter>
typename iterator_traits<RandomIter>::difference_type
    distance_dispatch(RandomIter first, RandomIter last, random_access_iterator_tag){
    return last-first;
}

template <class InputIterator>
typename iterator_traits<InputIterator>::difference_type
distance(InputIterator first, InputIterator last){
    return distance_dispatch(first,last,iterator_category(first));
}


// advance让迭代器前进n个距离
// input_iterator_tag版本
template <class InputIterator, class Distance>
void advance_dispatch(InputIterator& i, Distance n, input_iterator_tag){
    while(n--){
        ++i;
    }
}

// bidirectional_iterator_tag（根据n的正负前后移动）
template <class BidirectionalIterator, class Distance>
void advance_dispatch(BidirectionalIterator& i, Distance n, bidirectional_iterator_tag){
    if(n>=0){
        while(n--){
            ++i;
        }
    }
    else{
        while(n++){
            --i;
        }
    }
}

// random_access_iterator_tag
template <class RandomIter, class Distance>
void advance_dispatch(RandomIter& i, Distance n, random_access_iterator_tag){
    i+=n;
}

template <class InputIterator, class Distance>
void advance(InputIterator& i, Distance n){
    advance_dispatch(i,n,iterator_category(i));
}



// reverse_iterator（基于正向迭代器实现）---------------------------------------------------------
template <class Iterator>
class reverse_iterator
{
private:
    Iterator current; //对应正向迭代器

public:
    typedef typename iterator_traits<Iterator>::iterator_category iterator_category;
    typedef typename iterator_traits<Iterator>::value_type value_type;
    typedef typename iterator_traits<Iterator>::difference_type difference_type;
    typedef typename iterator_traits<Iterator>::pointer pointer;
    typedef typename iterator_traits<Iterator>::reference reference;

    typedef Iterator iterator_type;
    typedef reverse_iterator<Iterator> self;

public:
    reverse_iterator(){}
    explicit reverse_iterator(iterator_type i):current(i){}
    reverse_iterator(const self& rhs):current(rhs.current){}

public:
    // 提取对应的正向迭代器
    iterator_type base() const{
        return current;
    }

    // 重载操作符
        // 迭代器相当于处于两个元素中间的位置，正向迭代器取后一个值，反向迭代器取前面一个值
    reference operator*() const{
        auto tmp=current;
        return *--tmp;
    }

    pointer operator->()const{
        return &(operator*());
    }

    // 前++（返回当前对象）
    self& operator++(){
        --current;
        return *this;
    }
    // 后++（生成临时对象）
    self operator++(int){
        self tmp=*this;
        --current;
        return tmp;
    }
    
    self& operator--(){
        ++current;
        return *this;
    }

    self operator--(int){
        self tmp=*this;
        ++current;
        return tmp;
    }

    self& operator+=(difference_type n){
        current-=n;
        return *this;
    }
    
    self operator+(difference_type n) const{
        return self(current-n);
    }

    self& operator-=(difference_type n){
        current+=n;
        return *this;
    }

    self operator-(difference_type n) const{
        return self(current+n);
    }

    reference operator[](difference_type n) const{
        return *(*this+n);
    }
};


// 两个反向迭代器相减
template <class Iterator>
typename reverse_iterator<Iterator>::difference_type
    operator-(const reverse_iterator<Iterator>& lhs, const reverse_iterator<Iterator>& rhs){
    return rhs.base()-lhs.base();
}

// 重载比较操作符
template <class Iterator>
bool operator==(const reverse_iterator<Iterator>& lhs, const reverse_iterator<Iterator>& rhs){
    return lhs.base()==rhs.base();
}

template <class Iterator>
bool operator<(const reverse_iterator<Iterator>& lhs, const reverse_iterator<Iterator>& rhs){
    return rhs.base()<lhs.base();
}

template <class Iterator>
bool operator!=(const reverse_iterator<Iterator>& lhs, const reverse_iterator<Iterator>& rhs){
    return !(lhs==rhs);
}

template <class Iterator>
bool operator>(const reverse_iterator<Iterator>& lhs, const reverse_iterator<Iterator>& rhs){
    return rhs<lhs;
}

template <class Iterator>
bool operator<=(const reverse_iterator<Iterator>& lhs, const reverse_iterator<Iterator>& rhs){
    return !(rhs<lhs);
}

template <class Iterator>
bool operator>=(const reverse_iterator<Iterator>& lhs, const reverse_iterator<Iterator>& rhs){
    return !(lhs<rhs);
}


}