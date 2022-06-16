#include <iostream>
#include <hashing/fnv_hash.hpp>

int main()
{
    std::cout << hashing::details::fnv_hash('a') << "\n";
    std::cout << hashing::details::fnv_hash('b') << "\n";
    std::cout << hashing::details::fnv_hash('c') << "\n";
    std::cout << hashing::details::fnv_hash("a") << "\n";
    std::cout << hashing::details::fnv_hash("abc") << "\n";
    std::cout << hashing::details::fnv_hash(100) << "\n";
    std::cout << hashing::details::fnv_hash(101) << "\n";

    hashing::fnv_hash h;
    std::cout << h(std::optional<char>{'a'}) << "\n";

    std::cout << h('a') << "\n";

    std::cout << h(std::variant<char,int>{'a'}) << "\n";
}