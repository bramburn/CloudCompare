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

#ifndef CC_IMAGE_FILE_FILTER_HEADER
#define CC_IMAGE_FILE_FILTER_HEADER

/**
 * @file ImageFileFilter.h
 *
 * @brief Image file filter
 *
 * Filter for loading and saving images.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

#include "FileIOFilter.h"

/**
 * @brief Image file filter
 *
 * Filter for image file I/O.
 */
class QCC_IO_LIB_API ImageFileFilter : public FileIOFilter
{
  public:
	/// Constructor
	ImageFileFilter();

	// inherited from FileIOFilter
	CC_FILE_ERROR loadFile(const QString& filename, ccHObject& container, LoadParameters& parameters) override;

	bool canSave(CC_CLASS_ENUM type, bool& multiple, bool& exclusive) const override;
	CC_FILE_ERROR saveToFile(ccHObject* entity, const QString& filename, const SaveParameters& parameters) override;

	/**
	 * @brief Get load filename
	 * @param[in] dialogTitle Dialog title
	 * @param[in] imageLoadPath Default path
	 * @param[in] parentWidget Parent widget
	 * @return Selected filename
	 */
	static QString GetLoadFilename(const QString& dialogTitle,
	                               const QString& imageLoadPath,
	                               QWidget* parentWidget = nullptr);

	/**
	 * @brief Get save filename
	 * @param[in] dialogTitle Dialog title
	 * @param[in] baseName Default filename
	 * @param[in] imageSavePath Default path
	 * @param[in] parentWidget Parent widget
	 * @return Selected filename
	 */
	static QString GetSaveFilename(const QString& dialogTitle,
	                               const QString& baseName,
	                               const QString& imageSavePath,
	                               QWidget* parentWidget = nullptr);
};

#endif // CC_IMAGE_FILE_FILTER_HEADER
