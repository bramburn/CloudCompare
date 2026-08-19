// SPDX-License-Identifier: LGPL-2.0-or-later

// Copyright (C) 2026 — CloudCompare contributors

// Unit tests for ccInnerRect2DFinder.
// The algorithm finds the largest axis-aligned rectangle (in a cloud's bounding
// box) that contains NO points strictly inside it.  It takes a
// ccGenericPointCloud* and an axis index (zDim, 0–2); the remaining two axes
// form the 2-D search plane.

#include <QtTest/QtTest>

#include <ccPointCloud.h>
#include <ccBox.h>

#include "ccInnerRect2DFinder.h"



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



class TestInnerRect : public QObject
{
    Q_OBJECT

  private slots:

    // -----------------------------------------------------------------------
    // process() — cloud with 4 corner points of a 10×10 square in
    // the YZ plane (zDim=2).  No points fall strictly inside the bounding
    // rectangle, so the largest empty rectangle is the full bounding box:
    // width = 10, height = 10, area = 100.
    // -----------------------------------------------------------------------
    void testFindBiggestRectCornersOnly()
    {
        // 4 corners of a square in the YZ plane (x=0, yz ∈ {−5,5}×{−5,5})
        ccPointCloud* cloud = MakeCloud({
            CCVector3(0.0f, -5.0f, -5.0f),
            CCVector3(0.0f,  5.0f, -5.0f),
            CCVector3(0.0f, -5.0f,  5.0f),
            CCVector3(0.0f,  5.0f,  5.0f),
        });

        ccInnerRect2DFinder finder;
        ccBox* box = finder.process(cloud, 2);  // zDim=2 → YZ plane

        QBENCHMARK { ccInnerRect2DFinder f2; f2.process(cloud, 2); }
        QVERIFY2(box != nullptr, "process should return a box when interior is empty");
        if (box)
        {
            const CCVector3& dims = box->getDimensions();
            // dims.u[0] = X extent = 0 (rect covers full X range)
            // dims.u[1] = Y extent = 10 (full Y range -5..5)
            // dims.u[2] = Z extent = 10 (full Z range -5..5)
            // QTRY_COMPARE takes exactly 2 args: actual, expected
            QTRY_COMPARE(dims.u[1] == 10.0f, true);
            QTRY_COMPARE(dims.u[2] == 10.0f, true);
            delete box;
        }
        delete cloud;
    }

    // -----------------------------------------------------------------------
    // Same 4 corners plus a point at the exact centre (0, 0, 0).
    // The centre point splits the bounding box into 4 sub-rectangles;
    // none of them contains another interior point, so each has area 25.
    // The first one explored (left sub-rect) is returned: width = 5, height = 5.
    // -----------------------------------------------------------------------
    void testFindBiggestRectWithInteriorPoint()
    {
        ccPointCloud* cloud = MakeCloud({
            CCVector3(0.0f, -5.0f, -5.0f),
            CCVector3(0.0f,  5.0f, -5.0f),
            CCVector3(0.0f, -5.0f,  5.0f),
            CCVector3(0.0f,  5.0f,  5.0f),
            CCVector3(0.0f,  0.0f,  0.0f),  // interior point — splits the rect
        });

        ccInnerRect2DFinder finder;
        ccBox* box = finder.process(cloud, 2);

        QVERIFY2(box != nullptr, "process should return a box");
        if (box)
        {
            const CCVector3& dims = box->getDimensions();
            // After splitting around (0,0): left sub-rect (-5,-5)-(0,0)
            // width = 5, height = 5
            QTRY_COMPARE(dims.u[1] == 5.0f, true);
            QTRY_COMPARE(dims.u[2] == 5.0f, true);
            delete box;
        }
        delete cloud;
    }

    // -----------------------------------------------------------------------
    // Boundary points only (ring) — interior is empty, full BB is the answer.
    // This demonstrates that boundary cells are excluded from "interior".
    // -----------------------------------------------------------------------
    void testBoundaryCellsExcluded()
    {
        // 4 corners of a 4×4 square in the YZ plane.
        // Bounding box: y∈{-2,2}, z∈{-2,2}.
        // No points strictly inside → largest empty rect = 4×4.
        ccPointCloud* cloud = MakeCloud({
            CCVector3(0.0f, -2.0f, -2.0f),
            CCVector3(0.0f,  2.0f, -2.0f),
            CCVector3(0.0f, -2.0f,  2.0f),
            CCVector3(0.0f,  2.0f,  2.0f),
        });

        ccInnerRect2DFinder finder;
        ccBox* box = finder.process(cloud, 2);

        QVERIFY2(box != nullptr, "boundary-only cloud should yield a box");
        if (box)
        {
            const CCVector3& dims = box->getDimensions();
            // Full bounding box: Y span = 4, Z span = 4
            QTRY_COMPARE(dims.u[1] == 4.0f, true);
            QTRY_COMPARE(dims.u[2] == 4.0f, true);
            delete box;
        }
        delete cloud;
    }
};



QTEST_MAIN(TestInnerRect)
#include "TestInnerRect.moc"
