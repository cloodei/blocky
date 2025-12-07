#include <iostream>
#include <string>
#include <algorithm>
#include <fstream>
#include <queue>
#include "sha256.cpp"
using namespace std;

struct Transaction {
	string id;
	size_t size;
	size_t fee;
	
	inline bool operator<(const Transaction& t) const noexcept {
		return ((fee * 1.0) / size) > ((t.fee * 1.0) / t.size);
	}
};

int main() {
	vector<Transaction> all;
	ifstream file("fee.txt");
	string id, size, fee;
	size_t cap = 0, tax = 0;
	
	while (file >> id) {
		file >> size >> fee;
		cout << "Transaction " << id << ": " << size << "KB, " << fee << "BTC\n";

		size_t s = stol(size), f = stol(fee);
		all.push_back({
			.id = id,
			.size = s,
			.fee = f
		});
	}

	sort(all.begin(), all.end());
	cout << "\nPROCESSING:\n";
	for (auto& t : all) {
		if (cap + t.size > 1000)
			continue;

		cap += t.size;
		tax += t.fee;
		cout << t.id << ": " << t.size << "KB, " << t.fee << " BTC\n";
	}

	cout << "\nSize: " << cap << "KB\nFee: " << tax << " BTC";
    return 0;
}
