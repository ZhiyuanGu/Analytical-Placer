#include "Block.h"

Block::Block(int n) {
	number = n;
	isFixed = 0;
	bx = -1;
	by = -1;
	anchorX = -1;
	anchorY = -1;
}

Block::~Block() {
}

void Block::setFixed(double x, double y) {
	isFixed = 1;
	bx = x;
	by = y;
}

void Block::setNonFixed(double x, double y) {
	isFixed = 0;
	bx = x;
	by = y;
}

void Block::setAnchor(double x, double y) {
	if (isFixed == 0) {
		anchorX = x;
		anchorY = y;
	}
}

void Block::addConn(int N) {
	bool alreadyExist = false;
	for (unsigned int i = 0; i < connNet.size(); i++)
	{
		if (connNet[i] == N)
			alreadyExist = true;
	}
	if (!alreadyExist)
		connNet.push_back(N);
}

bool compareBlockHorizontal(Block b1, Block b2) {
	return (b1.getXY(0) < b2.getXY(0));
}

bool compareBlockVertical(Block b1, Block b2) {
	return (b1.getXY(1) < b2.getXY(1));
}

bool compareBlockNum(Block b1, Block b2) {
	return (b1.getNumber() < b2.getNumber());
}

