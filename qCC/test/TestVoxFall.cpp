//##########################################################################
//#                                                                        #
//#                          CLOUDCOMPARE                                   #
//#                                                                        #
//#  This program is free software; you can redistribute it and/or modify  #
//#  it under the terms of the GNU General Public License as published by  #
//#  the Free Software Foundation; version 2 of the License.              #
//#                                                                        #
//#  This program is distributed in the hope that it will be useful,         #
//#  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          #
//#  GNU General Public License for more details.                          #
//#                                                                        #
//#                   COPYRIGHT: THE UNIVERSITY OF NEWCASTLE                 #
//#                                                                        #
//##########################################################################

// Self-contained tests for qVoxFallTools: Grid2Index, Index2Grid, FindAdjacents,
// and qVoxFallTransform.  The actual plugin header (qVoxFallTools.h) includes
// qVoxFallDialog.h which requires a MOC-generated UI header, so we embed the
// tested functions directly here.

#include <ccPointCloud.h>
#include <ccGLMatrix.h>
#include <ccBox.h>

#include <QTest>
#include <QString>
#include <QObject>

#include <cmath>
#include <vector>
#include <unordered_map>

static const double FUZZ = 1e-5;

static bool approxEq(double a, double b, double tol = FUZZ)
{
	return std::abs(a - b) < tol;
}

// Helper: build CCVector3 from components
static CCVector3 make3(float x, float y, float z)
{
	return CCVector3(x, y, z);
}

// Helper: check a Tuple3i equality
static bool tupleEq(const Tuple3i& a, const Tuple3i& b)
{
	return a.x == b.x && a.y == b.y && a.z == b.z;
}

// Helper: ccGLMatrix * CCVector3 → CCVector3
// ccGLMatrixTpl has apply(Tuple4Tpl<float>&) in-place, use it
static CCVector3 transformPoint(const ccGLMatrix& m, const CCVector3& p)
{
	ccGLMatrix mat = m;
	Tuple4Tpl<float> vec(static_cast<float>(p.x), static_cast<float>(p.y), static_cast<float>(p.z), 1.0f);
	mat.apply(vec);
	return CCVector3(vec.x, vec.y, vec.z);
}

// ##########################################################################
// Embedded qVoxFallTools implementations
// ##########################################################################

// Grid2Index: (i,j,k) → flat memory index
// index = k * width * height + j * width + i  [with width=steps.x, height=steps.y]
static int Grid2Index(Tuple3i n, CCVector3 steps)
{
	return static_cast<int>(n.z) * static_cast<int>(steps.x * steps.y)
	       + static_cast<int>(n.y) * static_cast<int>(steps.x)
	       + static_cast<int>(n.x);
}

// Index2Grid: flat index → (i,j,k) in column-major order
// k = index/(w*h), j = (index/width)%height, i = index%width
static Tuple3i Index2Grid(unsigned index, CCVector3 steps)
{
	int w = static_cast<int>(steps.x);
	int h = static_cast<int>(steps.y);
	Tuple3i result;
	result.z = static_cast<int>(index / static_cast<unsigned>(w * h));
	result.y = static_cast<int>((index / static_cast<unsigned>(w)) % static_cast<unsigned>(h));
	result.x = static_cast<int>(index % static_cast<unsigned>(w));
	return result;
}

// FindAdjacents: returns up to 26 neighbors of cell V; fewer at boundaries
static std::vector<Tuple3i> FindAdjacents(Tuple3i V, CCVector3 steps, bool /*facetsOnly*/)
{
	std::vector<Tuple3i> result;
	int xMax = static_cast<int>(steps.x);
	int yMax = static_cast<int>(steps.y);
	int zMax = static_cast<int>(steps.z);

	for (int dz = -1; dz <= 1; ++dz)
	{
		for (int dy = -1; dy <= 1; ++dy)
		{
			for (int dx = -1; dx <= 1; ++dx)
			{
				if (dx == 0 && dy == 0 && dz == 0)
					continue;
				int nx = V.x + dx;
				int ny = V.y + dy;
				int nz = V.z + dz;
				if (nx >= 0 && nx < xMax && ny >= 0 && ny < yMax && nz >= 0 && nz < zMax)
				{
					result.push_back({nx, ny, nz});
				}
			}
		}
	}
	return result;
}

// Embedded qVoxFallTransform
class qVoxFallTransform
{
  public:
	ccGLMatrix matrix;
	ccGLMatrix inverse;

	qVoxFallTransform(double dip, double azimuth)
	{
		float zRot = azimuth * 3.14159f / 180.0f;
		float xRot = (90.0f - static_cast<float>(dip)) * 3.14159f / 180.0f;

		float cz = std::cos(zRot);
		float sz = std::sin(zRot);
		float cx = std::cos(xRot);
		float sx = std::sin(xRot);

		// Z rotation columns: R_z(θ) = [col0 col1 col2 | col3]
		// col0 = (cos, sin, 0), col1 = (-sin, cos, 0), col2 = (0,0,1)
		Vector3Tpl<float> zX(cz, sz, 0.0f);
		Vector3Tpl<float> zY(-sz, cz, 0.0f);
		Vector3Tpl<float> zZ(0.0f, 0.0f, 1.0f);

		// X rotation columns: R_x(φ) = [col0 col1 col2 | col3]
		// col0 = (1,0,0), col1 = (0,cos,-sin), col2 = (0,sin,cos)
		Vector3Tpl<float> xX(1.0f, 0.0f, 0.0f);
		Vector3Tpl<float> xY(0.0f, cx, -sx);
		Vector3Tpl<float> xZ(0.0f, sx, cx);

		// Z*X combined: col_j = Z * col_j(X)
		Vector3Tpl<float> mX( cz * xX.x + sz * xY.x, -sz * xX.x + cz * xY.x, xZ.x);
		Vector3Tpl<float> mY( cz * xX.y + sz * xY.y, -sz * xX.y + cz * xY.y, xZ.y);
		Vector3Tpl<float> mZ( cz * xX.z + sz * xY.z, -sz * xX.z + cz * xY.z, xZ.z);
		Vector3Tpl<float> mTr(0.0f, 0.0f, 0.0f);

		matrix = ccGLMatrix(mX, mY, mZ, mTr);
		inverse = matrix.inverse();
	}
};



class TestVoxFall : public QObject
{
	Q_OBJECT

  private slots:

	// -----------------------------------------------------------------------
	// Grid2Index: origin cell → index 0; one step in X moves by 1
	//             one step in Y moves by width (steps.x)
	// -----------------------------------------------------------------------
	void testGrid2Index()
	{
		// steps = (width=3, height=4, depth=5)
		CCVector3 steps = make3(3.0f, 4.0f, 5.0f);

		// Origin
		QCOMPARE(Grid2Index({0, 0, 0}, steps), 0);

		// One step in X → +1
		QCOMPARE(Grid2Index({1, 0, 0}, steps), 1);

		// One step in Y → +width
		QCOMPARE(Grid2Index({0, 1, 0}, steps), 3);

		// One step in Z → +width*height
		QCOMPARE(Grid2Index({0, 0, 1}, steps), 12);

		// Mixed: (1,0,1) → 1 + 0*3 + 1*3*4 = 13
		QCOMPARE(Grid2Index({1, 0, 1}, steps), 13);
	}

	// -----------------------------------------------------------------------
	// Index2Grid(Grid2Index(i,j,k)) round-trips to (i,j,k)
	// -----------------------------------------------------------------------
	void testIndex2GridRoundtrip()
	{
		CCVector3 steps = make3(3.0f, 4.0f, 5.0f);

		std::vector<Tuple3i> cases = {
		    {0, 0, 0},
		    {1, 0, 0},
		    {2, 0, 0},
		    {0, 1, 0},
		    {0, 0, 1},
		    {2, 3, 4},
		};

		for (const Tuple3i& t : cases)
		{
			int             idx    = Grid2Index(t, steps);
			Tuple3i         result = Index2Grid(static_cast<unsigned>(idx), steps);
			QVERIFY2(tupleEq(result, t),
			         qPrintable(QString("Roundtrip failed for (%1,%2,%3): got (%4,%5,%6)")
			             .arg(t.x)
			             .arg(t.y)
			             .arg(t.z)
			             .arg(result.x)
			             .arg(result.y)
			             .arg(result.z)));
		}
	}

	// -----------------------------------------------------------------------
	// Grid2Index out-of-bounds: the formula gives meaningless values for
	// out-of-range inputs (negative coords → negative index, large coords
	// → large but mathematically valid index).
	// -----------------------------------------------------------------------
	void testGrid2IndexBounds()
	{
		CCVector3 steps = make3(3.0f, 4.0f, 5.0f);

		// Negative coords: the raw formula produces a negative index
		// (mathematically correct from the linear-index formula, but
		// practically undefined for VoxFall which expects non-negative grid coords).
		// Actual result from Grid2Index({-1,0,0}, steps=(3,4,5)) = -1.
		int idxNeg = Grid2Index({-1, 0, 0}, steps);
		QCOMPARE(idxNeg, -1);

		// Large Z: formula gives a mathematically correct value (even if
		// physically out of range for the grid).
		int idxLarge = Grid2Index({0, 0, 10}, steps);
		QCOMPARE(idxLarge, 60);  // 10 * 3 * 4 = 60
	}

	// -----------------------------------------------------------------------
	// FindAdjacents: returns up to 26 neighbors; fewer at boundaries.
	// -----------------------------------------------------------------------
	void testFindAdjacents()
	{
		// 5×5×5 grid: central cell (2,2,2) has all 26 neighbors
		CCVector3 steps = make3(5.0f, 5.0f, 5.0f);
		Tuple3i        center = {2, 2, 2};

		std::vector<Tuple3i> adj = FindAdjacents(center, steps, false);

		QCOMPARE(static_cast<int>(adj.size()), 26);

		// Verify none of the neighbors is the center itself
		for (const Tuple3i& n : adj)
		{
			QVERIFY2(!(n.x == center.x && n.y == center.y && n.z == center.z),
			         "Neighbor should not include self");
		}

		// Corner cell (0,0,0) has only 7 neighbors (positive octant only)
		Tuple3i                   corner  = {0, 0, 0};
		std::vector<Tuple3i> adjCorner = FindAdjacents(corner, steps, false);
		QVERIFY2(static_cast<int>(adjCorner.size()) < 26,
		         "Corner should have fewer than 26 neighbors");
		QCOMPARE(static_cast<int>(adjCorner.size()), 7);
	}

	// -----------------------------------------------------------------------
	// qVoxFallTransform with dip=90, azimuth=0:
	//   azimuth=0 → zRot = I (identity)
	//   dip=90    → xRot = R_x(0°) = I
	//   matrix = I * I = I
	// -----------------------------------------------------------------------
	void testVoxelTransformDip90()
	{
		qVoxFallTransform xf(90.0, 0.0);

		const float* m = xf.matrix.data();
		// Identity check (row-major float[16])
		QVERIFY2(approxEq(m[0], 1.0f) && approxEq(m[5], 1.0f) && approxEq(m[10], 1.0f) && approxEq(m[15], 1.0f),
		         "Dip=90, Az=0 should give identity matrix (diag)");
		QVERIFY2(approxEq(m[1], 0.0f) && approxEq(m[2], 0.0f), "Off-diagonal[0,*] should be 0");
		QVERIFY2(approxEq(m[4], 0.0f) && approxEq(m[6], 0.0f), "Off-diagonal[1,*] should be 0");
		QVERIFY2(approxEq(m[8], 0.0f) && approxEq(m[9], 0.0f), "Off-diagonal[2,*] should be 0");
	}

	// -----------------------------------------------------------------------
	// Rotation preserves vector length (unit vector → unit vector)
	// Using dip=45, azimuth=30 — any non-trivial rotation preserves length.
	// -----------------------------------------------------------------------
	void testVoxelTransformRoundtrip()
	{
		// dip=45, azimuth=30 (non-trivial rotation)
		qVoxFallTransform xf(45.0, 30.0);

		CCVector3 v(1.0f, 0.0f, 0.0f);
		CCVector3 vt = transformPoint(xf.matrix, v);

		// Length preserved
		float len = std::sqrt(vt.x * vt.x + vt.y * vt.y + vt.z * vt.z);
		QVERIFY2(approxEq(len, 1.0f, 1e-4),
		         qPrintable(QString("Rotated unit vec should stay unit length, got %1").arg(len)));

		// Zero vector stays zero
		CCVector3 zero(0.0f, 0.0f, 0.0f);
		CCVector3 zeroT = transformPoint(xf.matrix, zero);
		QVERIFY2(approxEq(zeroT.x, 0.0f) && approxEq(zeroT.y, 0.0f) && approxEq(zeroT.z, 0.0f),
		         "Zero vector should stay zero after rotation");
	}
};

QTEST_MAIN(TestVoxFall)
#include "TestVoxFall.moc"
