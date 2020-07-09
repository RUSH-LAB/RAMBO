#include <stdio.h>
#include <iostream>
#include <fstream>
#include "bitArray.h"
#include "xsimd/xsimd.hpp"

using namespace std;

bitArray::bitArray(int size){
    this->ar_size = size;
    this->A = new unsigned char[ar_size/8 +1];
    this->bitIt = bit::bit_iterator<unsigned char*>(A);
    for (int i = 0; i < (ar_size/8 + 1); i++) {
        A[i] = 0; // Clear the bit array
    }
}


void bitArray::ANDop(unsigned char* B){
  auto first1 = this->bitIt;
  auto first2 = bit::bit_iterator<unsigned char*>(B);
  //bit::transform_and(first1, first1 + this->ar_size, first2, first1);
}

void bitArray::serializeBitAr(fs::path BF_file){
  ofstream out;
  out.open(BF_file);

  if(! out){
    cout<<"Cannot open output file\n";
  }
  out.write(reinterpret_cast<char*>(A), ar_size/8 +1);
    out.close();
}

int bitArray::getcount() {
    return bit::count(this->bitIt, this->bitIt + this->ar_size, bit::bit1);
}

// TODO why does deserializing combine repititions...?
void bitArray::deserializeBitAr(std::vector<fs::path> BF_file){
  for(uint j =0; j<BF_file.size(); j++){
    char* C;
    C = new char[ar_size/8 +1];
    ifstream in(BF_file[j]);

    if(! in){
      cout<<"Cannot open input file\n";
    }

    in.read(C,ar_size/8 +1); //optimise it

    if (j == 0){
      for (int i=0; i<(ar_size/8 +1); i++ ){
        A[i] = C[i];
      }
    }
    else{
      for (int i=0; i<(ar_size/8 +1); i++ ){
        A[i] |= C[i];
      }
    }
    in.close();
    delete[] C;
  }
}
