// SPDX-License-Identifier: LGPL-2.0-or-later

// Copyright (C) 2026 — CloudCompare contributors

// Unit tests for HornRegistrationTools, quaternion helpers, and registration math.

// Uses Qt Test — run via Python subprocess with explicit PATH (see BUILD-LOCAL.md §9).



#include <QtTest/QtTest>

#include <ccPointCloud.h>



#include "GeometricalAnalysisTools.h"

#include "RegistrationTools.h"

#include "SquareMatrix.h"

#include "PointProjectionTools.h"



#include <cmath>

#include <memory>

#include <vector>

#include <array>



// 3×3 matrix as flat array (avoids nested template >> parsing issue in Qt Test macros)

using Mat3 = std::array<double, 9>;



// M_SQRT2_2 = sqrt(2)/2 — not defined on MSVC by default

#ifndef M_SQRT2_2

#define M_SQRT2_2 0.707106781186547524400844362104849

#endif



using namespace CCCoreLib;



// Helper: build a test cloud from a vector of CCVector3.

// ccPointCloud is-a GenericIndexedCloudPersist → GenericIndexedCloud → GenericCloud

// (see ccGenericPointCloud.h:75), so it can be passed directly to registration methods.

static ccPointCloud* MakeCloud(const std::vector<CCVector3>& pts)

{

    ccPointCloud* cloud = new ccPointCloud("test");

    cloud->reserve(static_cast<unsigned>(pts.size()));

    for (const auto& p : pts)

        cloud->addPoint(p);

    return cloud;

}



// Helper: compare two CCVector3d with epsilon tolerance

static bool Vec3dEqual(const CCVector3d& a, const CCVector3d& b, double eps = 1e-9)

{

    return std::abs(a.x - b.x) < eps

        && std::abs(a.y - b.y) < eps

        && std::abs(a.z - b.z) < eps;

}



// Helper: compare two SquareMatrixd with epsilon tolerance (3×3 only)

static bool MatrixEqual(const SquareMatrixd& A, const SquareMatrixd& B, double eps = 1e-9)

{

    for (int i = 0; i < 3; ++i)

        for (int j = 0; j < 3; ++j)

            if (std::abs(A.m_values[i][j] - B.m_values[i][j]) > eps)

                return false;

    return true;

}



// Helper: create a 3×3 identity matrix

// SquareMatrixTpl has no static Identity() factory; use ctor + toIdentity()

static SquareMatrixd MakeIdentity3()

{

    SquareMatrixd I(3);

    I.toIdentity();

    return I;

}



class TestRegistration : public QObject

{

    Q_OBJECT



private slots:

    // --- HornRegistrationTools ---



    void testHornIdentity_data()

    {

        QTest::addColumn<std::vector<CCVector3>>("moving");

        QTest::addColumn<std::vector<CCVector3>>("reference");



        std::vector<CCVector3> cloud = {

            CCVector3(0, 0, 0),

            CCVector3(1, 0, 0),

            CCVector3(0, 1, 0),

            CCVector3(0, 0, 1),

        };

        QTest::newRow("identical_4pts") << cloud << cloud;

    }



    void testHornIdentity()

    {

        QFETCH(std::vector<CCVector3>, moving);

        QFETCH(std::vector<CCVector3>, reference);



        std::unique_ptr<ccPointCloud> movingCloud(MakeCloud(moving));

        std::unique_ptr<ccPointCloud> refCloud(MakeCloud(reference));



        PointProjectionTools::Transformation trans;

        bool ok = HornRegistrationTools::FindAbsoluteOrientation(movingCloud.get(), refCloud.get(), trans, false);

        QVERIFY(ok);



        // Scale should be 1.0

        QVERIFY(std::abs(trans.s - 1.0) < 1e-6);



        // Translation should be near zero

        QVERIFY(Vec3dEqual(trans.T, CCVector3d(0, 0, 0), 1e-6));



        // Rotation should be identity

        SquareMatrixd I(3);

        I.toIdentity();

        QVERIFY(MatrixEqual(trans.R, I, 1e-6));

    }



    void testHornKnownRotation90Z_data()

    {

        QTest::addColumn<std::vector<CCVector3>>("moving");

        QTest::addColumn<std::vector<CCVector3>>("reference");

        QTest::addColumn<double>("expectedScale");

        QTest::addColumn<CCVector3d>("expectedT");



        // Points rotated 90° CCW around Z: (x,y) → (-y, x)

        std::vector<CCVector3> moving = {

            CCVector3(1, 0, 0),

            CCVector3(0, 1, 0),

            CCVector3(0, 0, 1),

        };

        std::vector<CCVector3> ref = {

            CCVector3(0, 1, 0),

            CCVector3(-1, 0, 0),

            CCVector3(0, 0, 1),

        };

        QTest::newRow("90deg_z") << moving << ref << 1.0 << CCVector3d(0, 0, 0);

    }



    void testHornKnownRotation90Z()

    {

        QFETCH(std::vector<CCVector3>, moving);

        QFETCH(std::vector<CCVector3>, reference);

        QFETCH(double, expectedScale);

        QFETCH(CCVector3d, expectedT);



        std::unique_ptr<ccPointCloud> movingCloud(MakeCloud(moving));

        std::unique_ptr<ccPointCloud> refCloud(MakeCloud(reference));



        PointProjectionTools::Transformation trans;

        bool ok = HornRegistrationTools::FindAbsoluteOrientation(movingCloud.get(), refCloud.get(), trans, false);

        QVERIFY(ok);



        QVERIFY(std::abs(trans.s - expectedScale) < 1e-6);

        QVERIFY(Vec3dEqual(trans.T, expectedT, 1e-6));

    }



    void testHornFixedScale_data()

    {

        QTest::addColumn<std::vector<CCVector3>>("moving");

        QTest::addColumn<std::vector<CCVector3>>("reference");

        QTest::addColumn<bool>("fixedScale");



        std::vector<CCVector3> cloud = {

            CCVector3(0, 0, 0),

            CCVector3(2, 0, 0),

            CCVector3(0, 2, 0),

            CCVector3(2, 2, 0),

        };

        QTest::newRow("scale_fixed") << cloud << cloud << true;

        QTest::newRow("scale_free") << cloud << cloud << false;

    }



    void testHornFixedScale()

    {

        QFETCH(std::vector<CCVector3>, moving);

        QFETCH(std::vector<CCVector3>, reference);

        QFETCH(bool, fixedScale);



        std::unique_ptr<ccPointCloud> movingCloud(MakeCloud(moving));

        std::unique_ptr<ccPointCloud> refCloud(MakeCloud(reference));



        PointProjectionTools::Transformation trans;

        bool ok = HornRegistrationTools::FindAbsoluteOrientation(movingCloud.get(), refCloud.get(), trans, fixedScale);

        QVERIFY(ok);

        QVERIFY(std::abs(trans.s - 1.0) < 1e-6);

    }



    void testHornTranslationOnly_data()

    {

        QTest::addColumn<std::vector<CCVector3>>("moving");

        QTest::addColumn<std::vector<CCVector3>>("reference");

        QTest::addColumn<CCVector3d>("expectedT");



        // Non-degenerate tetrahedron — avoids planar degeneracy that can cause scale failures

        std::vector<CCVector3> ref = {

            CCVector3(0, 0, 0),

            CCVector3(1, 0, 0),

            CCVector3(0, 1, 0),

            CCVector3(0, 0, 1),

        };

        // Same geometry, translated by (10, -5, 3)

        std::vector<CCVector3> moving = {

            CCVector3(10, -5, 3),

            CCVector3(11, -5, 3),

            CCVector3(10, -4, 3),

            CCVector3(10, -5, 4),

        };

        // Horn: T = centroid_P - centroid_Q = -(10,-5,3)
        QTest::newRow("translate") << moving << ref << CCVector3d(-10, 5, -3);

    }



    void testHornTranslationOnly()

    {

        QFETCH(std::vector<CCVector3>, moving);

        QFETCH(std::vector<CCVector3>, reference);

        QFETCH(CCVector3d, expectedT);



        std::unique_ptr<ccPointCloud> movingCloud(MakeCloud(moving));

        std::unique_ptr<ccPointCloud> refCloud(MakeCloud(reference));



        PointProjectionTools::Transformation trans;

        bool ok = HornRegistrationTools::FindAbsoluteOrientation(movingCloud.get(), refCloud.get(), trans, true);

        QVERIFY(ok);



        // Horn returns T = centroid_P - centroid_Q (maps REFERENCE → MOVING).
        // For Q = P + (10,-5,3), T returned = -(10,-5,3) = (-10,5,-3).
        QVERIFY2(Vec3dEqual(trans.T, expectedT, 1e-3),
                 qPrintable(QString("T=(%1,%2,%3) expected=(%4,%5,%6)")
                            .arg(trans.T.x).arg(trans.T.y).arg(trans.T.z)
                            .arg(expectedT.x).arg(expectedT.y).arg(expectedT.z)));

        QVERIFY(std::abs(trans.s - 1.0) < 1e-6);

    }



    // --- Geometric helpers ---



    void testGravityCenterSimple_data()

    {

        QTest::addColumn<std::vector<CCVector3>>("pts");

        QTest::addColumn<CCVector3d>("expected");



        // Centroid of unit cube corners = (0.5, 0.5, 0.5)

        std::vector<CCVector3> pts = {

            CCVector3(0, 0, 0),

            CCVector3(1, 0, 0),

            CCVector3(0, 1, 0),

            CCVector3(1, 1, 0),

            CCVector3(0, 0, 1),

            CCVector3(1, 0, 1),

            CCVector3(0, 1, 1),

            CCVector3(1, 1, 1),

        };

        QTest::newRow("unit_cube") << pts << CCVector3d(0.5, 0.5, 0.5);



        std::vector<CCVector3> single = { CCVector3(3, -1, 2) };

        QTest::newRow("single") << single << CCVector3d(3, -1, 2);



        std::vector<CCVector3> two = {

            CCVector3(0, 0, 0),

            CCVector3(2, 4, 6),

        };

        QTest::newRow("two") << two << CCVector3d(1, 2, 3);

    }



    void testGravityCenterSimple()

    {

        QFETCH(std::vector<CCVector3>, pts);

        QFETCH(CCVector3d, expected);



        std::unique_ptr<ccPointCloud> cloud(MakeCloud(pts));

        CCVector3 g = GeometricalAnalysisTools::ComputeGravityCenter(cloud.get());

        CCVector3d gd(g.x, g.y, g.z);

        QVERIFY(Vec3dEqual(gd, expected, 1e-9));

    }



    void testCrossCovarianceMatrixIsSymmetric_data()

    {

        QTest::addColumn<std::vector<CCVector3>>("P");

        QTest::addColumn<std::vector<CCVector3>>("X");



        std::vector<CCVector3> P = {

            CCVector3(0, 0, 0),

            CCVector3(1, 0, 0),

            CCVector3(0, 1, 0),

            CCVector3(0, 0, 1),

        };

        QTest::newRow("same_cloud") << P << P;

    }



    void testCrossCovarianceMatrixIsSymmetric()

    {

        QFETCH(std::vector<CCVector3>, P);

        QFETCH(std::vector<CCVector3>, X);



        std::unique_ptr<ccPointCloud> cloudP(MakeCloud(P));

        std::unique_ptr<ccPointCloud> cloudX(MakeCloud(X));

        CCVector3 Gp = GeometricalAnalysisTools::ComputeGravityCenter(cloudP.get());

        CCVector3 Gx = GeometricalAnalysisTools::ComputeGravityCenter(cloudX.get());



        SquareMatrixd cov = GeometricalAnalysisTools::ComputeCrossCovarianceMatrix(cloudP.get(), cloudX.get(), Gp, Gx);



        QVERIFY(cov.isValid());



        // Covariance matrix must be symmetric

        for (int i = 0; i < 3; ++i)

            for (int j = i + 1; j < 3; ++j)

                QVERIFY2(std::abs(cov.m_values[i][j] - cov.m_values[j][i]) < 1e-12,

                         "Covariance matrix is not symmetric");

    }



    void testCrossCovarianceAgainstKnownValues_data()

    {

        QTest::addColumn<std::vector<CCVector3>>("P");

        QTest::addColumn<std::vector<CCVector3>>("X");

        QTest::addColumn<double>("expectedXX");



        // Unit square corners, centroid at (0.5, 0.5, 0)

        // deviations = {-0.5,+0.5,-0.5,+0.5}, sum of squares = 0.5 per axis

        // CCCoreLib normalises by sqrt(N) = sqrt(4) = 2

        // cov(x,x) = 0.5 / 2 = 0.25

        std::vector<CCVector3> P = {

            CCVector3(0, 0, 0),

            CCVector3(1, 0, 0),

            CCVector3(0, 1, 0),

            CCVector3(1, 1, 0),

        };

        QTest::newRow("unit_square") << P << P << 0.25;

    }



    void testCrossCovarianceAgainstKnownValues()

    {

        QFETCH(std::vector<CCVector3>, P);

        QFETCH(std::vector<CCVector3>, X);

        QFETCH(double, expectedXX);



        std::unique_ptr<ccPointCloud> cloudP(MakeCloud(P));

        std::unique_ptr<ccPointCloud> cloudX(MakeCloud(X));

        CCVector3 Gp = GeometricalAnalysisTools::ComputeGravityCenter(cloudP.get());

        CCVector3 Gx = GeometricalAnalysisTools::ComputeGravityCenter(cloudX.get());



        SquareMatrixd cov = GeometricalAnalysisTools::ComputeCrossCovarianceMatrix(cloudP.get(), cloudX.get(), Gp, Gx);



        double relErr = std::abs(cov.m_values[0][0] - expectedXX) / expectedXX;

        QVERIFY2(relErr < 0.01,

                 qPrintable(QString("cov[0][0]=%1 expected ~%2").arg(cov.m_values[0][0]).arg(expectedXX)));

    }



    // --- SquareMatrix quaternion ---



    void testQuatToMatrixRoundtrip_data()

    {

        QTest::addColumn<double>("qw");

        QTest::addColumn<double>("qx");

        QTest::addColumn<double>("qy");

        QTest::addColumn<double>("qz");



        QTest::newRow("identity") << 1.0 << 0.0 << 0.0 << 0.0;

        QTest::newRow("90deg_z") << M_SQRT2_2 << 0.0 << 0.0 << M_SQRT2_2;

        QTest::newRow("180deg_x") << 0.0 << 1.0 << 0.0 << 0.0;

    }



    void testQuatToMatrixRoundtrip()

    {

        QFETCH(double, qw);

        QFETCH(double, qx);

        QFETCH(double, qy);

        QFETCH(double, qz);



        double q[4] = { qw, qx, qy, qz };

        SquareMatrixd R(3);

        R.initFromQuaternion(q);



        QVERIFY(R.isValid());



        // 1. Orthogonal: R^T · R ≈ I

        SquareMatrixd Rt = R.transposed();

        SquareMatrixd I = MakeIdentity3();

        QVERIFY(MatrixEqual(Rt * R, I, 1e-8));



        // 2. det(R) ≈ 1

        QVERIFY(std::abs(R.computeDet() - 1.0) < 1e-8);



        // Roundtrip: R → q' → R'

        double q2[4];

        bool ok = R.toQuaternion(q2, true);

        QVERIFY(ok);



        // q and q2 may differ by sign (q and -q represent same rotation)

        double dot = std::abs(qw * q2[0] + qx * q2[1] + qy * q2[2] + qz * q2[3]);

        QVERIFY2(dot > 0.999, qPrintable(QString("q·q2=%1 (should be ≈1)").arg(dot)));

    }



    void testMatrixToQuaternionIdentity_data()

    {

        QTest::addColumn<Mat3>("Rdata");

        QTest::addColumn<double>("expectedQw");



        // row-major flat array: row0col0, row0col1, row0col2, row1col0, ...

        QTest::newRow("identity") << Mat3{1,0,0, 0,1,0, 0,0,1} << 1.0;

    }



    void testMatrixToQuaternionIdentity()

    {

        QFETCH(Mat3, Rdata);

        QFETCH(double, expectedQw);



        SquareMatrixd R(3);

        for (int i = 0; i < 3; ++i)

            for (int j = 0; j < 3; ++j)

                R.m_values[i][j] = static_cast<ScalarType>(Rdata[i * 3 + j]);



        double q[4];

        bool ok = R.toQuaternion(q, true);

        QVERIFY(ok);



        QVERIFY2(std::abs(q[0] - expectedQw) < 1e-6,

                 qPrintable(QString("q[0]=%1 expected ~%2").arg(q[0]).arg(expectedQw)));

        QVERIFY2(std::abs(q[1]) < 1e-6 && std::abs(q[2]) < 1e-6 && std::abs(q[3]) < 1e-6,

                 "Identity matrix should give pure real quaternion");

    }



    // --- SquareMatrix basic operations ---



    void testMatrixIdentity()

    {

        SquareMatrixd I = MakeIdentity3();

        for (int i = 0; i < 3; ++i)

            for (int j = 0; j < 3; ++j)

                QVERIFY2(std::abs(I.m_values[i][j] - (i == j ? 1.0 : 0.0)) < 1e-12,

                         qPrintable(QString("I[%1][%2]=%3 expected %4")

                                    .arg(i).arg(j).arg(I.m_values[i][j]).arg(i==j ? 1.0 : 0.0)));

    }



    void testMatrixMultiplication_data()

    {

        QTest::addColumn<Mat3>("A");

        QTest::addColumn<Mat3>("B");

        QTest::addColumn<Mat3>("expected");



        // R_z(90°) · I = R_z(90°)

        QTest::newRow("90deg_z_times_I") <<

            Mat3{0,-1,0, 1,0,0, 0,0,1} <<

            Mat3{1,0,0, 0,1,0, 0,0,1} <<

            Mat3{0,-1,0, 1,0,0, 0,0,1};



        // I · R_z(90°) = R_z(90°)

        QTest::newRow("I_times_90deg_z") <<

            Mat3{1,0,0, 0,1,0, 0,0,1} <<

            Mat3{0,-1,0, 1,0,0, 0,0,1} <<

            Mat3{0,-1,0, 1,0,0, 0,0,1};

    }



    void testMatrixMultiplication()

    {

        QFETCH(Mat3, A);

        QFETCH(Mat3, B);

        QFETCH(Mat3, expected);



        SquareMatrixd MA(3);

        SquareMatrixd MB(3);

        for (int i = 0; i < 3; ++i) {

            for (int j = 0; j < 3; ++j) {

                MA.m_values[i][j] = static_cast<ScalarType>(A[i * 3 + j]);

                MB.m_values[i][j] = static_cast<ScalarType>(B[i * 3 + j]);

            }

        }



        SquareMatrixd MC = MA * MB;



        for (int i = 0; i < 3; ++i)

            for (int j = 0; j < 3; ++j)

                QVERIFY2(std::abs(MC.m_values[i][j] - expected[i * 3 + j]) < 1e-8,

                         qPrintable(QString("C[%1][%2]=%3 expected %4")

                                    .arg(i).arg(j).arg(MC.m_values[i][j]).arg(expected[i * 3 + j])));

    }



    void testMatrixInverse_data()

    {

        QTest::addColumn<Mat3>("A");



        // [[1,2,3],[0,1,4],[5,6,0]] — det = 1 (row-major)

        QTest::newRow("simple_3x3") << Mat3{1,2,3, 0,1,4, 5,6,0};

    }



    void testMatrixInverse()

    {

        QFETCH(Mat3, A);



        SquareMatrixd MA(3);

        for (int i = 0; i < 3; ++i)

            for (int j = 0; j < 3; ++j)

                MA.m_values[i][j] = static_cast<ScalarType>(A[i * 3 + j]);



        SquareMatrixd MAinv = MA.inv();

        QVERIFY(MAinv.isValid());



        SquareMatrixd I = MakeIdentity3();

        SquareMatrixd product = MA * MAinv;

        QVERIFY2(MatrixEqual(product, I, 1e-8),

                 "A * A^-1 should equal identity");

    }



    void testMatrixDeterminant_data()

    {

        QTest::addColumn<Mat3>("M");

        QTest::addColumn<double>("expectedDet");



        QTest::newRow("identity") << Mat3{1,0,0, 0,1,0, 0,0,1} << 1.0;



        QTest::newRow("scalar_2") << Mat3{2,0,0, 0,2,0, 0,0,2} << 8.0;



        // [[1,2,3],[0,1,4],[5,6,0]] — det = 1

        QTest::newRow("simple") << Mat3{1,2,3, 0,1,4, 5,6,0} << 1.0;

    }



    void testMatrixDeterminant()

    {

        QFETCH(Mat3, M);

        QFETCH(double, expectedDet);



        SquareMatrixd mat(3);

        for (int i = 0; i < 3; ++i)

            for (int j = 0; j < 3; ++j)

                mat.m_values[i][j] = static_cast<ScalarType>(M[i * 3 + j]);



        double det = mat.computeDet();

        QVERIFY2(std::abs(det - expectedDet) < 1e-9,

                 qPrintable(QString("det=%1 expected %2").arg(det).arg(expectedDet)));

    }



    void testMatrixTranspose()

    {

        SquareMatrixd A(3);

        A.m_values[0][0] = 1; A.m_values[0][1] = 2; A.m_values[0][2] = 3;

        A.m_values[1][0] = 4; A.m_values[1][1] = 5; A.m_values[1][2] = 6;

        A.m_values[2][0] = 7; A.m_values[2][1] = 8; A.m_values[2][2] = 9;



        SquareMatrixd At = A.transposed();



        for (int i = 0; i < 3; ++i)

            for (int j = 0; j < 3; ++j)

                QVERIFY2(At.m_values[i][j] == A.m_values[j][i],

                         "Transpose: At[i][j] should equal A[j][i]");



        SquareMatrixd Att = At.transposed();

        QVERIFY(MatrixEqual(Att, A, 0));

    }



    void testMatrixTrace_data()

    {

        QTest::addColumn<Mat3>("M");

        QTest::addColumn<double>("expectedTrace");



        QTest::newRow("identity") << Mat3{1,0,0, 0,1,0, 0,0,1} << 3.0;



        QTest::newRow("custom") << Mat3{1,2,3, 4,5,6, 7,8,9} << 15.0;

    }



    void testMatrixTrace()

    {

        QFETCH(Mat3, M);

        QFETCH(double, expectedTrace);



        SquareMatrixd mat(3);

        for (int i = 0; i < 3; ++i)

            for (int j = 0; j < 3; ++j)

                mat.m_values[i][j] = static_cast<ScalarType>(M[i * 3 + j]);



        double trace = mat.trace();

        QVERIFY2(std::abs(trace - expectedTrace) < 1e-9,

                 qPrintable(QString("trace=%1 expected %2").arg(trace).arg(expectedTrace)));

    }



    // --- ScaledTransformation ---



    void testTransformationApply_data()

    {

        QTest::addColumn<PointProjectionTools::Transformation>("trans");

        QTest::addColumn<CCVector3>("input");

        QTest::addColumn<CCVector3d>("expected");



        PointProjectionTools::Transformation t;

        t.s = 2.0;

        t.T = CCVector3d(1, 0, -1);

        t.R.toIdentity();

        QTest::newRow("scale_translate") << t << CCVector3(1, 1, 1) << CCVector3d(3, 2, 1);

    }



    void testTransformationApply()

    {

        QFETCH(PointProjectionTools::Transformation, trans);

        QFETCH(CCVector3, input);

        QFETCH(CCVector3d, expected);



        CCVector3d result = trans.apply(input);

        QVERIFY(Vec3dEqual(result, expected, 1e-9));

    }



    void testTransformationIdentity()

    {

        PointProjectionTools::Transformation t;

        t.s = 1.0;

        t.T = CCVector3d(0, 0, 0);

        t.R.toIdentity();



        CCVector3 P(3, 4, 5);

        CCVector3d result = t.apply(P);

        CCVector3d expected(3, 4, 5);

        QVERIFY(Vec3dEqual(result, expected, 1e-12));

    }



    void testTransformationScaleOnly_data()

    {

        QTest::addColumn<double>("scale");

        QTest::addColumn<CCVector3>("input");

        QTest::addColumn<CCVector3d>("expected");



        QTest::newRow("scale_2") << 2.0 << CCVector3(1, 2, 3) << CCVector3d(2, 4, 6);

        QTest::newRow("scale_0p5") << 0.5 << CCVector3(2, 4, 6) << CCVector3d(1, 2, 3);

    }



    void testTransformationScaleOnly()

    {

        QFETCH(double, scale);

        QFETCH(CCVector3, input);

        QFETCH(CCVector3d, expected);



        PointProjectionTools::Transformation t;

        t.s = scale;

        t.R.toIdentity();

        t.T = CCVector3d(0, 0, 0);



        CCVector3d result = t.apply(input);

        QVERIFY(Vec3dEqual(result, expected, 1e-9));

    }



    // --- HornRegistrationTools: scale estimation (fixedScale=false) ---



    void testHornScaleEstimation_data()

    {

        QTest::addColumn<std::vector<CCVector3>>("moving");

        QTest::addColumn<std::vector<CCVector3>>("reference");

        QTest::addColumn<double>("expectedScale");



        // Reference: tetrahedron

        std::vector<CCVector3> ref = {

            CCVector3(0, 0, 0),

            CCVector3(1, 0, 0),

            CCVector3(0, 1, 0),

            CCVector3(0, 0, 1),

        };

        // Moving: same geometry scaled by 2

        std::vector<CCVector3> moving = {

            CCVector3(0, 0, 0),

            CCVector3(2, 0, 0),

            CCVector3(0, 2, 0),

            CCVector3(0, 0, 2),

        };

        // Horn scale = s_aligned / sqrt(s_ref) ~= 0.5 for 2x scaled cloud
        // (not 2.0 — the scale factor is RMS_aligned / sqrt(RMS_ref))
        QTest::newRow("scale_2x") << moving << ref << 0.5;

    }



    void testHornScaleEstimation()

    {

        QFETCH(std::vector<CCVector3>, moving);

        QFETCH(std::vector<CCVector3>, reference);

        QFETCH(double, expectedScale);



        std::unique_ptr<ccPointCloud> movingCloud(MakeCloud(moving));

        std::unique_ptr<ccPointCloud> refCloud(MakeCloud(reference));



        PointProjectionTools::Transformation trans;

        // fixedScale=false → scale is estimated by Horn's algorithm

        bool ok = HornRegistrationTools::FindAbsoluteOrientation(movingCloud.get(), refCloud.get(), trans, false);

        QVERIFY(ok);



        QVERIFY2(std::abs(trans.s - expectedScale) < 0.05,

                 qPrintable(QString("scale=%1 expected ~%2").arg(trans.s).arg(expectedScale)));

    }

};



QTEST_MAIN(TestRegistration)

#include "TestRegistration.moc"

