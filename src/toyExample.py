#!/usr/bin/env python
# coding: utf-8

# In[1]:


#!/usr/bin/env python
# coding: utf-8
# Author:Gaurav Gupta
# In[28]:

import numpy as np
import sys, os
import pickle
import matplotlib.pyplot as plt
from string import ascii_lowercase
sys.path.append("../python-bloomfilter/")
from pybloom import BloomFilter
from bitarray import bitarray
from sklearn.utils import murmurhash3_32


# In[2]:


def hashfunc(key, s, b):
#         s = s %len(primes) # seed
        return murmurhash3_32(key, s)%b
#         return (17*key + primes[s])%b

FP = 0.01
K = int(np.ceil(np.log(1/FP)))

#unique places to repeat
def getSunique(S,K):
    loc = set()
    while(len(loc)<S):
            loc.add(int(np.random.uniform(K+1))-1)
    return loc


# In[3]:


getSunique(4,1000)
hashfunc('aaaa', 6, 10)


# In[4]:


S = 5
K = 1000
InvIdx = [[] for k in range(K)]
FwdIdx = {}
for i in ascii_lowercase:
    for j in ascii_lowercase:
        for k in ascii_lowercase:
            for l in ascii_lowercase:
                key = i + j + k + l
                #choose S random sets
                loc = getSunique(S, K)
                FwdIdx[key] = loc
                for l in loc:
                    InvIdx[l].append(key)

#pickle.dump(InvIdx, open("toyInvIdx.p", 'wb'))


# In[5]:


# Now insert into standard BF array
BF = {}
print (K)
for k in range(0,K):
        #get the size
        n = len(InvIdx[k])
        m = int(-n*np.log(FP))
        BF[k] = BloomFilter(capacity=m, error_rate=FP)
        for key in InvIdx[k]:
                BF[k].add(key)

#pickle.dump(BF, open("toySTDBF.p", 'wb'))


# In[6]:


# grouping info of MRBF
B = 20
R = 4
MetaBF = []
for r in range(0,R):
        meta = [[] for b in range(0,B)]
        for k in range(0,K):
#                 print (hashfunc(k, r, B))
                meta[hashfunc(k, r, B)].append(k)
        MetaBF.append(meta)



# In[7]:


# print (MetaBF)


# In[8]:


#print (MetaBF)
# STORE META ONE HOT ENCODING
OneHotBF = []
for r in range(0,R):
    OneHot = {}
    for b in range(0,B):
        a = K *bitarray('0')
        for p in MetaBF[r][b]:
                a[p] =1
        OneHot[b]= a
    OneHotBF.append(OneHot)



# In[9]:


# print (OneHotBF)


# In[10]:


# find size of each MR Bloom filter
SizeBF = np.zeros((B,R))
for r in range(0,R):
    for b in range(0,B):
        bigset = set()
        for m in MetaBF[r][b]:
            bigset = bigset.union(set(InvIdx[m]))
        SizeBF[b,r] = len(bigset)


# In[11]:


print (SizeBF)


# In[12]:


# insertion to MR Bloom filter
MRBF = [{}for r in range(0,R)]
for r in range(0,R):
    for b in range(0,B):
        n = SizeBF[b,r]
        m = int(-n*np.log(FP))
        MRBF[r][b] = BloomFilter(capacity=m, error_rate=FP)
        for k in MetaBF[r][b]:
            for key in InvIdx[k]:
                MRBF[r][b].add(key)

#pickle.dump(MRBF, open("toyMRBF.p", 'wb'))


# In[13]:


# now that we have Standard Bloom filter array and merged Bloom filter array
# time to do some query and analyse FP rate and query time
import timeit

def qrySTDBF(q):
        return [q in BF[k] for k in range(0,K)]

def qryMRBF(q):
    op = []
    for r in range(0,R):
         op.append(np.where([q in MRBF[r][b] for b in range(0,B)])[0])
    C = np.array(np.meshgrid(op[0], op[1], op[2])).T.reshape(-1,3)
    res = K * bitarray('0')
    for c in C:
        res = res | (OneHotBF[0][c[0]]&OneHotBF[1][c[1]]&OneHotBF[2][c[2]])

    return res
#          res = res & dum
#         return res
#         res =  K * bitarray('1') #all ones

#                 dum = K * bitarray('0') #all zeros

#                         dum = OneHotBF[r][b] | dum # OR among buckets

cntstdBF = 0
cntMRBF = 0
t1 = timeit.default_timer()
for Idx in InvIdx[:5]:
    for q in Idx:
        np.where(qrySTDBF(q))
        cntstdBF+=1

t2 = timeit.default_timer()
for Idx in InvIdx[0:5]:
    for q in Idx:
        np.where(qryMRBF(q))
        cntMRBF+=1

t3 = timeit.default_timer()
print('Time STDBF: ', t2 - t1)
print('Time MRBF: ', t3 - t2)


# In[14]:


#time per query
print('Time STDBF: ', (t2 - t1)/cntstdBF)
print('Time MRBF: ', (t3 - t2)/cntMRBF)


# In[21]:


# FP analysis
# theoretical is 0.003 for B =20 and R = 4, S = 5

FP_STDbf = 0
cntstdBF = 0
for Idx in InvIdx[7:13]:
    for q in Idx:
        op = np.where(qrySTDBF(q))
        FP_STDbf = FP_STDbf + len(op[0]) - S
        cntstdBF+=995

FP_MRbf = 0
cntMRBF = 0
for Idx in InvIdx[7:13]:
    for q in Idx:
        op = np.where(qryMRBF(q))
        FP_MRbf = FP_MRbf + len(op[0]) - S
        cntMRBF+=995

print ((FP_STDbf/cntstdBF), (FP_MRbf/cntMRBF))


# In[ ]:


# find FP per bloom filter


# In[20]:


cntstdBF


# In[ ]:


For B = 10, R = 4
(array([  0, 333, 452, 882, 925]),)
(array([  0, 314, 330, 333, 345, 379, 416, 452, 522, 631, 654, 676, 685,
       832, 882, 897, 925]),)
Time STDBF:  0.008530129678547382
Time MRBF:  0.0015439791604876518

For B = 12, R = 3
    (array([  0, 156, 392, 766, 908]),)
(array([  0,  14,  44,  82, 117, 127, 156, 168, 250, 271, 285, 310, 320,
       329, 344, 347, 360, 367, 392, 498, 526, 542, 573, 600, 606, 614,
       623, 627, 636, 645, 670, 701, 735, 766, 769, 774, 787, 840, 859,
       881, 908, 970]),)
Time STDBF:  0.003346618264913559
Time MRBF:  0.0007836213335394859


# In[ ]:


R


# In[ ]:


# FP th cal
J = (1-(1/1000))**5
print ("STDBF ",J*0.01 +1 -J)

J = (1-(1/10))**5
print ("STDBF ",(J*0.01 +1 -J)**4)


# In[ ]:
