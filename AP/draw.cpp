#include "draw.h"


Placing* AP1;   // set as global variable so other functions can access easily
int weightLevel = 0;
bool firstTime = 0;  //enable print when press the button
int divideBy = 1;
float windowSize = 1000.0;
float drawRange = 900.0;
float whiteSpace = 50.0;
float labelDistance = 25.0;

int openGraphics(Placing* AP, int db) {
	AP1 = AP;
	divideBy = db;

	/* initialize display with WHITE background */
	cout << "Starting graphics...\n";
	init_graphics("Analytical Placer (by Zhiyuan Gu)", WHITE);               // text on the top of the window

	/* still picture drawing allows user to zoom, etc. */
	init_world(0., 0., windowSize, windowSize);                              // Set-up drawing coordinates.
	update_message("Press PROCEED to start Placing");                              // text down below

	event_loop(act_on_button_press, NULL, NULL, drawGrid);

	cout << "You pressed PROCEED!\n" << "Press NEXT PATH so the paths will be shown!\n";

	clearscreen();
	init_world(0., 0., windowSize, windowSize);
	update_message("Press SPREAD with three different levels to spread the non-fixed blocks");
	// make sure only cct2 and cct3 can be spread
	if ((AP1->getRange(1) - AP1->getRange(0)) == 64 && (AP1->getRange(3) - AP1->getRange(2)) == 64) {
		create_button("Window", "Spread1", Spread1);
		create_button("Window", "Spread2", Spread2);
		create_button("Window", "Spread3", Spread3);
		create_button("Window", "Rewind", Rewind);
	}

	drawPlacement();
	event_loop(act_on_button_press, NULL, NULL, drawPlacement);

	close_graphics();                                             // close graphics tool
	cout << "Graphics closed down.\n";
	return 0;
}

void drawGrid() {
	set_draw_mode(DRAW_NORMAL);                                   // Should set this if your program does any XOR drawing in callbacks.
	clearscreen();                                                 // Should precede drawing for all drawscreens

	setfontsize(10);
	setlinestyle(DASHED);
	setlinewidth(3);
	setcolor(BLACK);
	// print grid
	for (float x = whiteSpace; x <= whiteSpace + drawRange; x = x + drawRange / divideBy) {
		drawline(whiteSpace, x, whiteSpace + drawRange, x);
		drawline(x, whiteSpace, x, whiteSpace + drawRange);
	}

	double left = AP1->getRange(0);
	double right = AP1->getRange(1);
	double top = AP1->getRange(2);
	double bottom = AP1->getRange(3);

	// print corner coordinates
	setfontsize(24);
	char c_buffer[33];
	sprintf(c_buffer, "(%0.1f, %0.1f)", (float)(left), (float)(top));
	drawtext((int)(whiteSpace - labelDistance),
		(int)(whiteSpace - labelDistance),
		c_buffer,
		150.);
	sprintf(c_buffer, "(%0.1f, %0.1f)", (float)(left), (float)(bottom));
	drawtext((int)(whiteSpace - labelDistance),
		(int)(whiteSpace + drawRange + labelDistance),
		c_buffer,
		150.);
	sprintf(c_buffer, "(%0.1f, %0.1f)", (float)(right), (float)(top));
	drawtext((int)(whiteSpace + drawRange + labelDistance),
		(int)(whiteSpace - labelDistance),
		c_buffer,
		150.);
	sprintf(c_buffer, "(%0.1f, %0.1f)", (float)(right), (float)(bottom));
	drawtext((int)(whiteSpace + drawRange + labelDistance),
		(int)(whiteSpace + drawRange + labelDistance),
		c_buffer,
		150.);

	// draw anchor point
	if (weightLevel >= 0) {   // can set to >0
		for (int i = 0; i < divideBy; i++) {
			for (int j = 0; j < divideBy; j++) {
				setcolor(GREEN);
				double x = (0.5 + j) * drawRange / divideBy;
				double y = (0.5 + i) * drawRange / divideBy;
				fillarc(x + whiteSpace,
					y + whiteSpace,
					12.0,
					0., 360.);
			}
		}
	}

}

void drawPlacement() {                                          // redrawing routine for still pictures.
	// recalculate to spread
	if (weightLevel > 0) {
		AP1->PlacingQ3(divideBy, weightLevel, firstTime);
		firstTime = 0;  // once printed, turn off printing option
	}
	else {
		AP1->RedoQ12();
	}

	// start drawing
	set_draw_mode(DRAW_NORMAL);                                   // Should set this if your program does any XOR drawing in callbacks.
	clearscreen();                                                 // Should precede drawing for all drawscreens
	drawGrid();

	double left = AP1->getRange(0);
	double right = AP1->getRange(1);
	double top = AP1->getRange(2);
	double bottom = AP1->getRange(3);
	double x_range = right - left;
	double y_range = bottom - top;
	float scale = (x_range > y_range) ? (drawRange / x_range) : (drawRange / y_range);

	// draw lines(rat's nest)
	setlinestyle(SOLID);
	setlinewidth(1);
	setcolor(YELLOW);
	if (AP1->getNetModel() == clique) {
		for (unsigned int n = 0; n < AP1->getNets()->size(); n++) {
			for (unsigned int b1 = 0; b1 < AP1->getNets()->at(n).getConn().size(); b1++) {
				for (unsigned int b2 = b1 + 1; b2 < AP1->getNets()->at(n).getConn().size(); b2++) {
					int b1Num = AP1->getNets()->at(n).getConn()[b1];
					int b2Num = AP1->getNets()->at(n).getConn()[b2];
					float x1 = scale * (AP1->getBlocks()->at(b1Num - 1).getXY(0) - left);
					float y1 = scale * (AP1->getBlocks()->at(b1Num - 1).getXY(1) - top);
					float x2 = scale * (AP1->getBlocks()->at(b2Num - 1).getXY(0) - left);
					float y2 = scale * (AP1->getBlocks()->at(b2Num - 1).getXY(1) - top);
					drawline(x1 + whiteSpace,
						y1 + whiteSpace,
						x2 + whiteSpace,
						y2 + whiteSpace);
				}
			}
		}
	}
	else if (AP1->getNetModel() == tree) {
		for (unsigned int n = 0; n < AP1->getNets()->size(); n++) {
			for (unsigned int bl = 1; bl < AP1->getNets()->at(n).getConn().size(); bl++) {
				int bsNum = AP1->getNets()->at(n).getConn()[0];
				int blNum = AP1->getNets()->at(n).getConn()[bl];
				float x1 = scale * (AP1->getBlocks()->at(bsNum - 1).getXY(0) - left);
				float y1 = scale * (AP1->getBlocks()->at(bsNum - 1).getXY(1) - top);
				float x2 = scale * (AP1->getBlocks()->at(blNum - 1).getXY(0) - left);
				float y2 = scale * (AP1->getBlocks()->at(blNum - 1).getXY(1) - top);
				drawline(x1 + whiteSpace,
					y1 + whiteSpace,
					x2 + whiteSpace,
					y2 + whiteSpace);

			}
		}
	}


	// draw blocks
	for (unsigned int b = 0; b < AP1->getBlocks()->size(); b++) {
		setcolor(BLUE);
		double x = scale * (AP1->getBlocks()->at(b).getXY(0) - left);
		double y = scale * (AP1->getBlocks()->at(b).getXY(1) - top);
		fillarc(x + whiteSpace,
			y + whiteSpace,
			12.0,
			0., 360.);
		// draw No. of blocks
		int BNum = AP1->getBlocks()->at(b).getNumber();
		setcolor(WHITE);
		setfontsize(18);
		char c_buffer[33];
		sprintf(c_buffer, "%d", BNum);
		drawtext(x + whiteSpace,
			y + whiteSpace,
			c_buffer,
			150.);
	}



}

void act_on_button_press(float x, float y) {

	/* Called whenever event_loop gets a button press in the graphics *
	 * area.  Allows the user to do whatever he/she wants with button *
	 * clicks.                                                        */

	printf("User clicked a button at coordinates (%f, %f)\n", x, y);


}

void Rewind(void(*drawscreen_ptr) (void)) {
	weightLevel = 0;
	drawPlacement();
	printf("--------------------------------------------------------------------------------------\n");
	printf("Anchor removed!\n");
}

void Spread1(void(*drawscreen_ptr) (void)) {
	weightLevel = 1;
	firstTime = 1;
	drawPlacement();
	printf("Current weight level: %d\n", weightLevel);
}

void Spread2(void(*drawscreen_ptr) (void)) {
	weightLevel = 2;
	firstTime = 1;
	drawPlacement();
	printf("Current weight level: %d\n", weightLevel);
}

void Spread3(void(*drawscreen_ptr) (void)) {
	weightLevel = 3;
	firstTime = 1;
	drawPlacement();
	printf("Current weight level: %d\n", weightLevel);
}


