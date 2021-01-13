#include "Octree.h"


OcTreeNode::OcTreeNode(float x_min, float x_max, float y_min, float y_max, float z_min, float z_max) {
	this->x_min = x_min;
	this->x_max = x_max;
	this->y_min = y_min;
	this->y_max = y_max;
	this->z_min = z_min;
	this->z_max = z_max;

	children = std::vector<OcTreeNode*>(8, nullptr);

}


bool OcTreeNode::is_face_in_cube(Model& model, Face& face) {
	vector<Vertex> vertices;
	float x_min = FAR, x_max = -FAR; 
	float y_min = FAR, y_max = -FAR;
	float z_min = FAR, z_max = -FAR;

	for (int i = 0; i < face.vertex_id_list.size(); i++)
		vertices.push_back(model.vertices[face.vertex_id_list[i]]);

	for (int i = 0; i < vertices.size(); i++) {
		x_min = min(x_min, vertices[i].x);
		x_max = max(x_max, vertices[i].x);
		y_min = min(y_min, vertices[i].y);
		y_max = max(y_max, vertices[i].y);
		z_min = min(z_min, vertices[i].z);
		z_max = max(z_max, vertices[i].z);
	}

	return (x_min > this->x_min) && (x_max < this->x_max)
		&& (y_min > this->y_min) && (y_max < this->y_max)
		&& (z_min > this->z_min) && (z_max < this->z_max);
}


OcTree::OcTree(int width, int height, Model& model) {
	this->width = width;
	this->height = height;

	float x_min = FAR, x_max = -FAR;
	float y_min = FAR, y_max = -FAR;
	float z_min = FAR, z_max = -FAR;

	vector<Vertex> vertices = model.vertices;

	for (int i = 0; i < vertices.size(); i++) {
		x_min = min(x_min, vertices[i].x);
		x_max = max(x_max, vertices[i].x);
		y_min = min(y_min, vertices[i].y);
		y_max = max(y_max, vertices[i].y);
		z_min = min(z_min, vertices[i].z);
		z_max = max(z_max, vertices[i].z);
	}

	this->root = new OcTreeNode(x_min, x_max, y_min, y_max, z_min, z_max);
	this->pyramid = new Pyramid(width, height);
	this->pyramid->construct_pyramid();
}


void OcTree::construct_octree(Model& model) {
	vector<Vertex> vertices = model.vertices;
	vector<Face> faces = model.faces;

	for (int i = 0; i < faces.size(); i++) {
		add_face(root, model, faces[i]);
	}

	return;
}


void OcTree::add_face(OcTreeNode* node, Model& model, Face& face) {
	float x_mid = (node->x_min + node->x_max) / 2;
	float y_mid = (node->y_min + node->y_max) / 2;
	float z_mid = (node->z_min + node->z_max) / 2;


	if (node->children[0] == nullptr) {
		node->children[0] = new OcTreeNode(node->x_min, x_mid, node->y_min, y_mid, z_mid, node->z_max);
		node->children[1] = new OcTreeNode(x_mid, node->x_max, node->y_min, y_mid, z_mid, node->z_max);
		node->children[2] = new OcTreeNode(node->x_min, x_mid, y_mid, node->y_max, z_mid, node->z_max);
		node->children[3] = new OcTreeNode(x_mid, node->x_max, y_mid, node->y_max, z_mid, node->z_max);
		node->children[4] = new OcTreeNode(node->x_min, x_mid, node->y_min, y_mid, node->z_min, z_mid);
		node->children[5] = new OcTreeNode(x_mid, node->x_max, node->y_min, y_mid, node->z_min, z_mid);
		node->children[6] = new OcTreeNode(node->x_min, x_mid, y_mid, node->y_max, node->z_min, z_mid);
		node->children[7] = new OcTreeNode(x_mid, node->x_max, y_mid, node->y_max, node->z_min, z_mid);
	}

	bool is_in_next_level = false;

	for (int i = 0; i < node->children.size(); i++) {
		if (node->children[i]->is_face_in_cube(model, face)) {
			is_in_next_level = true;
			add_face(node->children[i], model, face);
		}
	}

	if (!is_in_next_level) {
		node->faces.push_back(face);
		// cout << "test" << endl;
	}

	return;
}


void OcTree::render_model(Model& model, OcTreeNode* node) {
	// clock_t start_time = clock();
	if (node->children[0] == nullptr) {
		for (int i = 0; i < node->faces.size(); i++) {
			pyramid->render_face(model, node->faces[i]);
		}

		return;
	}

	// z_test
	if (pyramid->ztest((int)round(node->x_min), (int)round(node->x_max), (int)round(node->y_min),
					   (int)round(node->y_max), node->z_max, pyramid->root)) {
		for (int i = 0; i < node->children.size(); i++) {
			for (int i = 0; i < node->faces.size(); i++) {
				pyramid->render_face(model, node->faces[i]);
			}
			render_model(model, node->children[i]);
		}
	}

	// cout << "OcTree Time Used: " << float((clock() - start_time)) << "ms" << endl;
	return ;
}