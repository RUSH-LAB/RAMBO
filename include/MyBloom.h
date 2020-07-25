#ifndef _MYBLOOM_
#define _MYBLOOM_
#include <filesystem>
#include <vector>
#include <bitset>
#include "bitArray.h"

namespace fs = std::filesystem;

std::vector<uint> myhash(std::string key, int len, int k, int range);

class BloomFilter{
    public:
        // BloomFilter(int capacity, float FPR, int k);
        BloomFilter(int sz, float FPR, int k);
        void insert(std::vector<uint> a);
        bool test(std::vector<uint> a);
        void serializeBF(fs::path BF_file);
        void deserializeBF(fs::path BF_file);

        // void serialize1(std::string BF_file);

        int n;
        float p;
        int R;
        int k;
        // std::vector<bool> m_bits;
        // std::bitset<capacity> m_bits;
        bitArray* m_bits;
};

#endif
