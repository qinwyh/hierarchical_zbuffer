#ifndef _MODEL_H
#define _MODEL_H

#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include <ctime>
#include <algorithm>
#include <omp.h>
#include "Vector.h"

using namespace std;

// For Vertex Coordinates
typedef Vector3f Vertex;

class Face {
	public:
		// 3 Vertices of 1 Face 
		std::vector<int> vertex_id_list;
		// 3 Vertices' Normal Vectors
		std::vector<int> normal_id_list;
		// Normal Vector of this Face
		Vector3f normal_vector;

		Face() {};
};


class Model {
	public:
		// All Vertices
		std::vector<Vertex> vertices;
		// All Vertex Normals
		std::vector<Vertex> vertex_normals;
		// All Faces
		std::vector<Face> faces;
		// Center Point of this Model
		Point3f center_point;

		Model(const std::string& file_path);
		bool load(const std::string& file_path);
		void rasterize();
};


#endif
