#include <iostream>
#include <vector>

using namespace std;

class Block {
private:
	int number;
	vector<int> connNet;
	bool isFixed;
	double bx;
	double by;
	double anchorX;
	double anchorY;

public:
	Block(int n);
	~Block();

	void setFixed(double x, double y);
	void setNonFixed(double x, double y);
	void setAnchor(double x, double y);
	int getNumber() { return number; }
	bool getIsFixed() { return isFixed; }
	double getXY(int i) { return (i ? by : bx); }
	double getAnchor(int i) { return (i ? anchorY : anchorX); }
	vector<int> getConn() { return connNet; }
	void addConn(int N);


};

bool compareBlockHorizontal(Block b1, Block b2);
bool compareBlockVertical(Block b1, Block b2);
bool compareBlockNum(Block b1, Block b2);
