#include <bitset>
#include <array>
#include <iostream>
#include <vector>

constexpr int rounds = 10;
constexpr std::array<uint8_t, 4> t0{101, 120, 112, 97};
constexpr std::array<uint8_t, 4> t1{110, 100, 32, 49};
constexpr std::array<uint8_t, 4> t2{54, 45, 98, 121};
constexpr std::array<uint8_t, 4> t3{116, 101, 32, 107};

// bitset left rotation
uint32_t rotl(uint32_t y1, int shift){
    return y1 << shift | y1 >> (32-shift);
}

// quarterround
void qround(uint32_t &y0, uint32_t &y1, uint32_t &y2, uint32_t &y3){
    y1 ^= rotl(y0 + y3, 7);
    y2 ^= rotl(y1 + y0, 9);
    y3 ^= rotl(y2 + y1, 13);
    y0 ^= rotl(y3 + y2, 18);
}

// rowround
void rround(std::array<uint32_t, 16> &y){
    qround(y[0], y[1], y[2], y[3]);
    qround(y[5], y[6], y[7], y[4]);
    qround(y[10], y[11], y[8], y[9]);
    qround(y[15], y[12], y[13], y[14]);
}

// columnround
void cround(std::array<uint32_t, 16> &y){
    for(int a = 0; a < 4; a++){// 4x4 transpose
        for(int b = 0; b < a; b++){
            std::swap(y[4*a + b], y[4*b + a]);
        }
    }
    rround(y);
}

// doubleround
void dround(std::array<uint32_t, 16> &y){
    cround(y);
    rround(y);
}

// littleendian
uint32_t littleendian(std::array<uint8_t,4> b){
    uint32_t out{0};
    for(auto x : b){
        out <<= 8;
        out |= x;
    }
    return out;
}

//littleendian inverse
std::array<uint8_t, 4> littleendianinv(uint32_t y1){
    std::array<uint8_t, 4> out;
    for(int a = 0; a < 4; a++){
        out[a] = static_cast<uint8_t>(y1);
        y1 >>= 8;
    }
    return out;
}

std::array<uint8_t, 64> salsa20(std::array<uint8_t, 64> input){
    std::array<uint32_t, 16> lendian, lendiancp;
    std::array<uint8_t, 64> s20_out;
    for(int a = 0; a < 16; a++){
        lendian[a] = littleendian({input[4*a + 0], input[4*a + 1], input[4*a + 2], input[4*a +3]});
    }
    std::copy(lendian.begin(), lendian.end(), lendiancp.begin());
    for(int a = 0; a < rounds; a++){
        dround(lendiancp);
    }
    for(int a = 0; a < 16; a++){
        lendian[a] += lendiancp[a];
    }
    for(int a = 0; a < 16; a++){
        auto [w, x, y, z] = littleendianinv(lendian[a]);
        s20_out[4*a + 0] = w, s20_out[4*a + 1] = x, s20_out[4*a + 2] = y, s20_out[4*a + 3] = z;
    }
    return s20_out;
}

std::array<uint8_t, 64> salsa20doubleprng(std::array<uint8_t, 16> n, std::array<uint8_t, 16> k){
    std::array<uint8_t, 64> input;
    std::copy(t0.begin(), t0.end(), input.begin());
    std::copy(k.begin(), k.end(), input.begin() + t0.size());
    std::copy(t1.begin(), t1.end(), input.begin() + t0.size() + k.size());
    std::copy(n.begin(), n.end(), input.begin() + t0.size() + k.size() + t1.size());
    std::copy(t2.begin(), t2.end(), input.begin() + t0.size() + k.size() + t1.size() + n.size());
    std::copy(k.begin(), k.end(), input.begin() + t0.size() + k.size() + t1.size() + n.size() + t1.size());
    std::copy(t3.begin(), t3.end(), input.begin() + t0.size() + 2*k.size() + t1.size() + n.size() + t1.size());
    return salsa20(input);
}

// length-doubling 32-byte prng using Salsa20 stream cipher
std::array<uint8_t, 64> salsa20doubleprng(std::array<uint8_t, 32> input){
    std::array<uint8_t, 16> n, k;
    std::copy(input.begin(), input.begin() + n.size(), n.begin());
    std::copy(input.begin() + n.size(), input.begin() + n.size() + k.size(), k.begin());
    return salsa20doubleprng(n, k);
}

// use length-doubling prng to generate prng length-preserving output for arbitrary input size
std::vector<uint8_t> salsa20prng(std::vector<uint8_t> input, bool bit){
    std::vector<uint8_t> output;
    int blocksize = input.size()/32 + static_cast<bool>(input.size()%32);
    for(int a = 0; a < blocksize; a++){
        std::array<uint8_t, 32> prng_input{0};
        std::copy(input.begin() + 32*a, input.begin() + std::min(32*(a+1), static_cast<int>(input.size()-1)), prng_input.begin());
        std::array<uint8_t, 64> prng = salsa20doubleprng(prng_input);
        std::copy(prng.begin() + 32*static_cast<int>(bit), prng.begin() + 32*(1+static_cast<int>(bit)), std::back_inserter(output));
    }
    output.resize(input.size());
    return output;
}

int main(){}
