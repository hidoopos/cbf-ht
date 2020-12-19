CPP=g++
CFLAGS = -g -w -std=c++11 -fpermissive -O3 $(INCLUDE)

# LIBS = -lghthash
fib:main.cpp NDN-PrefixGenerator.cpp NDN-HashBasedFIB.cpp city-hash.cpp hashFuncs.cpp BloomFilter.cpp hashtable.cpp CBF-HT.cpp
	${CPP} ${CFLAGS} -o fib main.cpp NDN-PrefixGenerator.cpp NDN-HashBasedFIB.cpp city-hash.cpp hashFuncs.cpp BloomFilter.cpp hashtable.cpp CBF-HT.cpp -lpthread

all:fib
clean:
	rm fib
