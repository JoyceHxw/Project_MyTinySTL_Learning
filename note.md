## STL六大部件 ##
容器Container、算法Algorithm、容器适配器Adapter、迭代器Iterator、分配器Allocator、仿函数Functor

container通过allocator取得数据存储空间，algorithm通过iterator存取container内容，functor协助algorithm完成不同的策略变化，adapter可以修饰或套接functor

## 容器container ##
分类：顺序容器array、vector、deque、list、forward-list，关联容器（红黑树）set、multiset、map、multimap，无序关联容器（哈希表）unordered_set、unordered_multiset、unordered_map、unordered_multimap

## 泛型编程Generic Programming ##
将容器和算法分开，两者通过迭代器连接

特化，偏特化

## 分配器allocator ##
allocate()分配内存->operator new()->malloc()

deallocate()回收内存->operator delete()->free()

## 迭代器iterator ##
算法对容器进行操作时，需要知道迭代器的特征，所以要求迭代器的设计包括五个相关特征：迭代器类型、元素类型、指针类型、引用类型、迭代器之间的距离。

但迭代器可能不是class，如一般的指针，就无法迭代器直接获取其特征，就需要萃取机iterator traits利用偏特化对不同类型的迭代器进行处理。