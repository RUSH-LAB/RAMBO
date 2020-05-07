#include "ArrayOfBF.h"
#include <math.h>

using namespace std;

ArBF::ArBF(int n, float fpr1, int n_sets){
  //total K setIDs
  //len = length of each setID
  p = fpr1;
  FPR = fpr1;
  K = n_sets;
  range = ceil(-(n*log(p))/(log(2)*log(2))); //range
  k = ceil(-log(p)/log(2)); //number of hash, k is 7 for 0.01

  ArBF_array = new BloomFilter*[K]; //array of pointers

  for(int b=0; b<K; b++){
      ArBF_array[b] = new BloomFilter(range, p, k);
  }
}

// give set and keys in the set
void ArBF::insertion (std::vector<string> alllines){
  //make this loop parallel
  // #pragma omp parallel for
  for(std::size_t i=3; i<alllines.size(); ++i){
    char d = ',';
    std::vector<string>KeySets =  line2array(alllines[i], d);//sets for a key
    vector<uint> temp = myhash(to_string(i).c_str(), to_string(i).size() , k, range);// i is the key

    for (uint j = 0; j<KeySets.size(); j++){
      // cout<<"string: "<< KeySets[j]<<"  int: "<<stoi(KeySets[j]);
        // cout<<"insert at:  "<<temp[0]<<endl;
        ArBF_array[stoi(KeySets[j])]->insert(temp);
    }

    // cout<<endl;
  }
}

set<int> ArBF::query (string query_key, int len){
  set<int> res;

  vector<uint> check = myhash(query_key.c_str(), query_key.size() , k, range); //hash values correspondign to the keys
    // opvals[r].push_back(Rambo_array[b + B*r]->test(check)); //will op the membership test output
  for (int b=0; b<K; b++){
    // cout<<"query at:  "<<check[0]<<endl;
    if (ArBF_array[b]->test(check)){
      res.insert(b);
    }
  }
  return res;
}
