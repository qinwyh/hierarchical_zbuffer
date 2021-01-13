#include "Pyramid.h"

PyramidNode::PyramidNode(int x_left, int x_right, int y_lower, int y_upper, PyramidNode* parent) {
	this->x_left = x_left;
	this->x_right = x_right;
	this->y_lower = y_lower;
	this->y_upper = y_upper;
	this->parent = parent;
	this->min_depth = -FAR;

	children = std::vector<PyramidNode*>(4, nullptr);
}


PyramidNode::~PyramidNode() {}


Pyramid::Pyramid(int width, int height) {
	this->width = width;
	this->height = height;

	this->root = new PyramidNode(0, width - 1, 0, height - 1);

	this->zbuffer_to_node = new PyramidNode** [height];
	for (int i = 0; i < height; ++i) {
		this->zbuffer_to_node[i] = new PyramidNode* [width];
	}
}


Pyramid::~Pyramid() {
	/*
	if (this->zbuffer_to_node != NULL) {
		for (int i = 0; i < height; ++i) {
			delete[] zbuffer_to_node[i];
			zbuffer_to_node[i] = NULL;
		}
	}
	delete[] zbuffer_to_node;
	zbuffer_to_node = NULL;
	*/
}


void Pyramid::construct_pyramid() {
	int x_mid = (root->x_left + root->x_right) / 2;
	int y_mid = (root->y_lower + root->y_upper) / 2;

	root->children[0] = init_children_node(root->x_left, x_mid, y_mid + 1, root->y_upper, root);
	root->children[1] = init_children_node(x_mid + 1, root->x_right, y_mid + 1, root->y_upper, root);
	root->children[2] = init_children_node(root->x_left, x_mid, root->y_lower, y_mid, root);
	root->children[3] = init_children_node(x_mid + 1, root->x_right, root->y_lower, y_mid, root);
}


PyramidNode* Pyramid::init_children_node(int x_left, int x_right, int y_lower, int y_upper, PyramidNode* parent) {
	if ((x_left > x_right) || (y_lower > y_upper))
		return nullptr;

	PyramidNode* node = new PyramidNode(x_left, x_right, y_lower, y_upper, parent);

	if ((x_left == x_right) && (y_lower == y_upper)) {
		this->zbuffer_to_node[y_lower][x_left] = node;
		return node;
	}
	else if ((x_left == x_right) && (y_lower != y_upper)) {
		int y_mid = (y_lower + y_upper) / 2;
		node->children[0] = init_children_node(x_left, x_right, y_mid + 1, y_upper, node);
		node->children[2] = init_children_node(x_left, x_right, y_lower, y_mid, node);
		return node;
	}
	else if ((x_left != x_right) && (y_lower == y_upper)) {
		int x_mid = (x_left + x_right) / 2;
		node->children[0] = init_children_node(x_left, x_mid, y_lower, y_upper, node);
		node->children[1] = init_children_node(x_mid + 1, x_right, y_lower, y_upper, node);
		return node;
	}
	else {
		int x_mid = (x_left + x_right) / 2;
		int y_mid = (y_lower + y_upper) / 2;
		node->children[0] = init_children_node(x_left, x_mid, y_mid + 1, y_upper, node);
		node->children[1] = init_children_node(x_mid + 1, x_right, y_mid + 1, y_upper, node);
		node->children[2] = init_children_node(x_left, x_mid, y_lower, y_mid, node);
		node->children[3] = init_children_node(x_mid + 1, x_right, y_lower, y_mid, node);
		return node;
	}
}


void Pyramid::set_min_depth(int y, int x, float z) {
	PyramidNode* leaf = this->zbuffer_to_node[y][x];

	if (leaf->min_depth > z)
		return;

	leaf->min_depth = z;
	update_pyramid(leaf);
}


void Pyramid::update_pyramid(PyramidNode* leaf) {
	if (leaf->parent == nullptr)
		return;

	PyramidNode* parent = leaf->parent;
	float old_min_depth = leaf->parent->min_depth;

	if (parent->children[1] != nullptr)
		parent->min_depth = min(parent->children[0]->min_depth, parent->children[1]->min_depth);
	if (parent->children[2] != nullptr)
		parent->min_depth = min(parent->min_depth, parent->children[2]->min_depth);
	if (parent->children[3] != nullptr)
		parent->min_depth = min(parent->min_depth, parent->children[3]->min_depth);
									

	if (!is_equal_f(old_min_depth, parent->min_depth))
		update_pyramid(leaf->parent);
}


// return [bbox_x_left, bbox_x_right, bbox_y_lower, bbox_y_upper]
int* Pyramid::generate_bbox(Model& model, Face& face) {
	int* bbox = new int[4];
	vector<Vertex> vertices;

	for (int i = 0; i < face.vertex_id_list.size(); i++)
		vertices.push_back(model.vertices[face.vertex_id_list[i]]);

	bbox[0] = width - 1, bbox[1] = 0, bbox[2] = height - 1, bbox[3] = 0;

	for (int i = 0; i < vertices.size(); i++) {
		bbox[0] = max(min((int)round(vertices[i].x), bbox[0]), 0);
		bbox[1] = min(max((int)round(vertices[i].x), bbox[1]), width - 1);
		bbox[2] = max(min((int)round(vertices[i].y), bbox[2]), 0);
		bbox[3] = min(max((int)round(vertices[i].y), bbox[3]), height - 1);
	}

	return bbox;
}


float Pyramid::get_zmax(Model& model, Face& face) {
	vector<Vertex> vertices;
	float zmax = -FAR;

	for (int i = 0; i < face.vertex_id_list.size(); i++)
		vertices.push_back(model.vertices[face.vertex_id_list[i]]);

	for (int i = 0; i < vertices.size(); i++)
		zmax = max(zmax, vertices[i].z);

	return zmax;
}


bool Pyramid::ztest(int bbox_x_left, int bbox_x_right, int bbox_y_lower, int bbox_y_upper, float z_max, PyramidNode* node) {
	if (node->min_depth > z_max)
		return false;

	// Ztest passed in this level
	if ((node->x_left == node->x_right) && (node->y_lower == node->y_upper))
		return true;

	if ((node->children[0] != nullptr) && (node->children[0]->x_left <= bbox_x_left) && (node->children[0]->x_right >= bbox_x_right)
		&& (node->children[0]->y_lower <= bbox_y_lower) && (node->children[0]->y_upper >= bbox_y_upper))
		return ztest(bbox_x_left, bbox_x_right, bbox_y_lower, bbox_y_upper, z_max, node->children[0]);

	if ((node->children[1] != nullptr) && (node->children[1]->x_left <= bbox_x_left) && (node->children[1]->x_right >= bbox_x_right)
		&& (node->children[1]->y_lower <= bbox_y_lower) && (node->children[1]->y_upper >= bbox_y_upper))
		return ztest(bbox_x_left, bbox_x_right, bbox_y_lower, bbox_y_upper, z_max, node->children[1]);

	if ((node->children[2] != nullptr) && (node->children[2]->x_left <= bbox_x_left) && (node->children[2]->x_right >= bbox_x_right)
		&& (node->children[2]->y_lower <= bbox_y_lower) && (node->children[2]->y_upper >= bbox_y_upper))
		return ztest(bbox_x_left, bbox_x_right, bbox_y_lower, bbox_y_upper, z_max, node->children[2]);

	if ((node->children[3] != nullptr) && (node->children[3]->x_left <= bbox_x_left) && (node->children[3]->x_right >= bbox_x_right)
		&& (node->children[3]->y_lower <= bbox_y_lower) && (node->children[3]->y_upper >= bbox_y_upper))
		return ztest(bbox_x_left, bbox_x_right, bbox_y_lower, bbox_y_upper, z_max, node->children[3]);

	return true;
}


void Pyramid::render_face(Model& model, Face& face) {
	int* bbox = generate_bbox(model, face);
	float z_max = get_zmax(model, face);
	vector<Vertex> vertices;

	for (int i = 0; i < face.vertex_id_list.size(); i++)
		vertices.push_back(model.vertices[face.vertex_id_list[i]]);
	
	for (int y = bbox[2]; y <= bbox[3]; y++)
		for (int x = bbox[0]; x <= bbox[1]; x++)
			if (is_in_triangle(x, y, vertices[0], vertices[1], vertices[2])) {
				// set_min_depth(y, x, 1.0);

				const Vertex& vt = model.vertices[face.vertex_id_list[0]];
				float a = face.normal_vector.x;
				float b = face.normal_vector.y;
				float c = face.normal_vector.z;
				float d = -(a * vt.x + b * vt.y + c * vt.z);

				if (is_equal_f(c, 0.0)) {
					set_min_depth(y, x, z_max);
				}
				else {
					float z = -(d + a * x + b * y) / c;
					set_min_depth(y, x, z);
				}
			}
	
	return;
}


bool Pyramid::is_in_triangle(int x, int y, Vertex& a, Vertex& b, Vertex& c) {
	float sign_of_trig = (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
	float sign_of_ab = (b.x - a.x) * (y - a.y) - (b.y - a.y) * (x - a.x);
	float sign_of_ca = (a.x - c.x) * (y - c.y) - (a.y - c.y) * (x - c.x);
	float sign_of_bc = (c.x - b.x) * (y - c.y) - (c.y - b.y) * (x - c.x);

	bool d1 = (sign_of_ab * sign_of_trig > 0);
	bool d2 = (sign_of_ca * sign_of_trig > 0);
	bool d3 = (sign_of_bc * sign_of_trig > 0);

	return d1 && d2 && d3;
}


void Pyramid::render_model(Model& model) {
	clock_t start_time = clock();

	for (int i = 0; i < model.faces.size(); i++) {
		int* bbox = generate_bbox(model, model.faces[i]);
		float z_max = get_zmax(model, model.faces[i]);
		if (ztest(bbox[0], bbox[1], bbox[2], bbox[3], z_max, root)) {
			render_face(model, model.faces[i]);
		}
	}

	cout << "Pyramid Time Used: " << float((clock() - start_time)) << "ms" << endl;

	return;
}


float Pyramid::get_min() {
	float min_value = FAR;

	for (int y = 0; y < height; y++)
		for (int x = 0; x < width; x++) {
			if (!is_equal_f(zbuffer_to_node[y][x]->min_depth, -FAR)) {
				min_value = min(zbuffer_to_node[y][x]->min_depth, min_value);
			}
		}

	return min_value;
}


float Pyramid::get_max() {
	float max_value = -FAR;

	for (int y = 0; y < height; y++)
		for (int x = 0; x < width; x++) {
			max_value = max(zbuffer_to_node[y][x]->min_depth, max_value);
		}

	return max_value;
}