#include <iomanip>
#include <fstream>
#include <iostream>
#include <chrono>
#include <vector>
#include <math.h>
#include <sstream>
#include <string>
#include <string.h>
#include <algorithm>
#include "MyBloom.h"
#include "MurmurHash3.h"
#include "Rambo_construction.h"
#include "utils.h"
#include "constants.h"
#include "bitArray.h"
#include <set>
#include <iterator>
#include <bitset>

using namespace std;

vector<uint> RAMBO:: hashfunc( std::string key, int len){
  // int hashvals[k];
  vector <uint> hashvals;
  uint op; // takes 4 byte
  for (int i=0; i<R; i++){
    MurmurHash3_x86_32(key.c_str(), len, i, &op); //seed i
    hashvals.push_back(op%B);
  }
  return hashvals;
}

std::vector <std::string>  RAMBO:: getdata(string filenameSet){

  //get the size of Bloom filter by count
  ifstream cntfile (filenameSet);
  std::vector <std::string> allKeys;
  int totKmerscnt = 0;
  while ( cntfile.good() )
      {
          string line1, vals;
          while( getline ( cntfile, line1 ) ){
              stringstream is;
              is<<line1;
              if (line1[0]!= '>' && line1.size()>30 ){
                for (uint idx =0; idx<line1.size()-31 +1; idx++){
                  // std::cout<<line1.substr(idx, 31)<<std::endl;
                  allKeys.push_back(line1.substr(idx, 31));
                  totKmerscnt++;
                }
                // while(getline (is, vals, '\n' )){
                //     // std::cout<<vals<<std::endl;
                //     allKeys.push_back(vals);
                //     // std::cout<<vals.c_str()<<std::endl;
                // }
              }
          }
      }
      std::cout<<"total inserted from one file: "<<totKmerscnt<<std::endl;
      return allKeys;
  }


RAMBO::RAMBO(int n, float fpr1, int r1, int b1, int K){
  p = fpr1;
  FPR = fpr1;
  R = r1;
  B = b1;
  K = K;

  //range = ceil(-(n*log(p))/(log(2)*log(2))); //range
  range = n;
  std::cout << "range" <<range<< '\n';
  // range = capacity;
  //k = ceil(-log(p)/log(2)); //number of hash, k is 7 for 0.01
  k = 2;

  Rambo_array = new BloomFiler*[B*R]; //array of pointers
  // std::vector<BloomFiler>  Rambo_array(B*R, BloomFiler(n, p, range, k));
  metaRambo = new vector<int>[B*R]; //constains set info in it.
  for(int b=0; b<B; b++){
    for(int r=0; r<R; r++){
      // Rambo_array[b + B*r] = new BloomFiler(range, p, k);
      // std::cout << "Range is: "<<range << '\n';
      Rambo_array[b + B*r] = new BloomFiler(range, p, k);
    }
  }

}
// one time process- a preprocess step
void RAMBO::createMetaRambo(int K, bool verbose){
  for(int i=0;i<K;i++){
    vector<uint> hashvals = RAMBO::hashfunc(std::to_string(i), std::to_string(i).size()); // R hashvals, each with max value B
    for(int r=0; r<R; r++){
      metaRambo[hashvals[r] + B*r].push_back(i);
    }
  }

  //print RAMBO meta deta
  if (verbose){
    for(int b=0; b<B; b++){
      for(int r=0; r<R; r++){
        for (auto it=metaRambo[b + B*r].begin(); it != metaRambo[b + B*r].end(); ++it)
          {std::cout << " " << *it;}
        {std::cout << "////";}
      }
      std::cout << '\n';
    }
}
}

// give set and keys in the set
void RAMBO::insertion (std::string setID, std::vector<std::string> keys){
  vector<uint> hashvals = RAMBO::hashfunc(setID, setID.size()); // R hashvals

  //make this loop parallel
  #pragma omp parallel for
  for(std::size_t i=0; i<keys.size(); ++i){
    vector<uint> temp = myhash(keys[i].c_str(), keys[i].size() , k, range);
    for(int r=0; r<R; r++){
      Rambo_array[hashvals[r] + B*r]->insert(temp);
    }
  }
}

// given inverted index type arrangement, kmer;files;files;..
void RAMBO::insertion2 (std::vector<string> alllines, int V, int mr){
  // V = 10; // multiplicity = 10
  // merge = 5; //merging 5 rambo

  //make this loop parallel
  // #pragma omp parallel for
  for(std::size_t i=0; i<alllines.size(); ++i){
    char d = ';';
    std::vector<string>KeySets =  line2array(alllines[i], d);//sets for a key
    vector<uint> temp = myhash(KeySets[0].c_str(), KeySets[0].size() , k, range);// i is the key

    std::vector<string>KeySet = line2array(KeySets[mr], ',');
    for (int j = 0; j<V; j++){
      vector<uint> hashvals = RAMBO::hashfunc(KeySet[j], KeySet[j].size()); // R hashvals
      for(int r=0; r<R; r++){
        Rambo_array[hashvals[r] + B*r]->insert(temp);
      }
    }
  }
}

set<int> RAMBO::takeunion(set<int> set1, set<int> set2){
  set<int> res;
  res.insert(set1.begin(), set1.end());
  res.insert(set2.begin(), set2.end());
  return res;
}

set<int> RAMBO::takeIntrsec(set<int>* setArray){
  set<int> res;
  set<int> s1 = setArray[0];
  for (int i=1; i<R; i++){
    set<int> res;
    set_intersection(s1.begin(),s1.end(),setArray[i].begin(),setArray[i].end(), std::inserter(res,res.begin()));
    s1 = res;
  }
  return s1;
}

bitArray RAMBO::query (string query_key, int len){
  // set<int> resUnion[R]; //constains union results in it.
  bitArray bitarray_K(Ki);
  // bitset<Ki> bitarray_K;
  // set<int> res;
  float count=0.0;
  vector<uint> check = myhash(query_key, len , k, range); //hash values correspondign to the keys

  for(int r=0; r<R; r++){
    bitArray bitarray_K1(Ki);
    // bitset<Ki> bitarray_K1;
    for(int b=0; b<B; b++){
        if (Rambo_array[b + B*r]->test(check)){
          chrono::time_point<chrono::high_resolution_clock> t5 = chrono::high_resolution_clock::now();
          for (uint j=0; j<metaRambo[b + B*r].size(); j++){
            bitarray_K1.SetBit(metaRambo[b + B*r][j]);
        }
        chrono::time_point<chrono::high_resolution_clock> t6 = chrono::high_resolution_clock::now();
        count+=((t6-t5).count()/1000000000.0);
      }
    }
    if (r ==0){
      bitarray_K = bitarray_K1;
    }
    else{
      bitarray_K.ANDop(bitarray_K1.A);
    }
  }
  return bitarray_K;
}

void RAMBO::serializeRAMBO(string dir){
  for(int b=0; b<B; b++){
    for(int r=0; r<R; r++){
      string br = dir + to_string(b) + "_" + to_string(r) + ".txt";
      Rambo_array[b + B*r]->serializeBF(br);
    }
  }
}

void RAMBO::deserializeRAMBO(vector<string> dir){
  for(int b=0; b<B; b++){
    for(int r=0; r<R; r++){
      vector<string> br;
     	for (uint j=0; j<dir.size(); j++){
	  br.push_back(dir[j] + to_string(b) + "_" + to_string(r) + ".txt");
	}
      Rambo_array[b + B*r]->deserializeBF(br);

    }
  }
}
