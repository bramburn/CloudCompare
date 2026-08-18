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

#ifndef CC_GLOBAL_SHIFT_MANAGER_HEADER
#define CC_GLOBAL_SHIFT_MANAGER_HEADER

/**
 * @file ccGlobalShiftManager.h
 *
 * @brief Coordinate shift/scale manager
 *
 * Handles big coordinates (shift/scale) during file loading.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

// CCCoreLib
#include <CCGeom.h>

// local
#include "qCC_io.h"

// Qt
#include <QString>

// STL
#include <vector>

class ccHObject;

/**
 * @brief Coordinate shift/scale manager
 *
 * Handles big coordinates during file loading to avoid
 * floating-point precision issues.
 */
class QCC_IO_LIB_API ccGlobalShiftManager
{
  public:
	/// Dialog mode for shift handling
	enum Mode
	{
		NO_DIALOG,           //!< No dialog, no shift
		NO_DIALOG_AUTO_SHIFT,//!< Auto shift without dialog
		DIALOG_IF_NECESSARY, //!< Show dialog if needed
		ALWAYS_DISPLAY_DIALOG//!< Always show dialog
	};

	/**
	 * @brief Handle coordinate shift/scale
	 * @param[in] P First point
	 * @param[in] diagonal Bounding box diagonal
	 * @param[in] mode Dialog mode
	 * @param[in] useInputCoordinatesShiftIfPossible Use existing shift
	 * @param[out] coordinatesShift Computed shift
	 * @param[out] _preserveCoordinateShift Preserve shift flag
	 * @param[out] _coordinatesScale Scale factor
	 * @param[out] _applyAll Apply to all points flag
	 * @return true if handled
	 */
	static bool Handle(const CCVector3d& P,
	                   double            diagonal,
	                   Mode              mode,
	                   bool              useInputCoordinatesShiftIfPossible,
	                   CCVector3d&       coordinatesShift,
	                   bool*             _preserveCoordinateShift = nullptr,
	                   double*           _coordinatesScale        = nullptr,
	                   bool*             _applyAll                = nullptr);

	/**
	 * @brief Check if point needs shifting
	 * @param[in] P Point to check
	 * @return true if shift needed
	 */
	static bool NeedShift(const CCVector3d& P);
	
	/**
	 * @brief Check if coordinate needs shifting
	 * @param[in] d Coordinate value
	 * @return true if shift needed
	 */
	static bool NeedShift(double d);
	
	/**
	 * @brief Check if dimension needs rescaling
	 * @param[in] d Dimension value
	 * @return true if rescale needed
	 */
	static bool NeedRescale(double d);

	/**
	 * @brief Get best shift for point
	 * @param[in] P Point in global coordinates
	 * @return Recommended shift vector
	 */
	static CCVector3d BestShift(const CCVector3d& P);
	
	/**
	 * @brief Get best scale for dimension
	 * @param[in] d Dimension value
	 * @return Recommended scale factor
	 */
	static double BestScale(double d);

	/// Get max coordinate absolute value
	static double MaxCoordinateAbsValue()
	{
		return MAX_COORDINATE_ABS_VALUE;
	}
	
	/// Set max coordinate absolute value
	static void SetMaxCoordinateAbsValue(double value)
	{
		MAX_COORDINATE_ABS_VALUE = std::max(value, 1.0);
	}

	/// Get max bounding box diagonal
	static double MaxBoundgBoxDiagonal()
	{
		return MAX_DIAGONAL_LENGTH;
	}
	
	/// Set max bounding box diagonal
	static void SetMaxBoundgBoxDiagonal(double value)
	{
		MAX_DIAGONAL_LENGTH = value;
	}

	/**
	 * @brief Store shift/scale pair
	 * @param[in] shift Shift vector
	 * @param[in] scale Scale factor
	 * @param[in] preserve Preserve flag
	 */
	static void StoreShift(const CCVector3d& shift, double scale, bool preserve = true);

  public: // Shift and scale info
	/**
	 * @brief Shift and scale information
	 */
	struct ShiftInfo
	{
		CCVector3d shift;  //!< Shift vector
		double     scale;   //!< Scale factor
		QString    name;    //!< Name
		bool       preserve;//!< Preserve flag

		/**
		 * @brief Create shift info
		 * @param[in] str Name
		 */
		ShiftInfo(QString str = QString("unnamed"))
		    : shift(0, 0, 0)
		    , scale(1.0)
		    , name(str)
		    , preserve(true)
		{
		}
		
		/**
		 * @brief Create shift info
		 * @param[in] str Name
		 * @param[in] T Shift vector
		 * @param[in] s Scale factor
		 */
		ShiftInfo(QString str, const CCVector3d& T, double s = 1.0)
		    : shift(T)
		    , scale(s)
		    , name(str)
		    , preserve(true)
		{
		}
	};

	/**
	 * @brief Get last shift/scale entries
	 * @return List of shift info
	 */
	const static std::vector<ShiftInfo>& GetLast();

	/**
	 * @brief Load shift info from file
	 * @param[in] filename File to read
	 * @param[out] infos Read information
	 * @return true on success
	 */
	static bool LoadInfoFromFile(QString filename, std::vector<ShiftInfo>& infos);

  protected:
	/// Max acceptable coordinate value
	static double MAX_COORDINATE_ABS_VALUE;

	/// Max acceptable diagonal length
	static double MAX_DIAGONAL_LENGTH;
};

#endif
