#include <iostream>
#include <string>
#include <cstdint>
#include <vector>
#include "sha256.cpp"
using namespace std;

struct Block {
	size_t index;
	string data;
	uint8_t* prev_hash;
	uint8_t* hash;
};
vector<Block> chain;

inline bool cmp(const char* str1, const char* str2) noexcept {
	size_t* s1 = (size_t*) str1, *s2 = (size_t*) str2;
	size_t* o1 = s1, *o2 = s2;
	size_t* t1 = s1 + 1, *t2 = s2 + 1;
	size_t* th1 = s1 + 2, *th2 = s2 + 2;
	size_t* f1 = s1 + 3, *f2 = s2 + 3;

	return (*o1 == *o2 && *t1 == *t2 && *th1 == *th2 && *f1 == *f2);
}

inline bool cmp(const uint8_t* str1, const uint8_t* str2) noexcept {
	size_t* s1 = (size_t*) str1, *s2 = (size_t*) str2;
	size_t* o1 = s1, *o2 = s2;
	size_t* t1 = s1 + 1, *t2 = s2 + 1;
	size_t* th1 = s1 + 2, *th2 = s2 + 2;
	size_t* f1 = s1 + 3, *f2 = s2 + 3;

	return (*o1 == *o2 && *t1 == *t2 && *th1 == *th2 && *f1 == *f2);
}


void add(const string& data) noexcept {
	const size_t n = chain.size();
	string s(chain[n - 1].hash, chain[n - 1].hash + 32);
	
	Block b = {
		.index = n,
		.data = data,
		.prev_hash = chain[n - 1].hash,
		.hash = sha256(to_string(n) + data + s)
	};
	
	chain.push_back(b);
}

bool isOK() noexcept {
	const size_t n = chain.size();
	string s(chain[0].hash, chain[0].hash + 32);
	auto hash = sha256(to_string(chain[0].index) + chain[0].data + to_string(NULL));
	if (!cmp(hash, chain[0].hash)) {
		delete hash;
		return false;
	}

	for (size_t i = n - 1; i; --i) {
		auto& b = chain[i];
		memcpy((uint8_t*)s.data(), b.prev_hash, 32);

		delete hash;
		hash = sha256(to_string(chain[i].index) + b.data + s);
		if (!cmp(hash, b.hash)) {
			delete hash;
			return false;
		}
		
		if (!cmp(b.prev_hash, chain[i - 1].hash)) {
			delete hash;
			return false;
		}
	}
	
	delete hash;
	return true;
}

void trace() {
	if (isOK())
		return;

	const size_t n = chain.size();
	string s(chain[0].hash, chain[0].hash + 32);
	auto hash = sha256(to_string(chain[0].index) + chain[0].data + to_string(NULL));

	cout << "1: CHECK HASH = " << to_hex(hash) << " | BLOCK HASH = " << to_hex(chain[0].hash) << '\n';
	if (!cmp(hash, chain[0].hash))
		cerr << "BLOCK 0 CHECK HASH ERROR\n";

	for (size_t i = n - 1; i; --i) {
		auto& b = chain[i];
		memcpy((uint8_t*)s.data(), b.prev_hash, 32);

		delete hash;
		hash = sha256(to_string(chain[i].index) + b.data + s);

		cout << n - i + 1 << ": CHECK HASH = " << to_hex(hash) << " | " << n - i + 1 << ": BLOCK HASH = " << to_hex(b.hash) << '\n';
		if (!cmp(hash, b.hash))
			cerr << "BLOCK " << n - i + 1 << " CHECK HASH ERROR\n";

		cout << n - i + 1 << ": CURR PREV_HASH = " << to_hex(b.prev_hash) << " | " << n - i << ": PREV HASH = " << to_hex(chain[i - 1].hash) << '\n';
		if (!cmp(b.prev_hash, chain[i - 1].hash))
			cerr << "BLOCK " << n - i << " HASH != BLOCK " << n - i + 1 << " MISMATCH ERROR\n";
	}
	
	delete hash;
}

int main() {
	chain.push_back({
		.index = 0,
		.data = "Hello,100,World",
		.prev_hash = NULL,
		.hash = sha256("0Hello,100,World0")
	});

	add("haha");
	add("bad");
	add("good");
	chain[3].data = "not good";
	cout << isOK() << "\n\n";
	trace();

    return 0;
}
