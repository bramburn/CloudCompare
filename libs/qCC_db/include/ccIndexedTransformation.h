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

#ifndef CC_INDEXED_TRANSFORMATION_HEADER
#define CC_INDEXED_TRANSFORMATION_HEADER

// Local
#include "ccGLMatrix.h"

/**
 * @file ccIndexedTransformation.h
 *
 * @brief Transformation matrix with index
 *
 * A 4x4 transformation matrix associated with an index
 * (typically a timestamp) for interpolation.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

/**
 * @brief Transformation matrix with index
 *
 * A 4x4 transformation matrix with an associated index
 * for temporal interpolation (e.g. sensor poses over time).
 */
class QCC_DB_LIB_API ccIndexedTransformation : public ccGLMatrix
{
  public:
	/**
	 * @brief Default constructor
	 *
	 * Matrix is set to identity, index to zero.
	 */
	ccIndexedTransformation();

	/**
	 * @brief Create from matrix
	 * @param[in] matrix Transformation matrix
	 */
	ccIndexedTransformation(const ccGLMatrix& matrix);

	/**
	 * @brief Create from matrix with index
	 * @param[in] matrix Transformation matrix
	 * @param[in] index Associated index (e.g. timestamp)
	 */
	ccIndexedTransformation(const ccGLMatrix& matrix, double index);

	/// Copy constructor
	ccIndexedTransformation(const ccIndexedTransformation& trans) = default;

	/**
	 * @brief Get index
	 * @return Index value
	 */
	inline double getIndex() const
	{
		return m_index;
	}

	/**
	 * @brief Set index
	 * @param[in] index New index value
	 */
	inline void setIndex(double index)
	{
		m_index = index;
	}

	/**
	 * @brief Interpolate between two transformations
	 * @param[in] interpIndex Interpolation position
	 * @param[in] trans1 First transformation
	 * @param[in] trans2 Second transformation
	 * @return Interpolated transformation
	 */
	static ccIndexedTransformation Interpolate(double interpIndex, const ccIndexedTransformation& trans1, const ccIndexedTransformation& trans2);

	//! Multiplication by a ccGLMatrix operator
	ccIndexedTransformation operator*(const ccGLMatrix& mat) const;

	//! (in place) Multiplication by a ccGLMatrix operator
	/** Warning: index is not modified by this operation.
	 **/
	ccIndexedTransformation& operator*=(const ccGLMatrix& mat);

	//! Multiplication operator
	// ccIndexedTransformation operator * (const ccIndexedTransformation& mat) const;

	//! (in place) Multiplication operator
	/** Warning: index is not modified by this operation.
	 **/
	// ccIndexedTransformation& operator *= (const ccIndexedTransformation& trans)

	//! (in place) Translation operator
	/** Warning: index is not modified by this operation.
	 **/
	ccIndexedTransformation& operator+=(const CCVector3& T);
	//! (in place) Translation operator
	/** Warning: index is not modified by this operation.
	 **/
	ccIndexedTransformation& operator-=(const CCVector3& T);

	//! Returns transposed transformation
	/** Warning: index is not modified by this operation.
	 **/
	ccIndexedTransformation transposed() const;

	//! Returns inverse transformation
	/** Warning: index is not modified by this operation.
	 **/
	ccIndexedTransformation inverse() const;

	// inherited from ccGLMatrix
	bool toAsciiFile(QString filename, int precision = 12) const override;
	bool fromAsciiFile(QString filename) override;

	// inherited from ccSerializableObject
	bool isSerializable() const override
	{
		return true;
	}
	bool  toFile(QFile& out, short dataVersion) const override;
	bool  fromFile(QFile& in, short dataVersion, int flags, LoadedIDMap& oldToNewIDMap) override;
	short minimumFileVersion() const override;

  protected:
	//! Associated index (e.g. timestamp)
	double m_index;
};

#endif // CC_INDEXED_TRANSFORMATION_HEADER
