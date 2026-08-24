// ##########################################################################
// #                                                                        #
// #                              CLOUDCOMPARE                              #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  the Free Software Foundation; version 2 or later of the License.      #
// #                                                                        #
// #  This program is distributed in the hope that it will be useful,       #
// #  WITHOUT ANY WARRANTY; without even the implied warranty of            #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
// #  GNU General Public License for more details.                          #
// #                                                                        #
// #          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
// #                                                                        //
// ##########################################################################

/**
 * @file ccColorTypes.h
 *
 * @brief Color type definitions and utilities.
 *
 * @details Defines color types and utilities including RGB,
 * RGBA, predefined color constants, and color space conversions.
 *
 * ## Color Types
 *
 * ### Component Types
 * - **ColorCompType**: unsigned char (0-255)
 *
 * ### RGB Types
 * - **Rgb**: 3 components, default byte type
 * - **Rgbf**: 3 components, float type
 * - **Rgbub**: 3 components, unsigned byte
 *
 * ### RGBA Types
 * - **Rgba**: 4 components, default byte type
 * - **Rgbaf**: 4 components, float type
 * - **Rgbaub**: 4 components, unsigned byte
 *
 * ## Predefined Colors
 *
 * RGB: whiteRGB, blackRGB, redRGB, greenRGB, blueRGB, etc.
 * RGBA: white, black, red, green, blue, etc.
 * Materials (float): bright, lighter, light, middle, dark, etc.
 *
 * ## Color Space Conversion
 *
 * - HSL to RGB
 * - HSV to RGB
 * - QColor / QRgb conversions
 *
 * ## Usage
 *
 * @code
 * // Create RGB color
 * ccColor::Rgb color(255, 0, 0);
 *
 * // Create RGBA color
 * ccColor::Rgba colorA(255, 0, 0, 128);
 *
 * // Use predefined colors
 * ccColor::Rgb red = ccColor::redRGB;
 *
 * // HSL to RGB
 * ccColor::Rgb hsl = ccColor::Convert::hsl2rgb(180.0f, 1.0f, 0.5f);
 *
 * // QColor conversion
 * QColor qColor = QColor(255, 0, 0);
 * ccColor::Rgb rgb = ccColor::FromQColor(qColor);
 * @endcode
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see ccColorScale for color scale management
 */

#ifndef CC_COLOR_TYPES_HEADER
#define CC_COLOR_TYPES_HEADER

// Local
#include "qCC_db.h"

// Qt
#include <QColor>

/**
 * @brief Color component type (0-255).
 */
using ColorCompType = unsigned char;

/**
 * @brief Color utilities namespace.
 */
namespace ccColor
{
	//! Maximum color component value.
	constexpr ColorCompType MAX = 255;

	//! Maximum color component value (int).
	constexpr int MAXi = 255;

	/**
	 * @brief RGB color template.
	 *
	 * @tparam Type Component type (e.g., unsigned char, float).
	 */
	template <typename Type>
	class RgbTpl
	{
	  public:
		//! RGB components union.
		union
		{
			//! Named components.
			struct
			{
				Type r, g, b;
			};
			//! Array access.
			Type rgb[3];
		};

		/**
		 * @brief Default constructor (black).
		 */
		constexpr inline RgbTpl()
		    : r(0)
		    , g(0)
		    , b(0)
		{
		}

		/**
		 * @brief Constructor from RGB values.
		 *
		 * @param[in] red Red component.
		 * @param[in] green Green component.
		 * @param[in] blue Blue component.
		 */
		explicit constexpr inline RgbTpl(Type red, Type green, Type blue)
		    : r(red)
		    , g(green)
		    , b(blue)
		{
		}

		/**
		 * @brief Constructor from array.
		 *
		 * @param[in] col Array of 3 values.
		 */
		explicit constexpr inline RgbTpl(const Type col[3])
		    : r(col[0])
		    , g(col[1])
		    , b(col[2])
		{
		}

		/**
		 * @brief Not-equal comparison.
		 */
		inline bool operator!=(const RgbTpl<Type>& t) const
		{
			return (r != t.r || g != t.g || b != t.b);
		}
	};

	//! 3 components, float type.
	using Rgbf = RgbTpl<float>;

	//! 3 components, unsigned byte type.
	using Rgbub = RgbTpl<unsigned char>;

	//! 3 components, default byte type.
	using Rgb = RgbTpl<ColorCompType>;

	/**
	 * @brief RGBA color template.
	 *
	 * @tparam Type Component type.
	 */
	template <class Type>
	class RgbaTpl
	{
	  public:
		//! RGBA components union.
		union
		{
			//! Named components.
			struct
			{
				Type r, g, b, a;
			};
			//! Array access.
			Type rgba[4];
		};

		/**
		 * @brief Default constructor (transparent black).
		 */
		constexpr inline RgbaTpl()
		    : r(0)
		    , g(0)
		    , b(0)
		    , a(0)
		{
		}

		/**
		 * @brief Constructor from RGBA values.
		 *
		 * @param[in] red Red component.
		 * @param[in] green Green component.
		 * @param[in] blue Blue component.
		 * @param[in] alpha Alpha component.
		 */
		explicit constexpr inline RgbaTpl(Type red, Type green, Type blue, Type alpha)
		    : r(red)
		    , g(green)
		    , b(blue)
		    , a(alpha)
		{
		}

		/**
		 * @brief Constructor from array.
		 *
		 * @param[in] col Array of 4 values.
		 */
		explicit constexpr inline RgbaTpl(const Type col[4])
		    : r(col[0])
		    , g(col[1])
		    , b(col[2])
		    , a(col[3])
		{
		}

		/**
		 * @brief Constructor from RGB + alpha.
		 *
		 * @param[in] col RGB array (3 values).
		 * @param[in] alpha Alpha value.
		 */
		explicit constexpr inline RgbaTpl(const Type col[3], Type alpha)
		    : r(col[0])
		    , g(col[1])
		    , b(col[2])
		    , a(alpha)
		{
		}

		/**
		 * @brief Copy constructor from RGB.
		 *
		 * @param[in] c RGB color.
		 * @param[in] alpha Alpha value.
		 */
		inline RgbaTpl(const RgbTpl<Type>& c, Type alpha)
		    : r(c.r)
		    , g(c.g)
		    , b(c.b)
		    , a(alpha)
		{
		}

		/**
		 * @brief Cast to RGB.
		 */
		inline operator RgbTpl<Type>() const
		{
			return RgbTpl<Type>(rgba);
		}

		/**
		 * @brief Not-equal comparison.
		 */
		inline bool operator!=(const RgbaTpl<Type>& t) const
		{
			return (r != t.r || g != t.g || b != t.b || a != t.a);
		}
	};

	//! 4 components, float type.
	using Rgbaf = RgbaTpl<float>;

	//! 4 components, unsigned byte type.
	using Rgbaub = RgbaTpl<unsigned char>;

	//! 4 components, default byte type.
	using Rgba = RgbaTpl<ColorCompType>;

	// Predefined RGB colors
	//! White RGB color.
	constexpr Rgb whiteRGB(MAX, MAX, MAX);
	//! Light grey RGB color.
	constexpr Rgb lightGreyRGB(static_cast<ColorCompType>(MAXi * 8 / 10), static_cast<ColorCompType>(MAXi * 8 / 10), static_cast<ColorCompType>(MAXi * 8 / 10));
	//! Dark grey RGB color.
	constexpr Rgb darkGreyRGB(MAX / 2, MAX / 2, MAX / 2);
	//! Red RGB color.
	constexpr Rgb redRGB(MAX, 0, 0);
	//! Green RGB color.
	constexpr Rgb greenRGB(0, MAX, 0);
	//! Blue RGB color.
	constexpr Rgb blueRGB(0, 0, MAX);
	//! CloudCompare blue RGB color.
	constexpr Rgb blueCCRGB(0, static_cast<ColorCompType>(MAXi * 7 / 10), MAX);
	//! Dark blue RGB color.
	constexpr Rgb darkBlueRGB(0, 0, MAX / 2);
	//! Magenta RGB color.
	constexpr Rgb magentaRGB(MAX, 0, MAX);
	//! Cyan RGB color.
	constexpr Rgb cyanRGB(0, MAX, MAX);
	//! Orange RGB color.
	constexpr Rgb orangeRGB(MAX, MAX / 2, 0);
	//! Black RGB color.
	constexpr Rgb blackRGB(0, 0, 0);
	//! Yellow RGB color.
	constexpr Rgb yellowRGB(MAX, MAX, 0);

	// Predefined RGBA colors
	//! White RGBA color.
	constexpr Rgba white(whiteRGB.r, whiteRGB.g, whiteRGB.b, MAX);
	//! Light grey RGBA color.
	constexpr Rgba lightGrey(lightGreyRGB.r, lightGreyRGB.g, lightGreyRGB.b, MAX);
	//! Dark grey RGBA color.
	constexpr Rgba darkGrey(darkGreyRGB.r, darkGreyRGB.g, darkGreyRGB.b, MAX);
	//! Red RGBA color.
	constexpr Rgba red(redRGB.r, redRGB.g, redRGB.b, MAX);
	//! Green RGBA color.
	constexpr Rgba green(greenRGB.r, greenRGB.g, greenRGB.b, MAX);
	//! Blue RGBA color.
	constexpr Rgba blue(blueRGB.r, blueRGB.g, blueRGB.b, MAX);
	//! CloudCompare blue RGBA color.
	constexpr Rgba blueCC(blueCCRGB.r, blueCCRGB.g, blueCCRGB.b, MAX);
	//! Dark blue RGBA color.
	constexpr Rgba darkBlue(darkBlueRGB.r, darkBlueRGB.g, darkBlueRGB.b, MAX);
	//! Magenta RGBA color.
	constexpr Rgba magenta(magentaRGB.r, magentaRGB.g, magentaRGB.b, MAX);
	//! Cyan RGBA color.
	constexpr Rgba cyan(cyanRGB.r, cyanRGB.g, cyanRGB.b, MAX);
	//! Orange RGBA color.
	constexpr Rgba orange(orangeRGB.r, orangeRGB.g, orangeRGB.b, MAX);
	//! Black RGBA color.
	constexpr Rgba black(blackRGB.r, blackRGB.g, blackRGB.b, MAX);
	//! Yellow RGBA color.
	constexpr Rgba yellow(yellowRGB.r, yellowRGB.g, yellowRGB.b, MAX);

	// Predefined material colors (float)
	//! Bright material.
	constexpr Rgbaf bright(1.00f, 1.00f, 1.00f, 1.00f);
	//! Lighter material.
	constexpr Rgbaf lighter(0.83f, 0.83f, 0.83f, 1.00f);
	//! Light material.
	constexpr Rgbaf light(0.66f, 0.66f, 0.66f, 1.00f);
	//! Middle material.
	constexpr Rgbaf middle(0.50f, 0.50f, 0.50f, 1.00f);
	//! Dark material.
	constexpr Rgbaf dark(0.34f, 0.34f, 0.34f, 1.00f);
	//! Darker material.
	constexpr Rgbaf darker(0.17f, 0.17f, 0.17f, 1.00f);
	//! Darkest material.
	constexpr Rgbaf darkest(0.08f, 0.08f, 0.08f, 1.00f);
	//! Night material.
	constexpr Rgbaf night(0.00f, 0.00f, 0.00f, 1.00F);
	//! Default mesh front diffuse.
	constexpr Rgbaf defaultMeshFrontDiff(0.00f, 0.90f, 0.27f, 1.00f);
	//! Default mesh back diffuse.
	constexpr Rgbaf defaultMeshBackDiff(0.27f, 0.90f, 0.90f, 1.00f);

	//! Default background color (dark blue).
	constexpr Rgbub defaultBkgColor(10, 102, 151);
	//! Default foreground color (white).
	constexpr Rgba defaultColor(MAX, MAX, MAX, MAX);
	//! Default label background color.
	constexpr Rgba defaultLabelBkgColor(MAX, MAX, MAX, MAX);
	//! Default label marker color (magenta).
	constexpr Rgba defaultLabelMarkerColor(MAX, 0, MAX, MAX);

	/**
	 * @brief Colors generator.
	 */
	class Generator
	{
	  public:
		/**
		 * @brief Generate random color.
		 *
		 * @param[in] lightOnly Only generate light colors.
		 *
		 * @return Random RGB color.
		 */
		QCC_DB_LIB_API static Rgb Random(bool lightOnly = true);
	};

	/**
	 * @brief Color space conversion utilities.
	 */
	class Convert
	{
	  public:
		/**
		 * @brief HSL to RGB conversion.
		 *
		 * @param[in] H Hue [0, 360].
		 * @param[in] S Saturation [0, 1].
		 * @param[in] L Lightness [0, 1].
		 *
		 * @return RGB color (unsigned byte).
		 */
		static Rgb hsl2rgb(float H, float S, float L)
		{
			H /= 360;
			float q = L < 0.5f ? L * (1.0f + S) : L + S - L * S;
			float p = 2 * L - q;

			float r = hue2rgb(p, q, H + 1.0f / 3.0f);
			float g = hue2rgb(p, q, H);
			float b = hue2rgb(p, q, H - 1.0f / 3.0f);

			return Rgb(static_cast<ColorCompType>(r * ccColor::MAX),
			           static_cast<ColorCompType>(g * ccColor::MAX),
			           static_cast<ColorCompType>(b * ccColor::MAX));
		}

		/**
		 * @brief HSV to RGB conversion.
		 *
		 * @param[in] H Hue [0, 360].
		 * @param[in] S Saturation [0, 1].
		 * @param[in] V Value [0, 1].
		 *
		 * @return RGB color (unsigned byte).
		 */
		QCC_DB_LIB_API static Rgb hsv2rgb(float H, float S, float V);

	  private:
		/**
		 * @brief Hue interpolation helper.
		 */
		static float hue2rgb(float m1, float m2, float hue)
		{
			if (hue < 0)
				hue += 1.0f;
			else if (hue > 1.0f)
				hue -= 1.0f;

			if (6 * hue < 1.0f)
				return m1 + (m2 - m1) * hue * 6;
			else if (2 * hue < 1.0f)
				return m2;
			else if (3 * hue < 2.0f)
				return m1 + (m2 - m1) * (4.0f - hue * 6);
			else
				return m1;
		}
	};

	/**
	 * @brief Rgbf to Rgb conversion.
	 */
	inline Rgb FromRgbfToRgb(const Rgbf& color)
	{
		return Rgb(static_cast<ColorCompType>(color.r * MAX),
		           static_cast<ColorCompType>(color.g * MAX),
		           static_cast<ColorCompType>(color.b * MAX));
	}

	/**
	 * @brief Rgbaf to Rgb conversion.
	 */
	inline Rgb FromRgbafToRgb(const Rgbaf& color)
	{
		return Rgb(static_cast<ColorCompType>(color.r * MAX),
		           static_cast<ColorCompType>(color.g * MAX),
		           static_cast<ColorCompType>(color.b * MAX));
	}

	/**
	 * @brief Rgb to Rgba conversion.
	 */
	inline Rgba FromRgbToRgba(const Rgb& color)
	{
		return Rgba(color, MAX);
	}

	/**
	 * @brief Rgbaf to Rgba conversion.
	 */
	inline Rgba FromRgbafToRgba(const Rgbaf& color)
	{
		return Rgba(static_cast<ColorCompType>(color.r * MAX),
		            static_cast<ColorCompType>(color.g * MAX),
		            static_cast<ColorCompType>(color.b * MAX),
		            static_cast<ColorCompType>(color.a * MAX));
	}

	/**
	 * @brief QRgb to Rgb conversion.
	 */
	inline Rgb FromQRgb(QRgb qColor)
	{
		return Rgb(static_cast<unsigned char>(qRed(qColor)),
		           static_cast<unsigned char>(qGreen(qColor)),
		           static_cast<unsigned char>(qBlue(qColor)));
	}

	/**
	 * @brief QRgb to Rgba conversion.
	 */
	inline Rgba FromQRgba(QRgb qColor)
	{
		return Rgba(static_cast<unsigned char>(qRed(qColor)),
		            static_cast<unsigned char>(qGreen(qColor)),
		            static_cast<unsigned char>(qBlue(qColor)),
		            static_cast<unsigned char>(qAlpha(qColor)));
	}

	/**
	 * @brief QColor to Rgb conversion.
	 */
	inline Rgb FromQColor(const QColor& qColor)
	{
		return Rgb(static_cast<unsigned char>(qColor.red()),
		           static_cast<unsigned char>(qColor.green()),
		           static_cast<unsigned char>(qColor.blue()));
	}

	/**
	 * @brief QColor to Rgba conversion.
	 */
	inline Rgba FromQColora(const QColor& qColor)
	{
		return Rgba(static_cast<unsigned char>(qColor.red()),
		            static_cast<unsigned char>(qColor.green()),
		            static_cast<unsigned char>(qColor.blue()),
		            static_cast<unsigned char>(qColor.alpha()));
	}

	/**
	 * @brief QColor to Rgbf conversion.
	 */
	inline Rgbf FromQColorf(const QColor& qColor)
	{
		return Rgbf(static_cast<float>(qColor.redF()),
		            static_cast<float>(qColor.greenF()),
		            static_cast<float>(qColor.blueF()));
	}

	/**
	 * @brief QColor to Rgbaf conversion.
	 */
	inline Rgbaf FromQColoraf(const QColor& qColor)
	{
		return Rgbaf(static_cast<float>(qColor.redF()),
		             static_cast<float>(qColor.greenF()),
		             static_cast<float>(qColor.blueF()),
		             static_cast<float>(qColor.alphaF()));
	}
} // namespace ccColor

#endif // CC_COLOR_TYPES_HEADER
