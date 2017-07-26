///////////////////////////////////////////////////////////////////////////////
///
/// \file Matrix4.cpp
/// Implementation of the Matrix 4 structure.
/// 
/// Authors: Joshua Davis, Benjamin Strukus
/// Copyright 2010-2012, DigiPen Institute of Technology
///
///////////////////////////////////////////////////////////////////////////////
#include "Precompiled.h"
#include "math/Matrix4.h"
#include "math/MathFunctions.h"

namespace Math
{
	const Matrix4 Matrix4::cIdentity(1.0f, 0.0f, 0.0f, 0.0f,
	  0.0f, 1.0f, 0.0f, 0.0f,
	  0.0f, 0.0f, 1.0f, 0.0f,
	  0.0f, 0.0f, 0.0f, 1.0f);

	Matrix4::Matrix4(ConstRealPointer data_)
	{
		m00 = data_[0];  m01 = data_[1];  m02 = data_[2];  m03 = data_[3];
		m10 = data_[4];  m11 = data_[5];  m12 = data_[6];  m13 = data_[7];
		m20 = data_[8];  m21 = data_[9];  m22 = data_[10];  m23 = data_[11];
		m30 = data_[12];  m31 = data_[13];  m32 = data_[14];  m33 = data_[15];
	}

	Matrix4::Matrix4(float p00, float p01, float p02, float p03, float p10, float p11,
	  float p12, float p13, float p20, float p21, float p22, float p23,
	  float p30, float p31, float p32, float p33)
	{
		m00 = p00;  m01 = p01;  m02 = p02;  m03 = p03;
		m10 = p10;  m11 = p11;  m12 = p12;  m13 = p13;
		m20 = p20;  m21 = p21;  m22 = p22;  m23 = p23;
		m30 = p30;  m31 = p31;  m32 = p32;  m33 = p33;
	}

	Matrix4::Matrix4(Vec4Param basisX, Vec4Param basisY,
	  Vec4Param basisZ, Vec4Param basisW)
	{
		SetBasis(0, basisX);
		SetBasis(1, basisY);
		SetBasis(2, basisZ);
		SetBasis(3, basisW);
	}

	float* Matrix4::ToFloats()
	{
		return (float*)this;
	}

	////////// Binary Assignment Operators (reals) /////////////////////////////////

	void Matrix4::operator*=(float rhs)
	{
		Matrix4& self = *this;
		self[0] *= rhs;
		self[1] *= rhs;
		self[2] *= rhs;
		self[3] *= rhs;
	}

	void Matrix4::operator/=(float rhs)
	{
		Matrix4& self = *this;
		//ErrorIf(Math::IsZero(rhs), "Matrix4 - Division by zero.");
		self[0] /= rhs;
		self[1] /= rhs;
		self[2] /= rhs;
		self[3] /= rhs;
	}

	////////// Binary Operators (reals) ////////////////////////////////////////////

	Matrix4 Matrix4::operator*(float rhs) const
	{
		Matrix4 ret = *this;
		ret *= rhs;
		return ret;
	}

	Matrix4 Matrix4::operator/(float rhs) const
	{
		//ErrorIf(Math::IsZero(rhs), "Matrix4 - Division by zero.");
		Matrix4 ret = *this;
		ret /= rhs;
		return ret;
	}

	////////// Binary Assignment Operator (Matrices) ///////////////////////////////

	void Matrix4::operator+=(Mat4Param rhs)
	{
		Matrix4& self = *this;
		self[0] += rhs[0];
		self[1] += rhs[1];
		self[2] += rhs[2];
		self[3] += rhs[3];
	}

	void Matrix4::operator-=(Mat4Param rhs)
	{
		Matrix4& self = *this;
		self[0] -= rhs[0];
		self[1] -= rhs[1];
		self[2] -= rhs[2];
		self[3] -= rhs[3];
	}

	////////// Binary Operators (Matrices) /////////////////////////////////////////

	Matrix4 Matrix4::operator+(Mat4Param rhs) const
	{
		Matrix4 ret = *this;
		ret += rhs;
		return ret;
	}

	Matrix4 Matrix4::operator-(Mat4Param rhs) const
	{
		Matrix4 ret = *this;
		ret -= rhs;
		return ret;
	}

	Matrix4 Matrix4::operator*(Mat4Param rhs) const
	{
		return Concat(rhs);
	}

	////////// Binary Comparisons //////////////////////////////////////////////////

	bool Matrix4::operator==(Mat4Param rhs) const
	{
		const Matrix4& self = *this;
		return self[0] == rhs[0] &&
		  self[1] == rhs[1] &&
		  self[2] == rhs[2] &&
		  self[3] == rhs[3];
	}

	bool Matrix4::operator!=(Mat4Param rhs) const
	{
		return !(*this == rhs);
	}

	float Matrix4::operator()(unsigned r, unsigned c) const
	{
		//ErrorIf(r > 3, "Matrix4 - Index out of range.");
		//ErrorIf(c > 3, "Matrix4 - Index out of range.");

#ifdef ColumnBasis
		return array[c + r * 4];
#else
		return array[r + c * 4];
#endif
	}

	float& Matrix4::operator()(unsigned r, unsigned c)
	{
		//ErrorIf(r > 3, "Matrix4 - Index out of range.");
		//ErrorIf(c > 3, "Matrix4 - Index out of range.");

#ifdef ColumnBasis
		return array[c + r * 4];
#else
		return array[r + c * 4];
#endif
	}


	Matrix4 Matrix4::Transposed() const
	{
		Matrix4 ret = *this;
		ret.Transpose();
		return ret;
	}

	Mat4Ref Matrix4::Transpose()
	{
		Math::Swap(m01, m10);
		Math::Swap(m02, m20);
		Math::Swap(m03, m30);
		Math::Swap(m12, m21);
		Math::Swap(m13, m31);
		Math::Swap(m23, m32);
		return *this;
	}

	Matrix4 Matrix4::Inverted() const
	{
		Matrix4 inverted;
		float determinant = Determinant();
		//ErrorIf(Math::IsZero(determinant), "Matrix4 - Uninvertible matrix.");
		determinant = 1.0f / determinant;
		inverted.m00 = m12 * m23 * m31 - m13 * m22 * m31;
		inverted.m00 += m13 * m21 * m32 - m11 * m23 * m32;
		inverted.m00 += m11 * m22 * m33 - m12 * m21 * m33;
		inverted.m00 *= determinant;

		inverted.m01 = m03 * m22 * m31 - m02 * m23 * m31;
		inverted.m01 += m01 * m23 * m32 - m03 * m21 * m32;
		inverted.m01 += m02 * m21 * m33 - m01 * m22 * m33;
		inverted.m01 *= determinant;

		inverted.m02 = m02 * m13 * m31 - m03 * m12 * m31;
		inverted.m02 += m03 * m11 * m32 - m01 * m13 * m32;
		inverted.m02 += m01 * m12 * m33 - m02 * m11 * m33;
		inverted.m02 *= determinant;

		inverted.m03 = m03 * m12 * m21 - m02 * m13 * m21;
		inverted.m03 += m01 * m13 * m22 - m03 * m11 * m22;
		inverted.m03 += m02 * m11 * m23 - m01 * m12 * m23;
		inverted.m03 *= determinant;

		inverted.m10 = m13 * m22 * m30 - m12 * m23 * m30;
		inverted.m10 += m10 * m23 * m32 - m13 * m20 * m32;
		inverted.m10 += m12 * m20 * m33 - m10 * m22 * m33;
		inverted.m10 *= determinant;

		inverted.m11 = m02 * m23 * m30 - m03 * m22 * m30;
		inverted.m11 += m03 * m20 * m32 - m00 * m23 * m32;
		inverted.m11 += m00 * m22 * m33 - m02 * m20 * m33;
		inverted.m11 *= determinant;

		inverted.m12 = m03 * m12 * m30 - m02 * m13 * m30;
		inverted.m12 += m00 * m13 * m32 - m03 * m10 * m32;
		inverted.m12 += m02 * m10 * m33 - m00 * m12 * m33;
		inverted.m12 *= determinant;

		inverted.m13 = m02 * m13 * m20 - m03 * m12 * m20;
		inverted.m13 += m03 * m10 * m22 - m00 * m13 * m22;
		inverted.m13 += m00 * m12 * m23 - m02 * m10 * m23;
		inverted.m13 *= determinant;

		inverted.m20 = m11 * m23 * m30 - m13 * m21 * m30;
		inverted.m20 += m13 * m20 * m31 - m10 * m23 * m31;
		inverted.m20 += m10 * m21 * m33 - m11 * m20 * m33;
		inverted.m20 *= determinant;

		inverted.m21 = m03 * m21 * m30 - m01 * m23 * m30;
		inverted.m21 += m00 * m23 * m31 - m03 * m20 * m31;
		inverted.m21 += m01 * m20 * m33 - m00 * m21 * m33;
		inverted.m21 *= determinant;

		inverted.m22 = m01 * m13 * m30 - m03 * m11 * m30;
		inverted.m22 += m03 * m10 * m31 - m00 * m13 * m31;
		inverted.m22 += m00 * m11 * m33 - m01 * m10 * m33;
		inverted.m22 *= determinant;

		inverted.m23 = m03 * m11 * m20 - m01 * m13 * m20;
		inverted.m23 += m00 * m13 * m21 - m03 * m10 * m21;
		inverted.m23 += m01 * m10 * m23 - m00 * m11 * m23;
		inverted.m23 *= determinant;

		inverted.m30 = m12 * m21 * m30 - m11 * m22 * m30;
		inverted.m30 += m10 * m22 * m31 - m12 * m20 * m31;
		inverted.m30 += m11 * m20 * m32 - m10 * m21 * m32;
		inverted.m30 *= determinant;

		inverted.m31 = m01 * m22 * m30 - m02 * m21 * m30;
		inverted.m31 += m02 * m20 * m31 - m00 * m22 * m31;
		inverted.m31 += m00 * m21 * m32 - m01 * m20 * m32;
		inverted.m31 *= determinant;

		inverted.m32 = m02 * m11 * m30 - m01 * m12 * m30;
		inverted.m32 += m00 * m12 * m31 - m02 * m10 * m31;
		inverted.m32 += m01 * m10 * m32 - m00 * m11 * m32;
		inverted.m32 *= determinant;

		inverted.m33 = m01 * m12 * m20 - m02 * m11 * m20;
		inverted.m33 += m02 * m10 * m21 - m00 * m12 * m21;
		inverted.m33 += m00 * m11 * m22 - m01 * m10 * m22;
		inverted.m33 *= determinant;

		return inverted;
	}

	Mat4Ref Matrix4::Invert()
	{
		*this = Inverted();
		return *this;
	}

	Matrix4 Matrix4::Concat(Mat4Param rhs) const
	{
		Matrix4 ret;

		ret.ZeroOut();

		for (int r = 0; r < 4; ++r)
		{
			for (int c = 0; c < 4; ++c)
			{
				for (int i = 0; i < 4; ++i)
				{
					ret.m[r][c] += m[r][i] * rhs.m[i][c];
				}
			}
		}

		return ret;
	}

	Mat4Ref Matrix4::SetIdentity()
	{
		Matrix4& self = *this;
		self[0].Set(1.0f, 0.0f, 0.0f, 0.0f);
		self[1].Set(0.0f, 1.0f, 0.0f, 0.0f);
		self[2].Set(0.0f, 0.0f, 1.0f, 0.0f);
		self[3].Set(0.0f, 0.0f, 0.0f, 1.0f);
		return *this;
	}

	Mat4Ref Matrix4::ZeroOut()
	{
		Matrix4& self = *this;
		self[0].ZeroOut();
		self[1].ZeroOut();
		self[2].ZeroOut();
		self[3].ZeroOut();
		return *this;
	}

	float Matrix4::Determinant() const
	{
		float det = m03 * m12 * m21 * m30 - m02 * m13 * m21 * m30;
		det += m01 * m13 * m22 * m30 - m03 * m11 * m22 * m30;
		det += m02 * m11 * m23 * m30 - m01 * m12 * m23 * m30;
		det += m02 * m13 * m20 * m31 - m03 * m12 * m20 * m31;
		det += m03 * m10 * m22 * m31 - m00 * m13 * m22 * m31;
		det += m00 * m12 * m23 * m31 - m02 * m10 * m23 * m31;
		det += m03 * m11 * m20 * m32 - m01 * m13 * m20 * m32;
		det += m00 * m13 * m21 * m32 - m03 * m10 * m21 * m32;
		det += m01 * m10 * m23 * m32 - m00 * m11 * m23 * m32;
		det += m01 * m12 * m20 * m33 - m02 * m11 * m20 * m33;
		det += m02 * m10 * m21 * m33 - m00 * m12 * m21 * m33;
		det += m00 * m11 * m22 * m33 - m01 * m10 * m22 * m33;
		return det;
	}

	bool Matrix4::Valid() const
	{
		const Matrix4& self = *this;
		return self[0].Valid() && self[1].Valid() &&
		  self[2].Valid() && self[3].Valid();
	}

	Matrix4 Matrix4::Scale(float x, float y, float z)
	{
		// set to scalar matrix
		// TODO: implement by student if desired
		Matrix4 res;
		res.ZeroOut();
		res.m00 = x;
		res.m11 = y;
		res.m22 = z;
		res.m33 = 1.f;
		return res;
	}

	Matrix4 Matrix4::Scale(Vec3Param axis)
	{
		return Scale(axis.x, axis.y, axis.z);
	}

	void Matrix4::Rotate(float x, float y, float z, float radians)
	{
		// set to rotation matrix given axis-angle pair
		// TODO: arbitrary axis rotation; implement by student if desired
	}

	void Matrix4::Rotate(Vec3Param axis, float radians)
	{
		Rotate(axis.x, axis.y, axis.z, radians);
	}

	Matrix4 Matrix4::Translate(float x, float y, float z)
	{
		// set to translation matrix
		// TODO: implement by student if desired
		Matrix4 res = cIdentity;
		res.m03 = x;
		res.m13 = y;
		res.m23 = z;
		return res;
	}

	Matrix4 Matrix4::Translate(Vec3Param axis)
	{
		return Translate(axis.x, axis.y, axis.z);
	}

	void Matrix4::BuildTransform(Vec3Param translate, QuatParam rotate,
	  Vec3Param scale)
	{
		// not necessary to implement for any CS300 assignment; use this if you
		// would prefer to use quaternions instead of Euler angles for rotations,
		// which is acceptable in CS300
	}

	void Matrix4::BuildTransform(Vec3Param translate, Mat3Param rotate,
	  Vec3Param scale)
	{
		// TODO(student): implement the assembly of the general transformation
		// matrix, taking into consideration translate coordinates, a rotation
		// matrix, and scale coordinates (scale does not need to be implemented
		// correctly and will not be tested, but it's assumed you can implement
		// this)
	}

	void Matrix4::Decompose(Vec3Ptr scale, Mat3Ptr rotate, Vec3Ptr translate) const
	{
		Vector3 shear;
		Decompose(scale, &shear, rotate, translate);
	}

	//Shear values that are calculated are XY, XZ, and YZ. They are stored as the 
	//element their name does not contain, so shear->x would have YZ in it
	void Matrix4::Decompose(Vec3Ptr scale, Vec3Ptr shear, Mat3Ptr rotate,
	  Vec3Ptr translate) const
	{
		//ErrorIf(scale == NULL, "Matrix4 - Null pointer passed for scale.");
		//ErrorIf(shear == NULL, "Matrix4 - Null pointer passed for shear.");
		//ErrorIf(rotate == NULL, "Matrix4 - Null pointer passed for rotation.");
		//ErrorIf(translate == NULL, "Matrix4 - Null pointer passed for translation.");

		//Translation is the last basis vector
		translate->x = m03;
		translate->y = m13;
		translate->z = m23;

		//X' == first basis vector
		//Y' == second basis vector
		//Z' == third basis vector

		//       X'                  Y'                  Z'
		rotate->m00 = m00;  rotate->m01 = m01;  rotate->m02 = m02;
		rotate->m10 = m10;  rotate->m11 = m11;  rotate->m12 = m12;
		rotate->m20 = m20;  rotate->m21 = m21;  rotate->m22 = m22;

		//ScaleX is the magnitude of X'
		scale->x = Math::Sqrt(Math::Sq(m00) + Math::Sq(m10) + Math::Sq(m20));

		//X' is normalized
		rotate->m00 /= scale->x;
		rotate->m10 /= scale->x;
		rotate->m20 /= scale->x;

		//ShearXY is the dot product of X' and Y'
		shear->z = rotate->m00 * rotate->m01 +
		  rotate->m10 * rotate->m11 +
		  rotate->m20 * rotate->m21;

		//Make Y' orthogonal to X' by " Y' = Y' - (ShearXY * X') "
		rotate->m01 -= shear->z * rotate->m00;
		rotate->m11 -= shear->z * rotate->m10;
		rotate->m21 -= shear->z * rotate->m20;

		//ScaleY is the magnitude of the modified Y'
		scale->y = Math::Sqrt(Math::Sq(m01) + Math::Sq(m11) + Math::Sq(m21));

		//Y' is normalized
		rotate->m01 /= scale->y;
		rotate->m11 /= scale->y;
		rotate->m21 /= scale->y;

		//ShearXY is divided by ScaleY to get it's final value
		shear->z /= scale->y;

		//ShearXZ is the dot product of X' and Z'
		shear->y = rotate->m00 * rotate->m02 +
		  rotate->m10 * rotate->m12 +
		  rotate->m20 * rotate->m22;

		//ShearYZ is the dot product of Y' and Z'
		shear->x = rotate->m01 * rotate->m02 +
		  rotate->m11 * rotate->m12 +
		  rotate->m21 * rotate->m22;

		//Make Z' orthogonal to X' by " Z' = Z' - (ShearXZ * X') "
		rotate->m02 -= shear->y * rotate->m00;
		rotate->m12 -= shear->y * rotate->m10;
		rotate->m22 -= shear->y * rotate->m20;

		//Make Z' orthogonal to Y' by " Z' = Z' - (ShearYZ * Y') "
		rotate->m02 -= shear->x * rotate->m01;
		rotate->m12 -= shear->x * rotate->m11;
		rotate->m22 -= shear->x * rotate->m21;

		//ScaleZ is the magnitude of the modified Z'
		scale->z = Math::Sqrt(Math::Sq(m02) + Math::Sq(m12) + Math::Sq(m22));

		//Z' is normalized
		rotate->m02 /= scale->z;
		rotate->m12 /= scale->z;
		rotate->m22 /= scale->z;

		//ShearXZ is divided by ScaleZ to get it's final value
		shear->y /= scale->z;

		//ShearYZ is divided by ScaleZ to get it's final value
		shear->x /= scale->z;

		//If the determinant is negative, then the rotation and scale contain a flip
		Vector3 v = Vector3(rotate->m11 * rotate->m22 - rotate->m21 * rotate->m12,
		  rotate->m21 * rotate->m02 - rotate->m01 * rotate->m22,
		  rotate->m01 * rotate->m12 - rotate->m11 * rotate->m02);
		float dot = v.x * rotate->m00 + v.y * rotate->m10 + v.z * rotate->m20;
		if (dot < 0.0f)
		{
			(*rotate) *= -1.0f;
			Negate(scale);
		}
	}

	Matrix4::BasisVector Matrix4::Basis(unsigned index) const
	{
		const Matrix4& self = *this;
		//ErrorIf(index > 3, "Matrix4 - Index out of range.");
#ifdef ColumnBasis
		return Vector4(array[index], array[4 + index],
		  array[8 + index], array[12 + index]);
#else
		return self[index];
#endif
	}

	Matrix4::BasisVector Matrix4::BasisX() const
	{
		const Matrix4& self = *this;
#ifdef ColumnBasis
		return Vector4(array[0], array[4], array[8], array[12]);
#else
		return self[0];
#endif
	}

	Matrix4::BasisVector Matrix4::BasisY() const
	{
		const Matrix4& self = *this;
#ifdef ColumnBasis
		return Vector4(array[1], array[5], array[9], array[13]);
#else
		return self[1];
#endif
	}

	Matrix4::BasisVector Matrix4::BasisZ() const
	{
		const Matrix4& self = *this;
#ifdef ColumnBasis
		return Vector4(array[2], array[6], array[10], array[14]);
#else
		return self[2];
#endif
	}

	Matrix4::BasisVector Matrix4::BasisW() const
	{
		const Matrix4& self = *this;
#ifdef ColumnBasis
		return Vector4(array[3], array[7], array[11], array[15]);
#else
		return self[3];
#endif
	}

	Matrix4::CrossVector Matrix4::Cross(unsigned index) const
	{
		const Matrix4& self = *this;

		//ErrorIf(index > 3, "Matrix4 - Index out of range.");
#ifdef ColumnBasis
		return self[index];
#else
		return Vector4(array[index],     array[4 + index], 
		  array[8 + index], array[12 + index]);
#endif
	}

	void Matrix4::SetBasis(unsigned index, Vec4Param basisVector)
	{
		SetBasis(index, basisVector.x, basisVector.y, basisVector.z, basisVector.w);
	}

	void Matrix4::SetBasis(unsigned index, Vec3Param basisVector3, float w)
	{
		SetBasis(index, basisVector3.x, basisVector3.y, basisVector3.z, w);
	}

	void Matrix4::SetBasis(unsigned index, float x, Vec3Param basisVector3)
	{
		SetBasis(index, x, basisVector3.x, basisVector3.y, basisVector3.z);
	}

	void Matrix4::SetBasis(unsigned index, float x, float y, float z, float w)
	{
		Matrix4& self = *this;
		//ErrorIf(index > 3, "Matrix4 - Index out of range.");
#ifdef ColumnBasis
		array[index] = x;
		array[4 + index] = y;
		array[8 + index] = z;
		array[12 + index] = w;
#else
		self[index].Set(x, y, z, w);
#endif
	}

	void Matrix4::SetCross(unsigned index, Vec4Param crossVector)
	{
		SetCross(index, crossVector.x, crossVector.y, crossVector.z, crossVector.w);
	}

	void Matrix4::SetCross(unsigned index, Vec3Param crossVector3, float w)
	{
		SetCross(index, crossVector3.x, crossVector3.y, crossVector3.z, w);
	}

	void Matrix4::SetCross(unsigned index, float x, float y, float z, float w)
	{
		Matrix4& self = *this;
		//ErrorIf(index > 3, "Matrix4 - Index out of range.");
#ifdef ColumnBasis
		self[index].Set(x, y, z, w);
#else
		array[index] = x;
		array[4 + index] = y;
		array[8 + index] = z;
		array[12 + index] = w;
#endif
	}

	Vector3 Matrix4::Basis3(unsigned index) const
	{
		const Matrix4& self = *this;
		//ErrorIf(index > 3, "Matrix4 - Index out of range.");
#ifdef ColumnBasis
		return Vector3(array[index], array[4 + index], array[8 + index]);
#else
		return Vector3(self[index].x, self[index].y, self[index].z);
#endif
	}

	Vector3 Matrix4::Basis3X() const
	{
		const Matrix4& self = *this;
#ifdef ColumnBasis
		return Vector3(array[0], array[4], array[8]);
#else
		return Vector3(self[0].x, self[0].y, self[0].z);
#endif
	}

	Vector3 Matrix4::Basis3Y() const
	{
		const Matrix4& self = *this;
#ifdef ColumnBasis
		return Vector3(array[1], array[5], array[9]);
#else
		return Vector3(self[1].x, self[1].y, self[1].z);
#endif
	}

	Vector3 Matrix4::Basis3Z() const
	{
		const Matrix4& self = *this;
#ifdef ColumnBasis
		return Vector3(array[2], array[6], array[10]);
#else
		return Vector3(self[2].x, self[2].y, self[2].z);
#endif
	}

	Vector3 Matrix4::Basis3W() const
	{
		const Matrix4& self = *this;
#ifdef ColumnBasis
		return Vector3(array[3], array[7], array[11]);
#else
		return Vector3(self[3].x, self[3].y, self[3].z);
#endif
	}

	Vector3 Matrix4::Cross3(unsigned index) const
	{
		const Matrix4& self = *this;
		//ErrorIf(index > 3, "Matrix4 - Index out of range.");
#ifdef ColumnBasis
		return Vector3(self[index].x, self[index].y, self[index].z);
#else
		return Vector3(array[index], array[4 + index], array[8 + index]);
#endif
	}

	Matrix4 operator*(float lhs, Mat4Param rhs)
	{
		return rhs * lhs;
	}

	Matrix4 Concat(Mat4Param lhs, Mat4Param rhs)
	{
		return lhs.Concat(rhs);
	}

	Vector4 Transform(Mat4Param mat, Vec4Param vector)
	{
		float x = Dot(mat.Cross(0), vector);
		float y = Dot(mat.Cross(1), vector);
		float z = Dot(mat.Cross(2), vector);
		float w = Dot(mat.Cross(3), vector);
		return Vector4(x, y, z, w);
	}

	void Transform(Mat4Param mat, Vec4Ptr vector)
	{
		//ErrorIf(vector == NULL, "Matrix4 - Null pointer passed for vector.");
		float x = Dot(mat.Cross(0), *vector);
		float y = Dot(mat.Cross(1), *vector);
		float z = Dot(mat.Cross(2), *vector);
		float w = Dot(mat.Cross(3), *vector);
		vector->Set(x, y, z, w);
	}

	Matrix4 BuildTransform(Vec3Param translate, QuatParam rotate, Vec3Param scale)
	{
		Matrix4 newMatrix;
		newMatrix.BuildTransform(translate, rotate, scale);
		return newMatrix;
	}

	Matrix4 BuildTransform(Vec3Param translate, Mat3Param rotate, Vec3Param scale)
	{
		Matrix4 newMatrix;
		newMatrix.BuildTransform(translate, rotate, scale);
		return newMatrix;
	}

	Vector3 TransformPoint(Mat4Param matrix, Vec3Param point)
	{
		float x = Dot(*(Vector3*)&matrix[0], point) + matrix[0][3];
		float y = Dot(*(Vector3*)&matrix[1], point) + matrix[1][3];
		float z = Dot(*(Vector3*)&matrix[2], point) + matrix[2][3];
		return Vector3(x, y, z);
	}

	Vector3 TransformNormal(Mat4Param matrix, Vec3Param normal)
	{
		float x = Dot(*(Vector3*)&matrix[0], normal);
		float y = Dot(*(Vector3*)&matrix[1], normal);
		float z = Dot(*(Vector3*)&matrix[2], normal);
		return Vector3(x, y, z);
	}

	Vector3 TransformPointProjected(Mat4Param matrix, Vec3Param point)
	{
		float x = Dot(*(Vector3*)&matrix[0], point) + matrix[3][0];
		float y = Dot(*(Vector3*)&matrix[1], point) + matrix[3][1];
		float z = Dot(*(Vector3*)&matrix[2], point) + matrix[3][2];
		float w = Dot(*(Vector3*)&matrix[3], point) + matrix[3][3];
		return Vector3(x / w, y / w, z / w);
	}

	Vector3 TransformNormalCol(Mat4Param matrix, Vec3Param normal)
	{
		float x = Dot(Vector3(matrix.m00, matrix.m10, matrix.m20), normal);
		float y = Dot(Vector3(matrix.m01, matrix.m11, matrix.m21), normal);
		float z = Dot(Vector3(matrix.m02, matrix.m12, matrix.m22), normal);
		return Vector3(x, y, z);
	}

	Vector3 TransformPointCol(Mat4Param matrix, Vec3Param point)
	{
		float x = Dot(Vector3(matrix.m00, matrix.m10, matrix.m20), point) + matrix[3][0];
		float y = Dot(Vector3(matrix.m01, matrix.m11, matrix.m21), point) + matrix[3][1];
		float z = Dot(Vector3(matrix.m02, matrix.m12, matrix.m22), point) + matrix[3][2];
		return Vector3(x, y, z);
	}

	Vector3 TransformPointProjectedCol(Mat4Param matrix, Vec3Param point)
	{
		float x = Dot(Vector3(matrix.m00, matrix.m10, matrix.m20), point) + matrix[3][0];
		float y = Dot(Vector3(matrix.m01, matrix.m11, matrix.m21), point) + matrix[3][1];
		float z = Dot(Vector3(matrix.m02, matrix.m12, matrix.m22), point) + matrix[3][2];
		float w = Dot(Vector3(matrix.m03, matrix.m13, matrix.m23), point) + matrix[3][3];
		return Vector3(x / w, y / w, z / w);
	}

	Vector3 TransformPointProjectedCol(Mat4Param matrix, Vec3Param point, float* wOut)
	{
		float x = Dot(Vector3(matrix.m00, matrix.m10, matrix.m20), point) + matrix[3][0];
		float y = Dot(Vector3(matrix.m01, matrix.m11, matrix.m21), point) + matrix[3][1];
		float z = Dot(Vector3(matrix.m02, matrix.m12, matrix.m22), point) + matrix[3][2];
		float w = Dot(Vector3(matrix.m03, matrix.m13, matrix.m23), point) + matrix[3][3];
		*wOut = w;
		return Vector3(x / w, y / w, z / w);
	}

	float Trace(Mat4Param matrix)
	{
		return matrix.m00 + matrix.m11 + matrix.m22 + matrix.m33;
	}

	Matrix4 Matrix4::PerspectiveProjection(float FOV, int width, int height, float zNear, float zFar)
	{
		// TODO(student): implement this matrix properly; it shouldn't be hardcoded
		// and, when implemented right, will not distort the scene when the window
		// is resized			FLOAT32 fovRadians = Math::DegreesToRadians(fovy);
		Matrix4 m; m.ZeroOut();
		float cot = 1.f / tanf(FOV*0.5f);
		float aspect = static_cast<float>(width) / height;
		m.m00 = cot / aspect;
		m.m11 = cot;
		m.m22 = -(zFar + zNear) / (zFar - zNear);
		m.m23 = -2.0f * zNear * zFar / (zFar - zNear); 
		m.m32 = -1.0f;
		m.m33 = 0.0f;
		return m;
	}

	Matrix4 Matrix4::LookAt(const Vector3& vEye, const Vector3& vTarget, const Vector3& vInUp)
	{
		Vector3 vForward, vSide, vUp;
		Matrix4 vMat;
		vForward = vEye - vTarget;
		vForward.Normalize();

		vSide = vInUp.Cross(vForward);
		vSide.Normalize();

		vUp = vForward.Cross(vSide);
		vMat.SetIdentity();

		vMat.m00 = vSide.x;	vMat.m02 = vUp.x;	vMat.m02 = vForward.x;	vMat.m03 = vEye.Dot(-vSide);
		vMat.m10 = vSide.y;	vMat.m12 = vUp.y;	vMat.m12 = vForward.y;	vMat.m13 = vEye.Dot(-vUp);
		vMat.m20 = vSide.z;	vMat.m22 = vUp.z;	vMat.m22 = vForward.z;	vMat.m23 = vEye.Dot(-vForward);
		vMat.m30 = 0; vMat.m31 = 0; vMat.m32 = 0; vMat.m33 = 1;

		return vMat;
	}

	Math::Matrix4 Matrix4::RotateEuler(float a, float b, float c)
	{
		float cosc = cosf(c);
		float sinc = sinf(c);
		Matrix4 ym;
		ym[0][0] = cosc; ym[0][1] = 0; ym[0][2] = sinc; ym[0][3] = 0;
		ym[1][0] = 0; ym[1][1] = 1; ym[1][2] = 0; ym[1][3] = 0;
		ym[2][0] = -sinc; ym[2][1] = 0; ym[2][2] = cosc; ym[2][3] = 0;
		ym[3][0] = 0; ym[3][1] = 0; ym[3][2] = 0; ym[3][3] = 1;

		float cosb = cosf(b);
		float sinb = sinf(b);
		Matrix4 xm;
		xm[0][0] = 1; xm[0][1] = 0; xm[0][2] = 0; xm[0][3] = 0;
		xm[1][0] = 0; xm[1][1] = cosb; xm[1][2] = -sinb; xm[1][3] = 0;
		xm[2][0] = 0; xm[2][1] = sinb; xm[2][2] = cosb; xm[2][3] = 0;
		xm[3][0] = 0; xm[3][1] = 0; xm[3][2] = 0; xm[3][3] = 1;

		float cosa = cosf(a);
		float sina = sinf(a);
		Matrix4 zm;
		zm[0][0] = cosa; zm[0][1] = -sina; zm[0][2] = 0; zm[0][3] = 0;
		zm[1][0] = sina; zm[1][1] = cosa; zm[1][2] = 0; zm[1][3] = 0;
		zm[2][0] = 0; zm[2][1] = 0; zm[2][2] = 1; zm[2][3] = 0;
		zm[3][0] = 0; zm[3][1] = 0; zm[3][2] = 0; zm[3][3] = 1;

		return ym * xm * zm;
	}

}// namespace Math

