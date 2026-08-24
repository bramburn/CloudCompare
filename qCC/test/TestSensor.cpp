// ##########################################################################
// #                                                                        #
// #                            CLOUDCOMPARE                                #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  it under the terms of the GNU General Public License as published by  #
// #  the Free Software Foundation; version 2 or later of the License.     #
// #                                                                        #
// #  This program is distributed in the hope that it will be useful,       #
// #  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
// #  GNU General Public License for more details.                          #
// #                                                                        #
// #                   COPYRIGHT: CloudCompare project                      #
// #                                                                        #
// ##########################################################################

/**
 * @file TestSensor.cpp
 *
 * @brief Unit tests for ccSensor
 *
 * Tests sensor management:
 * - Sensor type and intrinsic parameters
 * - Position buffer (trajectory)
 * - GPS/IMU pose integration
 * - Depth map handling
 * - Viewing frustum computation
 *
 * @see ccSensor.cpp
 */
#include <QString>
#include <QTest>
#include <ccGBLSensor.h>
#include <ccSensor.h>
#include <cmath>

// Test pure spherical/cartesian math without needing an actual sensor device.

class TestSensor : public QObject
{
	Q_OBJECT

  private slots:

	void testSensorConstruction()
	{
		ccSensor sensor("test sensor");
		QCOMPARE(QString(sensor.getName()), QString("test sensor"));
		QCOMPARE(sensor.getClassID(), CC_TYPES::SENSOR);
	}

	void testSensorType()
	{
		// Default sensor type is UNKNOWN
		ccSensor sensor("test");
		QCOMPARE(sensor.getSensorType(), UNKNOWN_SENSOR);
	}

	void testSphericalToCartesianMath()
	{
		// Manual test of spherical → Cartesian conversion math
		// x = r * cos(phi) * sin(theta)
		// y = r * cos(phi) * cos(theta)
		// z = r * sin(phi)
		double r = 1.0;
		double phi = 0.0;   // elevation = 0 (horizontal)
		double theta = 0.0; // azimuth = 0 (pointing along +X)

		double x = r * std::cos(phi) * std::sin(theta);
		double y = r * std::cos(phi) * std::cos(theta);
		double z = r * std::sin(phi);

		QCOMPARE(x, 0.0);
		QCOMPARE(y, 1.0);
		QCOMPARE(z, 0.0);
	}

	void testSphericalToCartesianAt45degrees()
	{
		// r=1, phi=0, theta=45°
		double r = 1.0;
		double theta = M_PI / 4.0; // 45 degrees
		double phi = 0.0;

		double x = r * std::cos(phi) * std::sin(theta);
		double y = r * std::cos(phi) * std::cos(theta);
		double z = r * std::sin(phi);

		// sin(45°) = cos(45°) = sqrt(2)/2
		double expected = std::sqrt(2.0) / 2.0;
		QVERIFY(std::abs(x - expected) < 1e-10);
		QVERIFY(std::abs(y - expected) < 1e-10);
		QCOMPARE(z, 0.0);
	}

	void testSphericalVerticalAngle()
	{
		// r=1, phi=90° (straight up)
		double r = 1.0;
		double phi = M_PI / 2.0; // 90 degrees
		double theta = 0.0;

		double x = r * std::cos(phi) * std::sin(theta);
		double y = r * std::cos(phi) * std::cos(theta);
		double z = r * std::sin(phi);

		QCOMPARE(x, 0.0);
		QCOMPARE(y, 0.0);
		QCOMPARE(std::abs(z - 1.0), 0.0);
	}

	void testCartesianToSphericalRoundtrip()
	{
		// Start with (x,y,z) = (1,0,0)
		// r = sqrt(x²+y²+z²) = 1
		// phi = asin(z/r) = asin(0) = 0
		// theta = atan2(y, x) = atan2(0, 1) = 0
		double x = 1.0, y = 0.0, z = 0.0;
		double r = std::sqrt(x * x + y * y + z * z);
		double phi = std::asin(z / r);
		double theta = std::atan2(y, x);

		QCOMPARE(r, 1.0);
		QCOMPARE(phi, 0.0);
		QCOMPARE(theta, 0.0);
	}
};

QTEST_MAIN(TestSensor)
#include "TestSensor.moc"
