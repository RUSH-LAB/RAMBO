// to unit test

#include <iomanip>
#include <iostream>
#include <fstream>
#include <math.h>
#include <sstream>
#include <string>
#include <string.h>
#include <algorithm>
#include <chrono>
#include "MyBloom.h"
#include "MurmurHash3.h"
#include "Rambo_construction.h"
#include "utils.h"
#include <bitset>
#include "bitArray.h"

using namespace std;
set<int> takeIntrsec(set<int>* setArray){
  set<int> res;
  set<int> s1 = setArray[0];
  for (int i=1; i<5; i++){
    set<int> res;
    set_intersection(s1.begin(),s1.end(),setArray[i].begin(),setArray[i].end(), std::inserter(res,res.begin()));
    s1 = res;
    // for (auto it=s1.begin(); it != s1.end(); ++it)
    //   {cout << ' ' << *it;}
    // cout<<endl;
  }
  return s1;
}

void checkBF(){
  int n = 100; // number of keys
  float p = 0.01; //fpr
  int R = ceil(-(n*log(p))/(log(2)*log(2))); //range
  int k = ceil(-log(p)/log(2)); //number of hash, k is 7 for 0.01

  // find hash values
  char key[10][10] = {{'s','e','q','n','c','e','q','n','c','e'},
                      {'s','5','q','n','c','e','q','n','c','e'},
                      {'s','e','q','n','c','e','q','j','c','e'},
                      {'s','e','d','n','c','e','q','n','c','e'},
                      {'l','s','q','n','c','e','q','n','c','e'},
                      {'s','e','q','r','c','e','q','5','c','e'},
                      {'s','e','q','u','c','t','q','n','c','e'},
                      {'s','e','q','n','c','e','q','3','c','e'},
                      {'s','e','9','n','c','k','q','n','c','e'},
                      {'s','e','q','n','y','e','q','n','c','e'}};
  int len = 10;

  char testkey[10][10] = {{'s','u','f','n','c','e','q','n','2','1'},
                      {'s','5','q','n','c','e','q','n','0','5'},
                      {'s','e','q','n','c','e','q','n','6','8'},
                      {'s','e','d','n','c','e','q','n','c','0'},
                      {'l','s','q','n','c','e','q','n','c','9'},
                      {'s','e','q','r','c','e','q','n','c','8'},
                      {'s','e','q','u','c','t','q','n','c','7'},
                      {'s','e','q','n','c','e','q','n','c','5'},
                      {'s','e','9','n','c','k','q','n','c','4'},
                      {'s','e','q','n','c','e','q','3','c','e'}};

  vector<uint> allhashes;

  for(int i=0;i<n;i++){
    vector<uint> temp = myhash(key[i%10], len , k, R);
      allhashes.insert(allhashes.end(), temp.begin(), temp.end());
  }

  //initialize Bloom filter
  BloomFiler BF(R, p, k);

  // insert hash values
  BF.insert(allhashes);
  cout<< "count is: "<<BF.m_bits->getcount()<<endl;

  //check
  chrono::time_point<chrono::high_resolution_clock> start = chrono::high_resolution_clock::now();
  for(int i=0;i<10000000;i++){
    vector<uint> check = myhash(testkey[i%10], len , k, R);
    bool op = BF.test(check);
    // std::cout << "output is: " <<op<< '\n';
  }
  chrono::time_point<chrono::high_resolution_clock> finish = chrono::high_resolution_clock::now();
  cout << chrono::duration_cast<chrono::nanoseconds>(finish-start).count()/10000000.0 << "ns per query \n";
  // cout<<op<<endl;
}

void test_bitArray(){
  bitArray bitarray_K1(1000);
  for (int i =0; i<10; i++){
    bitarray_K1.SetBit(i);
  }

  for (int i =0; i<20; i++){
    cout<<"bit is: "<<bitarray_K1.TestBit(i)<<endl;
  }
  cout<<"count is: "<<endl;

  cout<<bitarray_K1.getcount()<<endl;
}

void testkeygen(){
  std::vector<string> s;
  s = getRandomTestKeys(30, 10);
  for (int j = 0; j < 10; ++j){
  cout<<s[j]<<endl;
  }
}

int main(){
// check the Bloom filter:
// checkBF();
// test_bitArray();
testkeygen();
  //checking Union
//   std::vector<int> v1 = {245, 52, 423, 523};
//   std::vector<int> v2 = {25, 52, 90, 23};
//   std::vector<int> v;
//   set<int> s;
//
//   set<int> s1 = {245, 52, 423, 523};
//   set<int> s2 = {25, 52, 90, 23};
//
//
//
//   chrono::time_point<chrono::high_resolution_clock> t1 = chrono::high_resolution_clock::now();
//   v = arrayunion(v1, v2);
//
//   v1.insert( v1.end(), v2.begin(), v2.end() );
//
//   chrono::time_point<chrono::high_resolution_clock> t2 = chrono::high_resolution_clock::now();
//   cout << chrono::duration_cast<chrono::nanoseconds>(t2-t1).count()/1000000000.0 << "sec\n";
//
//   chrono::time_point<chrono::high_resolution_clock> t3 = chrono::high_resolution_clock::now();
//   s = takeunion(s1, s2);
//   chrono::time_point<chrono::high_resolution_clock> t4 = chrono::high_resolution_clock::now();
//   cout << chrono::duration_cast<chrono::nanoseconds>(t4-t3).count()/1000000000.0 << "sec\n";
//
//
//   chrono::time_point<chrono::high_resolution_clock> t5 = chrono::high_resolution_clock::now();
//
//   v1.insert( v1.end(), v2.begin(), v2.end() );
//
//   chrono::time_point<chrono::high_resolution_clock> t6 = chrono::high_resolution_clock::now();
//   cout << chrono::duration_cast<chrono::nanoseconds>(t6-t5).count()/1000000000.0 << "sec\n";
//
//
//
// //bitset
// chrono::time_point<chrono::high_resolution_clock> t7 = chrono::high_resolution_clock::now();
// bitset<1000000> set8;
// for (int i=0; i<v1.size(); i++){
//   set8[v1[i]] = 1;
// }
// chrono::time_point<chrono::high_resolution_clock> t8 = chrono::high_resolution_clock::now();
// cout << chrono::duration_cast<chrono::nanoseconds>(t8-t7).count()/1000000000.0 << "sec\n";
//
//
//   for (auto it=s.begin(); it != s.end(); ++it)
//     {cout << ' ' << *it;}
//
//   cout<<endl;
//   for (auto it=v.begin(); it != v.end(); ++it)
//     {cout << ' ' << *it;}
  //checking line2array
  // string line = "23,45,234";
  // std::vector<string> testvar = line2array( line, ",");
  // cout<<testvar[0]<<endl;

  //checking takeIntrsec
  // set<int> setArray[5] = {{0,1,4,9},{0,1,2,7,4,5,6,14},{0,3,5,8}, {0,3,5,7,9,2,4,8,11,12,13}, {0,2,8}};
  // set<int> res= takeIntrsec(setArray);
  // for (auto it=res.begin(); it != res.end(); ++it)
  //   {cout << ' ' << *it;}



}
