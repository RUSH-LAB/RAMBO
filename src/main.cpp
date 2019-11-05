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
#include "ArrayOfBF.h"
#include "utils.h"
#include "mybitarray.h"
#include "constants.h"
#include "intbitArray.h"


using namespace std;

int main(int argc, char** argv){

//RAMBO number is :
string job(argv[1]);

bool insert  = false;
bool ser = false;
bool test = true;
bool deser = true;

int n_perSet = 500000000; //cardinality of each set
float FPR = 0.01;
int R_all = 5;
int B_all = 500;

int K = Ki; // total number of sets

float fp_ops;
float ins_time;
float query_time;
//cout<<"here"<<endl;

// constructor
RAMBO myRambo(n_perSet, FPR, R_all, B_all, K);

//  details of RAMBO set partitioning
myRambo.createMetaRambo (K, false);
cout<<"created meta"<<endl;

//insert itno RAMBO

string SerOpFile ="results/RAMBO_Ser" + job + '/';

if (deser){
  vector<string> SerOpFile2;
  SerOpFile2.push_back("/home/gg29/BIG_exp/FinalOps/RAMBO_Ser5/");
  SerOpFile2.push_back("/home/gg29/BIG_exp/FinalOps/RAMBO_Ser4/");

  cout<<"deser starting"<<endl;
  myRambo.deserializeRAMBO(SerOpFile2);
  std::cout << "desealized!" << '\n';
}

if (insert == true){
  //log files
  std::ofstream failedFiles;
  failedFiles.open("logFileB500R5_"+job+".txt");

  for (int batch =0; batch<47; batch++){
    chrono::time_point<chrono::high_resolution_clock> t3 = chrono::high_resolution_clock::now();

    string dataPath = "data/"+ job +"/inflated/" + to_string(batch) + "_indexed.txt";
    std::vector<string> setIDs = readlines(dataPath, 0);

    for (uint ss=0; ss<setIDs.size(); ss++){
      char d = ',';
      vector<std::string> setID = line2array(setIDs[ss], d);
      string mainfile = "data/"+ job +"/inflated/" + setID[1]+ ".out";
      vector<std::string> keys = getctxdata(mainfile);
      failedFiles<<mainfile<<" : "<<keys.size()<<std::endl;

      if (keys.size()==0){
          std::cout<<mainfile<<" does not exists or empty "<<std::endl;
          failedFiles<<mainfile<<" does not exists or empty "<<std::endl;
      }

      myRambo.insertion(setID[0], keys);
      
    }
    chrono::time_point<chrono::high_resolution_clock> t4 = chrono::high_resolution_clock::now();
    cout << chrono::duration_cast<chrono::nanoseconds>(t4-t3).count()/1000000000.0 << "sec\n";
    ins_time = (t4-t3).count()/1000000000.0;
    failedFiles<<"insertion time (sec) of 100 files: "<<ins_time<<endl;
  }

  //serialize myRambo
  if (ser){
	cout<<"Serializing RAMBO at: "<<SerOpFile<<endl;
    myRambo.serializeRAMBO(SerOpFile);
   for (int kpp=0;kpp<30;kpp++){
	cout<<"packing: "<<myRambo.Rambo_array[kpp]->m_bits->getcount()<<endl;
	 }

  }
}

if(test){
  // test RAMBO
    for (int kpp=0;kpp<30;kpp++){
	cout<<"packing: "<<myRambo.Rambo_array[kpp]->m_bits->getcount()<<endl;
    }
    chrono::time_point<chrono::high_resolution_clock> t5 = chrono::high_resolution_clock::now();
    int keysize = 30;
    std::vector<string> testKeys;
    testKeys = getRandomTestKeys(keysize, 1000);
    cout<<"loaded keys"<<endl;
    //testKeys = getctxdata("data/SRR1792494.out");
    float fp=0, tot=0;
    std::ofstream FPtestFile;
    FPtestFile.open("FPtestFile5+4.txt");
    // #pragma omp parallel for
    for (uint i=0; i<1000; i++){
	cout<<testKeys[i]<<endl;
      bitArray MemVec = myRambo.query (testKeys[i], testKeys[i].size());

      int gt_size = 0;
      //fp = fp + MemVec.getcount() - gt_size;
      //tot = tot + (4605 - gt_size);
      cout<<MemVec.getcount() <<endl;
      FPtestFile<<MemVec.getcount() <<endl;
      fp = fp + (MemVec.getcount() - gt_size)*0.1/((Ki - gt_size)*0.1);
    }

    cout<<"fp rate is: "<<fp/testKeys.size();; // false positives/(all negatives)
    fp_ops = fp/testKeys.size();;

    cout<<endl;
    chrono::time_point<chrono::high_resolution_clock> t6 = chrono::high_resolution_clock::now();
    cout <<"query time:" <<chrono::duration_cast<chrono::nanoseconds>(t6-t5).count()/1000000000.0 << "sec\n";
    query_time = (t6-t5).count()/1000000000.0;

    // writeRAMBOresults("results/fp_ops.txt", numB, numR, fp_ops);
    // writeRAMBOresults("results/ins_time.txt", numB, numR, ins_time);
    // writeRAMBOresults("results/query_time.txt", numB, numR, query_time);
  }

return 0;
}
