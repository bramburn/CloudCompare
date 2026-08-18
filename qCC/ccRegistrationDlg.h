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

#ifndef CC_REGISTRATION_DLG_HEADER
#define CC_REGISTRATION_DLG_HEADER

/**
 * @file ccRegistrationDlg.h
 *
 * @brief Registration dialog
 *
 * Dialog for configuring point cloud/mesh registration (ICP).
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

#include <QDialog>

// CCCoreLib
#include <ReferenceCloud.h>
#include <RegistrationTools.h>
#include <ui_registrationDlg.h>

class ccHObject;

/**
 * @brief Registration dialog
 *
 * Configure ICP registration parameters.
 */
class ccRegistrationDlg : public QDialog
    , public Ui::RegistrationDialog
{
	Q_OBJECT

  public:
	/**
	 * @brief Create dialog
	 * @param[in] data Data entity
	 * @param[in] model Model entity
	 * @param[in] parent Parent widget
	 */
	ccRegistrationDlg(ccHObject* data, ccHObject* model, QWidget* parent = nullptr);

	/// Destructor
	virtual ~ccRegistrationDlg();

	/// Convergence method type
	typedef CCCoreLib::ICPRegistrationTools::CONVERGENCE_TYPE ConvergenceMethod;

	/// Get convergence method
	ConvergenceMethod getConvergenceMethod() const;

	/// Get max iterations (only for ITERATION_REG)
	unsigned getMaxIterationCount() const;

	/// Get final overlap
	unsigned getFinalOverlap() const;

	/// Get min RMS decrease (only for MAX_ERROR_REG)
	double getMinRMSDecrease() const;

	/// Get absolute minimum RMS decrease
	static double GetAbsoluteMinRMSDecrease();

	/// Set min RMS decrease (only for MAX_ERROR_REG)
	void setMinRMSDecrease(double value);

	/// Get remove farthest points flag
	bool removeFarthestPoints() const;

	/// Get random sampling limit
	unsigned randomSamplingLimit() const;

	/// Get model entity
	ccHObject* getModelEntity();

	// 48 more lines - truncated for brevity
};

#endif // CC_REGISTRATION_DLG_HEADER
