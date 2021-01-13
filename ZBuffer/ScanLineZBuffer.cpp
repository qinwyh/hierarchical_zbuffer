#include "ScanLineZBuffer.h"

using namespace std;


ScanLineZBuffer::ScanLineZBuffer() {
	this->width = 800;
	this->height = 600;

	id_buffer = new int* [height];
	zbuffer = new float[width];
	complete_zbuffer = new float* [height];
	for (int i = 0; i < height; ++i) {
		id_buffer[i] = new int[width];
		complete_zbuffer[i] = new float[width];
	}
}


ScanLineZBuffer::~ScanLineZBuffer() {

	if (this->zbuffer != NULL)
	{
		delete[] zbuffer;
		zbuffer = NULL;
	}

	if (this->complete_zbuffer != NULL) {
		for (int i = 0; i < height; ++i) {
			delete[] complete_zbuffer[i];
			complete_zbuffer[i] = NULL;
		}
	}
	delete[] complete_zbuffer;
	complete_zbuffer = NULL;

	if (this->id_buffer != NULL) {
		for (int i = 0; i < height; ++i) {
			delete[] id_buffer[i];
			id_buffer[i] = NULL;
		}
	}
	delete[] id_buffer;
	id_buffer = NULL;
}


void ScanLineZBuffer::construct(const Model& model) {
	this->edge_table.clear();
	this->edge_table.resize(this->height);
	this->polygon_table.clear();
	this->polygon_table.resize(this->height);


	for (int i = 0; i < model.faces.size(); i++) {
		// construct polygon table and edge table
		const Face& cur_face = model.faces[i];
		
		Vertex vt1 = model.vertices[cur_face.vertex_id_list[0]];
		Vertex vt2 = model.vertices[cur_face.vertex_id_list[1]];
		Vertex vt3 = model.vertices[cur_face.vertex_id_list[2]];

		float y_min = min(min(vt1.y, vt2.y), vt3.y);
		float y_max = max(max(vt1.y, vt2.y), vt3.y);

		// Polygon
		Polygon polygon = Polygon();
		polygon.dy = ceil(y_max) - ceil(y_min);
		if ((polygon.dy != 0) && (y_max < height) && (y_min > 0)) {
			polygon.id = i;

			const Vertex& vt = model.vertices[cur_face.vertex_id_list[0]];
			polygon.a = cur_face.normal_vector.x;
			polygon.b = cur_face.normal_vector.y;
			polygon.c = cur_face.normal_vector.z;
			polygon.d = -(polygon.a * vt.x + polygon.b * vt.y + polygon.c * vt.z);

			polygon_table[floor(y_max)].push_back(polygon);
		}
		else
			continue;

		float y_upper, y_lower;
		// Edge 1
		y_upper = vt1.y > vt2.y ? vt1.y : vt2.y;
		y_lower = vt1.y < vt2.y ? vt1.y : vt2.y;

		Edge edge_1 = Edge();
		edge_1.x = vt1.y > vt2.y ? vt1.x : vt2.x;
		edge_1.dy = abs(ceil(vt1.y) - ceil(vt2.y));
		if (edge_1.dy > 0) {
			edge_1.dx = -(vt2.x - vt1.x) / (vt2.y - vt1.y);
			edge_1.id = i;

			edge_table[floor(y_upper)].push_back(edge_1);
		}

		// Edge 2
		y_upper = vt2.y > vt3.y ? vt2.y : vt3.y;
		y_lower = vt2.y < vt3.y ? vt2.y : vt3.y;

		Edge edge_2 = Edge();
		edge_2.x = vt2.y > vt3.y ? vt2.x : vt3.x;
		edge_2.dy = abs(ceil(vt2.y) - ceil(vt3.y));
		if (edge_2.dy != 0) {
			edge_2.dx = -(vt3.x - vt2.x) / (vt3.y - vt2.y);
			edge_2.id = i;

			edge_table[floor(y_upper)].push_back(edge_2);
		}

		// Edge 3
		y_upper = vt1.y > vt3.y ? vt1.y : vt3.y;
		y_lower = vt2.y < vt3.y ? vt2.y : vt3.y;

		Edge edge_3 = Edge();
		edge_3.x = vt1.y > vt3.y ? vt1.x : vt3.x;
		edge_3.dy = abs(ceil(vt1.y) - ceil(vt3.y));
		if (edge_3.dy != 0) {
			edge_3.dx = -(vt3.x - vt1.x) / (vt3.y - vt1.y);
			edge_3.id = i;

			edge_table[floor(y_upper)].push_back(edge_3);
		}
		
	}


	int edge_table_size = 0;
	int polygon_table_size = 0;
	for (int i = 0; i < edge_table.size(); i++) {
		edge_table_size += edge_table[i].size();
	}
	for (int i = 0; i < polygon_table.size(); i++) {
		polygon_table_size += polygon_table[i].size();
	}
	cout << "Polygon Table Size: " << polygon_table_size << endl;
	cout << "Edge Table Size: " << edge_table_size << endl;
}


void ScanLineZBuffer::perform(const Model& model) {
	clock_t start_time = clock();

	construct(model);
	cout << "ScanLineZbuffer Constructing Time Used: " << float((clock() - start_time)) << "ms" << endl;
	start_time = clock();

	this->active_edge_table.clear();
	this->active_polygon_table.clear();

	for (int y = this->height - 1; y >= 0; y--) {
		// Frame Buffer
		memset(this->id_buffer[y], -1, sizeof(int) * width);
		memset(this->complete_zbuffer[y], -0xfffffff, sizeof(float) * width);
		// Scan Line ZBuffer
		fill(this->zbuffer, this->zbuffer + width, -0xfffffff);
		// fill(this->complete_zbuffer[y], this->complete_zbuffer[y] + width, -0xfffffff);

		// Add Polygons to Active Polygon Table
		ActiveEdge tmp_edge;
		for (int i = 0; i < polygon_table[y].size(); i++) {
			active_polygon_table.push_back(polygon_table[y][i]);
			tmp_edge = add_active_edge(polygon_table[y][i], model, y);
			
			if (tmp_edge.x_left < tmp_edge.x_right)
				active_edge_table.push_back(tmp_edge);
		}


		// Update ZBuffer
		vector<ActiveEdge>::iterator iter;
		for (iter = active_edge_table.begin(); iter != active_edge_table.end(); iter++) {
			if ((iter->dy_left <= 0) || (iter->dy_right <= 0))
				continue;

			float x_left = iter->x_left, x_right = iter->x_right, z_left = iter->z_left, dz_x = iter->dz_x;
			float z;
			int offset = 0;

			for (int i = ceil(x_left); i < ceil(x_right); i++) {
				offset = i - ceil(x_left);
				z = z_left + dz_x * offset;

				if (z > zbuffer[i]) {
					zbuffer[i] = z;
					complete_zbuffer[y][i] = z;
					id_buffer[y][i] = iter->id;
				}
			}

			// Update Active Edge
			iter->dy_left -= 1;
			iter->dy_right -= 1;

			if ((iter->dy_left <= 0) || (iter->dy_right <= 0))
				continue;

			iter->x_left += iter->dx_left;
			iter->x_right += iter->dx_right;
			iter->z_left += (iter->dz_x * iter->dx_left) + iter->dz_y;
		}

		// Update Active Polygon Table
		vector<ActivePolygon>::iterator poly_iter;
		for (poly_iter = active_polygon_table.begin(); poly_iter != active_polygon_table.end(); ) {
			poly_iter->dy -= 1;

			if (poly_iter->dy <= 0) {
				poly_iter = active_polygon_table.erase(poly_iter);
			}
			else
				poly_iter++;
		}

		// Update Active Edge Table
		vector<ActiveEdge> tmp_table;
		tmp_table.clear();
		for (iter = active_edge_table.begin(); iter != active_edge_table.end(); ) {

			if ((iter->dy_left != 0) && (iter->dy_right != 0))
				iter++;

			else {
				int face_id = iter->id;
				iter = active_edge_table.erase(iter);

				for (int i = 0; i < active_polygon_table.size(); i++)
					if ((active_polygon_table[i].id == face_id) && (y >= 1)) {
						tmp_edge = add_active_edge(active_polygon_table[i], model, y - 1);

						if ((tmp_edge.dy_left != 0) && (tmp_edge.dy_right != 0))
							tmp_table.push_back(tmp_edge);
					}
			}
		}
		active_edge_table.insert(active_edge_table.end(), tmp_table.begin(), tmp_table.end());
	}

	cout << "ScanLineZbuffer Time Used: " << float((clock() - start_time)) << "ms" << endl;
}


ActiveEdge ScanLineZBuffer::add_active_edge(const Polygon& polygon, const Model& model, int y) {
	vector<vector<int>> edges;
	vector<Vertex> vertices;
	vector<int> left_edge, right_edge;
	const Face& cur_face = model.faces[polygon.id];
	float x_left, x_right;

	vertices.push_back(model.vertices[cur_face.vertex_id_list[0]]);
	vertices.push_back(model.vertices[cur_face.vertex_id_list[1]]);
	vertices.push_back(model.vertices[cur_face.vertex_id_list[2]]);

	if ((vertices[0].y < y && vertices[1].y > y) || (vertices[0].y > y && vertices[1].y < y)) {
		vector<int> tmp = { 0, 1 };
		edges.push_back(tmp);
	}
	if ((vertices[0].y < y && vertices[2].y > y) || (vertices[0].y > y && vertices[2].y < y)) {
		vector<int> tmp = { 0, 2 };
		edges.push_back(tmp);
	}
	if ((vertices[1].y < y && vertices[2].y > y) || (vertices[1].y > y && vertices[2].y < y)) {
		vector<int> tmp = { 1, 2 };
		edges.push_back(tmp);
	}

	if (edges.size() < 2) {
		ActiveEdge active_edge = ActiveEdge();
		active_edge.x_left = -1.0f;
		active_edge.x_right = 1.0f;
		active_edge.dy_left = 0;
		active_edge.dy_right = 0;
		active_edge.id = polygon.id;

		return active_edge;
	}
	
	left_edge = edges[0], right_edge = edges[1];
	x_left = vertices[left_edge[0]].x + (y - vertices[left_edge[0]].y) * (vertices[left_edge[1]].x - vertices[left_edge[0]].x) /
			(vertices[left_edge[1]].y - vertices[left_edge[0]].y);
	x_right = vertices[right_edge[0]].x + (y - vertices[right_edge[0]].y) * (vertices[right_edge[1]].x - vertices[right_edge[0]].x) /
			 (vertices[right_edge[1]].y - vertices[right_edge[0]].y);

	if (x_left > x_right) {
		vector<int> tmp_edge = left_edge;
		left_edge = right_edge;
		right_edge = tmp_edge;
	}

	ActiveEdge active_edge = ActiveEdge();
	active_edge.x_left = min(x_left, x_right);
	active_edge.dx_left = -(vertices[left_edge[1]].x - vertices[left_edge[0]].x) / (vertices[left_edge[1]].y - vertices[left_edge[0]].y);
	active_edge.dy_left = max(y - floor(vertices[left_edge[0]].y), y - floor(vertices[left_edge[1]].y));

	active_edge.x_right = max(x_left, x_right);
	active_edge.dx_right = -(vertices[right_edge[1]].x - vertices[right_edge[0]].x) / (vertices[right_edge[1]].y - vertices[right_edge[0]].y);
	active_edge.dy_right = max(y - floor(vertices[right_edge[0]].y), y - floor(vertices[right_edge[1]].y));
	
	if (is_equal_f(polygon.c, 0.0)) {
		active_edge.z_left = 0;
		active_edge.dz_x = 0;
		active_edge.dz_y = 0;
	}
	else {
		active_edge.z_left = -(polygon.d + polygon.a * active_edge.x_left + polygon.b * y) / polygon.c;
		active_edge.dz_x = -polygon.a / polygon.c;
		active_edge.dz_y = polygon.b / polygon.c;
	}

	active_edge.id = polygon.id;

	return active_edge;
}


float ScanLineZBuffer::get_zbuffer_min() {
	float min_value = 0xfffffff;
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			if (complete_zbuffer[y][x] < min_value)
				min_value = complete_zbuffer[y][x];
		}
	}

	return min_value;
}


float ScanLineZBuffer::get_zbuffer_max() {
	float max_value = -0xfffffff;
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			if (complete_zbuffer[y][x] > max_value)
				max_value = complete_zbuffer[y][x];
		}
	}

	return max_value;
}


float ScanLineZBuffer::get_zbuffer(int y, int x) {
	return complete_zbuffer[y][x];
}