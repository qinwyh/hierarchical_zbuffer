#ifndef __SCANLINEZBUFFER_H
#define __SCANLINEZBUFFER_H

#include <vector>
#include <cmath>
#include <ctime>
#include <algorithm>
#include "Model.h"


class Edge {
	public:
		// Upper X Coordinate of the Edge
		float x;
		// X Coordinate Difference
		float dx;
		// Scan Line Numbers (Vertical)
		int dy;
		// Face id
		int id;

		Edge() {
			this->x = 0.0f;
			this->dx = 0.0f;
			this->dy = -1;
			this->id = -1;
		};

		inline void set_x(float x = 0.0f) { this->x = x; }

		inline void set_dx(float dx = 0.0f) { this->dx = dx; }

		inline void set_dy(int dy = -1) { this->dy = dy; }

		inline void set_id(int id = -1) { this->id = id; }
};


class Polygon {
public:
	// Coefficients of Plane Equation
	float a, b, c, d;
	// Scan Line Numbers (Vertical)
	int dy;
	// Face id
	int id;

	Polygon() {
		this->a = 0.0f;
		this->b = 0.0f;
		this->c = 0.0f;
		this->d = 0.0f;
		this->dy = -1;
		this->id = -1;
	};

	inline void set_coef(float a, float b = 0.0f, float c = 0.0f, float d = 0.0f) {
		this->a = a;
		this->b = b;
		this->c = c;
		this->d = d;
	}

	inline void set_dy(int dy = -1) { this->dy = dy; }

	inline void set_id(int id = -1) { this->id = id; }
};


typedef Polygon ActivePolygon;


class ActiveEdge {
public:
	// Left or Right Intersection Point
	float x_left, x_right;
	// X Coordinate Difference of Adjacent Scan Lines
	float dx_left, dx_right;
	// Remained Scan Line Numbers (Vertical)
	int dy_left, dy_right;
	// Depth of Left Intersection Point
	float z_left;
	// Depth Increment in X Axis
	float dz_x;
	// Depth Increment in Y Axis
	float dz_y;
	// Face id
	int id;
	// Corresponding Active Polygon
	ActivePolygon active_polygon;

	ActiveEdge() {
		this->x_left = 0.0f;
		this->x_right = 0.0f;
		this->dx_left = 0.0f;
		this->dx_right = 0.0f;
		this->dy_left = -1;
		this->dy_right = -1;
		this->z_left = 0.0f;
		this->dz_x = 0.0f;
		this->dz_y = 0.0f;
		this->id = -1;
	}

	inline void set_x(float x_left = 0.0f, float x_right = 0.0f) {
		this->x_left = x_left;
		this->x_right = x_right;
	}

	inline void set_dx(float dx_left = 0.0f, float dx_right = 0.0f) {
		this->dx_left = dx_left;
		this->dx_right = dx_right;
	}

	inline void set_dy(int dy_left = -1, int dy_right = -1) {
		this->dy_left = dy_left;
		this->dy_right = dy_right;
	}

	inline void set_z(float z_left = 0.0f) { this->z_left = z_left; }

	inline void set_dz_x(float dz_x = 0.0f) { this->dz_x = dz_x; }

	inline void set_dz_y(float dz_y = 0.0f) { this->dz_y = dz_y; }

	inline void set_id(int id = -1) { this->id = id; }
};


class ScanLineZBuffer {
	public:
		ScanLineZBuffer();
		~ScanLineZBuffer();

		void construct(const Model& model);
		void perform(const Model& model);
		float get_zbuffer_min();
		float get_zbuffer_max();
		float get_zbuffer(int y, int x);
		ActiveEdge add_active_edge(const Polygon& polygon, const Model& model, int y);

		int width, height;
		int** id_buffer;
		float* zbuffer;
		float** complete_zbuffer;

	private:
		std::vector<std::vector<Polygon>> polygon_table;
		std::vector<std::vector<Edge>> edge_table;
		std::vector<Polygon> active_polygon_table;
		std::vector<ActiveEdge> active_edge_table;
};


#endif // !__SCANLINEZBUFFER_H

