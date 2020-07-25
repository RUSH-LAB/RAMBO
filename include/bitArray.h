#ifndef _INTBITARRAY_
#define _INTBITARRAY_
#include <vector>
#include <filesystem>
#include "bit.hpp"

namespace fs = std::filesystem;

// #include <boost/archive/text_oarchive.hpp>
// #include <boost/archive/text_iarchive.hpp>

class bitArray{
    public:
        // friend class boost::serialization::access;
        bitArray(int size);
        void ANDop(unsigned char* B);
        int getcount();
        void serializeBitAr(fs::path BF_file);
        void deserializeBitAr(fs::path BF_file);
        bool empty();
        unsigned char *A;
        bit::bit_iterator<unsigned char*> bitIt;
        bit::bit_iterator<unsigned char*> end;
        int ar_size;
};

#endif
