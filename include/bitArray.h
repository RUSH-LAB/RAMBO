#ifndef _INTBITARRAY_
#define _INTBITARRAY_
#include<vector>

// #include <boost/archive/text_oarchive.hpp>
// #include <boost/archive/text_iarchive.hpp>

class bitArray{
    public:
        // friend class boost::serialization::access;

        bitArray(int size);
        void SetBit(uint k);
        void ClearBit( uint k);
        bool TestBit( uint k);
        void ANDop(char* B);
        void serializeBitAr(std::string BF_file);
        void deserializeBitAr(std::vector<std::string> BF_file);
        int getcount(void);
        char *A;
        int ar_size;
        // template<class Archive>
        // void serialize(Archive & ar, const unsigned int version){
        //   ar & A;
        // }
};

#endif
