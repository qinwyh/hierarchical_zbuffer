#ifndef __PYRAMID_H
#define __PYRAMID_H

#include <vector>
#include <algorithm>
#include <ctime>
#include <math.h>
#include "Vector.h"
#include "Model.h"
#define FAR 1e6

using namespace std;

class PyramidNode {
	public:
		int x_left, x_right;
		int y_upper, y_lower;
		float min_depth;

		PyramidNode* parent;
		std::vector<PyramidNode*> children;

		PyramidNode() {};
		PyramidNode(int x_left, int x_right, int y_lower, int y_upper, PyramidNode* parent = nullptr);
		~PyramidNode();
};


class Pyramid {
	public:
		int width, height;
		PyramidNode* root;
		PyramidNode*** zbuffer_to_node;

		Pyramid() {};
		Pyramid(int width, int height);
		~Pyramid();

		void construct_pyramid();
		PyramidNode* init_children_node(int x_left, int x_right, int y_lower, int y_upper, PyramidNode* parent);
		void set_min_depth(int y, int x, float z);
		void update_pyramid(PyramidNode* leaf);
		int* generate_bbox(Model& model, Face& face);
		float get_zmax(Model& model, Face& face);
		bool ztest(int bbox_x_left, int bbox_x_right, int bbox_y_lower, int bbox_y_upper, float z_max, PyramidNode* node);
		void render_model(Model& model);
		void render_face(Model& model, Face& face);
		bool is_in_triangle(int x, int y, Vertex& a, Vertex& b, Vertex& c);
		float get_min();
		float get_max();
};

#endif // !__PYRAMID_H

