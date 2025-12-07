#include <iostream>
#include <string>
#include <unordered_map>
#include <fstream>
#include "sha256.cpp"
using namespace std;

int main() {
	unordered_map<string, long long> chain;
	string from, amount, to;
	ifstream file("trans.txt");

	while (file >> from) {
		file >> amount >> to;
		
		long long n = stol(amount);
		chain[from] -= n;
		chain[to] += n;
	}

	for (auto p : chain)
		cout << p.first << ": " << p.second << '\n';;

    return 0;
}
