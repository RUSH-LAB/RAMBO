#ifndef _MYBITARRAY_
#define _MYBITARRAY_
#include <vector>

class mybitarray{
    public:
      char* mybit;
      mybitarray( int size);
      void set(int loc);
      void andops(char * ar);
      int givecount();


};

#endif
