# cbf-ht

Firstly, the longest name prefix matching result is obtained by using binary search in counting bloom filter, then linear backtracking is carried out in hash table to acquire the final correct lookup result. 

CBF-HT can achieve high name lookup and update speed, moreover, significantly reduce memory consumption. 

Besides, we propose a memory-optimized binary search (MOBS) algorithm to further optimize memory consumption.