#include <iostream>
#include <cstdint>
#include <string>
#include "sha256.cpp"
using namespace std;


uint32_t n, nonce = 0;
uint64_t blockNum, go;
uint64_t* hsah;
const string d = "Hello PoW";

inline uint64_t be(uint64_t* p) noexcept {
    uint64_t x = *p;
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    x = __builtin_bswap64(x);
#endif
    return x;
}

inline bool check() noexcept {
	hsah = (uint64_t*) sha256(d + to_string(nonce));
	if (be(hsah) > 0x0fffffffffffffff)
		return false;

	switch (blockNum) {
		case 0:
			if (be(hsah) <= go)
				return true;

			return false;
		case 1:
			if (be(hsah) == 0 && be(hsah + 1) <= go)
				return true;

			return false;
		case 2:
			if (be(hsah) == 0 && be(hsah + 1) == 0 && be(hsah + 2) <= go)
				return true;

			return false;
		case 3:
			if (be(hsah) == 0 && be(hsah + 1) == 0 && be(hsah + 2) == 0 && be(hsah + 3) <= go)
				return true;

			return false;
	}

	return false;
}

int main() {
	do {
		cout << "N = ";
		cin >> n;
	} while(n == 0 || n > 63);

	blockNum = n / 16;
	go = 0xffffffffffffffff >> ((n * 4) % 64);
	cout << "GO\n\n";

	while (true) {
		if (check()) {
			cout << "Nonce = " << nonce << "\nHash = " << to_hex((uint8_t*)hsah) << "\nTry = " << nonce + 1;
			delete hsah;
			return 0;
		}

		delete hsah;
		if (++nonce == 0)
			break;
	}

	cerr << "!";
    return 0;
}
