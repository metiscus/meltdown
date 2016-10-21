#pragma once

#include <cmath>

struct float3
{
	float3(float x_ = 0.f, float y_ = 0.f, float z_ = 0.f)
		: x(x_), y(y_), z(z_)
	{
		;
	}

	float x;
	float y;
	float z;

	inline void operator=(const float3& rhs) {
		x = rhs.x;
		y = rhs.y;
		z = rhs.z;
	}

	inline float3 operator+(const float3& rhs) const {
		return float3(x+rhs.x, y+rhs.y, z+rhs.z);
	}

	inline float3 operator-(const float3& rhs) const {
		return float3(x-rhs.x, y-rhs.y, z-rhs.z);
	}

	inline float3 operator*(const float& f) const {
		return float3(x*f, y*f, z*f);
	}
};

float length2(const float3& value);
float length(const float3& value);
void normalize(float3& vector);
float3 direction(const float3& value);

template <typename Real>
Real clamp(const Real& value, const Real& low, const Real& high)
{
	if(value > high) return high;
	if(value < low) return low;
	return value;
}