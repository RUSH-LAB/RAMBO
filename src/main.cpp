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
#include "tqdm.h"
#include "bitArray.h"
#include "MyBloom.h"
#include "Rambo_construction.h"
#include "utils.h"

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
    fs::path input_kmers_file = "";
    fs::path database_output_dir("database_dir");
    fs::path query_output_prefix("query");
    bool verbose = false;
    bool show_progress = false;
    unsigned int num_threads = 1;

    // Generic flags
    app.add_flag(
            "-v,--verbose",
            verbose,
            "Display debug output"
    );
    app.add_flag(
            "--show-progress",
            show_progress,
            "Display progress bars"
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
            database_output_dir,
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
            "-n, --bloom-size",
            n_per_set,
            "Size of each bloom filter (in bits)"
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
    bool flatten_input = false;
    query_sub->add_option(
            "input-files",
            input_files,
            "Input files. For each file, RAMBO will check if database contains any entries which are a superset of the input file. Use the --flatten flag if each kmer should be treated independently"
    )->check(CLI::ExistingFile);
    //query_sub->add_flag(
            //"-f,--flatten",
            //flatten_input,
            //"Treat each kmer from each input file as an individual sample"
    //);
    query_sub->add_option(
            "-d,--database",
            database_dir,
            "Path to RAMBO database directory"
    )->required()->check(CLI::ExistingDirectory);
    query_sub->add_option(
            "-o,--output",
            query_output_prefix,
            "Prefix to TSV output file. Results will be stored at <prefix>_results.tsv"
    );

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
        myRambo.serializeRAMBO(database_output_dir);
    } else if (app.got_subcommand("insert")) {
        RAMBO rambo(database_dir);
        spdlog::info("Inserting kmers...");
        tqdm bar;
        unsigned int prog_i = 0;
        #pragma omp parallel
        {
            #pragma omp for schedule(dynamic)
            for (unsigned int i = 0; i < input_files.size(); ++i) {
                auto input_f = input_files[i];
                rambo.insertion(input_f);
                if (show_progress) {
                    #pragma omp critical
                    {
                        prog_i += 1;
                        bar.progress(prog_i, input_files.size());
                    }
                }
            }
        }
        bar.finish();
        rambo.serializeRAMBO(database_dir);
    } else if(app.got_subcommand("query")){
        fs::path output_file = fs::path(query_output_prefix.string() + "_results.tsv");
        RAMBO rambo(database_dir);
        spdlog::info("Querying RAMBO index...");
        for (fs::path input_f: input_files) {
            rambo.query_kmers(input_f, output_file);
        }
        // Implement once fasta support is added
        //if (flatten_input) {
            //for (fs::path input_f: input_files) {
                //rambo.query_kmers(input_f, output_file);
            //}
        //} else {
            //for (fs::path input_f: input_files) {
                //auto results = rambo.query_full_file(input_f, show_progress);
                //if (results.size() > 0) {
                    //spdlog::info("{} found in the following samples:", input_f.stem().string());
                    //for (auto sample : results) {
                        //std::cout << sample << " ";
                    //}
                    //std::cout << std::endl;
                //} else {
                    //spdlog::info("{} not found in database!", input_f.stem().string());
                //}
            //}
        //}
        
    }
    return 0;
}
