#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <algorithm>
#include "umfpack.h"
#include "Block.h"
#include "Net.h"

using namespace std;

enum netModel {
	clique,
	tree
};

class Placing {

private:
	int notFixed;   // total number of nonfixed blocks
	int n;
	//string inputFile;
	double totalHPWL;
	// for graphics
	double range_left, range_right;
	double range_top, range_bottom;
	netModel NM;
	//int* Ap;
	//int* Ai;
	//double* Ax;
	//double* b;
	//double* x;
	vector<int> Ap;
	vector<int> Ai;
	vector<double> Ax;
	vector<double> bx, by;
	vector<double> x, y;
	vector<vector<double>> Q;

	vector<Block> Blocks;
	vector<Net> Nets;

public:
	Placing();
	~Placing();


	double getHPWL() { return totalHPWL; }
	double getRange(int i);
	netModel getNetModel() { return NM; }
	vector<Block>* getBlocks() { return &Blocks; }
	vector<Net>* getNets() { return &Nets; }
	void setNetModel(netModel NM1);

	// methods for different questions
	void PlacingQ12();
	void PlacingQ3(int divideBy, int anchorWeightLevel, bool shouldPrint);


	void inputFile(string filePath);
	void printBlocks();
	void printNets();

	int notFixedIndex(int index);
	void createQb();
	void calculateCliqueMatrix();
	void calculateTreeMatrix();
	void calculateWeight();
	void printQb();

	// method to calculate position and HPWL given Q and bx/by, can be reused in spreading
	void QbToPosition(vector<vector<double>>* Q1, vector<double>* bx1, vector<double>* by1, bool printKnob);
	// methods inside QbToPosition
	void CreateMatrices(vector<vector<double>>* Q1);
	void printMatrices();
	void LinearSolver(vector<double>* bx1, vector<double>* by1);
	void printBlockPosition();
	void updatePosition();
	void calculateHPWL();

	// spread-specific methods
	void BlockPartition(int divideBy);
	void RedoQ12();

};



