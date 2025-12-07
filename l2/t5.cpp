#include <iostream>
#include <cstdint>
#include <string>
#include <vector>
#include "ripemd160.cpp"
#include "sha256.cpp"
using namespace std;

enum Op {
    OP_ADD,
    OP_EQUAL,
    OP_DUP,
    OP_HASH160,
    OP_EQUALVERIFY
};

struct ob {
    int type, value;
};

vector<void*> stack;

int main() {
    vector<ob> all = {
        { .type = 1, .value = 5 },
        { .type = 1, .value = 6 },
        { .type = 0, .value = OP_ADD },
        { .type = 1, .value = 10 },
        { .type = 0, .value = OP_EQUAL }
    };
    bool T = 1, F = 0;

    for (size_t i = 0; i < all.size(); ++i) {
        ob& o = all[i];
        if (o.type)
            stack.push_back(&(o.value));
        else {
            switch (o.value) {
                case OP_ADD: {
                    int x = *((int*)stack[stack.size() - 1]), y = *((int*)stack[stack.size() - 2]) + x;
                    stack.pop_back();
                    stack.pop_back();
                    stack.push_back(&y);
                    break;
                }
                case OP_EQUAL: {
                    int x = *((int*)stack[stack.size() - 1]), y = *((int*)stack[stack.size() - 2]);
                    stack.pop_back();
                    stack.pop_back();
                    stack.push_back(((x == y) ? &T : &F));
                    break;
                }
                case OP_DUP: {
                    stack.push_back(stack[stack.size() - 1]);
                    break;
                }
                case OP_HASH160: {
                    auto x = (uint8_t*)stack[stack.size() - 1];
                    stack[stack.size() - 1] = ripe160(sha256(x, 4), 32);
                    break;
                }
                case OP_EQUALVERIFY: {
                    int x = *((int*)stack[stack.size() - 1]), y = *((int*)stack[stack.size() - 2]);
                    cout << (x == y);
                    return 0;
                }
            }
        }
    }

    cout << *((bool*)stack[stack.size() - 1]);
    return 0;
}
