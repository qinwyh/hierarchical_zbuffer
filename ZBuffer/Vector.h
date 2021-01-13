#ifndef __VECTOR_H
#define __VECTOR_H

#include <cmath>

#define EPS 1e-6

inline bool is_equal_f(float a, float b) {
	if (fabs(a - b) < EPS) {
		return true;
	}
	return false;
}

class Vector3f {
	public:
		float x, y, z;
		float& r = x;
		float& g = y;
		float& b = z;

		Vector3f(float x = 0.0f, float y = 0.0f, float z = 0.0f) {
			this->x = x;
			this->y = y;
			this->z = z;
		};

		inline Vector3f& operator = (const Vector3f& v) { x = v.x; y = v.y; z = v.z; return *this; }

		inline Vector3f& operator += (float num) { x += num; y += num; z += num; return *this; }
		inline Vector3f& operator += (const Vector3f& v) { x += v.x; y += v.y; z += v.z; return *this; }

		inline Vector3f& operator -= (float num) { x -= num; y -= num; z -= num; return *this; }
		inline Vector3f& operator -= (const Vector3f& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }

		inline Vector3f& operator *= (float num) { x *= num; y *= num; z *= num; return *this; }
		inline Vector3f& operator *= (const Vector3f& v) { x *= v.x; y *= v.y; z *= v.z; return *this; }

		inline Vector3f& operator /= (float num) { x /= num; y /= num; z /= num; return *this; }
		inline Vector3f& operator /= (const Vector3f& v) { x /= v.x; y /= v.y; z /= v.z; return *this; }


		friend inline Vector3f operator + (const Vector3f& u, float num) { return Vector3f(u.x + num, u.y + num, u.z + num); }
		friend inline Vector3f operator + (float num, const Vector3f& u) { return Vector3f(num + u.x, num + u.y, num + u.z); }
		friend inline Vector3f operator + (const Vector3f& u, const Vector3f& v) { return Vector3f(u.x + v.x, u.y + v.y, u.z + v.z); }
		friend inline Vector3f operator - (const Vector3f& u, float num) { return Vector3f(u.x - num, u.y - num, u.z - num); }
		friend inline Vector3f operator - (float num, const Vector3f& u) { return Vector3f(num - u.x, num - u.y, num - u.z); }
		friend inline Vector3f operator - (const Vector3f& u, const Vector3f& v) { return Vector3f(u.x - v.x, u.y - v.y, u.z - v.z); }
		friend inline Vector3f operator * (const Vector3f& u, float num) { return Vector3f(u.x * num, u.y * num, u.z * num); }
		friend inline Vector3f operator * (float num, const Vector3f& u) { return Vector3f(num * u.x, num * u.y, num * u.z); }
		friend inline Vector3f operator * (const Vector3f& u, const Vector3f& v) { return Vector3f(u.x * v.x, u.y * v.y, u.z * v.z); }
		friend inline Vector3f operator / (const Vector3f& u, float num) { return Vector3f(u.x / num, u.y / num, u.z / num); }
		friend inline Vector3f operator / (float num, const Vector3f& u) { return Vector3f(num / u.x, num / u.y, num / u.z); }
		friend inline Vector3f operator / (const Vector3f& u, const Vector3f& v) { return Vector3f(u.x / v.x, u.y / v.y, u.z / v.z); }

		inline bool operator == (const Vector3f& u) { return is_equal_f(x, u.x) && is_equal_f(y, u.y) && is_equal_f(z, u.z); }
		inline bool operator != (const Vector3f& u) { return !(is_equal_f(x, u.x) && is_equal_f(y, u.y) && is_equal_f(z, u.z)); }

};

inline Vector3f cross(const Vector3f& u, const Vector3f& v) {
	return Vector3f(u.y * v.z - u.z * v.y, u.z * v.x - u.x * v.z, u.x * v.y - u.y * v.x);
}

inline float dot(const Vector3f& u, const Vector3f& v) {
	return u.x * v.x + u.y * v.y + u.z * v.z;
}

inline float length(const Vector3f& u) {
	return sqrt(u.x * u.x + u.y * u.y + u.z * u.z);
}

inline Vector3f normalize(const Vector3f& u) {
	return u * (1.0f / sqrt(u.x * u.x + u.y * u.y + u.z * u.z));
}

// For Shader
typedef Vector3f Color3f;
typedef Vector3f Point3f;

#endif // !__VECTOR_H

