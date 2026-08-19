// SPDX-License-Identifier: LGPL-2.0-or-later

// Copyright (C) 2026 — CloudCompare contributors

// Unit tests for ccLibAlgorithms: GetDefaultCloudKernelSize.
// GetDensitySFName is a static helper inside ccLibAlgorithms.cpp and is not
// accessible from other translation units; those tests are omitted.

#include <QtTest/QtTest>

#include <ccPointCloud.h>

#include "ccLibAlgorithms.h"
#include "ccCommon.h"



// Helper: build a ccPointCloud from a list of CCVector3 points.
// ccPointCloud::addPoint updates the bounding box automatically.
static ccPointCloud* MakeCloud(const std::vector<CCVector3>& pts)
{
    ccPointCloud* cloud = new ccPointCloud("test");
    cloud->reserve(static_cast<unsigned>(pts.size()));
    for (const auto& p : pts)
        cloud->addPoint(p);
    return cloud;
}



class TestLibAlgorithms : public QObject
{
    Q_OBJECT

  private slots:

    // -----------------------------------------------------------------------
    // GetDefaultCloudKernelSize(ccGenericPointCloud*, unsigned knn)
    //
    // Formula (from ccLibAlgorithms.cpp):
    //   volume     = d[0] * d[1] * d[2]            // d = bounding-box diagonal
    //   surface    = pow(volume, 2.0/3.0)
    //   surfPerPt  = surface / cloud->size()
    //   result     = sqrt(surfPerPt * knn)
    //
    // Test cloud: 8 corners of a cube from (-5,-5,-5) to (5,5,5).
    //   diagonal  = (10, 10, 10)
    //   volume    = 10 * 10 * 10          = 1000
    //   surface   = pow(1000, 2/3)         = 600
    //   surfPerPt = 600 / 8                = 75
    //   knn       = 12
    //   result    = sqrt(75 * 12)          = sqrt(900) = 30
    // -----------------------------------------------------------------------
    void testGetDefaultCloudKernelSize()
    {
        std::vector<CCVector3> corners = {
            CCVector3(-5.0f, -5.0f, -5.0f),
            CCVector3( 5.0f, -5.0f, -5.0f),
            CCVector3(-5.0f,  5.0f, -5.0f),
            CCVector3( 5.0f,  5.0f, -5.0f),
            CCVector3(-5.0f, -5.0f,  5.0f),
            CCVector3( 5.0f, -5.0f,  5.0f),
            CCVector3(-5.0f,  5.0f,  5.0f),
            CCVector3( 5.0f,  5.0f,  5.0f),
        };
        ccPointCloud* cloud = MakeCloud(corners);

        double kernelSize = ccLibAlgorithms::GetDefaultCloudKernelSize(cloud, 12);

        // sqrt(75 * 12) = sqrt(900) = 30
        QBENCHMARK {
            ccLibAlgorithms::GetDefaultCloudKernelSize(cloud, 12);
        }
        QVERIFY2(kernelSize > 0.0, "kernel size must be positive for non-empty cloud");
        QTRY_COMPARE(qAbs(kernelSize - 30.0) < 1e-9, true);

        delete cloud;
    }

    // -----------------------------------------------------------------------
    // GetDefaultCloudKernelSize — empty cloud returns a sentinel value
    // (specifically -CCCoreLib::PC_ONE, which is negative).
    // -----------------------------------------------------------------------
    void testGetDefaultCloudKernelSizeEmptyCloud()
    {
        ccPointCloud emptyCloud("empty");
        double kernelSize = ccLibAlgorithms::GetDefaultCloudKernelSize(&emptyCloud, 12);

        // Sentinel is negative; exact value is implementation-defined.
        QVERIFY2(kernelSize < 0.0, "empty cloud should return negative sentinel");
    }
};



QTEST_MAIN(TestLibAlgorithms)
#include "TestLibAlgorithms.moc"
