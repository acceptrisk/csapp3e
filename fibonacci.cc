// Example program
#include <iostream>
#include <string>

template<long num> 
struct Fibonacci{
    static const long val = Fibonacci<num - 1>::val + Fibonacci<num - 2>::val;
};

template<> struct Fibonacci<2> 
{
    static const long val = 1;
};

template<> struct Fibonacci<1> 
{
    static const long val = 1;
};

int main()
{
    constexpr int fib[]=
    {
        Fibonacci<11>::val, Fibonacci<12>::val,
        Fibonacci<13>::val, Fibonacci<14>::val,
        Fibonacci<15>::val, Fibonacci<16>::val,
    };
    
    for(auto i : fib)
        std::cout << i << std::endl;
    
}
