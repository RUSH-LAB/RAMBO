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
#include "CLI/App.hpp"
#include "CLI/Validators.hpp"
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
    /*
     *Command line parsing
     */
    CLI::App app{"RAMBO application"};
    app.require_subcommand(1, 1);
    CLI::App* build_sub = app.add_subcommand("build", "Build database")->fallthrough();
    std::vector<fs::path> input_files;
    std::vector<fs::path> input_kmers;
    fs::path output_dir("output");
    bool verbose;
    unsigned int num_threads = 1;

    // Generic flags
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
    
    // Build flags
    int n_per_set = 100000000; //cardinality of each set
    float FPR = 0.01;
    int R_all = 10;
    int B_all = 50;
    build_sub->add_option(
            "input-files",
            input_files,
            "Input files"
    )->required()->check(CLI::ExistingFile);
    build_sub->add_option(
            "-o,--output",
            output_dir,
            "Directory to store serialized output"
    );
    build_sub->add_option(
            "-R, --repitions",
            R_all,
            "Number of repititions in RAMBO index"
    );
    build_sub->add_option(
            "-B, --blooms-per-rep",
            B_all,
            "Number of bloom filters per repitition"
    );
    build_sub->add_option(
            "-n, --set-cardinality",
            n_per_set,
            "Approximate number of elements per set"
    );

    // Insert flags
    CLI::App* insert_sub = app.add_subcommand("insert", "Insert samples into database")->fallthrough();
    fs::path database_dir;
    insert_sub->add_option(
            "input-files",
            input_files,
            "Input files"
    )->required()->check(CLI::ExistingFile);
    insert_sub->add_option(
            "-d,--database",
            database_dir,
            "Path to RAMBO database directory"
    )->required()->check(CLI::ExistingDirectory);

    // Query flags
    CLI::App* query_sub = app.add_subcommand("query", "Check if query is in database")->fallthrough();
    auto input_file_opt = query_sub->add_option(
            "-i,--input-files",
            input_files,
            "Input files. For each file, RAMBO will check if database contains any entries which are a superset of the input file."
    )->check(CLI::ExistingFile);
    query_sub->add_option(
            "-k,--input-kmers",
            input_kmers,
            "Input kmers. For each kmer, RAMBO will check if database contains any entries which contain the kmer"
    )->excludes(input_file_opt);
    query_sub->add_option(
            "-d,--database",
            database_dir,
            "Path to RAMBO database directory"
    )->required()->check(CLI::ExistingDirectory);

    CLI11_PARSE(app, argc, argv);

    /*
     *Constants and assertions 
     */
    spdlog::set_level(verbose ? spdlog::level::debug : spdlog::level::info); // Set global log level to debug
    omp_set_num_threads(num_threads);


    /*
     *Main workflow
     */
    if (app.got_subcommand("build")) {
        // constructor
        RAMBO myRambo(n_per_set, FPR, R_all, B_all, input_files);
        myRambo.serializeRAMBO(output_dir);
    } else if (app.got_subcommand("insert")) {
        RAMBO rambo(database_dir);
        spdlog::info("Inserting kmers...");
        for (fs::path input_f: input_files) {
            rambo.insertion(input_f);
        }
        rambo.serializeRAMBO(database_dir);
    } else if(app.got_subcommand("query")){
        RAMBO rambo(database_dir);
        spdlog::info("Querying RAMBO index...");
        for (fs::path input_f: input_files) {
            auto results = rambo.query(input_f);
            if (results.size() > 0) {
                spdlog::info("{} found in the following files:", input_f.stem().string());
                for (auto sample : results) {
                    std::cout << sample << " ";
                }
                std::cout << std::endl;
            } else {
                spdlog::info("{} not found in database!", input_f.stem().string());
            }
        }
        
    }


    return 0;
}
