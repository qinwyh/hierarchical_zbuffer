#ifndef __OCTREE_H
#define __OCTREE_H

#include <vector>
#include <algorithm>
#include <ctime>
#include <math.h>
#include "Vector.h"
#include "Model.h"
#include "Pyramid.h"

using namespace std;


class OcTreeNode {
	public:
		float x_min, x_max;
		float y_min, y_max;
		float z_min, z_max;

		std::vector<OcTreeNode*> children;
		std::vector<Face> faces;

		OcTreeNode() {};
		OcTreeNode(float x_min, float x_max, float y_min, float y_max, float z_min, float z_max);
		~OcTreeNode() {};

		bool is_face_in_cube(Model& model, Face& face);
};


class OcTree {
	public:
		int width, height;
		OcTreeNode* root;
		Pyramid* pyramid;

		OcTree() {};
		OcTree(int width, int height, Model& model);
		~OcTree() {};

		void construct_octree(Model& model);
		void add_face(OcTreeNode* node, Model& model, Face& face);
		void render_model(Model& model, OcTreeNode* node);
};


#endif // !__OCTREE_H
