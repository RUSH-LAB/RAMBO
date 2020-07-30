#include <iomanip>
#include <unordered_set>
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
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "tqdm.h"
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

RAMBO::RAMBO(int n, float fpr1, int r1, int b1, std::vector<fs::path> input_files){
    FPR = fpr1;
    R = r1;
    B = b1;
    K = input_files.size();

    k = 3;
    range = n;
    spdlog::info("Creating RAMBO index with R={}, B={}, n={}, k={}", R, B, range, k);

    Rambo_array = new BloomFilter*[B*R]; //array of pointers

    metaRambo = new vector<int>[B*R]; //constains set info in it.
    for(int b=0; b<B; b++){
        for(int r=0; r<R; r++){
            Rambo_array[b + B*r] = new BloomFilter(range, p, k);
        }
    }
    spdlog::info("Inserting kmers...");
    tqdm bar;
    unsigned int prog_i = 0;
    #pragma omp parallel
    {
        #pragma omp for schedule(dynamic)
        for (fs::path input_f: input_files) {
            this->insertion(input_f);
            #pragma omp critical
            {
                prog_i += 1;
                bar.progress(prog_i, input_files.size());
            }
        }
    }
    bar.finish();
}

RAMBO::RAMBO(fs::path rambo_dir){
    string line;
    std::ifstream idx_stream (rambo_dir / "idx_to_name.txt"); 
    unsigned int idx = 0;
    while (idx_stream.good()) {
        getline(idx_stream, line);
        if (line.size() > 0) {
            this->name_to_idx[line] = idx;
            this->idx_to_name.push_back(line);
            this->idx_and_r_to_b.push_back(std::vector<unsigned int>());
        }
    }
    std::ifstream meta_stream (rambo_dir / "metarambo.txt"); 
    getline(meta_stream, line);
    size_t pos = line.find(' ');
    this->R = std::stoi(line.substr(0, pos));
    line.erase(0, pos + 1);
    pos = line.find(' ');
    this->B = std::stoi(line.substr(0, pos));
    line.erase(0, pos + 1);
    pos = line.find(' ');
    this->range = std::stoi(line.substr(0, pos));
    line.erase(0, pos + 1);
    pos = line.find(' ');
    this->k = std::stoi(line.substr(0, pos));
    this->metaRambo = new vector<int>[this->B*this->R]; //constains set info in it.
    unsigned int r = 0;
    unsigned int b = 0;
    while (meta_stream.good()) {
        getline(meta_stream, line);
        if (line[0] == '#') {
            line.erase(0, 2);
            pos = line.find(' ');
            r = std::stoi(line.substr(0, pos));
            line.erase(0, pos + 1);
            pos = line.find(' ');
            b = std::stoi(line.substr(0, pos));
        } else if (line.length() > 0) {
            this->metaRambo[B*r + b].push_back(std::stoi(line));
            this->idx_and_r_to_b[std::stoi(line)].push_back(b); // Assumes everything is in order
        }
    }
    spdlog::debug("Metarambo loaded");
    spdlog::info("Loading RAMBO index with R={}, B={}, n={}, k={}", R, B, range, k);
    spdlog::debug("Allocating memory for bloom filters...");
    Rambo_array = new BloomFilter*[B*R]; //array of pointers
    for(int b=0; b<B; b++){
        for(int r=0; r<R; r++){
            Rambo_array[b + B*r] = new BloomFilter(range, range, k);
        }
    }
    spdlog::debug("Deserializing bloom filters...");
    this->deserializeRAMBO(rambo_dir);
}


// give set and keys in the set
void RAMBO::insertion (fs::path input_file) {
    auto sample_name = input_file.stem().string();
    std::vector<unsigned int> hashvals;
    #pragma omp critical 
    {
        if (name_to_idx.find(sample_name) != name_to_idx.end()) {
            spdlog::error("{} already in rambo index!", sample_name);
        } else { 
            hashvals = RAMBO::hashfunc(
                sample_name, 
                sample_name.length()); // R hashvals, each with max value B
            name_to_idx[sample_name] = idx_to_name.size();
            this->idx_to_name.push_back(sample_name);
            this->idx_and_r_to_b.push_back(hashvals);
        }
    }
    if (hashvals.empty()) {
        return;
    }
    vector<std::string> keys = get_kmers(input_file);

    // For each repitition, put the dataset name in the assigned partition
    for(int r=0; r<R; r++){
        this->metaRambo[B*r + hashvals[r]].push_back(this->name_to_idx[sample_name]);
    }

    //#pragma omp parallel for
    for(std::size_t i=0; i<keys.size(); ++i){
        for(int r=0; r<R; r++){
            vector<uint> temp = myhash(keys[i], keys[i].size() , k, range, r+1);
            this->Rambo_array[hashvals[r] + B*r]->insert(temp);
        }
    }
}

//std::vector<std::string> RAMBO::query(std::string query_key) {
    //spdlog::debug("Query RAMBO for {}", query_key);
    //auto begin_time = std::chrono::high_resolution_clock::now();
    //std::set<unsigned int> hits;
    //vector<uint> check = myhash(query_key, query_key.size(), k, range, 1);
    //for(int b = 0; b < B; b++) {
        //if (Rambo_array[b]->test(check)){
            //for (uint j=0; j<metaRambo[b].size(); j++){
                //hits.insert(metaRambo[b][j]);
            //}
        //}
    //}
    //for(int r=1; r<R; r++){
        //if (hits.empty()) {
            //break;
        //}
        //std::set<unsigned int> rep_hits;
        //int hit_count = 0;
        //vector<uint> check = myhash(query_key, query_key.size(), k, range, r+1);
        //for(int b = 0; b < B; b++) {
            //if (Rambo_array[b + B*r]->test(check)){
                //++hit_count;
                //for (uint j=0; j<metaRambo[b + B*r].size(); j++){
                    //rep_hits.insert(metaRambo[b + B*r][j]);
                //}
            //}
        //}
        ////std::cout << "BFs hit " << hit_count << std::endl;
        ////std::cout << "Potential_hits: " << hits.size() << std::endl;
        ////std::cout << "Rep hits: " << rep_hits.size() << std::endl;
        //std::set<unsigned int> temp_set;
        //std::set_intersection(hits.begin(), hits.end(), rep_hits.begin(), rep_hits.end(), std::inserter(temp_set, temp_set.end())); 
        //std::swap(temp_set, hits);
    //}
        ////std::cout << "Potential_hits: " << hits.size() << std::endl;

    //std::vector<std::string> ret_samples;
    //for (unsigned int idx: hits) {
        //ret_samples.push_back(this->idx_to_name[idx]);
    //}
    //auto end_time = std::chrono::high_resolution_clock::now();
    //auto total_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time-begin_time).count();
    //spdlog::debug("Query took {} microseconds", total_time);

    //return ret_samples;
//}

std::vector<std::string> RAMBO::query(std::string query_key) {
    auto begin_time = std::chrono::high_resolution_clock::now();
    bitArray bitarray_K(this->idx_to_name.size());
    bool is_init = false;
    //for(int r=0; r<R; r++){
        //vector<uint> check = myhash(query_key, query_key.size(), k, range, r+1);
        //bitArray bitarray_K1(this->idx_to_name.size());
        //for(int b = 0; b < B; b++) {
            //if (Rambo_array[b + B*r]->test(check)){
                //for (uint j=0; j<metaRambo[b + B*r].size(); j++){
                    //bitarray_K1.bitIt[metaRambo[b + B*r][j]] = bit::bit1;
                //}
            //}
        //}
        //if (!is_init) {
            //bitarray_K = std::move(bitarray_K1);
            //is_init = true;
        //}
        //else{
            //bitarray_K.ANDop(&*bitarray_K1.A.begin());
        //}
    //}
    vector<uint> check = myhash(query_key, query_key.size(), k, range, 1);
    bitArray bitarray_K1(this->idx_to_name.size());
    std::set<unsigned int> to_check_next;
    for(int b = 0; b < B; b++) {
        if (Rambo_array[b]->test(check)){
            for (auto idx: metaRambo[b]) {
                to_check_next.insert(this->idx_and_r_to_b[idx][1]);
                bitarray_K.bitIt[idx] = bit::bit1;
            }
        }
    }
    for(int r=1; r<R; r++){
        std::set<unsigned int> to_check = to_check_next;
        to_check_next.clear();
        vector<uint> check = myhash(query_key, query_key.size(), k, range, r+1);
        bitArray bitarray_K1(this->idx_to_name.size());
        for(auto b: to_check) {
            if (Rambo_array[b + B*r]->test(check)){
                for (auto idx: metaRambo[b + B*r]) {
                    if (r < R - 1) {
                        to_check_next.insert(this->idx_and_r_to_b[idx][r + 1]);
                    }
                    bitarray_K1.bitIt[idx] = bit::bit1;
                }
            }
        }
        
        bitarray_K.ANDop(&*bitarray_K1.A.begin());
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    auto total_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time-begin_time).count();
    spdlog::debug("Query took {} microseconds", total_time);
    std::vector<std::string> ret_samples;
    auto it = bitarray_K.bitIt;
    it = bit::find(it, bitarray_K.end, bit::bit1);
    while (it != bitarray_K.end) {
        ret_samples.push_back(this->idx_to_name[bit::distance(bitarray_K.bitIt, it)]);
        it = bit::find(it + 1, bitarray_K.end, bit::bit1);
    }
    end_time = std::chrono::high_resolution_clock::now();
    total_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time-begin_time).count();
    spdlog::debug("Query took {} microseconds", total_time);
    return ret_samples;
}

std::vector<std::string> RAMBO::query_full_file(fs::path input_file, bool show_progress) {
    bitArray bitarray_K(this->range);
    vector<std::string> keys = get_kmers(input_file);
    bool is_init = false;
    tqdm bar;
    unsigned int prog_i = 0;
    for (auto query_key: keys) {
        if (show_progress) 
            bar.progress(prog_i++, keys.size());
        for(int r=0; r<R; r++){
            vector<uint> check = myhash(query_key, query_key.size(), k, range, r+1);
            bitArray bitarray_K1(this->range);
            for(int b = 0; b < B; b++) {
                if (Rambo_array[b + B*r]->test(check)){
                    for (uint j=0; j<metaRambo[b + B*r].size(); j++){
                        bitarray_K1.bitIt[metaRambo[b + B*r][j]] = bit::bit1;
                    }
                }
            }
            if (!is_init) {
                bitarray_K = std::move(bitarray_K1);
                is_init = true;
            }
            else{
                bitarray_K.ANDop(&*bitarray_K1.A.begin());
            }
        }
        if (bitarray_K.empty()) {
            break;
        }
    }
    bar.finish();
    std::vector<std::string> ret_samples;
    auto it = bitarray_K.bitIt;
    it = std::find(it, bitarray_K.end, bit::bit1);
    while (it != bitarray_K.end) {
        ret_samples.push_back(this->idx_to_name[std::distance(bitarray_K.bitIt, it)]);
        it = std::find(it + 1, bitarray_K.end, bit::bit1);
    }
    return ret_samples;
}

std::vector<std::string> RAMBO::query_kmers(fs::path input_file) {
    spdlog::debug("Query RAMBO for kmers in {}", input_file.string());
    vector<std::string> keys = get_kmers(input_file);
    vector<std::string> ret;
    auto ret_samples = std::unordered_set<std::string>();
    for (auto query_key : keys) {
        auto results = query(query_key);
        if (results.size() > 0) {
            spdlog::info("{} found in {} samples:", query_key, results.size());
            //for (auto sample : results) {
                //std::cout << sample << " ";
            //}
            //std::cout << std::endl;
        } else {
            spdlog::info("{} not found in database!", query_key);
        }
    }
    ret.insert(ret.end(), ret_samples.begin(), ret_samples.end());
    return ret;
}

void RAMBO::serializeRAMBO(const fs::path dir){
    spdlog::info("Saving RAMBO to {}", dir.string());
    std::filesystem::create_directories(dir);
    std::ofstream meta_idx(dir / "idx_to_name.txt");
    for (auto p : this->idx_to_name) {
        meta_idx << p << std::endl;
    } std::cout << std::endl;
    meta_idx.close();
    std::ofstream meta_out(dir / "metarambo.txt");
    meta_out << R << " " << B << " " << this->range << " " << this->k << std::endl;
    double total_bits = 0;
    for(int r=0; r<R; r++){
        auto rep_dir = dir / fs::path("repitition_" + std::to_string(r));
        std::filesystem::create_directories(rep_dir);
        for(int b=0; b<B; b++){
            auto bloom_filter_path = rep_dir / fs::path("filter_" + std::to_string(b) + ".bloom");
            //spdlog::debug("{} {} --> {}", r, b, Rambo_array[b + B*r]->m_bits->getcount() / (float) this->range);
            total_bits += Rambo_array[b + B*r]->m_bits->getcount();
            Rambo_array[b + B*r]->serializeBF(bloom_filter_path);
            meta_out << "# " << r << " " << b << std::endl;
            for (auto sample_idx : this->metaRambo[B*r + b]) {
                meta_out << sample_idx << std::endl;
            }
        }
    }

    spdlog::info("Average RAMBO density is {}", (float) total_bits / (float) (this->B * this->R) / this->range);
    meta_out.close();
}

void RAMBO::deserializeRAMBO(fs::path dir){
    for(int r=0; r<R; r++){
        auto rep_dir = dir / fs::path("repitition_" + std::to_string(r));
        for(int b=0; b<B; b++){
            auto bloom_filter_path = rep_dir / fs::path("filter_" + std::to_string(b) + ".bloom");
            Rambo_array[b + B*r]->deserializeBF(bloom_filter_path);
        }
    }
}
