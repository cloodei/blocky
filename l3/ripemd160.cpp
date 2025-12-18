#include <sstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <cstring>
#include <cstdint>
using namespace std;


static uint32_t state[8];
static constexpr uint32_t K[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

static inline uint32_t rotr(uint32_t x, uint32_t n) noexcept {
    return (x >> n) | (x << (32 - n));
}

static inline uint32_t choose(uint32_t e, uint32_t f, uint32_t g) noexcept {
    return (e & f) ^ (~e & g);
}

static inline uint32_t majority(uint32_t a, uint32_t b, uint32_t c) noexcept {
    return (a & (b | c)) | (b & c);
}

static inline uint32_t sig0(uint32_t x) noexcept {
    return rotr(x, 7) ^ rotr(x, 18) ^ (x >> 3);
}

static inline uint32_t sig1(uint32_t x) noexcept {
    return rotr(x, 17) ^ rotr(x, 19) ^ (x >> 10);
}

static inline uint32_t Sig0(uint32_t x) noexcept {
    return rotr(x, 2) ^ rotr(x, 13) ^ rotr(x, 22);
}

static inline uint32_t Sig1(uint32_t x) noexcept {
    return rotr(x, 6) ^ rotr(x, 11) ^ rotr(x, 25);
}


static void transform(uint8_t* chunk) noexcept {
    uint32_t a = state[0], b = state[1], c = state[2], d = state[3], e = state[4], f = state[5], g = state[6], h = state[7];
    uint32_t w[64];

    for (size_t i = 0; i < 16; ++i)
        w[i] = (chunk[i << 2] << 24) | (chunk[(i << 2) + 1] << 16) | (chunk[(i << 2) + 2] << 8)  | (chunk[(i << 2) +3]);
        
    for (size_t i = 16; i < 64; ++i)
        w[i] = sig1(w[i - 2]) + w[i - 7] + sig0(w[i - 15]) + w[i - 16];

    for (size_t i = 0; i < 64; ++i) {
        uint32_t t1 = h + Sig1(e) + choose(e, f, g) + K[i] + w[i];
        uint32_t t2 = Sig0(a) + majority(a, b, c);

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

string final_hex(uint8_t* data, size_t length) noexcept {
    size_t l = length;
    while (l >= 64) {
        transform(data);
        data += 64;
        l -= 64;
    }

    uint8_t buffer[128];
    size_t paddingSize = (l < 56) ? (56 - l) : (120 - l);

    memcpy(buffer, data, l);
    buffer[l] = 0x80;
    memset(buffer + l + 1, 0, paddingSize - 1);

    size_t bitLength = length * 8;
    for (int i = 7; i >= 0; i--) {
        buffer[l + paddingSize + i] = bitLength & 0xff;
        bitLength >>= 8;
    }

    transform(buffer);
    if (l >= 56)
        transform(buffer + 64);

    stringstream ss;
    ss << std::hex << setfill('0');
    for (size_t j = 0; j < 8; ++j)
        ss << setw(8) << state[j];

    return ss.str();
}

uint8_t* final(uint8_t* data, size_t length) noexcept {
    size_t l = length;
    while (l >= 64) {
        transform(data);
        data += 64;
        l -= 64;
    }

    uint8_t buffer[128];
    size_t paddingSize = (l < 56) ? (56 - l) : (120 - l);

    memcpy(buffer, data, l);
    buffer[l] = 0x80;
    memset(buffer + l + 1, 0, paddingSize - 1);

    size_t bitLength = length * 8;
    for (int i = 7; i >= 0; i--) {
        buffer[l + paddingSize + i] = bitLength & 0xff;
        bitLength >>= 8;
    }

    transform(buffer);
    if (l >= 56)
        transform(buffer + 64);

    uint8_t* res = new uint8_t[32];
    for (size_t i = 0; i < 8; i++) {
        res[i * 4]     = (state[i] >> 24) & 0xff;
        res[i * 4 + 1] = (state[i] >> 16) & 0xff;
        res[i * 4 + 2] = (state[i] >>  8) & 0xff;
        res[i * 4 + 3] = (state[i]) & 0xff;
    }

    return res;
}

inline string to_hex(const uint8_t* d) noexcept {
    ostringstream oss;
    oss << std::hex << setfill('0');
    for (size_t i = 0; i < 32; ++i)
        oss << setw(2) << (unsigned int)d[i];

    return oss.str();
}


inline string sha2(uint8_t* input, size_t length) noexcept {
    state[0] = 0x6a09e667;
    state[1] = 0xbb67ae85;
    state[2] = 0x3c6ef372;
    state[3] = 0xa54ff53a;
    state[4] = 0x510e527f;
    state[5] = 0x9b05688c;
    state[6] = 0x1f83d9ab;
    state[7] = 0x5be0cd19;
    auto res = final_hex(input, length);

    return res;
}

inline string sha2(const string& input) noexcept {
    return sha2((uint8_t*)input.data(), input.size());
}

inline uint8_t* sha256(uint8_t* input, size_t length) noexcept {
    state[0] = 0x6a09e667;
    state[1] = 0xbb67ae85;
    state[2] = 0x3c6ef372;
    state[3] = 0xa54ff53a;
    state[4] = 0x510e527f;
    state[5] = 0x9b05688c;
    state[6] = 0x1f83d9ab;
    state[7] = 0x5be0cd19;
    auto res = final(input, length);

    return res;
}

inline uint8_t* sha256(const string& input) noexcept {
    return sha256((uint8_t*)input.data(), input.size());
}
