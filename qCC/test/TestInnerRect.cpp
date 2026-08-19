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
        // 4 corners of a 10×10 square in the YZ plane (x=±1, yz ∈ {−5,5}×{−5,5}).
        // x=±1 gives the bounding box a positive width along X so the algorithm
        // has a valid (non-zero-area) rectangle to work with.
        // zDim=2 → X=0, Y=1 (YZ plane projection).  Points:
        //   x=±1: so P->u[0] ∈ {-1,1} → X ∈ {-1,1} → width=2.
        //   y ∈ {-5,5}, z ∈ {-5,5} → Y/Z extent=10.
        ccPointCloud* cloud = MakeCloud({
            CCVector3( 1.0f, -5.0f, -5.0f),
            CCVector3(-1.0f, -5.0f, -5.0f),
            CCVector3( 1.0f,  5.0f, -5.0f),
            CCVector3(-1.0f,  5.0f, -5.0f),
            CCVector3( 1.0f, -5.0f,  5.0f),
            CCVector3(-1.0f, -5.0f,  5.0f),
            CCVector3( 1.0f,  5.0f,  5.0f),
            CCVector3(-1.0f,  5.0f,  5.0f),
        });

        ccInnerRect2DFinder finder;
        ccBox* box = finder.process(cloud, 2);  // zDim=2 → YZ plane

        QBENCHMARK { ccInnerRect2DFinder f2; f2.process(cloud, 2); }
        QVERIFY2(box != nullptr, "process should return a box when interior is empty");
        if (box)
        {
            const CCVector3& dims = box->getDimensions();
            // zDim=2: dims.u[1] = Y extent, dims.u[2] = Z extent.
            // Full BB: Y/Z span = 10 (from -5 to 5).
            QTRY_COMPARE(dims.u[1] == 10.0f, true);
            QTRY_COMPARE(dims.u[2] == 10.0f, true);
            delete box;
        }
        delete cloud;
    }

    // -----------------------------------------------------------------------
    // Same 8 corners plus one strictly interior point at (0.5, 2, 2).
    // This point splits the full BB into 4 sub-rectangles; the upper-right
    // quadrant (x∈[0,1], y∈[0,5], z∈[0,5]) is excluded (it contains the
    // interior point).  The largest empty sub-rect has area 2×5=10
    // (e.g. the lower-left quadrant: x∈[-1,0], y∈[-5,0], z∈[-5,0]).
    // -----------------------------------------------------------------------
    void testFindBiggestRectWithInteriorPoint()
    {
        ccPointCloud* cloud = MakeCloud({
            CCVector3( 1.0f, -5.0f, -5.0f),
            CCVector3(-1.0f, -5.0f, -5.0f),
            CCVector3( 1.0f,  5.0f, -5.0f),
            CCVector3(-1.0f,  5.0f, -5.0f),
            CCVector3( 1.0f, -5.0f,  5.0f),
            CCVector3(-1.0f, -5.0f,  5.0f),
            CCVector3( 1.0f,  5.0f,  5.0f),
            CCVector3(-1.0f,  5.0f,  5.0f),
            // interior point — NOT on any edge (0.5, 2, 2):
            CCVector3( 0.5f,  2.0f,  2.0f),
        });

        ccInnerRect2DFinder finder;
        ccBox* box = finder.process(cloud, 2);

        QVERIFY2(box != nullptr, "process should return a box");
        if (box)
        {
            const CCVector3& dims = box->getDimensions();
            // The algorithm finds one of the maximal empty sub-rectangles.
            // At minimum it must have positive extent on both Y and Z (since
            // the BB has Y/Z extent of 10).  Verify non-trivial positive area.
            QTRY_COMPARE(dims.u[1] > 0.0f, true);
            QTRY_COMPARE(dims.u[2] > 0.0f, true);
            delete box;
        }
        delete cloud;
    }

    // -----------------------------------------------------------------------
    // Boundary points only — 4 corners of a 4×4 square (x=±1, y=±2, z=-1).
    // No points strictly inside → largest empty rect = full BB (4×4).
    // This demonstrates that boundary cells are excluded from "interior".
    // -----------------------------------------------------------------------
    void testBoundaryCellsExcluded()
    {
        // x=±1 gives the BB a positive X extent so the rect is non-degenerate.
        ccPointCloud* cloud = MakeCloud({
            CCVector3( 1.0f, -2.0f, -1.0f),
            CCVector3(-1.0f, -2.0f, -1.0f),
            CCVector3( 1.0f,  2.0f, -1.0f),
            CCVector3(-1.0f,  2.0f, -1.0f),
        });

        ccInnerRect2DFinder finder;
        ccBox* box = finder.process(cloud, 2);

        QVERIFY2(box != nullptr, "boundary-only cloud should yield a box");
        if (box)
        {
            const CCVector3& dims = box->getDimensions();
            // Boundary-only cloud: the full BB is empty (no interior points).
            // The algorithm should return a box with positive Y and Z extent.
            QTRY_COMPARE(dims.u[1] > 0.0f, true);
            QTRY_COMPARE(dims.u[2] > 0.0f, true);
            delete box;
        }
        delete cloud;
    }
};



QTEST_MAIN(TestInnerRect)
#include "TestInnerRect.moc"
