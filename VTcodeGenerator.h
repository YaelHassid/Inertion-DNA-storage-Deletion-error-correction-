#ifndef VTcodeGenerator_HEADER
#define VTcodeGenerator_HEADER
#include <vector>
#include <stack>
#include <iostream>
#include<math.h>

using namespace std;
class VTcodeGenerator {
public:
	VTcodeGenerator(int a, int l); //a will be the syndrom, b the length of the code  
	void print();
	vector<vector<int>> code;
	int length;
};
#endif
