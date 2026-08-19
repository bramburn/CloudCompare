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

#ifndef CC_GL_MATRIX_HEADER
#define CC_GL_MATRIX_HEADER

/**
 * @file ccGLMatrix.h
 *
 * @brief OpenGL transformation matrix classes
 *
 * Provides single-precision (ccGLMatrix) and double-precision (ccGLMatrixd)
 * 4x4 transformation matrices compatible with OpenGL conventions.
 *
 * Matrices use column-major order (OpenGL convention).
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 * @see ccGLMatrixTpl for template implementation
 */

// Local
#include "ccGLMatrixTpl.h"

// CCCoreLib
#include <SquareMatrix.h>

/**
 * @brief Single-precision 4x4 OpenGL transformation matrix
 *
 * Extends ccGLMatrixTpl<float> with OpenGL-specific constructors.
 * Uses column-major order consistent with OpenGL conventions.
 */
class QCC_DB_LIB_API ccGLMatrix : public ccGLMatrixTpl<float>
{
  public:
	/**
	 * @brief Default constructor
	 * Initializes to identity matrix.
	 */
	ccGLMatrix()
	    : ccGLMatrixTpl<float>()
	{
	}

	/**
	 * @brief Copy constructor
	 * @param[in] mat Source matrix
	 */
	ccGLMatrix(const ccGLMatrixTpl<float>& mat)
	    : ccGLMatrixTpl<float>(mat)
	{
	}

	/**
	 * @brief Constructor from float array
	 * @param[in] mat16f 16-element array (column-major order)
	 */
	explicit ccGLMatrix(const float* mat16f)
	    : ccGLMatrixTpl<float>(mat16f)
	{
	}

	/**
	 * @brief Constructor from double array
	 * @param[in] mat16d 16-element array (column-major order)
	 * @warning Casts double to float, potentially losing precision
	 */
	explicit ccGLMatrix(const double* mat16d)
	    : ccGLMatrixTpl<float>(mat16d)
	{
	}

	/**
	 * @brief Constructor from column vectors
	 * @param[in] X First column (x-axis direction)
	 * @param[in] Y Second column (y-axis direction)
	 * @param[in] Z Third column (z-axis direction)
	 * @param[in] Tr Translation column
	 */
	ccGLMatrix(const Vector3Tpl<float>& X, const Vector3Tpl<float>& Y, const Vector3Tpl<float>& Z, const Vector3Tpl<float>& Tr)
	    : ccGLMatrixTpl<float>(X, Y, Z, Tr)
	{
	}
};

//! Double version of ccGLMatrixTpl
class QCC_DB_LIB_API ccGLMatrixd : public ccGLMatrixTpl<double>
{
  public:
	//! Default constructor
	/** Matrix is set to identity (see ccGLMatrixTpl::toIdentity) by default.
	 **/
	ccGLMatrixd()
	    : ccGLMatrixTpl<double>()
	{
	}

	//! Copy constructor from a ccGLMatrixTpl
	/** \param mat matrix
	 **/
	ccGLMatrixd(const ccGLMatrixTpl<double>& mat)
	    : ccGLMatrixTpl<double>(mat)
	{
	}

	//! Constructor from a float GL matrix array
	/** \param mat16f a 16 elements array (column major order)
	 **/
	explicit ccGLMatrixd(const float* mat16f)
	    : ccGLMatrixTpl<double>(mat16f)
	{
	}

	//! Constructor from a double GL matrix array
	/** \param mat16d a 16 elements array (column major order)
	 **/
	explicit ccGLMatrixd(const double* mat16d)
	    : ccGLMatrixTpl<double>(mat16d)
	{
	}

	//! Constructor from 4 columns (X,Y,Z,Tr)
	/** \param X 3 first elements of the 1st column (last one is 0)
	    \param Y 3 first elements of the 2nd column (last one is 0)
	    \param Z 3 first elements of the 3rd column (last one is 0)
	    \param Tr 3 first elements of the last column (last one is 1)
	**/
	ccGLMatrixd(const Vector3Tpl<double>& X, const Vector3Tpl<double>& Y, const Vector3Tpl<double>& Z, const Vector3Tpl<double>& Tr)
	    : ccGLMatrixTpl<double>(X, Y, Z, Tr)
	{
	}
};

//! Required for Qt::QueuedConnection when passing ccGLMatrixd through signals
Q_DECLARE_METATYPE(ccGLMatrixd);

/*** Helpers ***/

//! Constructor from a 3x3 rotation matrix R and a vector Tr
template <typename Tin, typename Tout>
ccGLMatrixTpl<Tout> FromCCLibMatrix(const CCCoreLib::SquareMatrixTpl<Tin>& R, const Vector3Tpl<Tin>& Tr)
{
	ccGLMatrixTpl<Tout> outputMat;
	// outputMat.toIdentity(); //already done in the constructor

	if (R.isValid() && R.size() == 3)
	{
		// we copy each column
		Tout* mat = outputMat.data();
		for (unsigned j = 0; j < 3; ++j)
		{
			*mat++ = static_cast<Tout>(R.m_values[0][j]);
			*mat++ = static_cast<Tout>(R.m_values[1][j]);
			*mat++ = static_cast<Tout>(R.m_values[2][j]);
			mat++;
		}
	}

	outputMat.setTranslation(Vector3Tpl<Tout>(static_cast<Tout>(Tr.x),
	                                          static_cast<Tout>(Tr.y),
	                                          static_cast<Tout>(Tr.z)));

	return outputMat;
}

//! Constructor from a 3x3 rotation matrix R, a vector Tr, a scale S
template <typename Tin, typename Tout>
ccGLMatrixTpl<Tout> FromCCLibMatrix(const CCCoreLib::SquareMatrixTpl<Tin>& R, const Vector3Tpl<Tin>& Tr, Tin S)
{
	ccGLMatrixTpl<Tout> outputMat;
	// outputMat.toIdentity(); //already done in the constructor

	if (R.isValid() && R.size() == 3)
	{
		// we copy each column
		Tout* mat = outputMat.data();
		for (unsigned j = 0; j < 3; ++j)
		{
			*mat++ = static_cast<Tout>(R.m_values[0][j] * S);
			*mat++ = static_cast<Tout>(R.m_values[1][j] * S);
			*mat++ = static_cast<Tout>(R.m_values[2][j] * S);
			mat++;
		}
	}

	outputMat.setTranslation(Vector3Tpl<Tout>(static_cast<Tout>(Tr.x),
	                                          static_cast<Tout>(Tr.y),
	                                          static_cast<Tout>(Tr.z)));

	return outputMat;
}

//! Constructor from a rotation center G, a 3x3 rotation matrix R and a vector Tr
template <typename Tin, typename Tout>
ccGLMatrixTpl<Tout> FromCCLibMatrix(const CCCoreLib::SquareMatrixTpl<Tin>& R, const Vector3Tpl<Tin>& Tr, const Vector3Tpl<Tin>& rotCenter)
{
	ccGLMatrixTpl<Tout> outputMat = FromCCLibMatrix<Tin, Tout>(R, Tr);

	outputMat.shiftRotationCenter(rotCenter);

	return outputMat;
}

#endif // CC_GL_MATRIX_HEADER
