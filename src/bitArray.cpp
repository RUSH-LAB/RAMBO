#include <stdio.h>
#include <iostream>
#include <fstream>
#include "bitArray.h"
#include "spdlog/spdlog.h"
#include "xsimd/xsimd.hpp"

using namespace std;

bitArray::bitArray(int size){
    this->ar_size = size;
    this->A = std::vector<unsigned char>(ar_size/8 +1);
    this->bitIt = bit::bit_iterator<unsigned char*>(&(*A.begin()));
    this->end = this->bitIt + ar_size;
    for (int i = 0; i < (ar_size/8 + 1); i++) {
        A[i] = 0; // Clear the bit array
    }
}


void bitArray::ANDop(unsigned char* B){
  auto first1 = this->bitIt;
  auto first2 = bit::bit_iterator<unsigned char*>(B);
  xsimd::transform(first1.base(), first1.base() + ar_size/8 + 1, first2.base(), first1.base(),
         [](const auto& x, const auto& y) {return x & y;}); 
}

bool bitArray::empty() {
    return bit::find(this->bitIt, this->end, bit::bit1) == this->end;
}

void bitArray::serializeBitAr(fs::path BF_file){
  ofstream out;
  out.open(BF_file);

  if(! out){
      spdlog::critical("Cannot open {}", BF_file.string());
      exit(1);
  }
  out.write(reinterpret_cast<char*>(&(*A.begin())), ar_size/8 +1);
  out.close();
}

int bitArray::getcount() {
    return bit::count(this->bitIt, this->end, bit::bit1);
}

// TODO why does deserializing combine repititions...?
void bitArray::deserializeBitAr(fs::path BF_file){
    ifstream in(BF_file);
    if(! in){
      spdlog::critical("Cannot open {}", BF_file.string());
      exit(1);
    }
    in.read((char *) (&(*A.begin())), ar_size/8 +1); //optimise it
    in.close();
}
