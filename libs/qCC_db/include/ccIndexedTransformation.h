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
 * @file ccIndexedTransformation.h
 *
 * @brief Transformation matrix with temporal index.
 *
 * @details A 4x4 transformation matrix associated with an index
 * (typically a timestamp) for interpolation between poses.
 *
 * ## Overview
 *
 * ccIndexedTransformation extends ccGLMatrix with a temporal index
 * for interpolating between transformation poses. This is useful for:
 * - Sensor trajectory representation
 * - Animation keyframes
 * - Time-series transformations
 *
 * ## Interpolation
 *
 * Use Interpolate() to compute intermediate transformations:
 * - Linear interpolation for translation
 * - SLERP for rotation
 *
 * ## Usage
 *
 * @code
 * // Create transformations with timestamps
 * ccIndexedTransformation t1(matrix1, 0.0);
 * ccIndexedTransformation t2(matrix2, 1.0);
 *
 * // Interpolate at t=0.5
 * ccIndexedTransformation t = ccIndexedTransformation::Interpolate(0.5, t1, t2);
 *
 * // Get interpolated values
 * double index = t.getIndex();
 * CCVector3 trans = t.getTranslation();
 * @endcode
 *
 * @extends ccGLMatrix
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see ccGLMatrix for base matrix operations
 */

#ifndef CC_INDEXED_TRANSFORMATION_HEADER
#define CC_INDEXED_TRANSFORMATION_HEADER

// Local
#include "ccGLMatrix.h"

/**
 * @brief Transformation matrix with index.
 *
 * @details A 4x4 transformation matrix with an associated index
 * for temporal interpolation (e.g. sensor poses over time).
 *
 * @extends ccGLMatrix
 */
class QCC_DB_LIB_API ccIndexedTransformation : public ccGLMatrix
{
  public:
	/**
	 * @brief Default constructor.
	 *
	 * Matrix is set to identity, index to zero.
	 */
	ccIndexedTransformation();

	/**
	 * @brief Create from matrix.
	 *
	 * @param[in] matrix Transformation matrix.
	 */
	ccIndexedTransformation(const ccGLMatrix& matrix);

	/**
	 * @brief Create from matrix with index.
	 *
	 * @param[in] matrix Transformation matrix.
	 * @param[in] index Associated index (e.g., timestamp).
	 */
	ccIndexedTransformation(const ccGLMatrix& matrix, double index);

	/// Copy constructor.
	ccIndexedTransformation(const ccIndexedTransformation& trans) = default;

	/**
	 * @brief Get index.
	 *
	 * @return Index value.
	 */
	inline double getIndex() const
	{
		return m_index;
	}

	/**
	 * @brief Set index.
	 *
	 * @param[in] index New index value.
	 */
	inline void setIndex(double index)
	{
		m_index = index;
	}

	/**
	 * @brief Interpolate between two transformations.
	 *
	 * @param[in] interpIndex Interpolation position.
	 * @param[in] trans1 First transformation.
	 * @param[in] trans2 Second transformation.
	 *
	 * @return Interpolated transformation.
	 */
	static ccIndexedTransformation Interpolate(double interpIndex, const ccIndexedTransformation& trans1, const ccIndexedTransformation& trans2);

	/**
	 * @brief Multiplication by a ccGLMatrix.
	 *
	 * @param[in] mat Matrix to multiply.
	 *
	 * @return Result transformation.
	 *
	 * @note Index is not modified.
	 */
	ccIndexedTransformation operator*(const ccGLMatrix& mat) const;

	/**
	 * @brief In-place multiplication by a ccGLMatrix.
	 *
	 * @param[in] mat Matrix to multiply.
	 *
	 * @return Result transformation.
	 *
	 * @note Index is not modified.
	 */
	ccIndexedTransformation& operator*=(const ccGLMatrix& mat);

	/**
	 * @brief In-place translation addition.
	 *
	 * @param[in] T Translation vector.
	 *
	 * @return Result transformation.
	 *
	 * @note Index is not modified.
	 */
	ccIndexedTransformation& operator+=(const CCVector3& T);

	/**
	 * @brief In-place translation subtraction.
	 *
	 * @param[in] T Translation vector.
	 *
	 * @return Result transformation.
	 *
	 * @note Index is not modified.
	 */
	ccIndexedTransformation& operator-=(const CCVector3& T);

	/**
	 * @brief Get transposed transformation.
	 *
	 * @return Transposed matrix.
	 *
	 * @note Index is not modified.
	 */
	ccIndexedTransformation transposed() const;

	/**
	 * @brief Get inverse transformation.
	 *
	 * @return Inverse matrix.
	 *
	 * @note Index is not modified.
	 */
	ccIndexedTransformation inverse() const;

	// inherited from ccGLMatrix
	/**
	 * @brief Save to ASCII file.
	 *
	 * @param[in] filename Output file.
	 * @param[in] precision Floating point precision.
	 *
	 * @return true if successful.
	 */
	bool toAsciiFile(QString filename, int precision = 12) const override;

	/**
	 * @brief Load from ASCII file.
	 *
	 * @param[in] filename Input file.
	 *
	 * @return true if successful.
	 */
	bool fromAsciiFile(QString filename) override;

	// inherited from ccSerializableObject
	/**
	 * @brief Check if serializable.
	 */
	bool isSerializable() const override
	{
		return true;
	}

	/**
	 * @brief Save to binary file.
	 */
	bool toFile(QFile& out, short dataVersion) const override;

	/**
	 * @brief Load from binary file.
	 */
	bool fromFile(QFile& in, short dataVersion, int flags, LoadedIDMap& oldToNewIDMap) override;

	/**
	 * @brief Minimum file version.
	 */
	short minimumFileVersion() const override;

  private:
	//! Associated index (e.g., timestamp).
	double m_index;
};

#endif // CC_INDEXED_TRANSFORMATION_HEADER
