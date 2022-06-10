#pragma once
#include <windows.h>
#include <math.h>

struct Vector
{
	Vector();
	Vector(float x, float y, float z);

	Vector&	operator *=(const float);

	float&	operator [](int index);

	Vector	operator+(const Vector& v) const;
	Vector	operator-(const Vector& v) const;
	Vector	operator*(float fl) const;

	float	DistTo(const Vector&);
	bool	IsZero(float tolerance = FLT_EPSILON);
	void    Zero();
	float	Length2D();
	float	length_sqr();
	float	Length();
	float	Normalize();
	float	NormalizeInPlace();
	float	Dot(const Vector&);

	float	x;
	float	y;
	float	z;


};



inline float Vector::length_sqr()
{
	return x * x + y * y + z * z;
}

inline float Vector::DistTo(const Vector &in)
{
	Vector vecDelta = *this - in;
	return vecDelta.Length();
}

inline bool Vector::IsZero(float tolerance)
{
	return (x > -tolerance && x < tolerance && y > -tolerance && y < tolerance && z > -tolerance && z < tolerance);
}

inline Vector& Vector::operator *= (const float in)
{
	x *= in;
	y *= in;
	z *= in;

	return *this;
}

inline float& Vector::operator [] (int index)
{
	return ((&x)[index]);
}

inline Vector Vector::operator+(const Vector& v) const
{
	Vector res;
	res.x = x + v.x;
	res.y = y + v.y;
	res.z = z + v.z;
	return res;
}

inline Vector Vector::operator-(const Vector& v) const
{
	Vector res;
	res.x = x - v.x;
	res.y = y - v.y;
	res.z = z - v.z;
	return res;
}

inline Vector Vector::operator*(float fl) const
{
	Vector res;
	res.x = x * fl;
	res.y = y * fl;
	res.z = z * fl;
	return res;
}

inline Vector::Vector()
{
	x = y = z = 0.f;
}

inline void Vector::Zero()
{
	x = y = z = 0.f;
}

inline Vector::Vector(float _x, float _y, float _z)
{
	x = _x;
	y = _y;
	z = _z;
}

inline float Vector::Length2D()
{
	float root = 0.f;
	float sqst = x * x + y * y;

	__asm
	{
		sqrtss xmm0, sqst
		movss root, xmm0
	}

	return root;
	//return sqrt(sqst);
}

inline float Vector::Dot(const Vector &in)
{
	return (x * in.x + y * in.y + z * in.z);
}

inline float Vector::Length()
{
	float root = 0.f;
	float sqsr = x * x + y * y + z * z;

	__asm
	{
		sqrtss xmm0, sqsr
		movss root, xmm0
	}

	return root;
}

inline float Vector::Normalize()
{
	float flLength = Length();
	float flLengthNormal = 1.f / (FLT_EPSILON + flLength);

	x = x * flLengthNormal;
	y = y * flLengthNormal;
	z = z * flLengthNormal;

	return flLength;
}

inline float Vector::NormalizeInPlace()
{
	return Normalize();
}

class VectorAligned : public Vector
{
public:
	VectorAligned()
	{
		x = y = z = 0;
	}

	VectorAligned(const Vector &in)
	{
		x = in.x;
		y = in.y;
		z = in.z;
	}

	float w;
};