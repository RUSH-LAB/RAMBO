#include "MurmurHash3.h"
#include <iostream>
#include <cstring>
#include <chrono>
#include <vector>
#include <math.h>
#include "MyBloom.h"

using namespace std;

vector<uint> myhash( std::string key, int len, int k, int range, int seed){
  // int hashvals[k];
  vector <uint> hashvals;
  uint op; // takes 4 byte

  for (int i=0; i<k; i++){
    MurmurHash3_x86_32(key.c_str(), len, i + (seed*range), &op);
    hashvals.push_back(op%range);
  }
  return hashvals;
}

BloomFilter::BloomFilter(int sz, float FPR, int _k) : p(FPR), k(_k){
      this->m_bits = new bitArray(sz);
}

void BloomFilter::insert(vector<uint> a){
  int N = a.size();
  for (int n =0 ; n<N; n++){
    this->m_bits->bitIt[a[n]] = bit::bit1;
  }
}

//TODO theres gotta be a better way for this
bool BloomFilter::test(vector<uint> a) {
  int N = a.size();
  for (int n =0 ; n<N; n++){
      auto arr = this->m_bits->bitIt;
      auto idx = a[n];
      if (arr[idx] == bit::bit0){
        return false;
      }
  }
  return true;
}

void BloomFilter::serializeBF(fs::path BF_file){
  this->m_bits->serializeBitAr(BF_file);
}

void BloomFilter::deserializeBF(fs::path BF_file){
  this->m_bits->deserializeBitAr(BF_file);
}
