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

#ifndef CC_INDEXED_TRANSFORMATION_BUFFER_HEADER
#define CC_INDEXED_TRANSFORMATION_BUFFER_HEADER

/**
 * @file ccIndexedTransformationBuffer.h
 *
 * @brief Transformation buffer
 *
 * Buffer of indexed transformations for temporal
 * interpolation (e.g. sensor trajectories).
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */
// Local
#include "ccHObject.h"
#include "ccIndexedTransformation.h"

// system
#include <cfloat>

/**
 * @brief Transformation buffer
 *
 * Buffer of indexed transformations with temporal
 * interpolation support.
 */
class QCC_DB_LIB_API ccIndexedTransformationBuffer : public ccHObject
    , public std::vector<ccIndexedTransformation>
{
  public:
	/**
	 * @brief Create buffer
	 * @param[in] name Buffer name
	 */
	ccIndexedTransformationBuffer(const QString& name = QString("Trans. buffer"));
	
	/**
	 * @brief Copy constructor
	 * @param[in] buffer Source buffer
	 */
	ccIndexedTransformationBuffer(const ccIndexedTransformationBuffer& buffer);

	// inherited from ccHObject
	/// Get class type
	CC_CLASS_ENUM getClassID() const override
	{
		return CC_TYPES::TRANS_BUFFER;
	}
	/// Is serializable
	bool isSerializable() const override
	{
		return true;
	}

	/// Sort transformations by index
	void sort();

	/**
	 * @brief Find nearest transformations
	 * @param[in] index Query index
	 * @param[out] trans1 Preceding transformation
	 * @param[out] trans2 Following transformation
	 * @param[out] trans1IndexInBuffer Index of trans1
	 * @param[out] trans2IndexInBuffer Index of trans2
	 * @return true on success
	 */
	bool findNearest(double                          index,
	                 const ccIndexedTransformation*& trans1,
	                 const ccIndexedTransformation*& trans2,
	                 size_t*                         trans1IndexInBuffer = nullptr,
	                 size_t*                         trans2IndexInBuffer = nullptr) const;

	/**
	 * @brief Get interpolated transformation
	 * @param[in] index Query index
	 * @param[out] trans Output transformation
	 * @param[in] maxIndexDistForInterpolation Max distance for interpolation
	 * @return true on success
	 */
	bool getInterpolatedTransformation(double                   index,
	                                   ccIndexedTransformation& trans,
	                                   double                   maxIndexDistForInterpolation = DBL_MAX) const;

	//! [Display option] Returns whether trihedrons should be displayed or not (otherwise only points or a polyline)
	bool trihedronsShown() const
	{
		return m_showTrihedrons;
	}
	//! [Display option] Sets whether trihedrons should be displayed or not (otherwise only points or a polyline)
	void showTrihedrons(bool state)
	{
		m_showTrihedrons = state;
	}

	//! [Display option] Returns trihedron display size
	float trihedronsDisplayScale() const
	{
		return m_trihedronsScale;
	}
	//! [Display option] Sets trihedron display size
	void setTrihedronsDisplayScale(float scale)
	{
		m_trihedronsScale = scale;
	}

	//! [Display option] Returns whether the path should be displayed as a polyline or not (otherwise only points)
	bool isPathShownAsPolyline() const
	{
		return m_showAsPolyline;
	}
	//! [Display option] Sets whether the path should be displayed as a polyline or not (otherwise only points)
	void showPathAsPolyline(bool state)
	{
		m_showAsPolyline = state;
	}

	//! Invalidates the bounding box
	/** Should be called whenever the content of this structure changes!
	 **/
	void invalidateBoundingBox();

	// Inherited from ccHObject
	ccBBox getOwnBB(bool withGLFeatures = false) override;

  protected:
	// inherited from ccHObject
	bool  toFile_MeOnly(QFile& out, short dataVersion) const override;
	bool  fromFile_MeOnly(QFile& in, short dataVersion, int flags, LoadedIDMap& oldToNewIDMap) override;
	short minimumFileVersion_MeOnly() const override;
	void  drawMeOnly(CC_DRAW_CONTEXT& context) override;

	//! Bounding box
	ccBBox m_bBox;
	//! Bounding box last 'validity' size
	size_t m_bBoxValidSize;

	//! Whether the path should be displayed as a polyline or not
	bool m_showAsPolyline;
	//! Whether trihedrons should be displayed or not
	bool m_showTrihedrons;
	//! Trihedrons display scale
	float m_trihedronsScale;
};

#endif
