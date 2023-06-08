#include "SegmentedCode.h"

SegmentedCode::SegmentedCode(int b) : length(b) {
	int max_syn_00 = 0;
	int max_syn_11 = 0;
	int max_syn_01 = 0;
	bool onesSequence = false;
	for (int a = 0; a <= length; a++) {
		VTcodeGenerator* vtGenerator = new VTcodeGenerator(a, length); //generate a set of VT codes when a is the syndrome
		vector<vector<int>> temp_prefix_00;
		vector<vector<int>> temp_prefix_11;
		vector<vector<int>> temp_prefix_01;
		for (vector<int> codeword : vtGenerator->code) { //loop through the VT codes and insert the codes with prefix 00 or 11 to their vectors 
			if (codeword.at(0) == 0 && codeword.at(1) == 0) temp_prefix_00.push_back(codeword);
			else {
				if (codeword.at(0) == 1 && codeword.at(1) == 1) temp_prefix_11.push_back(codeword);
				else {
					if (codeword.at(0) == 0 && codeword.at(1) == 1 && !(codeword.at(2) == 0 && codeword.at(3) == 1)) {
						for (int i = 4; i < a; i++) {
							if (codeword.at(i) != 1) break;
							if (i == a - 1) onesSequence = true;
						}
						if (!onesSequence) temp_prefix_01.push_back(codeword);
						onesSequence = false;
					}
				}
			}
		}
		//checking if the prefixs vectors of the encoder have bigger vectors currently
		//if yes - we replace the encoder's prefix vectors with the new ones
		if (temp_prefix_00.size() > prefix_00_code.size()) {
			prefix_00_code.clear();
			prefix_00_code = temp_prefix_00;
			max_syn_00 = a;
		}
		if (temp_prefix_11.size() > prefix_11_code.size()) {
			prefix_11_code.clear();
			prefix_11_code = temp_prefix_11;
			max_syn_11 = a;
		}
		if (temp_prefix_01.size() > prefix_01_code.size()) {
			prefix_01_code.clear();
			prefix_01_code = temp_prefix_01;
			max_syn_01 = a;
		}
		//else we clear the the temps
		temp_prefix_00.clear();
		temp_prefix_11.clear();
		temp_prefix_01.clear();
	}

	//making sure the sizes of the vectors are the same using Ms
	if (prefix_00_code.size() <= prefix_11_code.size()) {
		Ms = prefix_00_code.size();
		for (int i = 0; i < (Ms - prefix_11_code.size()); i++) {
			prefix_11_code.pop_back();
		}
	}
	else {
		Ms = prefix_11_code.size();
		for (int i = 0; i < (Ms - prefix_00_code.size()); i++) {
			prefix_00_code.pop_back();
		}
	}
	a0 = max_syn_00;
	a1 = max_syn_11;
	a01 = max_syn_01;
	//initialize the maps:
	stack<int> stack;
	vector<int> binNum;
	int k, bit;
	//the 00 and 11 maps:
	float pwrOf2 = log2(this->Ms);
	if (!(floor(pwrOf2) == pwrOf2)) pwrOf2 = floor(pwrOf2) + 1;
	for (int i = 0; i < this->Ms; i++) {
		vector<int> codeWord0 = this->prefix_00_code.at(i);
		vector<int> codeWord1 = this->prefix_11_code.at(i);
		k = i;
		while (k != 0) {
			stack.push(k % 2);
			k = k / 2;
		}
		while (stack.size() < pwrOf2) {
			stack.push(0);
		}
		for (int i = 1; i <= pwrOf2; i++) {
			bit = stack.top();
			stack.pop();
			binNum.push_back(bit);
		}
		dict0encrypt[binNum] = codeWord0;
		dict1encrypt[binNum] = codeWord1;
		dict0decrypt[codeWord0] = binNum;
		dict1decrypt[codeWord1] = binNum;
		binNum.clear();
	}
	//the 01 map:
	pwrOf2 = log2(this->prefix_01_code.size());
	if (!(floor(pwrOf2) == pwrOf2)) pwrOf2 = floor(pwrOf2) + 1;
	for (int i = 0; i < this->prefix_01_code.size(); i++) {
		vector<int> codeWord01 = this->prefix_01_code.at(i);
		k = i;
		while (k != 0) {
			stack.push(k % 2);
			k = k / 2;
		}
		while (stack.size() < pwrOf2) {
			stack.push(0);
		}
		for (int i = 1; i <= pwrOf2; i++) {
			bit = stack.top();
			stack.pop();
			binNum.push_back(bit);
		}
		dict01encrypt[binNum] = codeWord01;
		dict01decrypt[codeWord01] = binNum;
		binNum.clear();
	}
}

int SegmentedCode::syn(const vector<int> codeword) {
	int sigma = 0;
	for (int i = 0; i < length; i++) {
		sigma += codeword.at(i) * (i + 1);
	}
	return sigma % (length + 1);
}

vector<int> SegmentedCode::encode(vector<int> data, int mode) {
	int i = 0;
	vector<int> encrypted2;
	float pwrOf2 = log2(this->Ms);
	if (!(floor(pwrOf2) == pwrOf2)) pwrOf2 = floor(pwrOf2) + 1;
	vector<vector<int>> segmentedData;
	//slicing the data into segmented blocks
	while (i < data.size()) {
		if (!(data.size() - i >= pwrOf2)) {
			segmentedData.push_back(slice(data, i, i + (data.size() - i) - 1));
			i += data.size() - i;
		}
		else {
			segmentedData.push_back(slice(data, i, i + pwrOf2 - 1));
			i += pwrOf2;
		}
	}
	numOfSegments = segmentedData.size();
	if (mode == 0) {
		bool firstWord = true;
		for (vector<int> word : segmentedData) {
			if (firstWord) {

				for (int bit : dict0encrypt.at(word)) {
					encrypted2.push_back(bit);
				}
				firstWord = false;
			}
			else {
				if (encrypted2.back() == 0) {
					for (int bit : dict1encrypt.at(word)) {
						encrypted2.push_back(bit);
					}
				}
				else {
					for (int bit : dict0encrypt.at(word)) {
						encrypted2.push_back(bit);
					}
				}
			}
		}
	}
	else {
		for (vector<int> word : segmentedData) {
			for (int bit : dict01encrypt.at(word)) {
				encrypted2.push_back(bit);
			}
		}
	}
	return encrypted2;
}

vector<int> SegmentedCode::decode(vector<int> block, int mode) {
	correctData.clear();
	vector<int> codeword;
	int pi = 0;
	int a_kova;
	bool shortSeg = false;
	bool lastSeg = false;
	for (int i = 0; i < numOfSegments; i++) {
		if (pi + length - 1 >= block.size()) {
			shortSeg = true;
			codeword = slice(block, pi, pi + length - 2);
		}
		else {
			if (pi + length + 1 >= block.size()) lastSeg = true;
			codeword = slice(block, pi, pi + length - 1);
			a_kova = syn(codeword);
		}
		if (mode == 0) { //it's deletion error correction
			int segType = codeword.at(0);  //segtype stands for which kind of segment we're decodin now (00 or 11)
			if (segType == 0) {
				if (!shortSeg && a_kova == a0) {
					for (int bit : dict0decrypt.at(codeword)) {
						correctData.push_back(bit);
					}
					pi = pi + length;
				}
				else {
					if (!shortSeg) codeword.pop_back();
					for (int bit : dict0decrypt.at(restore(codeword, a0, 0))) {
						correctData.push_back(bit);
					}
					pi = pi + length - 1;
				}
			}
			else
			{
				if (!shortSeg && a_kova == a1) {
					for (int bit : dict1decrypt.at(codeword)) {
						correctData.push_back(bit);
					}
					pi = pi + length;
				}
				else {
					if (!shortSeg) codeword.pop_back();
					for (int bit : dict1decrypt.at(restore(codeword, a1, 0))) {
						correctData.push_back(bit);
					}
					pi = pi + length - 1;
				}
			}
		}
		else { //it's inserion error correction mode
			if (a_kova == a01) {
				for (int bit : dict01decrypt.at(codeword)) {
					correctData.push_back(bit);
				}
				pi = pi + length;
				//check if there was an insertion in the end
				if (!lastSeg) {
					if (!(block.at(pi) == 0 && block.at(pi + 1) == 1)) pi++;
					else if (block.at(pi) == 0 && block.at(pi + 1) == 1 && block.at(pi + 2) == 0 && block.at(pi + 3) == 1) {
						//check if the inserted bit is 0  at the position 2 in the next segment
						codeword.clear();
						for (int i = 0; i < (length + 1); i++) {
							if (i != 2) {
								codeword.push_back(block.at(pi + i));
							}
						}
						if (syn(codeword) == a01) {
							pi = pi + length + 1;
							for (int bit : dict01decrypt.at(codeword)) {
								correctData.push_back(bit);
							}
						}
						else {
							//check if the inserted bit is 1 at the position 3 in the next segment
							codeword.clear();
							for (int i = 0; i < (length + 1); i++) {
								if (i != 3) {
									codeword.push_back(block.at(pi + i));
								}
							}
							if (syn(codeword) == a01) {
								pi = pi + length + 1;
								for (int bit : dict01decrypt.at(codeword)) {
									correctData.push_back(bit);
								}
							}
							else {
								//check if the inserted bit is 0  at the position length+1 in the current segment and the inserted bit 1 in the position 0 in the next segment
								codeword.clear();
								for (int i = 2; i < length; i++) {
									codeword.push_back(block.at(pi + i));
								}
								if (syn(codeword) == a01) {
									pi = pi + length + 2;
									for (int bit : dict01decrypt.at(codeword)) {
										correctData.push_back(bit);
									}
								}
							}
						}
					}
				}
			}
			else {
				codeword.push_back(block.at(pi));
				for (int bit : dict01decrypt.at(restore(codeword, a01, 1))) {
					correctData.push_back(bit);
				}
				pi = pi + length + 1;
			}

		}
		codeword.clear();
	}
	return correctData;
}

vector<int> SegmentedCode::restore(vector<int> codeword, int syndrome, int mode) {
	if (mode == 0) {
		int weight = 0, checksum = 0, R1 = 0, L0 = 0;
		int s; //s is the missing bit 
		for (int i = 0; i < (length - 1); i++) {
			checksum += codeword.at(i) * (i + 1);
			weight += codeword.at(i);
		}
		//number correction for modulo calculation:
		int delta = (syndrome - checksum) % (length + 1);
		if (delta < 0) delta += (length + 1);

		//case of 0 was deleted
		if (delta <= weight) {
			s = 0;
			R1 = delta;
			for (int j = (length - 2); j >= 0; j--) {
				if (R1 == 0) {
					codeword.insert(codeword.begin() + (j + 1), 0);
					break;
				}
				else if (codeword.at(j) == 1) R1--;
			}
		}
		else {//case of 1 was deleted
			s = 1;
			L0 = delta - 1 - weight;
			for (int j = 0; j < length; j++) {
				if (L0 == 0) {
					codeword.insert(codeword.begin() + j, 1);
					break;
				}
				else if (codeword.at(j) == 0) L0--;
			}
		}
	}
	else {
		int weight = 0;
		for (int i = 0; i < length + 1; i++) {
			weight += codeword.at(i);
		}
		int S = syn(codeword);
		if (S - a01 == 0) { // special case when the inserted bit is at the end
			codeword.pop_back();
		}
		else if (S - a01 == weight) {
			codeword.erase(codeword.begin());
		}
		else if (S - a01 < weight) { //the inserted bit is 0
			int rOnes = S - a01;
			int position = -1;
			// Count rOnes bits from the right
			for (int i = length; i >= 0; i--) {
				if (codeword.at(i) == 1) {
					rOnes--;
					if (rOnes == 0) {
						position = i;
						break;
					}
				}
			}
			codeword.erase(codeword.begin() + position-1);
		}
		else if (S - a01 > weight) { // the inserted bit is 1
			int rZeros = length + 2 - S;
			int position = -1;
			// Count rZeros bits from the right
			for (int i = length; i >= 0; i--) {
				if (codeword.at(i) == 0) {
					rZeros--;
					if (rZeros == 0) {
						position = i;
						break;
					}
				}
			}
			codeword.erase(codeword.begin() + position-1);
		}

	}
	return codeword;
}

vector<int> SegmentedCode::slice(const vector<int>& v, int m, int n) {
	vector<int>::const_iterator start = v.begin() + m;
	vector<int>::const_iterator end = v.begin() + n +1;
	return vector<int>(start,end);
}

void SegmentedCode::print() {
	cout << "00 prefix code: ";
	for (vector<int> code : prefix_00_code) {
		for (int bit : code) {
			cout << bit;
		}
		cout << " ";
	}
	cout << endl;
	cout << "syndrom : " << a0 << endl;
	cout << "11 prefix code: ";
	for (vector<int> code : prefix_11_code) {
		for (int bit : code) {
			cout << bit;
		}
		cout << " ";
	}
	cout << endl;
	cout << "syndrom : " << a1 << endl;
	cout << "insertion code: ";
	for (vector<int> code : prefix_01_code) {
		for (int bit : code) {
			cout << bit;
		}
		cout << " ";
	}
	cout << endl;
	cout << "syndrom : " << a01 << endl;
}

