#pragma once
#include <cmath>
#include<iostream>
#include<vector>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <class t> struct Vec2 {
	union {
		struct { t u, v; };
		struct { t x, y; };
		t raw[2];
	};
	Vec2() : u(0), v(0) {}
	Vec2(t _u, t _v) : u(_u), v(_v) {}
	Vec2<t>& operator=(const Vec2<t>& v) {
		if (this != &v) {
			x = v.x;
			y = v.y;
		}
		return *this;
	}
	inline Vec2<t> operator +(const Vec2<t>& V) const { return Vec2<t>(u + V.u, v + V.v); }
	inline Vec2<t> operator -(const Vec2<t>& V) const { return Vec2<t>(u - V.u, v - V.v); }
	inline Vec2<t> operator *(float f)          const { return Vec2<t>(u * f, v * f); }
	inline t&      operator[](int i) { return raw[i]; }
	float norm() const { return std::sqrt(x * x + y * y); }
	Vec2<t> normalize(t l = 1) {
		return (*this) * (l / norm());// i modified here,i let normailzed vector return a copy;
	}
	template <class > friend std::ostream& operator<<(std::ostream& s, Vec2<t>& v);
	template <class > friend Vec2<t> operator *(float f, const Vec2<t>& v1);
};
//add some useful function
template<class t>
inline t dot(const Vec2<t>&v1, const Vec2<t>& v2){
	return v1.x * v2.x + v1.y * v2.y;
}
template<class t>
inline Vec2<t> operator *(float f,const Vec2<t>&v1) { return Vec2<t>(v1.x * f, v1.y * f); }

////////////////////////////




template <class t> struct Vec3 {
	union {
		struct { t x, y, z; };
		struct { t ivert, iuv, inorm; };
		t raw[3];
	};
	Vec3() : x(0), y(0), z(0) {}
	Vec3(t _x, t _y, t _z) : x(_x), y(_y), z(_z) {}
	Vec3(const Vec2<t>&v,t _z) : x(v.x), y(v.y), z(_z) {}
	Vec3<t>& operator=(const Vec3<t>& v) {
		if (&v != this) {
			x = v.x;
			y = v.y;
			z = v.z;
		}
		return *this;
	}
	template<class u>Vec3<t>(const Vec3<u>& v);
	inline Vec3<t> operator ^(const Vec3<t>& v) const { return Vec3<t>(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x); }
	inline Vec3<t> operator +(const Vec3<t>& v) const { return Vec3<t>(x + v.x, y + v.y, z + v.z); }
	inline Vec3<t> operator -(const Vec3<t>& v) const { return Vec3<t>(x - v.x, y - v.y, z - v.z); }
	inline Vec3<t> operator *(float f)          const { return Vec3<t>(x * f, y * f, z * f); }
	inline Vec3<t> operator *(const Vec3<t>& v)          const { return Vec3<t>(x * v.x, y * v.y, z * v.z); }
	inline t&      operator[](int i) { return raw[i]; }
	float norm() const { return std::sqrt(x * x + y * y + z * z); }
	
	template <class > friend std::ostream& operator<<(std::ostream& s, Vec3<t>& v);
	template <class > friend Vec3<t> operator *(float f, const Vec3<t>& v1);
};
template<class t>
Vec3<t> normalize(Vec3<t> v) { return v*(1 / v.norm()); }// i modified here,i let normailzed vector return a copy;

template <class t>
Vec3<t> cross(Vec3<t> v1,Vec3<t> v2){
	return Vec3<t>(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v2.x * v1.y);
}

template<class t>
Vec3<t> operator -(Vec3<t> v) { return -1 * v; }
//add some useful functu=ion
template<class t>
inline t dot(const Vec3<t>& v1, const Vec3<t>& v2) {
	return v1.x * v2.x + v1.y * v2.y+ v1.z * v2.z;
}
template<class t>
inline Vec3<t> operator *(float f, const Vec3<t>& v1) { return Vec3<t>(v1.x * f, v1.y * f, v1.z * f); }
/////////////////////////
typedef Vec2<float> Vec2f;
typedef Vec2<int>   Vec2i;
typedef Vec3<float> Vec3f;
typedef Vec3<int>   Vec3i;

template <> template <> Vec3<int>::Vec3(const Vec3<float>& v);

template <> template <>Vec3<float>::Vec3(const Vec3<int>& v);

template <class t> std::ostream& operator<<(std::ostream& s, Vec2<t>& v) {
	s << "(" << v.x << ", " << v.y << ")\n";
	return s;
}

template <class t> std::ostream& operator<<(std::ostream& s, Vec3<t>& v) {
	s << "(" << v.x << ", " << v.y << ", " << v.z << ")\n";
	return s;
}



const int DEFAULT_ALLOC = 4;

class Matrix {
public:
	std::vector<std::vector<float> > m;
	int rows, cols;

	Matrix(int r = DEFAULT_ALLOC, int c = DEFAULT_ALLOC);
	inline int nrows();
	inline int ncols();

	static Matrix identity(int dimensions);
	std::vector<float>& operator[]( int i);
	
	Matrix transpose();
	Matrix inverse();
	
	friend std::ostream& operator<<(std::ostream& s, Matrix& m);
};
Matrix operator*(const Matrix& a, const Matrix& b);
Matrix operator*(const Matrix& mat, const Vec3f& v);
Matrix mat3x3( Matrix& mat);

