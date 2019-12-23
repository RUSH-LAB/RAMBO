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
3) Install wget
4) Install coretextpy
Refer to this installation [document] (https://cortexpy.readthedocs.io/en/latest/overview.html#installation)

To download dataset run the script RAMBO/data/0/download.sh 
```
sh download.sh
```
It downloads data files in RAMBO/data/0/inflated (ensure this path is present before downloading). Once it downloads the batch of 100 data files. 

Insertion in RAMBO:

Flags in RAMBO/src/main.cpp 
```
bool insert  = true;
bool ser = true;
bool test = false;
bool deser = false;
```
```
cd RAMBO
make
./build/program 0
```
To test RAMBO keep the flags in RAMBO/src/main.cpp as

```
bool insert  = false;
bool ser = false;
bool test = true;
bool deser = true;
```
and run the code
```
cd RAMBO
make
./build/program 0
```

