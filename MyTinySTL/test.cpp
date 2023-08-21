#include "algorithm_base.h"
#include <iostream>
#include <vector>

int main(){

    #ifdef max
    #pragma message("#undefing marco max")
    #undef max
    #endif // max
    
    int a=0;
    int b=20;
    std::cout<<mystl::min(a,b,[](int a, int b){return a<b;})<<std::endl;

    // std::vector<int> v={1,2,3,4,5,6,7,8};
    // std::vector<int>::iterator it1=v.begin()+2;
    // std::vector<int>::iterator it2=v.begin()+5;
    // mystl::iter_swap(it1,it2);
    
    // int arr1[] = { 1,2,3,4,5,6,7,8,9,10 };
    // int exp[5], act[5];
    // std::copy(arr1, arr1 + 5, exp);
    // mystl::copy(arr1, arr1 + 5, act);
    return 0;
} // namespace mystl
