#include <sstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <cstring>
#include <cstdint>
using namespace std;


static uint32_t h[5];

static constexpr uint32_t KL[5] = { 0x00000000, 0x5A827999, 0x6ED9EBA1, 0x8F1BBCDC, 0xA953FD4E };
static constexpr uint32_t KR[5] = { 0x50A28BE6, 0x5C4DD124, 0x6D703EF3, 0x7A6D76E9, 0x00000000 };
static constexpr int ZL[80] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
    7, 4, 13, 1, 10, 6, 15, 3, 12, 0, 9, 5, 2, 14, 11, 8,
    3, 10, 14, 4, 9, 15, 8, 1, 2, 7, 0, 6, 13, 11, 5, 12,
    1, 9, 11, 10, 0, 8, 12, 4, 13, 3, 7, 15, 14, 5, 6, 2,
    4, 0, 5, 9, 7, 12, 2, 10, 14, 1, 3, 8, 11, 6, 15, 13
};
static constexpr int ZR[80] = {
    5, 14, 7, 0, 9, 2, 11, 4, 13, 6, 15, 8, 1, 10, 3, 12,
    6, 11, 3, 7, 0, 13, 5, 10, 14, 15, 8, 12, 4, 9, 1, 2,
    15, 5, 1, 3, 7, 14, 6, 9, 11, 8, 12, 2, 10, 0, 4, 13,
    8, 6, 4, 1, 3, 11, 15, 0, 5, 12, 2, 13, 9, 7, 10, 14,
    12, 15, 10, 4, 1, 5, 8, 7, 6, 2, 13, 14, 0, 3, 9, 11
};
static constexpr int SL[80] = {
    11, 14, 15, 12, 5, 8, 7, 9, 11, 13, 14, 15, 6, 7, 9, 8,
    7, 6, 8, 13, 11, 9, 7, 15, 7, 12, 15, 9, 11, 7, 13, 12,
    11, 13, 6, 7, 14, 9, 13, 15, 14, 8, 13, 6, 5, 12, 7, 5,
    11, 12, 14, 15, 14, 15, 9, 8, 9, 14, 5, 6, 8, 6, 5, 12,
    9, 15, 5, 11, 6, 8, 13, 12, 5, 12, 13, 14, 11, 8, 5, 6
};
static constexpr int SR[80] = {
    8, 9, 9, 11, 13, 15, 15, 5, 7, 7, 8, 11, 14, 14, 12, 6,
    9, 13, 15, 7, 12, 8, 9, 11, 7, 7, 12, 7, 6, 15, 13, 11,
    9, 7, 15, 11, 8, 6, 6, 14, 12, 13, 5, 14, 13, 13, 7, 5,
    15, 5, 8, 11, 14, 14, 6, 14, 6, 9, 12, 9, 12, 5, 15, 8,
    8, 5, 12, 9, 12, 5, 14, 6, 8, 13, 6, 5, 15, 13, 11, 11
};

static inline uint32_t rotl(uint32_t x, int n) {
    return (x << n) | (x >> (32 - n));
}

static inline uint32_t f(int j, uint32_t x, uint32_t y, uint32_t z) noexcept {
    if (j < 16)
        return x ^ y ^ z;
    if (j < 32)
        return (x & y) | (~x & z);
    if (j < 48)
        return (x | ~y) ^ z;
    if (j < 64)
        return (x & z) | (y & ~z);

    return x ^ (y | ~z);
}

void transform(const uint8_t* block) noexcept {
    uint32_t X[16];

    for (int i = 0; i < 16; i++) {
        X[i] = (block[i * 4]) | 
               (block[i * 4 + 1] << 8) | 
               (block[i * 4 + 2] << 16) | 
               (block[i * 4 + 3] << 24);
    }

    uint32_t AL = h[0], BL = h[1], CL = h[2], DL = h[3], EL = h[4];
    uint32_t AR = h[0], BR = h[1], CR = h[2], DR = h[3], ER = h[4];

    for (int j = 0; j < 80; j++) {
        uint32_t T = AL + f(j, BL, CL, DL) + X[ZL[j]] + KL[j / 16];
        T = rotl(T, SL[j]) + EL;
        AL = EL;
        EL = DL;
        DL = rotl(CL, 10);
        CL = BL;
        BL = T;
    }

    for (int j = 0; j < 80; j++) {
        uint32_t T = AR + f(79 - j, BR, CR, DR) + X[ZR[j]] + KR[j / 16];
        T = rotl(T, SR[j]) + ER;
        AR = ER;
        ER = DR;
        DR = rotl(CR, 10);
        CR = BR;
        BR = T;
    }

    uint32_t T = h[1] + CL + DR;
    h[1] = h[2] + DL + ER;
    h[2] = h[3] + EL + AR;
    h[3] = h[4] + AL + BR;
    h[4] = h[0] + BL + CR;
    h[0] = T;
}

uint8_t* out(uint8_t* data, size_t length) noexcept {
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
    for (int i = 0; i < 8; i++)
        buffer[l + paddingSize + i] = (bitLength >> (i * 8)) & 0xff;

    transform(buffer);
    if (l >= 56)
        transform(buffer + 64);

    uint8_t* res = new uint8_t[20];
    for (size_t i = 0; i < 5; i++) {
        res[i * 4] 	   = (h[i]) & 0xff;
        res[i * 4 + 1] = (h[i] >>  8) & 0xff;
        res[i * 4 + 2] = (h[i] >> 16) & 0xff;
        res[i * 4 + 3] = (h[i] >> 24) & 0xff;
    }

    return res;
}

inline string ripe_to_hex(const uint8_t* d) noexcept {
    ostringstream oss;
    oss << std::hex << setfill('0');
    for (size_t i = 0; i < 20; ++i)
        oss << setw(2) << (unsigned int)d[i];

    return oss.str();
}


inline string ripe100(uint8_t* input, size_t length) noexcept {
    h[0] = 0x67452301;
    h[1] = 0xEFCDAB89;
    h[2] = 0x98BADCFE;
    h[3] = 0x10325476;
    h[4] = 0xC3D2E1F0;
    return ripe_to_hex(out(input, length));
}

inline string ripe100(const string& input) noexcept {
    return ripe100((uint8_t*)input.data(), input.size());
}

inline uint8_t* ripe160(uint8_t* input, size_t length) noexcept {
    h[0] = 0x67452301;
    h[1] = 0xEFCDAB89;
    h[2] = 0x98BADCFE;
    h[3] = 0x10325476;
    h[4] = 0xC3D2E1F0;
    return out(input, length);
}

inline uint8_t* ripe160(const string& input) noexcept {
    return ripe160((uint8_t*)input.data(), input.size());
}
