#ifndef _utils_
#define _utils_
#include <vector>
#include <set>
#include <string>
#include <map>
#include <cassert>

std::vector<std::string> getsets( std::string path);
std::vector<std::string> line2array( std::string line, char d);
void writeRAMBOresults(std::string path, int rows, int cols, float* values);
std::vector<int>  arrayunion(std::vector<int> &v1, std::vector<int> &v2);
std::vector<int> arrayintersection(std::vector<int> &v1, std::vector<int> &v2);
std::set<int> takeunion(std::set<int> set1, std::set<int> set2);
std::vector <std::string> getctxdata(std::string filenameSet);
std::vector<std::string> readlines( std::string path, int num);
std::vector<std::string> getRandomTestKeys(int keysize, int n);
std::map<std::string, std::vector<int>> makeInvIndex(int n, std::vector<std::string> foldernames);

#endif
