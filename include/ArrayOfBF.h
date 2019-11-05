#ifndef _ArrayOfBF_
#define _ArrayOfBF_
#include <vector>
#include <set>
#include <string>


class ArBF{
    public:
        ArBF(int n, float fpr1, int n_sets);
        float p;
        int range;
        int k;
        float FPR;
        int K;
        BloomFiler** ArBF_array;
        void insertion (std::vector<std::string> alllines);
        std::set<int> query (std::string query_key, int len);
};

#endif
