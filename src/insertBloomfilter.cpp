#include <iomanip>
#include <fstream>
#include <iostream>
#include <chrono> 
#include <vector>
#include <math.h>
#include <sstream>
#include <string>
#include <string.h>
#include <algorithm>
#include <vector>
#include "smhasher/src/MyBloom.h"
using namespace std;

// the code is mostly file reading

int main(){

    struct ConstrintBF {
    string refinement;
    BloomFiler BF;
    } ;

    //get the size of Bloom filter by count
    ifstream cntfile ("finalData3/counts.csv");
    while ( cntfile.good() )
        {
            string line1, vals;
            while( getline ( cntfile, line1 ) ){
                stringstream is;
                is<<line1;
                while(getline (is, vals, ',' )){
                    //initialize BF
                    cout<<vals<<endl;
                }
                cout<<endl;
            }
        }

    ifstream file ( "finalData3/refinement_files.txt" ); // declare file stream
        string path;
        while ( file.good() )
        {
            while( getline ( file, path, ',' ) ){
                // make BF object 
                int n = 10000; // number of keys
                float p = 0.01; //fpr
                int R = ceil(-(n*log(p))/(log(2)*log(2))); //range
                int k = ceil(-log(p)/log(2)); //number of hash, k is 7 for 0.01

                //read each path files and read ASINs
                ifstream filterfile ( "finalData3/" +path ); // declare file stream
                string line2, asin;
                while ( filterfile.good() )
                {
                    while( getline ( filterfile, line2 ) ){
                        stringstream iss;
                        iss<<line2;
                        while (getline ( iss, asin, ',' ));
                            if (asin.size()>10){
                                //do something about it
                                // cout<<'$'<<asin<<'$'<<endl;
                            }
                        //insert asin in BF
                    }
                    
                }
                // cout<<path<<endl; 
            }
            
        }

   
}

