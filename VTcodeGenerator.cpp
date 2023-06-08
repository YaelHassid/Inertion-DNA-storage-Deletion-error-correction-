#include "VTcodeGenerator.h"

VTcodeGenerator::VTcodeGenerator(int a, int l): length(l) {
	stack<int> stack;
	vector<int> codeword;
	int bit, sigma, k;
	int possible_codes = pow(2, length);
	for (int num = 0; num < possible_codes; num++) {
		k = num;
		sigma = 0;
		while (k != 0) {
			stack.push(k % 2);
			k = k / 2;
		}
		while (stack.size() < length) {
			stack.push(0);
		}
		for (int i = 1; i <= length; i++) {
			bit = stack.top();
			stack.pop();
			codeword.push_back(bit);
			sigma += i * bit;
		}
		if (sigma % (length + 1) == (a % (length + 1))) {
			code.push_back(codeword);
		}
		codeword.clear();
	}
}


void VTcodeGenerator::print() {
	for (vector<int> codeword : code) {
		for (int bit : codeword) {
			cout << bit;
		}
		cout << " ";
	}
	cout << endl;
}
