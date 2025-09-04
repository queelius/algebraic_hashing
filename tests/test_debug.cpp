#include <iostream>
#include "../include/algebraic_hashing/functions/fnv_hash_modern.hpp"
#include "../include/algebraic_hashing/dsl/algebraic_operations.hpp"

using namespace algebraic_hashing;
using namespace algebraic_hashing::functions;
using namespace algebraic_hashing::dsl;

int main() {
    fnv64 f1, f2;
    std::string test = "test";
    
    auto h1 = f1(test);
    auto h2 = f2(test);
    
    std::cout << "f1(test) = " << h1.to_hex() << std::endl;
    std::cout << "f2(test) = " << h2.to_hex() << std::endl;
    std::cout << "f1 == f2? " << (h1 == h2) << std::endl;
    std::cout << "f1 ^ f2 = " << (h1 ^ h2).to_hex() << std::endl;
    
    auto xor_comp = f1 ^ f2;
    auto result = xor_comp(test);
    std::cout << "(f1 ^ f2)(test) = " << result.to_hex() << std::endl;
    
    return 0;
}
