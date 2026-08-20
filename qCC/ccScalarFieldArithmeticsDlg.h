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

#ifndef CC_SF_ARITMETHIC_DLG_HEADER
#define CC_SF_ARITMETHIC_DLG_HEADER

/**
 * @file ccScalarFieldArithmeticsDlg.h
 *
 * @brief Scalar field arithmetics dialog for mathematical operations.
 *
 * @details Dialog for performing arithmetic and mathematical operations
 * on scalar fields.
 *
 * Operations include:
 * - **Binary operations**: +, -, *, /, MIN, MAX (between two SFs)
 * - **Unary operations**: SQRT, POW2, POW3, ABS, SQRT, etc.
 * - **Trigonometric**: SIN, COS, TAN, ASIN, ACOS, ATAN
 * - **Exponential/Log**: EXP, LOG, LOG10
 * - **Other**: INVERSE, SET, INT
 *
 * Operations can be applied in-place (modifying the existing SF)
 * or create a new scalar field.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see ccScalarField
 */

#include <QDialog>

class ccPointCloud;

namespace Ui
{
	class SFArithmeticsDlg;
}

/**
 * @brief Dialog for scalar field arithmetic operations.
 *
 * @details Provides a UI for performing mathematical operations
 * on point cloud scalar fields.
 *
 * Features:
 * - Binary operations between two SFs
 * - Unary operations on a single SF
 * - Mixed mode (SF op constant value)
 * - In-place or new SF output
 *
 * @extends QDialog
 */
class ccScalarFieldArithmeticsDlg : public QDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Arithmetic operations.
	 */
	enum Operation
	{
		/* Binary operations (require two SFs) */
		PLUS     = 0, //!< Addition
		MINUS    = 1, //!< Subtraction
		MULTIPLY = 2, //!< Multiplication
		DIVIDE   = 3, //!< Division
		MIN      = 4, //!< Minimum
		MAX      = 5, //!< Maximum

		/* Unary operations (require one SF) */
		SQRT    = 6,  //!< Square root
		POW2    = 7,  //!< Square (x^2)
		POW3    = 8,  //!< Cube (x^3)
		EXP     = 9,  //!< Exponential (e^x)
		LOG     = 10, //!< Natural logarithm
		LOG10   = 11, //!< Base-10 logarithm
		COS     = 12, //!< Cosine
		SIN     = 13, //!< Sine
		TAN     = 14, //!< Tangent
		ACOS    = 15, //!< Arc cosine
		ASIN    = 16, //!< Arc sine
		ATAN    = 17, //!< Arc tangent
		INT     = 18, //!< Integer (truncate)
		INVERSE = 19, //!< Inverse (1/x)
		SET     = 20, //!< Set to constant value
		ABS     = 21, //!< Absolute value

		/* Invalid enum (always last) */
		INVALID = 255
	};

	/**
	 * @brief Secondary SF descriptor for binary operations.
	 */
	struct SF2
	{
		bool   isConstantValue = true; //!< Use constant vs. SF
		double constantValue   = 0.0;   //!< Constant value
		int    sfIndex         = -1;    //!< SF index
	};

	/**
	 * @brief Construct the arithmetic dialog.
	 *
	 * @param[in] cloud Point cloud with scalar fields.
	 * @param[in] parent Parent widget.
	 */
	ccScalarFieldArithmeticsDlg(ccPointCloud* cloud, QWidget* parent = nullptr);

	/**
	 * @brief Destructor.
	 */
	~ccScalarFieldArithmeticsDlg() override;

	/**
	 * @brief Get the selected operation.
	 * @return Selected operation.
	 */
	Operation getOperation() const;

	/**
	 * @brief Get operation by name.
	 *
	 * @param[in] name Operation name.
	 * @return Operation enum.
	 */
	static Operation GetOperationByName(const QString& name);

	/**
	 * @brief Get operation name.
	 *
	 * @param[in] op Operation.
	 * @param[in] sf1 First SF name.
	 * @param[in] sf2 Second SF name.
	 * @return Formatted operation name.
	 */
	static QString GetOperationName(Operation op, const QString& sf1, const QString& sf2 = QString());

	/**
	 * @brief Apply operation on a cloud.
	 *
	 * @param[in] cloud Cloud to apply operation on.
	 * @return true on success.
	 */
	bool apply(ccPointCloud* cloud);

	/**
	 * @brief Apply operation on a cloud.
	 *
	 * @param[in] cloud Cloud to apply operation on.
	 * @param[in] op Operation to perform.
	 * @param[in] sf1Idx First SF index.
	 * @param[in] inplace Apply in-place or create new SF.
	 * @param[in] sf2 Secondary SF/value.
	 * @param[in] parent Parent widget.
	 * @return true on success.
	 */
	static bool Apply(ccPointCloud* cloud,
	                  Operation     op,
	                  int           sf1Idx,
	                  bool          inplace,
	                  SF2*          sf2    = nullptr,
	                  QWidget*      parent = nullptr);

  protected slots:
	/**
	 * @brief Handle operation change.
	 *
	 * @param[in] index New operation index.
	 */
	void onOperationIndexChanged(int index);

	/**
	 * @brief Handle SF2 selection change.
	 *
	 * @param[in] index New SF2 index.
	 */
	void onSF2IndexChanged(int index);

  protected:
	/**
	 * @brief Get first SF index.
	 * @return SF1 index.
	 */
	int getSF1Index();

	/**
	 * @brief Get second SF index.
	 * @return SF2 index.
	 */
	int getSF2Index();

  private:
	//! UI definition
	Ui::SFArithmeticsDlg* m_ui;
};

#endif // CC_SF_ARITMETHIC_DLG_HEADER
