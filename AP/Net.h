#include <iostream>
#include <vector>


using namespace std;

class Net {
private:
	int number;
	vector<int> connBlock;
	double HPWL;

public:
	Net(int n);
	~Net();


	int getNumber() { return number; }
	vector<int> getConn() { return connBlock; }
	void setHPWL(double WL);
	void addConn(int B);

};
