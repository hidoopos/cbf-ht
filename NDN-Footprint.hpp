#ifndef H_NDN_FOOTPRINT
#define H_NDN_FOOTPRINT

#include <string>
#include <unordered_map>
#include "NDN-Config.hpp"
#include "city-hash.h"

using namespace std;

#if FOOTPRINT_AS_KEY_ON //若采用footprint

#define LEN ((FOOTPRINT_BYTE_LEN > 8) ? 8 : ((FOOTPRINT_BYTE_LEN < 1) ? 1 : FOOTPRINT_BYTE_LEN))

//为了节省空间 我们采用name的哈希值(称作footprint)作为表的key
struct Footprint {
public:
	//初始化自字符串
	void InitFromStr(const string& s);
	Footprint(const string& s) { InitFromStr(s); }

	char operator[] (int index) const { return byte[index]; }

	const char* to_str() const { return byte; }

	//debug 输出
	void print() const {
		for (int i = 0; i < LEN; i++)
			printf("%#02X ", (unsigned char) byte[i]);
	}
private:
	char byte[LEN];
};

//由字符串产生footprint
inline void Footprint::InitFromStr(const string& s) {
	string s_nonce = FOOTPRINT_HASH_NONCE + s;
	uint64 HashValue = CityHash64(s_nonce.c_str(), s_nonce.length());
	for (int i = 0; i < LEN; i++) {
		byte[i] = (char)0;
		char p = (char)1;
		for (int j = 0; j < 8; j++) {
			if (HashValue % 2 == 1)
				byte[i] |= p;
			p <<= 1;
			HashValue /= 2;
		}
	}
}

//重载hash和比较函数
struct hash_FP {
	size_t operator() (const Footprint& fp) const {
		return CityHash32(fp.to_str(), LEN);
	}
};

struct cmp_FP {
	bool operator() (const Footprint& ls, const Footprint& rs) const {
		for (int i = 0; i < LEN; i++) {
			if (ls[i] != rs[i])
				return false;
		}
		return true;
	}
};
#undef LEN

#endif //!FOOTPRINT_AS_KEY_ON
#endif //!H_NDN_FOOTPRINT