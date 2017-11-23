// Example program
#include <iostream>
#include <array>

template<typename T, std::size_t N>
constexpr std::size_t ArraySize(T (&)[N])
{
    return N;
}

int main()
{
    int aKeys[] = {1, 2, 3, 4};
    int aKeysOther[ArraySize(aKeys)];
    std::cout << "sizeof(aKeysOther) "  << sizeof(aKeysOther) << std::endl;
    
    std::array<int, ArraySize(aKeys)> aKeysAnother;
    std::cout << "sizeof(aKeysAnother) " << sizeof(aKeysAnother) << std::endl;
}
