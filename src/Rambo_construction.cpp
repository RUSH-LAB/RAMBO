#include <iomanip>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <chrono>
#include <vector>
#include <math.h>
#include <sstream>
#include <string>
#include <string.h>
#include <algorithm>
#include "Rambo_construction.h"
#include <set>
#include <iterator>
#include <bitset>

using namespace std;

vector<uint> RAMBO::hashfunc(std::string key, int len) {
  vector <uint> hashvals;
  uint op; // takes 4 byte
  for (int i = 0; i < R; i++) {
    MurmurHash3_x86_32(key.c_str(), len, i, &op); //seed i
    hashvals.push_back(op % B);
  }
  return hashvals;
}

//TODO shouldn't this be a set?
std::vector<std::string>  RAMBO::getdata(string filenameSet) {
    //get the size of Bloom filter by count
    ifstream cntfile (filenameSet);
    std::vector <std::string> allKeys;
    int totKmerscnt = 0;
    while (cntfile.good()) {
        string line1, vals;
        while (getline(cntfile, line1)) {
            stringstream is;
            is << line1;
            if (line1[0] != '>' && line1.size() > 30) {
                for (uint idx = 0; idx < line1.size() - 31 + 1; idx++) {
                    allKeys.push_back(line1.substr(idx, 31));
                    totKmerscnt++;
                }
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

  Rambo_array = new BloomFilter*[B*R]; //array of pointers
  // std::vector<BloomFilter>  Rambo_array(B*R, BloomFilter(n, p, range, k));
  metaRambo = new vector<int>[B*R]; //constains set info in it.
  for(int b=0; b<B; b++){
    for(int r=0; r<R; r++){
      // Rambo_array[b + B*r] = new BloomFilter(range, p, k);
      // std::cout << "Range is: "<<range << '\n';
      Rambo_array[b + B*r] = new BloomFilter(range, p, k);
    }
  }

}

// one time process- a preprocess step
void RAMBO::createMetaRambo(int K, bool verbose) {
    for (int i = 0; i < K; i++) {
        vector<uint> hashvals = RAMBO::hashfunc(
            std::to_string(i), 
            std::to_string(i).size()); // R hashvals, each with max value B
        // For each repitition, put the dataset name in the assigned partition
        for(int r=0; r<R; r++){
            this->metaRambo[hashvals[r] + B*r].push_back(i);
        }
    }
}

// give set and keys in the set
void RAMBO::insertion (std::string setID, std::vector<std::string> keys){
    vector<uint> hashvals = RAMBO::hashfunc(setID, setID.size()); // R hashvals
    #pragma omp parallel for
    for(std::size_t i=0; i<keys.size(); ++i){
        vector<uint> temp = myhash(keys[i].c_str(), keys[i].size() , k, range);
        for(int r=0; r<R; r++){
            this->Rambo_array[hashvals[r] + B*r]->insert(temp);
        }
    }
}

//// given inverted index type arrangement, kmer;files;files;..
//void RAMBO::insertion2 (std::vector<string> alllines, int V, int mr){
  //// V = 10; // multiplicity = 10
  //// merge = 5; //merging 5 rambo

  ////make this loop parallel
  //// #pragma omp parallel for
  //for(std::size_t i=0; i<alllines.size(); ++i){
    //char d = ';';
    //std::vector<string>KeySets =  line2array(alllines[i], d);//sets for a key
    //vector<uint> temp = myhash(KeySets[0].c_str(), KeySets[0].size() , k, range);// i is the key

    //std::vector<string>KeySet = line2array(KeySets[mr], ',');
    //for (int j = 0; j<V; j++){
      //vector<uint> hashvals = RAMBO::hashfunc(KeySet[j], KeySet[j].size()); // R hashvals
      //for(int r=0; r<R; r++){
        //Rambo_array[hashvals[r] + B*r]->insert(temp);
      //}
    //}
  //}
//}

set<int> RAMBO::takeunion(set<int> set1, set<int>& set2){
    set1.insert(set2.begin(), set2.end());
    return set1;
}

set<int> RAMBO::takeIntrsec(set<int>* setArray){
    set<int> s1 = setArray[0];
    for (int i = 1; i < R; i++){
        set<int> res;
        set_intersection(
            s1.begin(), 
            s1.end(),
            setArray[i].begin(),
            setArray[i].end(), 
            std::inserter(res,res.begin()));
        //TODO is res being copied at every step?
        s1 = res;
    }
    return s1;
}

bitArray RAMBO::query(string query_key, int len) {
    bitArray bitarray_K(Ki);
    float count=0.0;
    vector<uint> check = myhash(query_key, len , k, range); //hash values correspondign to the keys
    for(int r=0; r<R; r++){
        bitArray bitarray_K1(Ki);
        for(int b = 0; b < B; b++) {
            if (Rambo_array[b + B*r]->test(check)){
                auto t5 = chrono::high_resolution_clock::now();
                for (uint j=0; j<metaRambo[b + B*r].size(); j++){
                    bitarray_K1.bitIt[metaRambo[b + B*r][j]] = bit::bit1;
                }
                auto t6 = chrono::high_resolution_clock::now();
                count += ((t6-t5).count()/1000000000.0);
            }
        }
        if (r == 0) {
            bitarray_K = bitarray_K1;
        }
        else{
            bitarray_K.ANDop(bitarray_K1.A);
        }
    }
    return bitarray_K;
}

// TODO more freedom for naming/saving... 

void RAMBO::serializeRAMBO(const fs::path dir){
    std::filesystem::create_directories(dir);
    std::cout << dir << std::endl;
    for(int b=0; b<B; b++){
        for(int r=0; r<R; r++){
            fs::path br = dir / fs::path("repitition_" + to_string(r) + "_group" + to_string(r) + ".txt");
            Rambo_array[b + B*r]->serializeBF(br);
        }
    }
}

void RAMBO::deserializeRAMBO(vector<fs::path> dir){
    for(int b=0; b<B; b++){
        for(int r=0; r<R; r++){
            vector<fs::path> br;
            for (uint j=0; j<dir.size(); j++){
                br.push_back(dir[j] / fs::path("repitition_" + to_string(r) + "_group" + to_string(r) + ".txt"));
            }
            Rambo_array[b + B*r]->deserializeBF(br);
        }
    }
}
