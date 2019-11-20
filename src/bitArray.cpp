#include <stdio.h>
#include <iostream>
#include <fstream>
#include "bitArray.h"

using namespace std;

bitArray::bitArray(int size){
  ar_size = size;
  A = new char[ar_size/8 +1];
   for (int i=0; i<(ar_size/8 +1); i++ ){
     A[i] = '\0'; // Clear the bit array
   }
}

void bitArray::SetBit(uint k) {
   A[(k/8)] |= (1 << (k%8));
}

void bitArray::ClearBit(uint k) {
   A[(k/8)] &= ~(1 << (k%8));
}

bool bitArray::TestBit(uint k) {
   return (A[(k/8)] & (1 << (k%8)));
}

void bitArray::ANDop(char* B){
  for (int i=0; i<(ar_size/8 +1); i++ ){
    A[i] &= B[i];
  }
}

int bitArray::getcount(void){
  int count = 0;

  for (int kp=0; kp<ar_size; kp++ ){
    // std::cout << "kp is" <<kp<< ' ';
    if (A[(kp/8)] & (1 << (kp%8)) ){
      count++;
    }
  }
  return count;
}

void bitArray::serializeBitAr(string BF_file){
  ofstream out;
  out.open(BF_file);

  if(! out){
    cout<<"Cannot open output file\n";
  }
  out.write(A,ar_size/8 +1);
    out.close();
}

void bitArray::deserializeBitAr(std::vector<string> BF_file){
  for(uint j =0; j<BF_file.size(); j++){
    char* C;
    C = new char[ar_size/8 +1];
    ifstream in(BF_file[j]);

    if(! in){
      cout<<"Cannot open input file\n";
    }

    in.read(C,ar_size/8 +1); //optimise it

    if (j ==0){
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

   // // Check if SetBit() works:
   //
   // for ( i = 0; i < 320; i++ )
   //    if ( TestBit(A, i) )
   //       printf("Bit %d was set !\n", i);
   //
   // printf("\nClear bit poistions 200 \n");
   // ClearBit( A, 200 );
   //
   // // Check if ClearBit() works:
   //
   // for ( i = 0; i < 320; i++ )
   //    if ( TestBit(A, i) )
   //       printf("Bit %d was set !\n", i);
