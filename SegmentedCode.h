#pragma once
#include "VTcodeGenerator.h"
#include <unordered_map>

template <typename T>
class VectorHash {
public:
	size_t operator()(const vector<T>& v) const {
		size_t seed = 0;
		for (const auto& elem : v) {
			seed ^= hash<T>()(elem) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		}
		return seed;
	}
};
class SegmentedCode {
	public:
		SegmentedCode(int b); //b is the leangth of the codes considering (b>=2)
		vector<int> encode(vector<int> data, int mode);
		vector<int> decode(vector<int> block, int mode);
		vector<int> restore(vector<int> codeword, int syndrome, int mode);
		void print();
		int syn(const vector<int> codeword);
		vector<vector<int>> prefix_00_code;
		vector<vector<int>> prefix_11_code;
		vector<vector<int>> prefix_01_code;
		vector<int> slice(const vector<int>& v, int m, int n);
		int Ms;
		int a0;
		int a1;
		int a01;
		int length;
		int numOfSegments;
		int errorCorrectionMode; //0 for deletion, 1 for insertion
		vector<int> correctedBlock;
		vector<int> correctData;
		unordered_map<vector<int>, vector<int>, VectorHash<int>> dict0encrypt;
		unordered_map<vector<int>, vector<int>, VectorHash<int>> dict1encrypt;
		unordered_map<vector<int>, vector<int>, VectorHash<int>> dict01encrypt;
		unordered_map<vector<int>, vector<int>, VectorHash<int>> dict0decrypt;
		unordered_map<vector<int>, vector<int>, VectorHash<int>> dict1decrypt;
		unordered_map<vector<int>, vector<int>, VectorHash<int>> dict01decrypt;
};