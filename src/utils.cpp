//contains all the other non-RAMBO functions
#include <iomanip>
#include <iostream>
#include <fstream>
#include <math.h>
#include <sstream>
#include <string>
#include <string.h>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include "MyBloom.h"
#include "utils.h"
#include "spdlog/spdlog.h"
#include <map>
#include <cassert>
using namespace std;
namespace fs = std::filesystem;

std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

std::vector<std::string> get_kmers(const fs::path input_file) {
    auto ext = input_file.extension().string();
    vector<std::string> keys;
    if (ext == ".ctx") {
        keys = getctxdata(input_file);
    } else if (ext == ".txt" || ext == ".out") {
        keys = gettxtdata(input_file);
    } else if (ext == ".fna" || ext == ".fasta" || ext == ".fa" || ext == ".ffn") {
        // TODO
    } else {
        spdlog::warn("File extension {} not recognized! Defaulting to .txt format", ext);
        keys = gettxtdata(input_file);
    }  
    return keys;
}

std::vector <std::string> gettxtdata(fs::path txtfile){
  ifstream txtstream (txtfile);
  std::vector <std::string> keys;
  std::string line;
  std::vector<std::string> values;
  while(txtstream.good()) {
    getline(txtstream, line);
    std::stringstream line_stream(line);
    getline(line_stream, line, ' ');
    if (line.size() > 0) {
        keys.push_back(line);
    }
  }
  return keys;
}

std::vector <std::string> getctxdata(fs::path ctxfile){
    std::vector<std::string> keys;
    auto cortexpy_out = exec((
        "cortexpy view graph " + ctxfile.string() + " > " + (fs::temp_directory_path() / ctxfile.stem()).string()
    ).c_str());
    keys = gettxtdata(fs::temp_directory_path() / ctxfile.stem());
    return keys;
}

std::string sjoin(std::vector<std::string> v) {
    std::stringstream ss;
    for(size_t i = 0; i < v.size(); ++i)
    {
      if(i != 0)
        ss << "\t";
      ss << v[i];
    }
    std::string s = ss.str();
    return s;
}
