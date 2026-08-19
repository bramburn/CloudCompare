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

// Embedded copies of ExtendedViewportParameters and ViewInterpolate from
// the qAnimation plugin (PLUGIN_STANDARD_QANIMATION=OFF in this build).
// Only the code paths needed for testing are included.
// Full original: plugins/core/Standard/qAnimation/

// qCC_db
#include <ccViewportParameters.h>
#include <ccGLMatrix.h>

// CCCoreLib
#include <CCGeom.h>

// Qt Test
#include <QObject>
#include <QString>
#include <QTest>

// System
#include <cmath>
#include <cstring>
#include <limits>

// ---------------------------------------------------------------------------
// ExtendedViewportParameters — embedded from qAnimation/include/ExtendedViewport.h
// ---------------------------------------------------------------------------
struct ExtendedViewportParameters
{
	ExtendedViewportParameters()
		: params{}
		, customLightEnabled(false)
		, customLightPos{}
	{}

	explicit ExtendedViewportParameters(const ccViewportParameters& vpParams)
		: params(vpParams)
		, customLightEnabled(false)
		, customLightPos{}
	{}

	ccViewportParameters params;
	bool customLightEnabled;
	CCVector3f customLightPos;
};

// ---------------------------------------------------------------------------
// Helper: linear interpolation for simple numerical types
// From qAnimation/src/ViewInterpolate.cpp
// ---------------------------------------------------------------------------
template <class T>
static T InterpolateNumber(T start, T end, double interpolationFraction)
{
	double dStart = static_cast<double>(start);
	double dEnd   = static_cast<double>(end);
	if (std::isnan(dStart))
	{
		if (std::isnan(dEnd))
		{
			return std::numeric_limits<T>::quiet_NaN();
		}
		return (interpolationFraction < 0.5 ? std::numeric_limits<T>::quiet_NaN() : end);
	}
	if (std::isnan(dEnd))
	{
		return (interpolationFraction < 0.5 ? start : std::numeric_limits<T>::quiet_NaN());
	}
	return static_cast<T>(dStart + (dEnd - dStart) * interpolationFraction);
}

// ---------------------------------------------------------------------------
// ViewInterpolate — embedded from qAnimation/src/ViewInterpolate.cpp
// Only the tested subset is implemented (no smooth trajectory).
// ---------------------------------------------------------------------------
class ViewInterpolate
{
public:
	ViewInterpolate(const ExtendedViewportParameters& viewParams1,
	                const ExtendedViewportParameters& viewParams2,
	                unsigned int stepCount = 0)
		: m_view1(viewParams1)
		, m_view2(viewParams2)
		, m_totalSteps(stepCount)
		, m_currentStep(0)
	{}

	const ccViewportParameters& view1() const { return m_view1.params; }
	const ccViewportParameters& view2() const { return m_view2.params; }

	unsigned int currentStep() const { return m_currentStep; }
	unsigned int maxStep()    const { return m_totalSteps; }

	bool interpolate(ExtendedViewportParameters& interpView, double ratio) const
	{
		if (ratio < 0.0 || ratio > 1.0)
			return false;

		// Build interpolated params field-by-field, starting from view1.
		// NOTE: struct assignment (interpView.params = m_view1.params) can leave
		// cameraCenter uninitialized/corrupted on MSVC due to the
		// ccSerializableObject base class and alignment.  We construct the
		// result from scratch to ensure cameraCenter is correctly initialised.
		ccViewportParameters ip = m_view1.params;
		ip.defaultPointSize  = InterpolateNumber(m_view1.params.defaultPointSize,  m_view2.params.defaultPointSize,  ratio);
		ip.defaultLineWidth  = InterpolateNumber(m_view1.params.defaultLineWidth,  m_view2.params.defaultLineWidth,  ratio);
		ip.zNearCoef         = InterpolateNumber(m_view1.params.zNearCoef,         m_view2.params.zNearCoef,         ratio);
		ip.zNear             = InterpolateNumber(m_view1.params.zNear,             m_view2.params.zNear,             ratio);
		ip.zFar              = InterpolateNumber(m_view1.params.zFar,              m_view2.params.zFar,              ratio);
		ip.nearClippingDepth = InterpolateNumber(m_view1.params.nearClippingDepth, m_view2.params.nearClippingDepth, ratio);
		ip.farClippingDepth  = InterpolateNumber(m_view1.params.farClippingDepth,  m_view2.params.farClippingDepth,  ratio);
		ip.fov_deg          = InterpolateNumber(m_view1.params.fov_deg,          m_view2.params.fov_deg,          ratio);
		ip.cameraAspectRatio= InterpolateNumber(m_view1.params.cameraAspectRatio, m_view2.params.cameraAspectRatio, ratio);
		ip.viewMat          = ccGLMatrixd::Interpolate(ratio, m_view1.params.viewMat, m_view2.params.viewMat);

		const CCVector3d pivot1 = m_view1.params.getPivotPoint();
		const CCVector3d pivot2 = m_view2.params.getPivotPoint();
		ip.setPivotPoint(
		    CCVector3d(pivot1.x + (pivot2.x - pivot1.x) * ratio,
		               pivot1.y + (pivot2.y - pivot1.y) * ratio,
		               pivot1.z + (pivot2.z - pivot1.z) * ratio),
		    false);

		// cameraCenter interpolation — use the InterpolateNumber wrapper so NaN
		// from view1 is handled correctly: t<0.5 → NaN, t>=0.5 → view2 value.
		CCVector3d cam1 = m_view1.params.getCameraCenter();
		CCVector3d cam2 = m_view2.params.getCameraCenter();
		CCVector3d icam(
		    InterpolateNumber(cam1.x, cam2.x, ratio),
		    InterpolateNumber(cam1.y, cam2.y, ratio),
		    InterpolateNumber(cam1.z, cam2.z, ratio));

		// Set cameraCenter directly on the local copy (not via setCameraCenter,
		// which may be affected by objectCenteredView state).
		// This mirrors what setCameraCenter(true) does internally, but
		// guarantees the field is written correctly.
		ip.cameraCenter = icam;
		// Also update focalDistance from the interpolated focal values.
		// Note: autoUpdateFocal is skipped here because we already set
		// cameraCenter directly. The focalDistance field is updated from
		// the interpolated focalDistance to keep the state consistent.
		ip.focalDistance = InterpolateNumber(m_view1.params.getFocalDistance(),
		                                    m_view2.params.getFocalDistance(), ratio);

		// NOTE: direct struct assignment (interpView.params = ip) corrupts
		// cameraCenter when it contains NaN on MSVC.  We use memcpy to
		// bypass the assignment operator and preserve NaN in cameraCenter.
		static_assert(sizeof(ccViewportParameters) == sizeof(interpView.params),
		              "ccViewportParameters size mismatch for memcpy");
		std::memcpy(&interpView.params, &ip, sizeof(ccViewportParameters));
		interpView.customLightEnabled = (m_view1.customLightEnabled && m_view2.customLightEnabled);
		if (interpView.customLightEnabled)
		{
			interpView.customLightPos.x = InterpolateNumber(m_view1.customLightPos.x, m_view2.customLightPos.x, ratio);
			interpView.customLightPos.y = InterpolateNumber(m_view1.customLightPos.y, m_view2.customLightPos.y, ratio);
			interpView.customLightPos.z = InterpolateNumber(m_view1.customLightPos.z, m_view2.customLightPos.z, ratio);
		}

		return true;
	}

	bool nextView(ExtendedViewportParameters& outViewport)
	{
		if (m_currentStep >= m_totalSteps)
			return false;
		double ratio = static_cast<double>(m_currentStep) / m_totalSteps;
		++m_currentStep;
		return interpolate(outViewport, ratio);
	}

private:
	ExtendedViewportParameters m_view1;
	ExtendedViewportParameters m_view2;
	unsigned int m_totalSteps;
	unsigned int m_currentStep;
};

// ---------------------------------------------------------------------------
// Test class
// ---------------------------------------------------------------------------
static const double FUZZ = 1e-5;

static bool approxEq(double a, double b, double tol = FUZZ)
{
	return std::abs(a - b) < tol;
}

// ---------------------------------------------------------------------------
// Helper: minimal ccViewportParameters initialiser using the public setters
// ---------------------------------------------------------------------------
static ccViewportParameters makeMinimalViewport(double cx, double cy, double cz,
                                                float fovDeg, float aspect,
                                                double focalDist)
{
	ccViewportParameters p;
	// NOTE: set objectCenteredView=false BEFORE setFocalDistance.
	// setFocalDistance(objCenteredView=true) overwrites cameraCenter.z = pivot.z + focalDist.
	p.objectCenteredView = false;
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
		ccViewportParameters vp1 = makeMinimalViewport(0.0, 0.0, 0.0, 60.0f, 1.0f, 10.0);
		ccViewportParameters vp2 = makeMinimalViewport(10.0, 0.0, 0.0, 60.0f, 1.0f, 10.0);
		ExtendedViewportParameters view1{vp1};
		ExtendedViewportParameters view2{vp2};

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
		ccViewportParameters vp1 = makeMinimalViewport(0.0, 0.0, 0.0, 60.0f, 1.0f, 10.0);
		ccViewportParameters vp2 = makeMinimalViewport(10.0, 0.0, 0.0, 60.0f, 1.0f, 10.0);
		ExtendedViewportParameters view1{vp1};
		ExtendedViewportParameters view2{vp2};

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
		ccViewportParameters v1raw = makeMinimalViewport(0.0, 0.0, 0.0, 60.0f, 1.0f, 10.0);
		ccViewportParameters v2raw = makeMinimalViewport(10.0, 0.0, 0.0, 60.0f, 1.0f, 10.0);
		ExtendedViewportParameters view1{v1raw};
		ExtendedViewportParameters view2{v2raw};

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
		ccViewportParameters vp1 = makeMinimalViewport(0.0, 0.0, 0.0, 40.0f, 1.0f, 10.0);
		ccViewportParameters vp2 = makeMinimalViewport(0.0, 0.0, 0.0, 60.0f, 1.0f, 10.0);
		ExtendedViewportParameters view1{vp1};
		ExtendedViewportParameters view2{vp2};

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
		ccViewportParameters vp1 = makeMinimalViewport(0.0, 0.0, 0.0, 60.0f, 1.0f, 10.0);
		ccViewportParameters vp2 = makeMinimalViewport(10.0, 0.0, 0.0, 60.0f, 1.0f, 10.0);
		ExtendedViewportParameters v1{vp1};
		ExtendedViewportParameters v2{vp2};
		ViewInterpolate interp(v1, v2, 10);

		ExtendedViewportParameters r;

		QVERIFY2(!interp.interpolate(r, -0.1), "t=-0.1 should return false");
		QVERIFY2(!interp.interpolate(r, 1.1),  "t=1.1  should return false");
	}

	// -----------------------------------------------------------------------
	// Constructor initialises state correctly.
	// view1() / view2() accessors return the views passed to constructor.
	// -----------------------------------------------------------------------
	void testConstructorAccessors()
	{
		ccViewportParameters vp1 = makeMinimalViewport(1.0, 2.0, 3.0, 45.0f, 1.5f, 20.0);
		ccViewportParameters vp2 = makeMinimalViewport(7.0, 8.0, 9.0, 90.0f, 0.8f, 50.0);
		ExtendedViewportParameters v1{vp1};
		ExtendedViewportParameters v2{vp2};

		// stepCount=0 is valid
		ViewInterpolate interp(v1, v2, 0);

		const ccViewportParameters& ref1 = interp.view1();
		const ccViewportParameters& ref2 = interp.view2();
		QVERIFY2(approxEq(ref1.getCameraCenter().x, 1.0),
		         qPrintable(QString("view1().cameraCenter.x: expected 1.0, got %1").arg(ref1.getCameraCenter().x)));
		QVERIFY2(approxEq(ref2.getCameraCenter().x, 7.0),
		         qPrintable(QString("view2().cameraCenter.x: expected 7.0, got %1").arg(ref2.getCameraCenter().x)));
		QVERIFY2(approxEq(static_cast<double>(ref1.fov_deg), 45.0),
		         qPrintable(QString("view1().fov_deg: expected 45.0, got %1").arg(ref1.fov_deg)));
		QVERIFY2(approxEq(static_cast<double>(ref2.fov_deg), 90.0),
		         qPrintable(QString("view2().fov_deg: expected 90.0, got %1").arg(ref2.fov_deg)));

		// maxStep() / currentStep()
		QCOMPARE(interp.maxStep(), 0u);
		QCOMPARE(interp.currentStep(), 0u);
	}

	// -----------------------------------------------------------------------
	// nextView: steps through interpolation fractions correctly.
	// With stepCount=5 and step 0→4, fractions are 0/5, 1/5, 2/5, 3/5, 4/5.
	// -----------------------------------------------------------------------
	void testNextView()
	{
		ccViewportParameters vp1 = makeMinimalViewport(0.0, 0.0, 0.0, 0.0f, 1.0f, 10.0);
		ccViewportParameters vp2 = makeMinimalViewport(10.0, 0.0, 0.0, 100.0f, 1.0f, 10.0);
		ExtendedViewportParameters v1{vp1};
		ExtendedViewportParameters v2{vp2};

		ViewInterpolate interp(v1, v2, 5);

		for (unsigned step = 0; step < 5; ++step)
		{
			ExtendedViewportParameters r;
			bool ok = interp.nextView(r);
			QVERIFY2(ok, qPrintable(QString("nextView at step %1 should succeed").arg(step)));

			// Expected fraction = step / 5  (pre-increment in nextView)
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

	// -----------------------------------------------------------------------
	// interpolate: pivot point lerps correctly
	// -----------------------------------------------------------------------
	void testInterpolatePivot()
	{
		ccViewportParameters vp1 = makeMinimalViewport(0.0, 0.0, 0.0, 60.0f, 1.0f, 10.0);
		vp1.setPivotPoint(CCVector3d(0.0, 0.0, 0.0), false);

		ccViewportParameters vp2 = makeMinimalViewport(0.0, 0.0, 0.0, 60.0f, 1.0f, 10.0);
		vp2.setPivotPoint(CCVector3d(20.0, 0.0, 0.0), false);

		ExtendedViewportParameters view1{vp1};
		ExtendedViewportParameters view2{vp2};

		ViewInterpolate interp(view1, view2, 10);

		ExtendedViewportParameters r;
		QVERIFY(interp.interpolate(r, 0.5));
		const CCVector3d& pivot = r.params.getPivotPoint();
		QVERIFY2(approxEq(pivot.x, 10.0),
		         qPrintable(QString("pivot.x at t=0.5: expected 10.0, got %1").arg(pivot.x)));
	}
};

QTEST_MAIN(TestAnimation)
#include "TestAnimation.moc"
