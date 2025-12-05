#include <iostream>
#include <string>
#include "sha256.cpp"
using namespace std;

int main() {
	string h = "hello";
    cout << "Hello: " << shaff(h) << "\nGOOD";

    return 0;
}
