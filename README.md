# RAMBO: Repeated And Merged Bloom Filter for Multiple Set Membership Testing (MSMT) in Sub-linear time.

Approximate set membership is a common problem with wide applications in databases, networking, and search. 
Given a set S and a query q, the task is to determine whether q in S. For Multiple Set Membership Testing (MSMT) problem, 
we are given K different sets, and for any given query q the goal is the find all of the sets containing the query element. 
An example of MSMT problem is gene sequence searching in multiple number of files. The state of art sequence searching process 
is either very slow or it has database with impractical memory requirement. 

RAMBO solves this problem by achieving sublinear query time (O(\sqrt{K} log K)) in number of files with memory requirement 
of slightly more then the information theoretical limit. 

This code is the implementation of: 
[https://arxiv.org/abs/1910.02611](https://arxiv.org/abs/1910.02611)
for gene sequence search.


## Installation

### Requirements
* Automake
* g++ >= 8

### Instructions
To build RAMBO, first clone the repository with all submodules
```
git clone --recurse-submodules https://github.com/RUSH-LAB/RAMBO.git
```

Next, `cd` into the `RAMBO/` repository and run the following build commands:
```
./configure
make 
make install
```
The `rambo` binary will be available in `bin/`



## Running RAMBO
The `rambo` binary has 3 main subcommands, `build`, `insert`, and `query`. For universal flags and settings such as how many threads to use, see `rambo --help`

### Build and insert
`rambo build` and `rambo insert` both accept a list of input files as their first argument. Following the list of files, the user can specify the RAMBO parameters (see `rambo build --help` for more details) as well as the output directory.

RAMBO will treat each file as an input set and will name each set by its file stem. For example, building a RAMBO index via 
```
rambo build kmers1.txt kmers2.txt kmers3.txt -o example_kmers 
```
will build an index in the `example_kmers/` directory consiting of the 3 input files. Alternatively, the user can use a regular expression to input their list of files:

```
rambo build *.txt -o example_kmers
```

The syntax for inserting sets into an existing RAMBO index is similar. See the `--help` output for more details.


### Query
The syntax for querying a RAMBO index is similar to that of `build` and `insert`. For example, if I wanted to query the `example_kmers` index for each kmer in `query.txt`, I would run

```
rambo query query.txt --database example_kmers
```

The output of the above command would be stored in `query_results.txt`. Users can change this by providing an `-o,--output` flag followed by the desired output prefix. 

### Experimental dataset
For experiment we use  the  Whole Genome Sequence (WGS) dataset as  used by [1].  It  is  bacterial,  viral  and
parasitic WGS datasets in the European Nucleotide Archive (ENA) as of December 2016. The total size of data is ~170 TB. It is divided into 100 parts and indexed randomly. 

Requirements:
1) GCC version >= 6.2.0
2) Install latest GNU parallel
OS X:
```
brew install parallel
```
Debian/Ubuntu:
```
sudo apt-get install parallel
```

RedHat/CentOS:
```
sudo yum install parallel
```
3) Install wget and bzip2
4) Install cortexpy
Refer to this installation [document] (https://cortexpy.readthedocs.io/en/latest/overview.html#installation)

5) Disk memory > 2.5 TB
6) RAM > 160 GB

To download dataset run the script RAMBO/data/0/download.sh 
```
sh download.sh
```
It downloads data files in RAMBO/data/0/inflated (ensure this path is present before downloading). Once it downloads the batch of 100 data files. 


