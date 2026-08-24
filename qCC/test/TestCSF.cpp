// SPDX-License-Identifier: LGPL-2.0-or-later
// Copyright (C) 2026 — CloudCompare contributors
// Unit tests for qCSF: Vec3, Particle::timeStep, Cloth, Cloud2CloudDist.
// Uses Qt Test — run via Python subprocess with explicit PATH (see BUILD-LOCAL.md §9).

/**
 * @file TestCSF.cpp
 *
 * @brief Unit tests for Cloth Simulation Filter
 *
 * Tests ground/non-ground classification:
 * - Cloth mesh simulation (Verlet integration)
 * - Point-cloth collision detection
 * - Ground vs non-ground classification
 * - Parameter sensitivity (rigidity, resolution)
 *
 * @see qCSF.h
 */
#include <QDebug>
#include <QtTest/QtTest>

// CSF plugin headers (via plugins/core/Standard/qCSF/include/)
#include <CSF.h>
#include <Cloth.h>
#include <Cloud2CloudDist.h>
#include <Particle.h>

// qCC_db for ccMesh (Cloth::toMesh output)
#include <ccMesh.h>
#include <ccPointCloud.h>
#include <cmath>
#include <vector>

// =============================================================================
// TestCSF — Vec3, Particle physics, Cloth, Cloud2CloudDist
// =============================================================================
class TestCSF : public QObject
{
	Q_OBJECT

  private slots:

	// --- Vec3 ---

	void testVec3Length()
	{
		// (3, 4, 0) → length = 5
		Vec3 v(3.0, 4.0, 0.0);
		QVERIFY2(std::abs(v.length() - 5.0) < 1e-9, "3-4-0 triangle should have length 5");

		// (0, 0, 0) → length = 0
		Vec3 zero;
		QVERIFY2(std::abs(zero.length()) < 1e-9, "zero vector length should be 0");

		// (1, 1, 1) → length = √3
		Vec3 diag(1.0, 1.0, 1.0);
		QVERIFY2(std::abs(diag.length() - std::sqrt(3.0)) < 1e-9, "1-1-1 diagonal");
	}

	void testVec3Normalized()
	{
		// (3, 4, 0) → (0.6, 0.8, 0)
		Vec3 v(3.0, 4.0, 0.0);
		Vec3 n = v.normalized();
		QVERIFY2(std::abs(n.length() - 1.0) < 1e-9, "normalized vector should have unit length");
		QVERIFY2(std::abs(n.x - 0.6) < 1e-9, "normalized x");
		QVERIFY2(std::abs(n.y - 0.8) < 1e-9, "normalized y");
		QVERIFY2(std::abs(n.z) < 1e-9, "normalized z");
	}

	void testVec3Arithmetic()
	{
		Vec3 a(1.0, 2.0, 3.0);
		Vec3 b(0.5, 1.5, 2.5);

		Vec3 sum = a + b;
		QVERIFY2(std::abs(sum.x - 1.5) < 1e-9, "sum x");
		QVERIFY2(std::abs(sum.y - 3.5) < 1e-9, "sum y");
		QVERIFY2(std::abs(sum.z - 5.5) < 1e-9, "sum z");

		Vec3 diff = a - b;
		QVERIFY2(std::abs(diff.x - 0.5) < 1e-9, "diff x");
		QVERIFY2(std::abs(diff.y - 0.5) < 1e-9, "diff y");
		QVERIFY2(std::abs(diff.z - 0.5) < 1e-9, "diff z");

		Vec3 scaled = a * 2.0;
		QVERIFY2(std::abs(scaled.x - 2.0) < 1e-9, "scale x");
		QVERIFY2(std::abs(scaled.y - 4.0) < 1e-9, "scale y");
		QVERIFY2(std::abs(scaled.z - 6.0) < 1e-9, "scale z");

		Vec3 divided = scaled / 2.0;
		QVERIFY2(std::abs(divided.x - 1.0) < 1e-9, "divide x");

		Vec3 neg = -a;
		QVERIFY2(std::abs(neg.x + 1.0) < 1e-9, "negate x");
	}

	void testVec3DotCross()
	{
		// Dot product: (1,0,0) · (0,1,0) = 0
		Vec3 ex(1.0, 0.0, 0.0);
		Vec3 ey(0.0, 1.0, 0.0);
		Vec3 ez(0.0, 0.0, 1.0);
		QVERIFY2(std::abs(ex.dot(ey)) < 1e-9, "orthogonal dot product = 0");
		QVERIFY2(std::abs(ex.dot(ex) - 1.0) < 1e-9, "self dot product = 1");

		// Cross product: (1,0,0) × (0,1,0) = (0,0,1)
		Vec3 cross_zy = ey.cross(ez); // (0,1,0) × (0,0,1) = (1,0,0)
		QVERIFY2(std::abs(cross_zy.x - 1.0) < 1e-9, "cross product x");
		QVERIFY2(std::abs(cross_zy.y) < 1e-9, "cross product y");
		QVERIFY2(std::abs(cross_zy.z) < 1e-9, "cross product z");
	}

	// --- Particle::timeStep (Verlet integration) ---

	void testParticleTimeStepMovable()
	{
		// Verlet: pos.y += deltaY * (1-DAMPING) + accel
		// DAMPING = 0.01, so (1-DAMPING) = 0.99
		// Initial: pos.y=5, old_pos_y=5, accel=1.0
		// deltaY = 5-5 = 0
		// pos.y = 5 + 0*0.99 + 1.0 = 6.0
		Vec3 pos(0.0, 5.0, 0.0);
		Particle p(pos);
		p.resetAcceleration();
		p.addForce(1.0);
		p.timeStep();
		QVERIFY2(std::abs(p.getPos().y - 6.0) < 1e-9,
		         "movable particle: pos.y += accel (deltaY=0)");

		// Now old_pos_y=5, pos.y=6, accel=0
		// deltaY = 6-5 = 1
		// pos.y = 6 + 1*0.99 + 0 = 6.99
		p.resetAcceleration();
		p.timeStep();
		QVERIFY2(std::abs(p.getPos().y - 6.99) < 1e-9,
		         "movable particle: pos.y += deltaY * 0.99");
	}

	void testParticleTimeStepUnmovable()
	{
		// Pinned particle should not move
		Vec3 pos(0.0, 10.0, 0.0);
		Particle p(pos);
		p.makeUnmovable();
		p.resetAcceleration();
		p.addForce(100.0);

		double y_before = p.getPos().y;
		p.timeStep();
		QVERIFY2(std::abs(p.getPos().y - y_before) < 1e-9,
		         "unmovable particle should not change position");
	}

	void testParticleOffsetPos()
	{
		Vec3 pos(0.0, 5.0, 0.0);
		Particle p(pos);

		// Movable: offset works
		p.offsetPos(2.0);
		QVERIFY2(std::abs(p.getPos().y - 7.0) < 1e-9, "movable: offset changes Y");

		// Unmovable: offset ignored
		p.makeUnmovable();
		double y_before = p.getPos().y;
		p.offsetPos(100.0);
		QVERIFY2(std::abs(p.getPos().y - y_before) < 1e-9, "unmovable: offset ignored");
	}

	// --- Cloth terrain collision ---

	void testClothTerrainCollision()
	{
		// Create a minimal 2x2 cloth at origin
		Cloth cloth(Vec3(0.0, 10.0, 0.0), 2, 2, 1.0, 1.0, 0.3, 9999.0, 1);

		// Set heightvals: all cloth particles above terrain
		// Particle (0,0): cloth y=10, terrain=5 → no collision
		// Particle (1,0): cloth y=10, terrain=5 → no collision
		// Particle (0,1): cloth y=10, terrain=5 → no collision
		// Particle (1,1): cloth y=10, terrain=5 → no collision
		std::vector<double> hv(4, 5.0);
		cloth.setheightvals(hv);

		cloth.terrainCollision();

		// All particles at y=10 (above terrain 5) — no change
		for (int i = 0; i < cloth.getSize(); ++i)
		{
			const Particle& p = cloth.getParticleByIndex(i);
			QVERIFY2(std::abs(p.getPos().y - 10.0) < 1e-9,
			         qPrintable(QString("particle %1 above terrain: y=%2").arg(i).arg(p.getPos().y)));
		}

		// Now set all particles BELOW terrain (should be clamped)
		// Cloth y=10, terrain=12 → particle should be clamped to y=12
		hv.assign(4, 12.0);
		cloth.setheightvals(hv);
		cloth.terrainCollision();

		for (int i = 0; i < cloth.getSize(); ++i)
		{
			const Particle& p = cloth.getParticleByIndex(i);
			QVERIFY2(std::abs(p.getPos().y - 12.0) < 1e-9,
			         qPrintable(QString("particle %1 clamped to terrain: y=%2").arg(i).arg(p.getPos().y)));
		}
	}

	void testClothTerrainCollisionMixed()
	{
		// In Cloth's coordinate system: y varies with grid-i (step_x), z varies with grid-j (step_y).
		// world.y = origin_pos.y + i*step_x (i=row), world.z = origin_pos.z + j*step_y (j=col)
		// To get different world.y values, use offsetPos() after construction.
		// 2x2 cloth: all at origin y=10 initially, then offset each particle.
		Cloth cloth(Vec3(0.0, 10.0, 0.0), 2, 2, 1.0, 1.0, 0.3, 9999.0, 1);

		// Set world y values via offsetPos:
		// index 0: world y=10  hv[0]=12  → clamp  → y=12
		// index 1: world y=10  hv[1]=8   → no clamp → y=10
		// index 2: world y=11  hv[2]=13  → clamp  → y=13
		// index 3: world y=11  hv[3]=9   → no clamp → y=11
		cloth.getParticleByIndex(2).offsetPos(1.0); // y=10→11
		cloth.getParticleByIndex(3).offsetPos(1.0); // y=10→11

		std::vector<double> hv = {12.0, 8.0, 13.0, 9.0};
		cloth.setheightvals(hv);
		cloth.terrainCollision();

		const Particle& p0 = cloth.getParticleByIndex(0);
		const Particle& p1 = cloth.getParticleByIndex(1);
		const Particle& p2 = cloth.getParticleByIndex(2);
		const Particle& p3 = cloth.getParticleByIndex(3);

		QVERIFY2(std::abs(p0.getPos().y - 12.0) < 1e-6, "index 0 clamped to 12");
		QVERIFY2(std::abs(p1.getPos().y - 10.0) < 1e-6, "index 1 not clamped (at 10)");
		QVERIFY2(std::abs(p2.getPos().y - 13.0) < 1e-6, "index 2 clamped to 13");
		QVERIFY2(std::abs(p3.getPos().y - 11.0) < 1e-6, "index 3 not clamped (at 11)");
	}

	// --- Cloth toMesh ---

	void testClothToMesh()
	{
		// 3x3 cloth → 9 vertices, 8 quads → 16 triangles
		Cloth cloth(Vec3(0.0, 0.0, 0.0), 3, 3, 1.0, 1.0, 0.3, 9999.0, 1);

		// Deform cloth slightly: set y based on x,y grid position
		for (int x = 0; x < 3; ++x)
			for (int y = 0; y < 3; ++y)
			{
				Particle& p = cloth.getParticle(x, y);
				p.offsetPos(static_cast<double>(x + y)); // y += x+y
			}

		ccMesh* mesh = cloth.toMesh();

		QVERIFY2(mesh != nullptr, "toMesh should return a valid mesh");

		// Check vertex count: 9 vertices for 3x3 grid
		QVERIFY2(mesh->getAssociatedCloud() != nullptr, "mesh must have associated cloud");
		ccPointCloud* verts = static_cast<ccPointCloud*>(mesh->getAssociatedCloud());
		QVERIFY2(verts->size() == 9, qPrintable(QString("mesh should have 9 vertices, got %1").arg(verts->size())));

		// Check triangle count: (3-1)*(3-1) = 4 quads, each quad → 2 triangles = 8 triangles
		QVERIFY2(mesh->size() == 8, qPrintable(QString("mesh should have 8 triangles, got %1").arg(mesh->size())));

		delete mesh;
	}

	void testClothToMeshSingleCell()
	{
		// 2x2 cloth → 4 vertices, 1 quad → 2 triangles
		Cloth cloth(Vec3(0.0, 0.0, 0.0), 2, 2, 1.0, 1.0, 0.3, 9999.0, 1);
		ccMesh* mesh = cloth.toMesh();

		QVERIFY2(mesh != nullptr, "single-cell cloth should produce mesh");
		ccPointCloud* verts = static_cast<ccPointCloud*>(mesh->getAssociatedCloud());
		QVERIFY2(verts->size() == 4, qPrintable(QString("2x2 cloth: 4 vertices, got %1").arg(verts->size())));
		QVERIFY2(mesh->size() == 2, qPrintable(QString("2x2 cloth: 2 triangles, got %1").arg(mesh->size())));

		delete mesh;
	}

	// --- Cloud2CloudDist::Compute (flat ground) ---

	void testCloud2CloudDistFlatGround()
	{
		// All points are at y=0 (terrain), cloth surface is at y=0
		// All points should be classified as GROUND (within class_threshold)
		Cloth cloth(Vec3(0.0, 5.0, 0.0), 2, 2, 1.0, 1.0, 0.3, 9999.0, 1);

		// Set cloth particles just above the point cloud
		for (int i = 0; i < cloth.getSize(); ++i)
		{
			Particle& p = cloth.getParticleByIndex(i);
			p.offsetPos(-5.0); // move from y=5 to y=0
		}

		// Build point cloud: 4 points at (0,0,0), (1,0,0), (0,1,0), (1,1,0)
		wl::PointCloud pc;
		wl::Point p0{};
		p0.x = 0.0f;
		p0.y = 0.0f;
		p0.z = 0.0f;
		wl::Point p1{};
		p1.x = 1.0f;
		p1.y = 0.0f;
		p1.z = 0.0f;
		wl::Point p2{};
		p2.x = 0.0f;
		p2.y = 0.0f;
		p2.z = 1.0f;
		wl::Point p3{};
		p3.x = 1.0f;
		p3.y = 0.0f;
		p3.z = 1.0f;
		pc.push_back(p0);
		pc.push_back(p1);
		pc.push_back(p2);
		pc.push_back(p3);

		std::vector<bool> isGround;
		bool ok = Cloud2CloudDist::Compute(cloth, pc, 0.5, isGround);

		QVERIFY2(ok, "Cloud2CloudDist::Compute should succeed");
		QVERIFY2(isGround.size() == 4, "should classify all 4 points");
		for (size_t i = 0; i < isGround.size(); ++i)
		{
			QVERIFY2(isGround[i],
			         qPrintable(QString("point %1 should be ground (flat terrain)").arg(i)));
		}
	}

	void testCloud2CloudDistPointAboveCloth()
	{
		// Point far above cloth → NOT ground (distance > threshold)
		Cloth cloth(Vec3(0.0, 5.0, 0.0), 2, 2, 1.0, 1.0, 0.3, 9999.0, 1);

		// Cloth particles at y=5 (unchanged)
		// Point at y=10 (5 units above cloth)
		wl::PointCloud pc;
		wl::Point above{};
		above.x = 0.5f;
		above.y = 10.0f;
		above.z = 0.5f;
		pc.push_back(above);

		std::vector<bool> isGround;
		bool ok = Cloud2CloudDist::Compute(cloth, pc, 0.5, isGround);

		QVERIFY2(ok, "Cloud2CloudDist::Compute should succeed");
		QVERIFY2(!isGround[0], "point 5 units above cloth: not ground");
	}

	// --- Cloth::addForce + timeStep integration ---

	void testClothTimeStepDamping()
	{
		// Add force to all movable particles in a cloth
		// Force = -gravity * dt^2 (negative = downward in CSF coords)
		// After timeStep: pos.y += deltaY * 0.99 + force
		Cloth cloth(Vec3(0.0, 10.0, 0.0), 2, 2, 1.0, 1.0, 0.3, 9999.0, 1);

		// Capture y positions before
		double y_before = cloth.getParticle(0, 0).getPos().y;
		QVERIFY2(std::abs(y_before - 10.0) < 1e-9, "initial y");

		// Add downward force
		double squareTimeStep = 0.65 * 0.65;   // time_step^2
		cloth.addForce(-0.2 * squareTimeStep); // -gravity * dt^2

		double maxDiff = cloth.timeStep();
		QVERIFY2(maxDiff >= 0, "maxDiff should be >= 0");

		double y_after = cloth.getParticle(0, 0).getPos().y;
		QVERIFY2(y_after < y_before, "downward force should decrease y");
	}
};

QTEST_MAIN(TestCSF)
#include "TestCSF.moc"
