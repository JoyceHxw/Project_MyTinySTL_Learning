#include "algorithm_base.h"
#include <iostream>
#include <vector>
#include "util.h"

void test_min(){
    std::cout<<__func__<<"-----------------"<<std::endl;
    int a=0;
    int b=20;
    std::cout<<mystl::min(a,b,[](int a, int b){return a<b;})<<std::endl;
}

void test_iter_swap(){
    std::cout<<__func__<<"-----------------"<<std::endl;
    std::vector<int> v={1,2,3,4,5,6,7,8};
    std::vector<int>::iterator it1=v.begin()+2;
    std::vector<int>::iterator it2=v.begin()+5;
    mystl::iter_swap(it1,it2);
    for(auto num: v){
        std::cout<<num<<" ";
    }
    std::cout<<std::endl;
}

void test_swap_range(){
    std::cout<<__func__<<"-----------------"<<std::endl;
    std::vector<int> v1={1,2,3,4,5,6,7,8};
    std::vector<int> v2={4,3,3,6,4,2,6,7};
    mystl::swap_range(v1.begin(),v1.end(),v2.begin());
    for(auto num: v1){
        std::cout<<num<<" ";
    }
    std::cout<<std::endl;
    for(auto num: v2){
        std::cout<<num<<" ";
    }
    std::cout<<std::endl;
}

void test_copy(){
    std::cout<<__func__<<"-----------------"<<std::endl;
    int arr1[] = { 1,2,3,4,5,6,7,8,9,10 };
    int exp[5], act[5];
    std::copy(arr1, arr1 + 5, exp);
    mystl::copy(arr1, arr1 + 5, act);
    for(int i=0; i<10; i++){
        std::cout<<act[i]<<" ";
    }
    std::cout<<std::endl;
}

void test_pair(){
    std::cout<<__func__<<"-----------------"<<std::endl;
    mystl::pair<int,char> p1(1,'A');
    mystl::pair<int,char> p2(4,'D');
    p1=p2;
    std::cout<<p1.first<<" "<<p1.second<<std::endl;
    mystl::pair<double,char> p3(3.14,'B');
    mystl::pair<double,char> p4(p3);
    std::cout<<p4.first<<" "<<p4.second<<std::endl;
    mystl::pair<int,int> p5(p3);
    std::cout<<p5.first<<" "<<p5.second<<std::endl;
    p1=mystl::pair<int,char>(2,'B');
    mystl::swap(p1,p2);
    std::cout<<p1.first<<" "<<p1.second<<std::endl;
    std::cout<<p2.first<<" "<<p2.second<<std::endl;
    mystl::pair<int,int> p6=mystl::make_pair(1,2);
    std::cout<<p6.first<<" "<<p6.second<<std::endl;
}

void test_copy_backward(){
    std::vector<int> v1={1,2,3,4,5,6,7,8,9};
    mystl::copy_backward(v1.begin(), v1.begin()+3, v1.end());
    for(int i=0; i<v1.size(); i++){
        std::cout<<v1[i]<<" ";
    }
    std::cout<<std::endl;
}

int main(){

    #ifdef max
    #pragma message("#undefing marco max")
    #undef max
    #endif // max

    test_min();
    test_iter_swap();
    test_swap_range();
    test_copy();
    test_pair();
    test_copy_backward();
    
    return 0;
}
