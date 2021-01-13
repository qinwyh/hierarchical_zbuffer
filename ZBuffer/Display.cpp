#include "Display.h"

using namespace std;

int Display::width = 800, Display::height = 600;
ScanLineZBuffer* Display::scanline_zbuffer = NULL;
Pyramid* Display::pyramid = NULL;


Display::Display(int weight, int height) {
	width = width;
	height = height;
}


Display::~Display() {}


void Display::loop_scanline_zbuffer() {
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, width, 0, height);
	glBegin(GL_POINTS);

	float min_value = scanline_zbuffer->get_zbuffer_min();
	float max_value = scanline_zbuffer->get_zbuffer_max();

	cout << "Min: " << min_value << endl;
	cout << "Max: " << max_value << endl;

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {

			Color3f rgb = Color3f(0.0f, 0.0f, 0.0f);
			float tmp = scanline_zbuffer->get_zbuffer(y, x);

			if (tmp > -0xfffffff) {
				float gray_value = (scanline_zbuffer->get_zbuffer(y, x) - min_value) / (max_value - min_value);
				rgb.r = gray_value;
				rgb.g = gray_value;
				rgb.b = gray_value;

			}
			glColor3f(rgb.r, rgb.g, rgb.b);
			glVertex2i(x, y);
		}
	}
	glEnd();

	glFinish();
}


void Display::loop_pyramid() {
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, width, 0, height);
	glBegin(GL_POINTS);

	float min_value = pyramid->get_min();
	float max_value = pyramid->get_max();

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {

			Color3f rgb = Color3f(0.0f, 0.0f, 0.0f);
			float tmp = pyramid->zbuffer_to_node[y][x]->min_depth;

			if (!is_equal_f(tmp, -FAR)) {
				float gray_value = (tmp - min_value) / (max_value - min_value);
				//float gray_value = (tmp - 0) / (max_value - 0);
				rgb.r = gray_value;
				rgb.g = gray_value;
				rgb.b = gray_value;

			}
			glColor3f(rgb.r, rgb.g, rgb.b);
			glVertex2i(x, y);
		}
	}
	glEnd();

	glFinish();
}


void Display::display_scanline_zbuffer() {
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE | GLUT_DEPTH);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(100, 100);

	glutCreateWindow("ZBuffer");
	glutDisplayFunc(loop_scanline_zbuffer);
	glutMainLoop();
}


void Display::display_pyramid() {
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE | GLUT_DEPTH);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(100, 100);

	glutCreateWindow("Pyramid");
	glutDisplayFunc(loop_pyramid);
	glutMainLoop();
}