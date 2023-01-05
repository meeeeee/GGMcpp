#include <vector>
#include "salsa20.h"

std::vector<uint8_t> ggm(std::vector<uint8_t> seed, std::vector<uint8_t> input_){
    std::vector<bool> input;
    for(int a = 0; a < static_cast<int>(input_.size()); a++){
        for(int b = 0; b < 8; b++){
            input.push_back((input_[a] >> (8 - b))&1);
        }
    }
    for(auto x : input) seed = salsa20prng(seed, x);
    return seed;
}
