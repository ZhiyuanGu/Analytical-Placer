#include <iostream>
#include "draw.h"

using namespace std;

int main(void)
{
	cout << "Analytical Placer starting...\n";
	Placing* AP = new Placing;
	// for question 1 and 2
	AP->PlacingQ12();

	// Graphics which includes question 3
	int num_slices = 4; // partition to 4*4 sectors
	openGraphics(AP, num_slices);

	delete AP;

	return 0;
}

