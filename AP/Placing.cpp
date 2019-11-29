#include "Placing.h"

Placing::Placing() {
	notFixed = -1;
	n = 0;
	totalHPWL = 0.0;
	NM = clique;
	range_left = INT_MAX;
	range_right = -1;
	range_top = INT_MAX;
	range_bottom = -1;
}


Placing::~Placing() {

}

double Placing::getRange(int i) {
	if (i == 0) return range_left;
	else if (i == 1) return range_right;
	else if (i == 2) return range_top;
	else if (i == 3) return range_bottom;
	else return -1;
}

void Placing::setNetModel(netModel NM1) {
	NM = NM1;
}

void Placing::PlacingQ12() {
	// input routing parameters file path
	cout << "Please enter file path(cct1, etc.):\n";
	string filePath;
	getline(cin, filePath);


	cout << "Please enter net model(1:clique, 2:tree):\n";
	int NModel = 0;
	while (NModel != 1 && NModel != 2) {
		cin >> NModel;
	}


	inputFile(filePath);

	// set net model
	netModel NM1 = (NModel == 1) ? clique : tree;
	setNetModel(NM1);
	calculateWeight();

	vector<vector<double>>* pQ = &Q;
	vector<double>* pbx = &bx;
	vector<double>* pby = &by;
	QbToPosition(pQ, pbx, pby, 1);

}

// input block and net information from file
void Placing::inputFile(string filePath) {
	ifstream infile;
	infile.open(filePath.c_str(), ifstream::in);
	// TODO: how to ensure all the input is correct
	notFixed = 0;
	int bn, nn;
	double bx, by;
	while (infile >> bn && bn != -1) {
		notFixed++;
		unsigned int i;
		bool found_flag = false;
		for (i = 0; i < Blocks.size(); i++) {
			if (Blocks[i].getNumber() == bn) {
				found_flag = true;
				break;
			}
		}
		if (!found_flag) {
			// store new block
			Block block(bn);
			Blocks.push_back(block);
			i = Blocks.size() - 1;
		}

		while (infile >> nn && nn != -1) {
			Blocks[i].addConn(nn);
			// also store nets
			found_flag = false;
			unsigned int j;
			for (j = 0; j < Nets.size(); j++) {
				if (Nets[j].getNumber() == nn) {
					found_flag = true;
					break;
				}
			}
			if (!found_flag) {
				Net net(nn);
				Nets.push_back(net);
				j = Nets.size() - 1;
			}
			Nets[j].addConn(bn);

		}
	}
	while (infile >> bn && bn != -1) {
		if ((infile >> bx) && (infile >> by)) {
			notFixed--;
			unsigned int i;
			bool found_flag = false;
			for (i = 0; i < Blocks.size(); i++) {
				if (Blocks[i].getNumber() == bn) {
					found_flag = true;
					break;
				}
			}
			if (found_flag)
				Blocks[i].setFixed(bx, by);
			else {
				cout << "Inputfile Error: Fixed point information not found!\n";
			}
		}
	}
	infile.close();

}

// test function to print out the input information
void Placing::printBlocks() {
	cout << "--------------------------------------------------------------------------------------\n";
	cout << "Test: printing out blocks and connected nets...\n";
	for (unsigned int i = 0; i < Blocks.size(); i++) {
		vector<int> conn = Blocks[i].getConn();
		//if(Blocks[i].getIsFixed() )
		cout << "Block No." << Blocks[i].getNumber() << " Conn: ";
		for (unsigned int j = 0; j < conn.size(); j++) {
			cout << conn[j] << " ";
		}
		if (Blocks[i].getXY(0) != -1 && Blocks[i].getXY(1) != -1) {
			if (Blocks[i].getIsFixed())
				cout << "  fixed: (" << Blocks[i].getXY(0) << ", " << Blocks[i].getXY(1) << ")";
			else
				cout << "  nonfixed: (" << Blocks[i].getXY(0) << ", " << Blocks[i].getXY(1) << ")";
		}
		cout << endl;
	}
	cout << "Total number of nonfixed blocks: " << notFixed << endl;
}

// test function to print out the input information
void Placing::printNets() {
	cout << "--------------------------------------------------------------------------------------\n";
	cout << "Test: printing out nets and connected blocks...\n";
	for (unsigned int i = 0; i < Nets.size(); i++) {
		vector<int> conn = Nets[i].getConn();
		cout << "Net No." << Nets[i].getNumber() << " Conn: ";
		for (unsigned int j = 0; j < conn.size(); j++) {
			cout << conn[j] << " ";
		}
		cout << endl;
	}

}

int Placing::notFixedIndex(int number) {
	int index = 0;
	unsigned int i = 0;
	while ((i < Blocks.size()) && (index < notFixed)) {

		if (!Blocks[i].getIsFixed()) {
			index++;
		}
		if (Blocks[i].getNumber() == number)
			break;
		i++;
	}
	return (index - 1);
}

void Placing::createQb() {
	Q.clear();
	bx.clear();
	by.clear();
	// create matrix Q and b
	for (int i = 0; i < notFixed; i++) {
		vector<double> row;
		Q.push_back(row);
		double be = 0;
		bx.push_back(be);
		by.push_back(be);
		for (int j = 0; j < notFixed; j++) {
			double me = 0;  // new matrix element
			Q[i].push_back(me);
		}
	}
}

void Placing::calculateCliqueMatrix() {
	// loop through nets and increment weight in the matrix
	for (unsigned int n = 0; n < Nets.size(); n++) {
		// loop through blocks connected to the net
		for (unsigned int bb = 0; bb < Nets[n].getConn().size(); bb++) {
			for (unsigned int bc = bb + 1; bc < Nets[n].getConn().size(); bc++) {
				// TODO: circle out weight between fixed blocks?
				double weight = (double)2 / Nets[n].getConn().size();
				int bNum = Nets[n].getConn()[bb];
				int bcNum = Nets[n].getConn()[bc];
				int bi = notFixedIndex(bNum);
				int bci = notFixedIndex(bcNum);
				// update Q and b
				if ((bi >= 0) && (bci >= 0) && (!Blocks[bNum - 1].getIsFixed()) && (!Blocks[bcNum - 1].getIsFixed())) {
					Q[bi][bci] -= weight;
					Q[bci][bi] -= weight;
					Q[bi][bi] += weight;
					Q[bci][bci] += weight;
				}
				else if ((bi >= 0) && (Blocks[bNum - 1].getIsFixed() == false) && (Blocks[bcNum - 1].getIsFixed() == true)) {
					Q[bi][bi] += weight;
					bx[bi] += weight * Blocks[bcNum - 1].getXY(0);
					by[bi] += weight * Blocks[bcNum - 1].getXY(1);
				}
				else if ((bci >= 0) && (Blocks[bNum - 1].getIsFixed() == true) && (Blocks[bcNum - 1].getIsFixed() == false)) {
					Q[bci][bci] += weight;
					bx[bci] += weight * Blocks[bNum - 1].getXY(0);
					by[bci] += weight * Blocks[bNum - 1].getXY(1);
				}
			}
		}
	}
}

void Placing::calculateTreeMatrix() {
	// loop through nets and increment weight in the matrix
	for (unsigned int n = 0; n < Nets.size(); n++) {
		// loop through blocks connected to the net
		int bsNum = Nets[n].getConn()[0];
		int bs = notFixedIndex(bsNum);
		for (unsigned int bl = 1; bl < Nets[n].getConn().size(); bl++) {
			double weight = 1;
			int blNum = Nets[n].getConn()[bl];
			int bli = notFixedIndex(blNum);
			if ((bs >= 0) && (bli >= 0) && (!Blocks[bsNum - 1].getIsFixed()) && (!Blocks[blNum - 1].getIsFixed())) {
				Q[bs][bli] -= weight;
				Q[bli][bs] -= weight;
				Q[bs][bs] += weight;
				Q[bli][bli] += weight;
			}
			else if ((bs >= 0) && (!Blocks[bsNum - 1].getIsFixed()) && (Blocks[blNum - 1].getIsFixed())) {
				Q[bs][bs] += weight;
				bx[bs] += weight * Blocks[blNum - 1].getXY(0);
				by[bs] += weight * Blocks[blNum - 1].getXY(1);
			}
			else if ((bli >= 0) && (Blocks[bsNum - 1].getIsFixed()) && (!Blocks[blNum - 1].getIsFixed())) {
				Q[bli][bli] += weight;
				bx[bli] += weight * Blocks[bsNum - 1].getXY(0);
				by[bli] += weight * Blocks[bsNum - 1].getXY(1);
			}
		}
	}
}

// calculate weight and form matrix Q and b
void Placing::calculateWeight() {
	createQb();
	if (NM == clique)
		calculateCliqueMatrix();
	else if (NM == tree)
		calculateTreeMatrix();

}

// test function to print out Q and b
void Placing::printQb() {
	cout << "--------------------------------------------------------------------------------------\n";
	cout << "Test: printing out Q and b...\n";
	cout << "Q:\n";
	for (int i = 0; i < notFixed; i++) {
		for (int j = 0; j < notFixed; j++) {
			cout << Q[i][j] << " ";
		}
		cout << endl;
	}
	cout << "bx:\n";
	for (int i = 0; i < notFixed; i++) {
		cout << bx[i] << endl;
	}
	cout << "by:\n";
	for (int i = 0; i < notFixed; i++) {
		cout << by[i] << endl;
	}
}

// method to calculate position and HPWL given Q and bx/by
void Placing::QbToPosition(vector<vector<double>>* Q1, vector<double>* bx1, vector<double>* by1, bool printKnob) {
	CreateMatrices(Q1);
	LinearSolver(bx1, by1);

	updatePosition();
	calculateHPWL();
	// print out total HPWL
	if (printKnob)
		cout << "Total HPWL is: " << getHPWL() << endl;
}


// create specific matrices as inputs to umfpack
void Placing::CreateMatrices(vector<vector<double>>* Q1) {
	Ap.clear();
	Ai.clear();
	Ax.clear();
	n = notFixed;
	// fill in Ap
	Ap.push_back(0);
	int mcount = 0;
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			if (Q1->at(i)[j] != 0) {
				mcount++;
				Ai.push_back(j);
				Ax.push_back(Q1->at(i)[j]);
			}
		}
		Ap.push_back(mcount);
	}

	// initialize Matrix x
	for (int i = 0; i < n; i++) {
		x.push_back(0.);
		y.push_back(0.);
	}
}

// test function to print out input matrices for umfpack linear solver
void Placing::printMatrices() {
	cout << "-----------------------------------------------------------------------------------\n";
	cout << "Test: printing out Ap, Ai, Ax...\n";
	cout << "n = " << n << endl;
	cout << "Ap = " << n << endl;
	for (unsigned int i = 0; i < Ap.size(); i++) {
		cout << Ap[i] << " ";
	}
	cout << "\nAi = ";
	for (unsigned int i = 0; i < Ai.size(); i++) {
		cout << Ai[i] << " ";
	}
	cout << "\n--------------------------------------------------------------------------------\n";
	cout << "Ax = ";
	for (unsigned int i = 0; i < Ax.size(); i++) {
		cout << Ax[i] << " ";
	}
	cout << "\nbx = ";
	for (unsigned int i = 0; i < bx.size(); i++) {
		cout << bx[i] << " ";
	}
	cout << endl;

}

// the umfpack basic routine to solve sparse linear problems
void Placing::LinearSolver(vector<double>* bx1, vector<double>* by1) {

	double *null = (double *)NULL;
	void *Symbolic, *Numeric;
	(void)umfpack_di_symbolic(n, n, Ap.data(), Ai.data(), Ax.data(), &Symbolic, null, null);
	(void)umfpack_di_numeric(Ap.data(), Ai.data(), Ax.data(), Symbolic, &Numeric, null, null);
	umfpack_di_free_symbolic(&Symbolic);
	(void)umfpack_di_solve(UMFPACK_A, Ap.data(), Ai.data(), Ax.data(), x.data(), bx1->data(), Numeric, null, null);
	(void)umfpack_di_solve(UMFPACK_A, Ap.data(), Ai.data(), Ax.data(), y.data(), by1->data(), Numeric, null, null);
	umfpack_di_free_numeric(&Numeric);

}

// test function to print out calculated non-fixed block position
void Placing::printBlockPosition() {
	cout << "--------------------------------------------------------------------------------------\n";
	for (int i = 0; i < n; i++) printf("Non-fixed Block No.%d (x,y) = (%g, %g)\n", i, x[i], y[i]);
}

// update nonfixed block position with output of linear solver
void Placing::updatePosition() {
	int index = 0;
	for (unsigned int i = 0; i < Blocks.size(); i++) {
		if (!Blocks[i].getIsFixed()) {
			Blocks[i].setNonFixed(x[index], y[index]);
			index++;
		}
	}
}

// calculate HPWL of all nets
void Placing::calculateHPWL() {
	totalHPWL = 0.0;
	for (unsigned int i = 0; i < Nets.size(); i++) {
		// boundaries of net i
		double left, right, top, bottom;
		for (unsigned int j = 0; j < Nets[i].getConn().size(); j++) {
			int bindex = Nets[i].getConn()[j] - 1;
			if (j == 0) {
				left = Blocks[bindex].getXY(0);
				right = Blocks[bindex].getXY(0);
				top = Blocks[bindex].getXY(1);
				bottom = Blocks[bindex].getXY(1);
			}
			if (Blocks[bindex].getXY(0) < left)  left = Blocks[bindex].getXY(0);
			if (Blocks[bindex].getXY(0) > right)  right = Blocks[bindex].getXY(0);
			if (Blocks[bindex].getXY(1) < top)  top = Blocks[bindex].getXY(1);
			if (Blocks[bindex].getXY(1) > bottom)  bottom = Blocks[bindex].getXY(1);
		}
		if (left < range_left) range_left = left;
		if (right > range_right) range_right = right;
		if (top < range_top) range_top = top;
		if (bottom > range_bottom) range_bottom = bottom;
		// calculate wire length of net i
		double WL = right - left + bottom - top;
		Nets[i].setHPWL(WL);
		totalHPWL += WL;
	}
}


void Placing::BlockPartition(int divideBy) {
	// sort horizontally
	sort(Blocks.begin(), Blocks.end(), compareBlockHorizontal);
	vector<int> stripe;
	vector<int> offset;
	for (int i = 1; i < divideBy; i++) {
		stripe.push_back(round((double)notFixed / divideBy * i));
		offset.push_back(0);
	}
	int NF = 0;
	for (unsigned int i = 0; i < Blocks.size(); i++) {
		if (!Blocks[i].getIsFixed()) {
			NF++;
		}
		for (unsigned int j = 0; j < stripe.size(); j++) {
			if (NF == stripe[j]) offset[j] = i;
		}
	}
	// sort vertically
	sort(Blocks.begin(), Blocks.begin() + offset[0] + 1, compareBlockVertical);
	for (unsigned int i = 0; i < stripe.size() - 1; i++) {
		sort(Blocks.begin() + offset[i] + 1, Blocks.begin() + offset[i + 1] + 1, compareBlockVertical);
	}
	sort(Blocks.begin() + offset[stripe.size() - 1] + 1, Blocks.end(), compareBlockVertical);

	// divide into sectors
	vector<int> sector;
	for (int i = 0; i < divideBy; i++) {
		for (int j = 1; j <= divideBy; j++) {
			sector.push_back(((i == 0) ? 0 : stripe[i - 1]) + round((double)(((i == divideBy - 1) ? notFixed : stripe[i]) - ((i == 0) ? 0 : stripe[i - 1])) / divideBy * j));
		}
	}
	int sectorCount = 0;
	int stripeCount = 0;
	NF = 0;
	for (unsigned int i = 0; i < Blocks.size(); i++) {
		if (!Blocks[i].getIsFixed()) {
			double ax = (stripeCount + 0.5) * (range_right - range_left) / divideBy;
			double ay = (sectorCount + 0.5) * (range_bottom - range_top) / divideBy;
			Blocks[i].setAnchor(ax, ay);
			NF++;
			while (NF == sector[stripeCount * divideBy + sectorCount]) {
				if (sectorCount == divideBy - 1) {
					stripeCount++;
					sectorCount = 0;
				}
				else {
					sectorCount++;
				}
			}
		}
	}
	// sort back to original order
	sort(Blocks.begin(), Blocks.end(), compareBlockNum);
}

void Placing::PlacingQ3(int divideBy, int anchorWeightLevel, bool shouldPrint) {
	BlockPartition(divideBy);
	// anchor Q & bx, by
	int anchorWeight;
	int AW1 = 5;
	int AW2 = 25;
	int AW3 = 250;
	switch (anchorWeightLevel) {
	case 1: {anchorWeight = AW1; break; }
	case 2: {anchorWeight = AW2; break; }
	case 3: {anchorWeight = AW3; break; }
	default: {cout << "Error: Weight level not supported!\n"; return; }

	}
	if (shouldPrint) {
		cout << "--------------------------------------------------------------------------------------\n";
		cout << "Anchor weight is set to: " << anchorWeight << endl;
	}
	vector<vector<double>> anchoredQ;
	vector<double> anchoredBx, anchoredBy;
	// replicate matrix Q and b
	for (int i = 0; i < notFixed; i++) {
		vector<double> row;
		anchoredQ.push_back(row);
		anchoredBx.push_back(bx[i]);
		anchoredBy.push_back(by[i]);
		for (int j = 0; j < notFixed; j++) {
			anchoredQ[i].push_back(Q[i][j]);
		}
	}
	// add anchor effect
	int NFindex = 0;
	for (unsigned int i = 0; i < Blocks.size(); i++) {
		if (!Blocks[i].getIsFixed()) {
			anchoredQ[NFindex][NFindex] += anchorWeight;
			anchoredBx[NFindex] += anchorWeight * Blocks[i].getAnchor(0);
			anchoredBy[NFindex] += anchorWeight * Blocks[i].getAnchor(1);
			NFindex++;
		}
	}
	//
	vector<vector<double>>* pQ = &anchoredQ;
	vector<double>* pbx = &anchoredBx;
	vector<double>* pby = &anchoredBy;
	QbToPosition(pQ, pbx, pby, shouldPrint);
}

void Placing::RedoQ12() {
	vector<vector<double>>* pQ = &Q;
	vector<double>* pbx = &bx;
	vector<double>* pby = &by;
	QbToPosition(pQ, pbx, pby, 0);
}
