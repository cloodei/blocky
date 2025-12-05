#include <iostream>
#include <cmath>
#include <cstdint>
#include <string>
#include "sha256.cpp"
using namespace std;

uint32_t n, nonce = 0;
size_t* hsah, blockNum, go;
const string d = "Hello PoW";

inline bool check() noexcept {
	hsah = (size_t*) sha256(d + to_string(nonce));

	switch (blockNum) {
		case 0:
			if (*hsah <= go) {
				cout << "HERE 1 " << go << "  " << *(hsah) << "\n\n";
				return true;
			}
			return false;
		case 1:
			if (*hsah == 0 && *(hsah + 1) <= go) {
				cout << "HERE 2 " << go << "  " << *(hsah + 1) << "\n\n";
				return true;
			}
			return false;
		case 2:
			if (*hsah == 0 && *(hsah + 1) == 0 && *(hsah + 2) <= go) {
				cout << "HERE 3 " << go << "  " << *(hsah + 2) << "\n\n";
				return true;
			}
			return false;
		case 3:
			if (*hsah == 0 && *(hsah + 1) == 0 && *(hsah + 2) == 0 && *(hsah + 3) <= go) {
				cout << "HERE 4 " << go << "  " << *(hsah + 3) << "\n\n";
				return true;
			}
			return false;
	}

	return false;
}

int main() {
    cin.tie(nullptr)->sync_with_stdio(false);

	do {
		cin >> n;
	} while(n == 0 || n > 63);

	blockNum = n / 16;
	go = ((0xffffffffffffffff) >> (n % 16));

	while (true) {
		if (check()) {
			cout << "Nonce = " << nonce << "\nHash = " << to_hex((uint8_t*)hsah) << "\nTry = " << nonce + 1;
			return 0;
		}

		if (++nonce == 0)
			return 0;
	}

    return 0;
}
