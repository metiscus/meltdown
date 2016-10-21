#include "vmath.h"
#include <cfloat>

float length2(const float3& value)
{
	return value.x*value.x + value.y*value.y + value.z*value.z;
}

float length(const float3& value)
{
	return sqrtf(value.x*value.x + value.y*value.y + value.z*value.z);
}

void normalize(float3& vector)
{
	float inv_mag = 1.0f / (length(vector) + FLT_EPSILON);
	vector.x *= inv_mag;
	vector.y *= inv_mag;
	vector.z *= inv_mag;
}

float3 direction(const float3& value)
{
	float inv_mag = 1.0f / (length(value) + FLT_EPSILON);
	return float3(value.x * inv_mag, value.y * inv_mag, value.z * inv_mag);
}