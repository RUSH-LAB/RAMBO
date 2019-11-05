#ifndef _MYBLOOM_
#define _MYBLOOM_
#include <vector>
#include "constants.h"
#include <bitset>
#include "intbitArray.h"

std::vector<uint> myhash(std::string key, int len, int k, int range);

class BloomFiler{
    public:
        // BloomFiler(int capacity, float FPR, int k);
        BloomFiler(int sz, float FPR, int k);
        void insert(std::vector<uint> a);
        bool test(std::vector<uint> a);
        void serializeBF(std::string BF_file);
        void deserializeBF(std::vector<std::string> BF_file);

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
