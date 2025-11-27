#include <iostream>
#include <string>
using namespace std;

int main() {
    char key;
    long long test;
    do {
	    cout << "Key: ";
    	cin >> test;
    } while (test <= 0 || test > 255);
    key = test;

    getchar();
    string text;
    cout << "Text: ";
    getline(cin, text);

    for (int i = 0; i < text.length(); ++i)
    	cout << (char)(text[i] ^ key);

    return 0;
}
