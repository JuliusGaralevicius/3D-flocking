/*
Class:Vector4
Implements:
Author:Rich Davison
Description:VERY simple Vector4 class. Students are encouraged to modify this as necessary!
*/
#pragma once

class Vector4 {
public:
	Vector4(void) {
		x = y = z = w = 1.0f;
	}
	Vector4(float x, float y, float z, float w) {
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}

	~Vector4(void) {}

	float x;
	float y;
	float z;
	float w;

	inline Vector4 normalize() 
	{
		 (*this)/= Lenght();
		 return *this;
	} 
	inline float Lenght()
	{
		return sqrt(x*x + y * y + z * z + w * w);
	}
	inline Vector4  operator*(const float a) const {
		return Vector4(x * a, y * a, z * a, w * a);
	}

	inline Vector4  operator/(const float a) const {
		float r = 1.0f / a;
		return Vector4(x * r, y * r, z * r, w * r);
	}

	inline Vector4  operator+(const Vector4  &a) const {
		return Vector4(x + a.x, y + a.y, z + a.z, w + a.w);
	}

	inline Vector4  operator-(const Vector4  &a) const {
		return Vector4(x - a.x, y - a.y, z - a.z, w - a.w);
	}

	inline void operator+=(const Vector4  &a) {
		x += a.x;
		y += a.y;
		z += a.z;
		w += a.w;
	}

	inline void operator-=(const Vector4  &a) {
		x -= a.x;
		y -= a.y;
		z -= a.z;
		w -= a.w;
	}


	inline void operator*=(float f) {
		x *= f;
		y *= f;
		z *= f;
		w *= f;
	}

	inline void operator/=(float f) {
		float r = 1.0f / f;
		x *= r;
		y *= r;
		z *= r;
		w *= r;
	}

	inline void operator+=(float f) {
		x += f;
		y += f;
		z += f;
		w += f;
	}

	inline void operator-=(float f) {
		x -= f;
		y -= f;
		z -= f;
		w -= f;
	}
};