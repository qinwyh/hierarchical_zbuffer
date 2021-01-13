#ifndef __DISPLAY_H
#define __DISPLAY_H

#include <iostream>
#include <cmath>
#include <glut.h>
#include "Model.h"
#include "ScanLineZBuffer.h"
#include "Pyramid.h"


class Display {
	public:
		Display(int weight, int height);
		~Display();
		void display_scanline_zbuffer();
		void display_pyramid();

		static int width, height;
		static ScanLineZBuffer* scanline_zbuffer;
		static Pyramid* pyramid;

		static void loop_scanline_zbuffer();
		static void loop_pyramid();
};



#endif // !__DISPLAY_H

