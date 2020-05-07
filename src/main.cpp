#include <iomanip>
#include <iostream>
#include <fstream>
#include <math.h>
#include <sstream>
#include <string>
#include <string.h>
#include <algorithm>
#include <chrono>
#include <list>
#include <vector>
#include <filesystem>
#include "spdlog/spdlog.h"
#include "CLI/CLI.hpp"
#include "bitArray.h"
#include "MyBloom.h"
#include "ArrayOfBF.h"
#include "MurmurHash3.h"
#include "Rambo_construction.h"
#include "utils.h"
#include "constants.h"

using namespace std;
namespace fs = std::filesystem;


int main(int argc, char** argv){
    CLI::App app{"RAMBO application"};
    CLI::App* build_sub = app.add_subcommand("build", "Build database");
    std::vector<fs::path> input_files;
    fs::path output_dir;
    build_sub->add_option(
            "input-files",
            input_files,
            "Input files"
    )->required();
    build_sub->add_option(
            "-o,--output",
            output_dir,
            "Directory to store serialized output"
    )->required();

    CLI::App* query_sub = app.add_subcommand("query", "Check if query is in database");
    query_sub->add_option(
            "input-files",
            input_files,
            "Input files"
    );

    CLI11_PARSE(app, argc, argv);
    string job = "0";

    bool ser = true;
    bool test = false;
    bool deser = false;

    int n_perSet = 500000000; //cardinality of each set
    float FPR = 0.01;
    int R_all = 5;
    int B_all = 500;

    int K = Ki; // total number of sets

    float fp_ops;
    float ins_time;
    float query_time;

    // constructor
    RAMBO myRambo(n_perSet, FPR, R_all, B_all, K);

    //  details of RAMBO set partitioning
    myRambo.createMetaRambo(K, false);
    cout<<"created meta"<<endl;

    //insert itno RAMBO
    string SerOpFile ="results/RAMBO_Ser" + job + '/';

    //if (deser){
      //vector<string> SerOpFile2;
      //SerOpFile2.push_back("results/RAMBO_Ser4/"); // mutliple files can be pushed here

      //cout<<"deser starting"<<endl;
      //myRambo.deserializeRAMBO(SerOpFile2);
      //std::cout << "desealized!" << '\n';
    //}

    if (app.got_subcommand("build")) {
      //log files
      std::ofstream failedFiles;
      failedFiles.open("logFileB500R5_"+job+".txt");

      for (std::string input_f: input_files) {
        auto t3 = chrono::high_resolution_clock::now();
        std::vector<string> setIDs = readlines(input_f, 0);

        for (uint ss = 0; ss < setIDs.size(); ss++){
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
        auto t4 = chrono::high_resolution_clock::now();
        cout << chrono::duration_cast<chrono::nanoseconds>(t4-t3).count()/1000000000.0 << "sec\n";
        ins_time = (t4-t3).count()/1000000000.0;
        failedFiles << "insertion time (sec) "<< ins_time << endl;
      }

      //serialize myRambo
      if (ser){
        cout<<"Serializing RAMBO at: "<< output_dir <<endl;
        myRambo.serializeRAMBO(output_dir);
        //for (int kpp=0;kpp<R_all*B_all;kpp++){
          //cout<<"packing: "<< myRambo.Rambo_array[kpp]->m_bits->getcount()<<endl;
        //}
      }
    }

    else if(app.got_subcommand("query"){
        // test RAMBO
        auto t5 = chrono::high_resolution_clock::now();
        int keysize = 30;
        std::vector<string> testKeys;
        testKeys = getRandomTestKeys(keysize, 1000);
        cout<<"loaded keys"<<endl;
        //testKeys = getctxdata("data/SRR1792494.out");
        float fp=0;
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
