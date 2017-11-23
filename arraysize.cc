// Example program
#include <iostream>
#include <string>

template<typename T, std::size_t N>
constexpr std::size_t ArraySize(T (&)[N])
{
    return N;
}

int main()
{
    int aKeys[] = {1, 2, 3, 4};
    int aKeysOther[ArraySize(aKeys)];
    std::cout << sizeof(aKeysOther) << std::endl;
}
