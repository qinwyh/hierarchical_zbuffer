#include <iostream>
#include <string>
#include <glut.h>
#include <ctime>
#include "Vector.h"
#include "Model.h"
#include "Display.h"
#include "ScanLineZBuffer.h"
#include "Pyramid.h"
#include "Octree.h"

using namespace std;


int main(int argc, char** argv) {
	// Window Size
	int width = 800, height = 600;

	// Load obj File
	string file_path = "../models/Bunny.obj";
	Model model = Model(file_path);


	// Scan Line Zbuffer
	
	/*
	ScanLineZBuffer scanline_zbuffer = ScanLineZBuffer();
	scanline_zbuffer.perform(model);
	cout << "here" << endl;

	Display display = Display(width, height);
	display.scanline_zbuffer = &scanline_zbuffer;
	display.display_scanline_zbuffer();
	*/

	
	// Hierachical ZBuffer (Simple Mode with Pyramid Only)

	/*
	Pyramid pyramid = Pyramid(width, height);
	clock_t start_time = clock();
	pyramid.construct_pyramid();
	cout << "Pyramid Constructing Time Used: " << float((clock() - start_time)) << "ms" << endl;
	pyramid.render_model(model);

	Display display = Display(width, height);
	display.pyramid = &pyramid;
	display.display_pyramid();
	*/
	


	// Hierachical ZBuffer (Comlex Mode with OcTree)

	OcTree octree = OcTree(width, height, model);

	clock_t start_time = clock();
	octree.construct_octree(model);
	cout << "OcTree Constructing Time Used: " << float((clock() - start_time)) << "ms" << endl;
	start_time = clock();
	octree.render_model(model, octree.root);
	cout << "OcTree Time Used: " << float((clock() - start_time)) << "ms" << endl;

	Display display = Display(width, height);
	display.pyramid = octree.pyramid;
	display.display_pyramid();

	return 0;
}