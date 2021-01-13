#include "Model.h"

using namespace std;

Model::Model(const std::string& file_path) {
	if (this->load(file_path)) {
		cout << "Model \"" << file_path << "\" successfully load!" << endl;
		cout << "Total Vertex Num: " << this->vertices.size() << endl;
		cout << "Total Vertex Normal Num: " << this->vertex_normals.size() << endl;
		cout << "Total Face Num: " << this->faces.size() << endl;

		rasterize();
	}
	else {
		cout << "Can not open the obj file!" << endl;
	}
}


bool Model::load(const std::string& file_path) {
	clock_t start_time = clock();
	string type;
	ifstream file(file_path);

	if (!file.is_open()) 
		return false;

	// Start Loading
	while (file >> type) {
		// Vertex Coordinates
		if (type == "v") {
			Vertex v;
			file >> v.x >> v.y >> v.z;
			this->vertices.push_back(v);
		}

		// Vertex Normal Vector
		else if (type == "vn") {
			Vector3f vn;
			file >> vn.x >> vn.y >> vn.z;
			this->vertex_normals.push_back(vn);
		}

		// Face Vertices and Corresponding Vertex Normal Vectors(Optional)
		else if (type == "f") {
			Face face = Face();
			int vertex_id, vertex_normal_id;

			while (true) {
				char chr = file.get();

				if ((chr == '\n' || chr == EOF))
					break;
				else if (chr == ' ')
					continue;
				else
					file.putback(chr);
				
				file >> vertex_id;
				vertex_normal_id = 0;

				char next_chr = file.get();
				if (next_chr == '/') {
					next_chr = file.get();
					file >> vertex_normal_id;
				}
				else
					file.putback(next_chr);

				face.vertex_id_list.push_back(vertex_id - 1);
				face.normal_id_list.push_back(vertex_normal_id - 1);
			}

			if (face.vertex_id_list.size() > 2) {
				const Vertex& a = this->vertices[face.vertex_id_list[0]];
				const Vertex& b = this->vertices[face.vertex_id_list[1]];
				const Vertex& c = this->vertices[face.vertex_id_list[2]];
				
				// Normalized vector
				Vector3f normal = normalize(cross(b - a, c - b));
				face.normal_vector = normal;
				this->faces.push_back(face);
			}
		}
	}

	file.close();
	cout << "Model Loading Time: " << clock() - start_time << "ms" << endl;
}


void Model::rasterize() {
	int width = 800, height =  600;
	Vertex min_xyz(0xfffffff, 0xfffffff, 0xfffffff);
	Vertex max_xyz(-0xfffffff, -0xfffffff, -0xfffffff);
	Vertex center_xyz(0.0, 0.0, 0.0);

	int vertex_num = this->vertices.size();

	for (int i = 0; i < this->vertices.size(); i++) {
		const Vertex& vt = this->vertices[i];

		min_xyz.x = min(min_xyz.x, vt.x);
		min_xyz.y = min(min_xyz.y, vt.y);
		min_xyz.z = min(min_xyz.z, vt.z);
		max_xyz.x = max(max_xyz.x, vt.x);
		max_xyz.y = max(max_xyz.y, vt.y);
		max_xyz.z = max(max_xyz.z, vt.z);
	}

	center_xyz = (min_xyz + max_xyz) / 2;

	float model_width = max_xyz.x - min_xyz.x;
	float model_height = max_xyz.y - min_xyz.y;
	float max_model_len = max(model_width, model_height);
	float scale = min(width, height) / max_model_len;
	scale = 0.8 * scale;

#pragma omp parallel for 
	for (int i = 0; i < this->vertices.size(); ++i)
	{
		Point3f& vertex_point = this->vertices[i];
		vertex_point.x = (vertex_point.x - center_xyz.x) * scale + width / 2;
		vertex_point.y = (vertex_point.y - center_xyz.y) * scale + height / 2;
		vertex_point.z = (vertex_point.z - center_xyz.z) * scale;

	}

	this->center_point = Vertex(width / 2, height / 2, 0);
}