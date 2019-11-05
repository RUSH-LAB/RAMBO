#include <iostream>
#include <math.h>
#include "mybitarray.h"
using namespace std;

mybitarray:: mybitarray(int size){
      cout<<size/8 +1<<endl;
      char mybit[size/8 +1] = {0};
      }

void mybitarray:: set(int loc){
  // cout<<"here"<<loc<<endl;
  int r = loc/8;
  int c = loc%8;
  // cout<<"here"<<r<<c<<endl;

  // go to r and update the cth bit
  // mybit[r] |= 0x4;
  char newc = mybit[r];
  newc |= 1UL <<c;
  mybit[r] = newc;
  // cout<<"here"<<newc <<endl;

}

// void mybitarray:: get(int loc){
//   int r = loc/8;
//   int c = loc%8;
//   mybit[r] >> c & 1;
// }

void mybitarray::andops(char * ar){
  for (int i=0;i<sizeof(ar); i++){
    mybit[i] &= ar[i];
  }
}

int mybitarray::givecount(){
unsigned int count = 0;
for (int i=0;i<sizeof(mybit); i++){
  char n = mybit[i];
  while (n)
  {
      count += n & 1;
      n >>= 1;
  }
}
return count;
}
