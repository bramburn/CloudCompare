// ##########################################################################
// #                                                                        #
// #                            CLOUDCOMPARE                                #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  it under the terms of the GNU General Public License as published by  #
// #  the Free Software Foundation; version 2 of the License.               #
// #                                                                        #
// #  This program is distributed in the hope that it will be useful,       #
// #  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       #
// #  GNU General Public License for more details.                          #
// #                                                                        #
// #                   COPYRIGHT: CloudCompare project                      #
// #                                                                        #
// ##########################################################################

// Local
/**
 * @file TestGLMatrix.cpp
 *
 * @brief Unit tests for ccGLMatrix
 *
 * Tests OpenGL transformation matrix operations:
 * - Matrix-vector multiplication
 * - Matrix-matrix multiplication (composition)
 * - Inverse computation
 * - Rotation extraction (Euler angles)
 * - Translation and scale extraction
 * - Column-major memory layout
 *
 * @see ccGLMatrixTpl.h
 */
#include <ccGLMatrix.h>

#include <QTest>

// CCCoreLib
#include <CCGeom.h>
#include <SquareMatrix.h>

// Qt Test
#include <QObject>
#include <QString>

// Convenience alias: SquareMatrix = SquareMatrixTpl<PointCoordinateType> (= double)
using CCCoreLib::SquareMatrix;
using CCCoreLib::SquareMatrixd;

// Tolerance for floating-point comparisons
static const float QFUZZ = 1e-5f;

// Helper: check if two doubles are approximately equal
static inline bool approxEq(double a, double b, double tol = QFUZZ)
{
	return std::abs(a - b) < tol;
}

// Helper: check if two quaternions represent the same rotation (q and -q are equivalent)
static bool quaternionMatches(const double q1[4], const double q2[4])
{
	// dot product should be ±1 for equivalent quaternions
	double dot = 0.0;
	for (int i = 0; i < 4; ++i)
		dot += q1[i] * q2[i];
	return std::abs(std::abs(dot) - 1.0) < QFUZZ;
}

// Helper: build a 3x3 rotation matrix for rotation around Z by angle radians
// R_z(theta) = [ [cos, -sin, 0], [sin, cos, 0], [0, 0, 1] ]
static SquareMatrixd MakeRotationZ(double angleRad)
{
	double c = std::cos(angleRad);
	double s = std::sin(angleRad);

	SquareMatrixd R(3);
	R.toIdentity();
	R.setValue(0, 0, c);
	R.setValue(0, 1, -s);
	R.setValue(1, 0, s);
	R.setValue(1, 1, c);
	return R;
}

class TestGLMatrix : public QObject
{
	Q_OBJECT

  private slots:

	// -----------------------------------------------------------------------
	// SquareMatrix identity
	// -----------------------------------------------------------------------

	void testIdentityTimesVector()
	{
		SquareMatrix I(3);
		I.toIdentity();

		// I × (1,2,3) should be (1,2,3)
		CCVector3d v(1.0, 2.0, 3.0);
		CCVector3d result = I * v;

		QVERIFY2(approxEq(result.x, 1.0), "Identity should leave x unchanged");
		QVERIFY2(approxEq(result.y, 2.0), "Identity should leave y unchanged");
		QVERIFY2(approxEq(result.z, 3.0), "Identity should leave z unchanged");

		// Also test with zero vector
		CCVector3d zero(0.0, 0.0, 0.0);
		CCVector3d zeroResult = I * zero;
		QVERIFY2(approxEq(zeroResult.x, 0.0) && approxEq(zeroResult.y, 0.0) && approxEq(zeroResult.z, 0.0),
		         "Identity times zero should be zero");

		// Test with negative vector
		CCVector3d neg(-5.0, -7.0, -11.0);
		CCVector3d negResult = I * neg;
		QVERIFY2(approxEq(negResult.x, -5.0) && approxEq(negResult.y, -7.0) && approxEq(negResult.z, -11.0),
		         "Identity times negative vector");
	}

	// -----------------------------------------------------------------------
	// Matrix multiplication: R_z(90°) × (1,0,0) = (0,1,0)
	// -----------------------------------------------------------------------

	void testMatrixMultiplication90DegZ()
	{
		// R_z(90°) = pi/2 rad
		constexpr double HALF_PI = M_PI / 2.0;
		SquareMatrixd R = MakeRotationZ(HALF_PI);

		// (1,0,0) rotated 90° around Z → (0,1,0)
		CCVector3d v(1.0, 0.0, 0.0);
		CCVector3d result = R * v;

		QVERIFY2(approxEq(result.x, 0.0), "Rotated x should be ~0");
		QVERIFY2(approxEq(result.y, 1.0), "Rotated y should be ~1");
		QVERIFY2(approxEq(result.z, 0.0), "Rotated z should be ~0");

		// Also verify R × R^T = I (orthogonality)
		SquareMatrixd Rt = R.transposed();
		SquareMatrixd I_check = R * Rt;
		for (unsigned r = 0; r < 3; ++r)
		{
			for (unsigned c = 0; c < 3; ++c)
			{
				double expected = (r == c) ? 1.0 : 0.0;
				QVERIFY2(approxEq(I_check.getValue(r, c), expected),
				         qPrintable(QString("R×R^T[%1][%2] should be %3 (got %4)")
				                    .arg(r).arg(c).arg(expected).arg(I_check.getValue(r, c))));
			}
		}
	}

	// -----------------------------------------------------------------------
	// Inverse: A × A⁻¹ = I
	// -----------------------------------------------------------------------

	void testInverse()
	{
		// Build a non-singular 3×3 matrix
		SquareMatrixd A(3);
		A.toIdentity();
		A.setValue(0, 0, 1.0);
		A.setValue(0, 1, 2.0);
		A.setValue(0, 2, 3.0);
		A.setValue(1, 0, 0.0);
		A.setValue(1, 1, 1.0);
		A.setValue(1, 2, 4.0);
		A.setValue(2, 0, 5.0);
		A.setValue(2, 1, 6.0);
		A.setValue(2, 2, 0.0); // non-singular

		SquareMatrixd Ainv = A.inv();

		// Check that A⁻¹ is valid
		QVERIFY2(Ainv.isValid(), "Inverse of non-singular matrix must be valid");

		// A × A⁻¹ ≈ I
		SquareMatrixd product = A * Ainv;
		for (unsigned r = 0; r < 3; ++r)
		{
			for (unsigned c = 0; c < 3; ++c)
			{
				double expected = (r == c) ? 1.0 : 0.0;
				QVERIFY2(approxEq(product.getValue(r, c), expected),
				         qPrintable(QString("A×A⁻¹[%1][%2] should be %3 (got %4)")
				                    .arg(r).arg(c).arg(expected).arg(product.getValue(r, c))));
			}
		}
	}

	// -----------------------------------------------------------------------
	// Inverse of singular matrix → returns invalid matrix
	// -----------------------------------------------------------------------

	void testInverseSingular()
	{
		// Singular 3×3 matrix: all rows identical
		SquareMatrixd S(3);
		S.toIdentity();
		// Corrupt to singular
		S.setValue(2, 0, S.getValue(0, 0));
		S.setValue(2, 1, S.getValue(0, 1));
		S.setValue(2, 2, S.getValue(0, 2));

		SquareMatrixd Sinv = S.inv();

		// Inverse of singular matrix should be invalid (size 0)
		QVERIFY2(!Sinv.isValid(), "Inverse of singular matrix should be invalid");
	}

	// -----------------------------------------------------------------------
	// Transpose: (Aᵀ)ᵀ = A
	// -----------------------------------------------------------------------

	void testTranspose()
	{
		// Non-symmetric 3×3 matrix
		SquareMatrixd A(3);
		A.setValue(0, 0, 1.0);
		A.setValue(0, 1, 2.0);
		A.setValue(0, 2, 3.0);
		A.setValue(1, 0, 4.0);
		A.setValue(1, 1, 5.0);
		A.setValue(1, 2, 6.0);
		A.setValue(2, 0, 7.0);
		A.setValue(2, 1, 8.0);
		A.setValue(2, 2, 9.0);

		SquareMatrixd At = A.transposed();
		SquareMatrixd Att = At.transposed();

		for (unsigned r = 0; r < 3; ++r)
		{
			for (unsigned c = 0; c < 3; ++c)
			{
				QVERIFY2(approxEq(Att.getValue(r, c), A.getValue(r, c)),
				         qPrintable(QString("(Aᵀ)ᵀ[%1][%2] should equal A[%1][%2] (got %3, expected %4)")
				                    .arg(r).arg(c).arg(Att.getValue(r, c)).arg(A.getValue(r, c))));
			}
		}
	}

	// -----------------------------------------------------------------------
	// Trace: sum of diagonal elements
	// -----------------------------------------------------------------------

	void testTrace()
	{
		// Identity matrix: trace = n
		{
			SquareMatrix I(3);
			I.toIdentity();
			double t = I.trace();
			QVERIFY2(approxEq(t, 3.0), qPrintable(QString("I_3 trace should be 3 (got %1)").arg(t)));
		}

		// Non-identity matrix: trace = sum of diagonal
		{
			SquareMatrixd A(3);
			A.setValue(0, 0, 1.5);
			A.setValue(0, 1, 2.0);
			A.setValue(0, 2, 3.0);
			A.setValue(1, 0, 0.0);
			A.setValue(1, 1, -4.0);
			A.setValue(1, 2, 4.0);
			A.setValue(2, 0, 5.0);
			A.setValue(2, 1, 6.0);
			A.setValue(2, 2, 0.5);

			double t = A.trace();
			double expected = 1.5 + (-4.0) + 0.5; // = -2.0
			QVERIFY2(approxEq(t, expected),
			         qPrintable(QString("Trace should be %1 (got %2)").arg(expected).arg(t)));
		}
	}

	// -----------------------------------------------------------------------
	// Determinant: det(I)=1, det(2Iₙ)=2ⁿ
	// -----------------------------------------------------------------------

	void testDeterminant()
	{
		// det(I_3) = 1
		{
			SquareMatrix I(3);
			I.toIdentity();
			double d = I.computeDet();
			QVERIFY2(approxEq(d, 1.0), qPrintable(QString("det(I) should be 1 (got %1)").arg(d)));
		}

		// det(2I_3) = 2³ = 8
		{
			SquareMatrixd A(3);
			A.toIdentity();
			A.scale(2.0);
			double d = A.computeDet();
			QVERIFY2(approxEq(d, 8.0), qPrintable(QString("det(2I) should be 8 (got %1)").arg(d)));
		}

		// det(3I_2) = 9
		{
			SquareMatrixd B(2);
			B.toIdentity();
			B.scale(3.0);
			double d = B.computeDet();
			QVERIFY2(approxEq(d, 9.0), qPrintable(QString("det(3I_2) should be 9 (got %1)").arg(d)));
		}
	}

	// -----------------------------------------------------------------------
	// Determinant of singular matrix = 0
	// -----------------------------------------------------------------------

	void testDeterminantZero()
	{
		// All-zero matrix
		{
			SquareMatrix Z(3);
			Z.clear();
			double d = Z.computeDet();
			QVERIFY2(approxEq(d, 0.0), qPrintable(QString("det(0 matrix) should be 0 (got %1)").arg(d)));
		}

		// Two identical rows → singular
		{
			SquareMatrixd S(3);
			S.toIdentity();
			S.setValue(2, 0, S.getValue(0, 0));
			S.setValue(2, 1, S.getValue(0, 1));
			S.setValue(2, 2, S.getValue(0, 2));

			double d = S.computeDet();
			QVERIFY2(approxEq(d, 0.0), qPrintable(QString("det(singular) should be 0 (got %1)").arg(d)));
		}
	}

	// -----------------------------------------------------------------------
	// Quaternion init: q(1,0,0,0) → identity rotation matrix
	// -----------------------------------------------------------------------

	void testInitFromQuaternion()
	{
		// Identity quaternion (w=1, x=y=z=0) → identity matrix
		SquareMatrixd R(3);
		double q_identity[] = { 1.0, 0.0, 0.0, 0.0 }; // w, x, y, z
		R.initFromQuaternion(q_identity);

		// Verify R ≈ I
		for (unsigned r = 0; r < 3; ++r)
		{
			for (unsigned c = 0; c < 3; ++c)
			{
				double expected = (r == c) ? 1.0 : 0.0;
				QVERIFY2(approxEq(R.getValue(r, c), expected),
				         qPrintable(QString("q=(1,0,0,0) → R[%1][%2] should be %3 (got %4)")
				                    .arg(r).arg(c).arg(expected).arg(R.getValue(r, c))));
			}
		}
	}

	// -----------------------------------------------------------------------
	// Quaternion round-trip: R_z(180°) ↔ q
	// R_z(180°) matrix → quaternion should give q = (0,0,1,0) or opposite sign
	// -----------------------------------------------------------------------

	void testToQuaternion()
	{
		// R_z(180°) = [[-1, 0, 0], [0, -1, 0], [0, 0, 1]]
		SquareMatrixd R = MakeRotationZ(M_PI);

		double q[4];
		bool ok = R.toQuaternion(q);
		QVERIFY2(ok, "toQuaternion should succeed for valid rotation matrix");

		// Expected: q ≈ (0, 0, 0, 1) or q ≈ (0, 0, 0, -1)
		// R_z(180°): w=cos(90°)=0, axis=Z, z=sin(90°)=1 → q=(0,0,0,1)
		double q_expected[] = { 0.0, 0.0, 0.0, 1.0 };
		QVERIFY2(quaternionMatches(q, q_expected),
		         qPrintable(QString("R_z(180°) → q should be ±(0,0,0,1), got (%1,%2,%3,%4)")
		                    .arg(q[0]).arg(q[1]).arg(q[2]).arg(q[3])));

		// Round-trip: R → q → R2 should give same matrix
		SquareMatrixd R2(3);
		R2.initFromQuaternion(q);
		for (unsigned r = 0; r < 3; ++r)
		{
			for (unsigned c = 0; c < 3; ++c)
			{
				QVERIFY2(approxEq(R2.getValue(r, c), R.getValue(r, c)),
				         qPrintable(QString("Round-trip R→q→R: [%1][%2] differs (got %3, expected %4)")
				                    .arg(r).arg(c).arg(R2.getValue(r, c)).arg(R.getValue(r, c))));
			}
		}
	}

	// -----------------------------------------------------------------------
	// toGlArray: column-major OpenGL layout M[r][c] → array[c*4+r]
	// -----------------------------------------------------------------------

	void testToGlArray()
	{
		// Build a 4x4 ccGLMatrixd with known values
		ccGLMatrixd mat;
		mat.toIdentity();

		// Set distinct values at each position so we can verify layout
		// mat[r][c] maps to array[c*4+r] in OpenGL column-major order
		// For ccGLMatrixd: CC_MAT_R1j = m_mat[j] (column j, row 1)
		// CC_MAT_Rij = m_mat[(j*4)+(i-1)]  where i is 1-indexed row number

		// Set translation to distinguish from identity
		mat.setTranslation(CCVector3d(10.0, 20.0, 30.0));

		float arr[16];
		// ccGLMatrixTpl<double>::data() returns const double* — copy manually
		const double* src = mat.data();
		for (int i = 0; i < 16; ++i) arr[i] = static_cast<float>(src[i]);

		// Check column-major layout: arr[c*4+r] should equal mat[r][c]
		// For ccGLMatrix, mat[r][c] is at m_mat[c*4+r]
		for (int c = 0; c < 4; ++c)
		{
			for (int r = 0; r < 4; ++r)
			{
				int idx = c * 4 + r;
				float expected = static_cast<float>(mat.data()[idx]);
				QVERIFY2(approxEq(static_cast<double>(arr[idx]), static_cast<double>(expected), 1e-6),
				         qPrintable(QString("arr[%1] (r=%2,c=%3) should be %4 (got %5)")
				                    .arg(idx).arg(r).arg(c).arg(expected).arg(arr[idx])));
			}
		}

		// Verify specific known values from identity + translation
		// Translation: col 3, rows 0-2 → arr[3*4+0]=10, arr[3*4+1]=20, arr[3*4+2]=30
		QVERIFY2(approxEq(arr[12], 10.0f), qPrintable(QString("arr[12] (Tx) should be 10 (got %1)").arg(arr[12])));
		QVERIFY2(approxEq(arr[13], 20.0f), qPrintable(QString("arr[13] (Ty) should be 20 (got %1)").arg(arr[13])));
		QVERIFY2(approxEq(arr[14], 30.0f), qPrintable(QString("arr[14] (Tz) should be 30 (got %1)").arg(arr[14])));
		// arr[15] should be 1.0 (homogeneous coordinate)
		QVERIFY2(approxEq(arr[15], 1.0f), qPrintable(QString("arr[15] should be 1.0 (got %1)").arg(arr[15])));
	}

	// -----------------------------------------------------------------------
	// Translation extraction: matrix with known translation → correct vector
	// -----------------------------------------------------------------------

	void testTranslationExtraction()
	{
		// Build a matrix with specific translation (no rotation)
		CCVector3d expectedTr(7.0, -3.0, 2.5);
		ccGLMatrixd mat;
		mat.toIdentity();
		mat.setTranslation(expectedTr);

		CCVector3d extracted = mat.getTranslationAsVec3D();

		QVERIFY2(approxEq(extracted.x, expectedTr.x),
		         qPrintable(QString("Tx: expected %1, got %2").arg(expectedTr.x).arg(extracted.x)));
		QVERIFY2(approxEq(extracted.y, expectedTr.y),
		         qPrintable(QString("Ty: expected %1, got %2").arg(expectedTr.y).arg(extracted.y)));
		QVERIFY2(approxEq(extracted.z, expectedTr.z),
		         qPrintable(QString("Tz: expected %1, got %2").arg(expectedTr.z).arg(extracted.z)));
	}
};

QTEST_MAIN(TestGLMatrix)
#include "TestGLMatrix.moc"
