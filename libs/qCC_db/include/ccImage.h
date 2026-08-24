// ##########################################################################
// #                                                                        #
// #                              CLOUDCOMPARE                              #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  it under the terms of the GNU General Public License as published by  #
// #  the Free Software Foundation; version 2 or later of the License.      #
// #                                                                        #
// #  This program is distributed in the hope that it will be useful,       #
// #  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
// #  GNU General Public License for more details.                          #
// #                                                                        #
// #          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
// #                                                                        #
// ##########################################################################

#ifndef CC_IMAGE_HEADER
#define CC_IMAGE_HEADER

/**
 * @file ccImage.h
 *
 * @brief Image entity for textures, photos, and depth maps
 *
 * Represents an image that can be associated with 3D geometry.
 * Supports:
 * - **Textures**: applied to mesh faces via UV mapping
 * - **Photos**: georeferenced images from cameras
 * - **Depth maps**: stored alongside ccCameraSensor for photogrammetry
 *
 * Key properties:
 * - Stores QImage pixel data and dimensions
 * - Optional alpha/transparency
 * - Associated ccCameraSensor for georeferencing
 * - Aspect ratio (can differ from w/h if pixels are non-square)
 *
 * ## Serialization
 *
 * Images are serialized to .bin files with dimensions stored separately
 * from the QImage binary data.
 *
 * @extends ccHObject
 */

// Local
#include "ccHObject.h"

// Qt
#include <QImage>

class ccCameraSensor;

/**
 * @class ccImage
 *
 * @brief Image entity for textures, photos, and depth maps
 *
 * @extends ccHObject
 */
class QCC_DB_LIB_API ccImage : public ccHObject
{
  public:
	/**
	 * @brief Default constructor
	 */
	ccImage();

	/**
	 * @brief Create from QImage
	 * @param[in] image Source image
	 * @param[in] name Optional name
	 */
	ccImage(const QImage& image, const QString& name = QString("unknown"));

	/**
	 * @brief Copy constructor
	 * @param[in] image Source image
	 * @param[in] keepSensorLink Preserve sensor association
	 */
	ccImage(const ccImage& image, bool keepSensorLink = true);

	// inherited methods (ccHObject)
	/**
	 * @brief Check if serializable
	 * @return true
	 */
	virtual bool isSerializable() const override
	{
		return true;
	}

	/**
	 * @brief Get class type
	 * @return CC_TYPES::IMAGE
	 */
	virtual CC_CLASS_ENUM getClassID() const override
	{
		return CC_TYPES::IMAGE;
	}

	/**
	 * @brief Load image from file
	 * @param[in] filename Path to image file
	 * @param[out] error Error message if loading fails
	 * @return true if successful
	 */
	bool load(const QString& filename, QString& error);

	//! Returns image data
	inline QImage& data()
	{
		return m_image;
	}
	//! Returns image data (const version)
	inline const QImage& data() const
	{
		return m_image;
	}

	//! Sets image data
	void setData(const QImage& image);

	//! Returns image width
	inline unsigned getW() const
	{
		return m_width;
	}

	//! Returns image height
	inline unsigned getH() const
	{
		return m_height;
	}

	//! Sets image texture transparency
	void setAlpha(float value);

	//! Returns image texture transparency
	inline float getAlpha() const
	{
		return m_texAlpha;
	}

	//! Manually sets aspect ratio
	void setAspectRatio(float ar)
	{
		m_aspectRatio = ar;
	}

	//! Returns aspect ratio
	inline float getAspectRatio() const
	{
		return m_aspectRatio;
	}

	//! Sets associated sensor
	void setAssociatedSensor(ccCameraSensor* sensor);

	//! Returns associated sensor
	ccCameraSensor* getAssociatedSensor()
	{
		return m_associatedSensor;
	}

	//! Returns associated sensor (const version)
	const ccCameraSensor* getAssociatedSensor() const
	{
		return m_associatedSensor;
	}

	// inherited from ccHObject
	virtual ccBBox getOwnFitBB(ccGLMatrix& trans) override;

	//! Compute the displayed image size
	QSizeF computeDisplayedSize(int glWidth, int glHeight) const;

  protected:
	// inherited from ccHObject
	void drawMeOnly(CC_DRAW_CONTEXT& context) override;
	void onDeletionOf(const ccHObject* obj) override;
	bool toFile_MeOnly(QFile& out, short dataVersion) const override;
	bool fromFile_MeOnly(QFile& in, short dataVersion, int flags, LoadedIDMap& oldToNewIDMap) override;
	short minimumFileVersion_MeOnly() const override;

	//! Updates aspect ratio
	void updateAspectRatio();

  protected:
	//! Image width (in pixels)
	unsigned m_width;
	//! Image height (in pixels)
	unsigned m_height;

	//! Aspect ratio w/h
	/** Default is m_width/m_height.
	    Should be changed if pixels are not square.
	**/
	float m_aspectRatio;

	//! Texture transparency
	float m_texAlpha;

	//! Image data
	QImage m_image;

	//! Associated sensor
	ccCameraSensor* m_associatedSensor;
};

#endif // CC_IMAGE_HEADER
