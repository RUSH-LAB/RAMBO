#ifndef _ArrayOfBF_
#define _ArrayOfBF_
#include <vector>
#include <set>
#include <string>
#include <iomanip>
#include <fstream>
#include <iostream>
#include <chrono>
#include <vector>
#include <sstream>
#include <string>
#include <string.h>
#include <algorithm>
#include <set>
#include <iterator>
#include "MurmurHash3.h"
#include "utils.h"
#include "MyBloom.h"


class ArBF{
    public:
        ArBF(int n, float fpr1, int n_sets);
        float p;
        int range;
        int k;
        float FPR;
        int K;
        BloomFilter** ArBF_array;
        void insertion (std::vector<std::string> alllines);
        std::set<int> query (std::string query_key, int len);
};

#endif
