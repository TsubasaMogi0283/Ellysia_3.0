#include "VectorCalculation.h"

#include <corecrt_math.h>
#include <Matrix4x4.h>
#include <cassert>

Vector3 Add(Vector3 v1, Vector3 v2) {
	Vector3 result = {};
	result.x = v1.x + v2.x;
	result.y = v1.y + v2.y;
	result.z = v1.z + v2.z;

	return result;
}

//引き算
Vector3 Subtract(Vector3 v1, Vector3 v2) {
	Vector3 result = {};
	result.x = v1.x - v2.x;
	result.y = v1.y - v2.y;
	result.z = v1.z - v2.z;
	return result;
}

float Clamp(float t, float min, float max) {
	if (t < min) {
		return min;
	}
	else if (t > max) {
		return max;
	}

	return t;


}

float DotVector3(Vector3 v1, Vector3 v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

float DotVector2(Vector3 v1, Vector3 v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

float Length(Vector3 V1) {
	return sqrtf(V1.x * V1.x + V1.y * V1.y + V1.z * V1.z);
}

Vector3 Normalize(Vector3 V1) {
	Vector3 result = {};

	float length = sqrtf(V1.x * V1.x + V1.y * V1.y + V1.z * V1.z);

	float newX = V1.x;
	float newY = V1.y;
	float newZ = V1.z;


	if (length != 0.0f) {
		newX = V1.x / length;
		newY = V1.y / length;
		newZ = V1.z / length;

	}

	result.x = newX;
	result.y = newY;
	result.z = newZ;


	return result;
}


Vector3 Cross(const Vector3 v1, const Vector3 v2) {
	Vector3 result;
	result.x = v1.y * v2.z - v1.z * v2.y;
	result.y = v1.z * v2.x - v1.x * v2.z;
	result.z = v1.x * v2.y - v1.y * v2.x;

	return result;
}




Vector3 Project(const Vector3 a, const Vector3 b) {

	//Aベクトルを正射影ベクトルにする
	Vector3 Vector3C = {};

	//bの長さを求める
	float lengthB = Length(b);
	float dotAB = DotVector3(a, b);

	//||c||=||a||cosθ
	//     ↓
	// a・b=||a|| ||b||cosθより
	//     ↓
	//||c||=a・b/||b||になる

	//正射影ベクトルの長さ
	float t = dotAB / (lengthB * lengthB);
	float newT = Clamp(t, 0.0f, 1.0f);
	Vector3C.x = newT * b.x;
	Vector3C.y = newT * b.y;
	Vector3C.z = newT * b.z;


	return Vector3C;
}



//Transform
Vector3 TransformCalculation(const Vector3 vector, const Matrix4x4 matrix) {
	Vector3 result = {};

	result.x = (vector.x * matrix.m[0][0]) + (vector.y * matrix.m[1][0]) + (vector.z * matrix.m[2][0]) + (1.0f * matrix.m[3][0]);
	result.y = (vector.x * matrix.m[0][1]) + (vector.y * matrix.m[1][1]) + (vector.z * matrix.m[2][1]) + (1.0f * matrix.m[3][1]);
	result.z = (vector.x * matrix.m[0][2]) + (vector.y * matrix.m[1][2]) + (vector.z * matrix.m[2][2]) + (1.0f * matrix.m[3][2]);

	float w = (vector.x * matrix.m[0][3]) + (vector.y * matrix.m[1][3]) + (vector.z * matrix.m[2][3]) + (1.0f * matrix.m[3][3]);


	assert(w != 0.0f);
	result.x /= w;
	result.y /= w;
	result.z /= w;

	return result;


}