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
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        #
// #  GNU General Public License for more details.                          #
// #                                                                        #
// #                   COPYRIGHT: CloudCompare project                      #
// #                                                                        #
// ##########################################################################

#include <ccMaterial.h>
#include <ccColorTypes.h>

#include <QTest>
#include <QString>

class TestMaterial : public QObject
{
	Q_OBJECT

  private slots:

	// testSetTransparency — setTransparency(0.5) → all color alphas = 0.5
	void testSetTransparency()
	{
		ccMaterial mat("test");

		mat.setTransparency(0.5f);

		// diffuseFront
		QCOMPARE(mat.getDiffuseFront().r, 1.0f); // ccColor::bright.r
		QCOMPARE(mat.getDiffuseFront().g, 1.0f); // ccColor::bright.g
		QCOMPARE(mat.getDiffuseFront().b, 1.0f); // ccColor::bright.b
		QCOMPARE(mat.getDiffuseFront().a, 0.5f);

		// diffuseBack
		QCOMPARE(mat.getDiffuseBack().a, 0.5f);

		// ambient
		QCOMPARE(mat.getAmbient().a, 0.5f);

		// specular
		QCOMPARE(mat.getSpecular().a, 0.5f);

		// emission
		QCOMPARE(mat.getEmission().a, 0.5f);
	}

	// testSetShininess — setShininess(50) → front=50, back=40
	void testSetShininess()
	{
		ccMaterial mat("test");

		mat.setShininess(50.0f);

		QCOMPARE(mat.getShininessFront(), 50.0f);
		QCOMPARE(mat.getShininessBack(), 40.0f); // 0.8 * 50
	}

	// testSetDiffuse — sets both front and back diffuse to the same color
	void testSetDiffuse()
	{
		ccMaterial mat("test");

		ccColor::Rgbaf redHalfAlpha(1.0f, 0.0f, 0.0f, 0.7f);
		mat.setDiffuse(redHalfAlpha);

		// Both front and back should be set
		QCOMPARE(mat.getDiffuseFront().r, 1.0f);
		QCOMPARE(mat.getDiffuseFront().g, 0.0f);
		QCOMPARE(mat.getDiffuseFront().b, 0.0f);
		QCOMPARE(mat.getDiffuseFront().a, 0.7f);

		QCOMPARE(mat.getDiffuseBack().r, 1.0f);
		QCOMPARE(mat.getDiffuseBack().g, 0.0f);
		QCOMPARE(mat.getDiffuseBack().b, 0.0f);
		QCOMPARE(mat.getDiffuseBack().a, 0.7f);
	}

	// testTransparencyRoundtrip — setTransparency → getDiffuseFront().a returns same value
	void testTransparencyRoundtrip()
	{
		ccMaterial mat("test");

		mat.setTransparency(0.25f);
		QCOMPARE(mat.getDiffuseFront().a, 0.25f);

		mat.setTransparency(0.85f);
		QCOMPARE(mat.getDiffuseFront().a, 0.85f);

		mat.setTransparency(0.0f);
		QCOMPARE(mat.getDiffuseFront().a, 0.0f);
	}

	// testShininessRoundtrip — setShininess → getShininessFront returns same value
	void testShininessRoundtrip()
	{
		ccMaterial mat("test");

		mat.setShininess(10.0f);
		QCOMPARE(mat.getShininessFront(), 10.0f);

		mat.setShininess(128.0f);
		QCOMPARE(mat.getShininessFront(), 128.0f);

		mat.setShininess(0.0f);
		QCOMPARE(mat.getShininessFront(), 0.0f);
	}

	// testMaterialWithTransparency — material with partial alpha → alpha != 1.0
	void testMaterialWithTransparency()
	{
		ccMaterial mat("test");

		mat.setTransparency(0.6f);

		// alpha should be less than 1.0
		QVERIFY(mat.getDiffuseFront().a < 1.0f);
		QCOMPARE(mat.getDiffuseFront().a, 0.6f);

		// diffuseBack should also have the same alpha
		QCOMPARE(mat.getDiffuseBack().a, 0.6f);
	}

	// testDefaultTransparency — new material → alpha=1.0 (fully opaque)
	void testDefaultTransparency()
	{
		ccMaterial mat("default");

		// Default diffuseFront is ccColor::bright = (1,1,1,1)
		QCOMPARE(mat.getDiffuseFront().a, 1.0f);
		QCOMPARE(mat.getDiffuseFront().r, 1.0f);
		QCOMPARE(mat.getDiffuseFront().g, 1.0f);
		QCOMPARE(mat.getDiffuseFront().b, 1.0f);
	}
};

QTEST_MAIN(TestMaterial)
#include "TestMaterial.moc"
