#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <cstring>
#include <cstdint>
using namespace std;

unsigned int state[8] = { 0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19 };
unsigned char buffer[64];
unsigned long long bitlen;
constexpr unsigned int K[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

inline unsigned int rotr(uint32_t x, uint32_t n) noexcept {
    return (x >> n) | (x << (32 - n));
}

inline unsigned int choose(uint32_t e, uint32_t f, uint32_t g) noexcept {
    return (e & f) ^ (~e & g);
}

inline unsigned int majority(uint32_t a, uint32_t b, uint32_t c) noexcept {
    return (a & (b | c)) | (b & c);
}

inline unsigned int sig0(unsigned int x) noexcept {
	return rotr(x, 7) ^ rotr(x, 18) ^ (x >> 3);
}

inline unsigned int sig1(unsigned int x) noexcept {
	return rotr(x, 17) ^ rotr(x, 19) ^ (x >> 10);
}

inline unsigned int Sig0(unsigned int x) noexcept {
	return rotr(x, 2) ^ rotr(x, 13) ^ rotr(x, 22);
}

inline unsigned int Sig1(unsigned int x) noexcept {
	return rotr(x, 6) ^ rotr(x, 11) ^ rotr(x, 25);
}

inline void transform(unsigned char* chunk) noexcept {
    unsigned int a = state[0], b = state[1], c = state[2], d = state[3], e = state[4], f = state[5], g = state[6], h = state[7];
    unsigned int m[64];

    for (int i = 0; i < 16; ++i)
        m[i] = (chunk[i << 2] << 24) | (chunk[(i << 2) + 1] << 16) | (chunk[(i << 2) + 2] << 8)  | (chunk[(i << 2) +3]);
        
    for (int i = 16; i < 64; ++i)
        m[i] = sig1(m[i - 2]) + m[i - 7] + sig0(m[i - 15]) + m[i - 16];

    for (int i = 0; i < 64; ++i) {
        unsigned int t1 = h + Sig1(e) + choose(e, f, g) + K[i] + m[i];
        unsigned int t2 = Sig0(a) + majority(a, b, c);

        h = g;
        g = f;
        f = e;
        e = d + t1;
        d = c;
        c = b;
        b = a;
        a = t1 + t2;
    }

    state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;
    state[4] += e;
	state[5] += f;
	state[6] += g;
	state[7] += h;
}

void update(string& data) {
    for (char c : data) {
        buffer[(bitlen / 8) % 64] = c;
        bitlen += 8;
        if ((bitlen / 8) % 64 == 0) {
            transform(buffer);
        }
    }
}

string final() {
    unsigned long long i = (bitlen / 8) % 64;
    buffer[i++] = 0x80;

    if (i > 56) {
        while (i < 64)
			buffer[i++] = 0x00;

        transform(buffer);
        i = 0;
    }
    while (i < 56)
		buffer[i++] = 0x00;

    uint64_t len = bitlen; 
    for (int k = 0; k < 8; ++k) {
        buffer[63 - k] = len & 0xFF;
        len >>= 8;
    }

    transform(buffer);
    stringstream ss;
    for (int j = 0; j < 8; ++j)
        ss << std::hex << setw(8) << setfill('0') << state[j];
        
    return ss.str();
}

inline string shahash(string& input) {
	bitlen = 0;
	state[0] = 0x6a09e667;
	state[1] = 0xbb67ae85;
	state[2] = 0x3c6ef372;
	state[3] = 0xa54ff53a;
    state[4] = 0x510e527f;
	state[5] = 0x9b05688c;
	state[6] = 0x1f83d9ab;
	state[7] = 0x5be0cd19;
    update(input);
    return final();
}


int main() {
	int n = 5;
	vector<string> a = { "x,2,y", "y,3,z", "z,4,x", "z,5,y", "x,10,z" };
	for (int i = 0; i < n; ++i) {
		string t = a[i];
		a[i] = shahash(t);
	}
	
	if (n & 1) {	
		a.push_back(a[n - 1]);
		++n;
	}
	
	int id, hs = 0;
	do {
		cout << "STT: ";
		cin >> id;
	} while (id < 1 || id > n);
	--id;
	
	while (n) {
		if (id & 1)
			cout << "H" << (++hs) << ": " << a[id - 1] << endl;
		else
			cout << "H" << (++hs) << ": " << a[id + 1] << endl;

		for (int i = 0; i < n; i += 2) {
			string h1 = a[i] + a[i + 1];
			a[i >> 1] = shahash(h1);
			if (i == id || (i + 1) == id)
				id = i >> 1;
		}
		
		n >>= 1;
		if (n == 1)
			break;

		if (n & 1) {
			a[n] = a[n - 1];
			++n;
		}
	}
	
	cout << "H" << ++hs << ": " << a[0];
	return 0;
}

