#include <iomanip>
#include <iostream>
#include <fstream>
#include <math.h>
#include <sstream>
#include <string>
#include <string.h>
#include <algorithm>
#include <omp.h>
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
    app.require_subcommand(1, 1);
    CLI::App* build_sub = app.add_subcommand("build", "Build database")->fallthrough();
    std::vector<fs::path> input_files;
    fs::path output_dir("output");
    bool verbose;
    unsigned int num_threads = 1;
    app.add_flag(
            "-v,--verbose",
            verbose,
            "Display debug output"
    );
    app.add_option(
            "-p,--threads",
            num_threads,
            "Number of threads to use"
    );
    
    build_sub->add_option(
            "input-files",
            input_files,
            "Input files"
    )->required();
    build_sub->add_option(
            "-o,--output",
            output_dir,
            "Directory to store serialized output"
    );
    CLI::App* insert_sub = app.add_subcommand("insert", "Insert samples into database")->fallthrough();
    fs::path database_dir;
    insert_sub->add_option(
            "input-files",
            input_files,
            "Input files"
    )->required();
    insert_sub->add_option(
            "-d,--database",
            database_dir,
            "Path to RAMBO database directory"
    )->required();

    CLI::App* query_sub = app.add_subcommand("query", "Check if query is in database");
    query_sub->add_option(
            "input-files",
            input_files,
            "Input files"
    );

    CLI11_PARSE(app, argc, argv);
    spdlog::set_level(verbose ? spdlog::level::debug : spdlog::level::info); // Set global log level to debug
    omp_set_num_threads(num_threads);

    int n_perSet = 500000; //cardinality of each set
    float FPR = 0.01;
    int R_all = 5;
    int B_all = 100;

    if (app.got_subcommand("build")) {
        // constructor
        RAMBO myRambo(n_perSet, FPR, R_all, B_all, input_files);
        myRambo.serializeRAMBO(output_dir);
    } else if (app.got_subcommand("insert")) {
        RAMBO rambo(database_dir);
        spdlog::info("Inserting kmers...");
        for (fs::path input_f: input_files) {
            rambo.insertion(input_f);
        }
        rambo.serializeRAMBO(database_dir);
    } else if(app.got_subcommand("query")){
        // test RAMBO
        //auto t5 = chrono::high_resolution_clock::now();
        //int keysize = 30;
        //std::vector<string> testKeys;
        //testKeys = getRandomTestKeys(keysize, 1000);
        //cout<<"loaded keys"<<endl;
        ////testKeys = getctxdata("data/SRR1792494.out");
        //float fp=0;
        //std::ofstream FPtestFile;
        //FPtestFile.open("FPtestFile5+4.txt");
        //// #pragma omp parallel for
        //for (uint i=0; i<1000; i++){
        //cout<<testKeys[i]<<endl;
          //bitArray MemVec = myRambo.query (testKeys[i], testKeys[i].size());

          //int gt_size = 0;
          ////fp = fp + MemVec.getcount() - gt_size;
          ////tot = tot + (4605 - gt_size);
          //cout<<MemVec.getcount() <<endl;
          //FPtestFile<<MemVec.getcount() <<endl;
          //fp = fp + (MemVec.getcount() - gt_size)*0.1/((Ki - gt_size)*0.1);
        //}

        //cout<<"fp rate is: "<<fp/testKeys.size();; // false positives/(all negatives)

        //cout<<endl;
        //chrono::time_point<chrono::high_resolution_clock> t6 = chrono::high_resolution_clock::now();
        //cout <<"query time:" <<chrono::duration_cast<chrono::nanoseconds>(t6-t5).count()/1000000000.0 << "sec\n";

        // writeRAMBOresults("results/fp_ops.txt", numB, numR, fp_ops);
        // writeRAMBOresults("results/ins_time.txt", numB, numR, ins_time);
        // writeRAMBOresults("results/query_time.txt", numB, numR, query_time);
    } else {
        cout << "ERROR\n";
    }


    return 0;
}
