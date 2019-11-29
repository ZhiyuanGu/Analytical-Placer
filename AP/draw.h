#include "Placing.h"
#include "graphics.h"
#include <iostream>
#include <stdio.h>
#include <vector>

using namespace std;


int openGraphics(Placing* AP, int db);

void Spread1 (void (*drawscreen_ptr) (void));
void Spread2 (void (*drawscreen_ptr) (void));
void Spread3 (void (*drawscreen_ptr) (void));

void drawGrid();
void drawPlacement();
void act_on_button_press (float x, float y);

void Rewind (void (*drawscreen_ptr) (void));

