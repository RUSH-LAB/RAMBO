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

int main(){
std::map<std::string, std::vector<int>> m;
cout<<"here"<<endl;
vector<string> filenames;
filenames = {"data/81/inflated/"};
m = makeInvIndex(1000, filenames);

ofstream myfile;
myfile.open ("testdata.txt");
for(map<string, vector<int> >::iterator it = m.begin(); it != m.end(); ++it){
	myfile<< it->first <<",";
	for (int t =0; t<(it->second).size(); t++){
    		myfile<<it->second[t]<<",";
    	}
	myfile<<endl;
}
  myfile.close();

}
