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
//#                   COPYRIGHT: CloudCompare project                      #
//#                                                                        #
//##########################################################################

// qAnimation plugin
#include <ViewInterpolate.h>

// CCCoreLib
#include <CCGeom.h>

// Qt Test
#include <QObject>
#include <QString>
#include <QTest>

// System
#include <cmath>
#include <limits>

static const double FUZZ = 1e-5;

static bool approxEq(double a, double b, double tol = FUZZ)
{
	return std::abs(a - b) < tol;
}

// ---------------------------------------------------------------------------
// Helper: construct a minimal ccViewportParameters with known camera center.
// Uses the public setCameraCenter() setter to avoid accessing the protected
// member directly.
// ---------------------------------------------------------------------------
static ccViewportParameters makeViewportParams(double cx, double cy, double cz,
                                                float fovDeg, float aspect,
                                                double focalDist)
{
	ccViewportParameters p;
	p.setFocalDistance(focalDist);
	p.setPivotPoint(CCVector3d(0.0, 0.0, 0.0), false);
	p.setCameraCenter(CCVector3d(cx, cy, cz), false);
	p.fov_deg = fovDeg;
	p.cameraAspectRatio = aspect;
	p.defaultPointSize = 1.0f;
	p.defaultLineWidth = 1.0f;
	p.zNearCoef = 0.01;
	p.zNear = 0.1;
	p.zFar = 1000.0;
	p.perspectiveView = true;
	p.objectCenteredView = true;
	return p;
}


class TestAnimation : public QObject
{
	Q_OBJECT

private slots:

	// -----------------------------------------------------------------------
	// interpolate: camera center at t=0.5 → midpoint of a and b
	// -----------------------------------------------------------------------
	void testInterpolatePosition()
	{
		ExtendedViewportParameters view1(makeViewportParams(0.0, 0.0, 0.0, 60.0f, 1.0f, 10.0));
		ExtendedViewportParameters view2(makeViewportParams(10.0, 0.0, 0.0, 60.0f, 1.0f, 10.0));

		ViewInterpolate interp(view1, view2, 10);

		ExtendedViewportParameters result;
		bool ok = interp.interpolate(result, 0.5);
		QVERIFY2(ok, "interpolate(0.5) should succeed");

		const CCVector3d& cc = result.params.getCameraCenter();
		QVERIFY2(approxEq(cc.x, 5.0),
		         qPrintable(QString("cameraCenter.x: expected 5.0, got %1").arg(cc.x)));
		QVERIFY2(approxEq(cc.y, 0.0),
		         qPrintable(QString("cameraCenter.y: expected 0.0, got %1").arg(cc.y)));
		QVERIFY2(approxEq(cc.z, 0.0),
		         qPrintable(QString("cameraCenter.z: expected 0.0, got %1").arg(cc.z)));
	}

	// -----------------------------------------------------------------------
	// interpolate: t=0 → a, t=1 → b (camera center endpoints)
	// -----------------------------------------------------------------------
	void testInterpolatePositionEndpoints()
	{
		ExtendedViewportParameters view1(makeViewportParams(0.0, 0.0, 0.0, 60.0f, 1.0f, 10.0));
		ExtendedViewportParameters view2(makeViewportParams(10.0, 0.0, 0.0, 60.0f, 1.0f, 10.0));

		ViewInterpolate interp(view1, view2, 10);

		// t=0 → view1
		{
			ExtendedViewportParameters r;
			QVERIFY(interp.interpolate(r, 0.0));
			const CCVector3d& cc = r.params.getCameraCenter();
			QVERIFY2(approxEq(cc.x, 0.0),
			         qPrintable(QString("t=0: x expected 0.0, got %1").arg(cc.x)));
		}

		// t=1 → view2
		{
			ExtendedViewportParameters r;
			QVERIFY(interp.interpolate(r, 1.0));
			const CCVector3d& cc = r.params.getCameraCenter();
			QVERIFY2(approxEq(cc.x, 10.0),
			         qPrintable(QString("t=1: x expected 10.0, got %1").arg(cc.x)));
		}
	}

	// -----------------------------------------------------------------------
	// interpolate: NaN-safe lerp.
	// InterpolateNumber<>: if start is NaN and end is valid:
	//   t<0.5 → NaN, t>=0.5 → end
	// We test cameraCenter.x with NaN in view1.
	// -----------------------------------------------------------------------
	void testInterpolateWithNaN()
	{
		ExtendedViewportParameters view1(makeViewportParams(0.0, 0.0, 0.0, 60.0f, 1.0f, 10.0));
		ExtendedViewportParameters view2(makeViewportParams(10.0, 0.0, 0.0, 60.0f, 1.0f, 10.0));

		// Inject NaN into view1 camera center via the public setter
		view1.params.setCameraCenter(
		    CCVector3d(std::numeric_limits<double>::quiet_NaN(), 0.0, 0.0), false);

		ViewInterpolate interp(view1, view2, 10);

		// t < 0.5 → result should be NaN
		{
			ExtendedViewportParameters r;
			QVERIFY(interp.interpolate(r, 0.3));
			const CCVector3d& cc = r.params.getCameraCenter();
			QVERIFY2(std::isnan(cc.x),
			         "At t=0.3 with NaN start, cameraCenter.x should be NaN");
		}

		// t >= 0.5 → result should be the valid end value
		{
			ExtendedViewportParameters r;
			QVERIFY(interp.interpolate(r, 0.5));
			const CCVector3d& cc = r.params.getCameraCenter();
			QVERIFY2(!std::isnan(cc.x),
			         "At t=0.5 with NaN start, cameraCenter.x should not be NaN");
			QVERIFY2(approxEq(cc.x, 10.0),
			         qPrintable(QString("At t=0.5, cameraCenter.x should be 10.0 (got %1)").arg(cc.x)));
		}
	}

	// -----------------------------------------------------------------------
	// interpolate: fov_deg lerps linearly
	// -----------------------------------------------------------------------
	void testInterpolateFOV()
	{
		ExtendedViewportParameters view1(makeViewportParams(0.0, 0.0, 0.0, 40.0f, 1.0f, 10.0));
		ExtendedViewportParameters view2(makeViewportParams(0.0, 0.0, 0.0, 60.0f, 1.0f, 10.0));

		ViewInterpolate interp(view1, view2, 10);

		// t=0.5 → (40+60)/2 = 50
		{
			ExtendedViewportParameters r;
			QVERIFY(interp.interpolate(r, 0.5));
			QVERIFY2(approxEq(static_cast<double>(r.params.fov_deg), 50.0, 0.01),
			         qPrintable(QString("fov_deg at t=0.5: expected 50.0, got %1").arg(r.params.fov_deg)));
		}

		// t=0 → 40
		{
			ExtendedViewportParameters r;
			QVERIFY(interp.interpolate(r, 0.0));
			QVERIFY2(approxEq(static_cast<double>(r.params.fov_deg), 40.0, 0.01),
			         qPrintable(QString("fov_deg at t=0: expected 40.0, got %1").arg(r.params.fov_deg)));
		}
	}

	// -----------------------------------------------------------------------
	// interpolate: t out of [0,1] returns false
	// -----------------------------------------------------------------------
	void testInterpolateOutOfRange()
	{
		ExtendedViewportParameters v1(makeViewportParams(0.0, 0.0, 0.0, 60.0f, 1.0f, 10.0));
		ExtendedViewportParameters v2(makeViewportParams(10.0, 0.0, 0.0, 60.0f, 1.0f, 10.0));
		ViewInterpolate interp(v1, v2, 10);

		ExtendedViewportParameters r;

		QVERIFY2(!interp.interpolate(r, -0.1), "t=-0.1 should return false");
		QVERIFY2(!interp.interpolate(r, 1.1),  "t=1.1  should return false");
	}

	// -----------------------------------------------------------------------
	// Constructor initializes state correctly.
	// view1() / view2() accessors should return the views passed to constructor.
	// -----------------------------------------------------------------------
	void testSetSmoothTrajectory()
	{
		ExtendedViewportParameters v1(makeViewportParams(1.0, 2.0, 3.0, 45.0f, 1.5f, 20.0));
		ExtendedViewportParameters v2(makeViewportParams(7.0, 8.0, 9.0, 90.0f, 0.8f, 50.0));

		// Construct with stepCount=0 (valid, just no nextView stepping)
		ViewInterpolate interp(v1, v2, 0);

		// Accessors return what was set
		const CCVector3d& v1cc = interp.view1().getCameraCenter();
		const CCVector3d& v2cc = interp.view2().getCameraCenter();
		QVERIFY2(approxEq(v1cc.x, 1.0),
		         qPrintable(QString("view1().cameraCenter.x: expected 1.0, got %1").arg(v1cc.x)));
		QVERIFY2(approxEq(v2cc.x, 7.0),
		         qPrintable(QString("view2().cameraCenter.x: expected 7.0, got %1").arg(v2cc.x)));
		QVERIFY2(approxEq(static_cast<double>(interp.view1().fov_deg), 45.0),
		         qPrintable(QString("view1().fov_deg: expected 45.0, got %1").arg(interp.view1().fov_deg)));
		QVERIFY2(approxEq(static_cast<double>(interp.view2().fov_deg), 90.0),
		         qPrintable(QString("view2().fov_deg: expected 90.0, got %1").arg(interp.view2().fov_deg)));

		// maxStep() returns the constructor value
		QCOMPARE(interp.maxStep(), 0u);

		// currentStep() starts at 0
		QCOMPARE(interp.currentStep(), 0u);
	}

	// -----------------------------------------------------------------------
	// nextView: steps through interpolation fractions correctly.
	// With stepCount=5 and step 0→4, fractions are 0/5, 1/5, 2/5, 3/5, 4/5.
	// -----------------------------------------------------------------------
	void testNextView()
	{
		ExtendedViewportParameters v1(makeViewportParams(0.0, 0.0, 0.0, 0.0f, 1.0f, 10.0));
		ExtendedViewportParameters v2(makeViewportParams(10.0, 0.0, 0.0, 100.0f, 1.0f, 10.0));

		ViewInterpolate interp(v1, v2, 5);

		for (unsigned step = 0; step < 5; ++step)
		{
			ExtendedViewportParameters r;
			bool ok = interp.nextView(r);
			QVERIFY2(ok, qPrintable(QString("nextView at step %1 should succeed").arg(step)));

			// Expected fraction = step / 5
			double expectedFrac = static_cast<double>(step) / 5.0;
			double expectedFov = 0.0 + expectedFrac * 100.0;  // fov lerps 0→100

			QVERIFY2(approxEq(static_cast<double>(r.params.fov_deg), expectedFov, 0.01),
			         qPrintable(QString("step %1: fov expected %2, got %3")
			                     .arg(step).arg(expectedFov).arg(r.params.fov_deg)));
		}

		// Sixth call (step 5) should fail (currentStep >= totalSteps)
		{
			ExtendedViewportParameters r;
			QVERIFY2(!interp.nextView(r), "nextView beyond totalSteps should return false");
		}
	}
};

QTEST_MAIN(TestAnimation)
#include "TestAnimation.moc"
