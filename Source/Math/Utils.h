#pragma once

#include "Math/Constants.h"
#include "Math/Objects/Vector3.h"

namespace Silent::Math
{
    // Numeric

    float FloorToStep(float x, float step);
    float CeilToStep(float x, float step);
    float RoundToStep(float x, float step);
    float Remap(float x, float min0, float max0, float min1, float max1);
    bool  IsApproxEqual(float a, float b, float epsilon = EPSILON);

    // Geometric

	short   GetSurfaceSlopeAngle(const Vector3& normal, const Vector3& axis = Vector3::UnitY);
	short   GetSurfaceAspectAngle(const Vector3& normal, const Vector3& axis = Vector3::UnitY);
	float	GetDistanceToLine(const Vector3& from, const Vector3& lineStart, const Vector3& lineEnd);
	Vector3 GetClosestPointOnLine(const Vector3& from, const Vector3& lineStart, const Vector3& lineEnd);
	Vector3 GetClosestPointOnLinePerp(const Vector3& from, const Vector3& lineStart, const Vector3& lineEnd, const Vector3& axis = Vector3::UnitY);

	bool IsPointInFront(const Vector3& from, const Vector3& to, const Vector3& ref, const Vector3& axis = Vector3::UnitY);
	bool IsPointOnLeft(const Vector3& from, const Vector3& to, const Vector3& ref, const Vector3& axis = Vector3::UnitY);
}
