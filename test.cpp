#include <iostream>
#include <fstream>
#include <string>
#include <sys/stat.h>
#include "ggm.h"

int main(){
    std::string seedfile, inputfile, outputfile;
    std::cout << "Location of seed: ";
    std::cin >> seedfile;
    std::cout << std::endl;
    std::cout << "Location of input: ";
    std::cin >> inputfile;
    std::cout << std::endl;
    std::cout << "Location of output: ";
    std::cin >> outputfile;
    std::cout << std::endl;
    
    std::ifstream seed(seedfile, std::ios::in | std::ios::binary);
    std::ifstream input(inputfile, std::ios::in | std::ios::binary);
    std::ofstream output(outputfile, std::ios::out | std::ios::binary);
    seed.seekg(0, std::ios::end);
    input.seekg(0, std::ios::end);
    uint64_t seed_sz = seed.tellg(), input_sz = input.tellg();
    seed.seekg(0, std::ios::beg);
    input.seekg(0, std::ios::beg);
    
    std::vector<uint8_t> seed_(seed_sz), input_(input_sz);
    std::cout << "Size of seed in bytes: " << seed_sz << "\nSize of input in bytes: " << input_sz << std::endl;
    
    seed.read(reinterpret_cast<char*>(seed_.data()), seed_sz);
    input.read(reinterpret_cast<char*>(input_.data()), seed_sz);
    
    std::vector<uint8_t> output_ = ggm(seed_, input_);
    output.write(reinterpret_cast<char*>(output_.data()), static_cast<int>(output_.size()));
    std::cout << "Finished writing" << std::endl;
}
