/**
 * @file LasSaver.h
 *
 * @brief LAS file saver class.
 *
 * @details Saves point clouds to LAS/LAZ files.
 *
 * ## Usage
 *
 * @code
 * LasSaver::Parameters params;
 * params.versionMajor = 1;
 * params.versionMinor = 4;
 * params.pointFormat = 6;
 * params.shouldSaveRGB = true;
 * // ... configure fields ...
 *
 * LasSaver saver(cloud, params);
 * saver.open("output.las");
 * for (size_t i = 0; i < cloud.size(); ++i) {
 *     saver.saveNextPoint();
 * }
 * @endcode
 *
 * @author Thomas Montaigu
 */

#pragma once

// ##########################################################################
// #                                                                        #
// #                CLOUDCOMPARE PLUGIN: LAS-IO Plugin                      #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  the Free Software Foundation; version 2 of the License.               #
// #                                                                        #
// #  This program is distributed in the hope that it will be useful,       #
// #  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
// #  GNU General Public License for more details.                          #
// #                                                                        #
// #                   COPYRIGHT: Thomas Montaigu                           #
// #                                                                        //
// ##########################################################################

#include "LasScalarFieldSaver.h"
#include "LasWaveformSaver.h"

// qCC_db
#include <FileIOFilter.h>

// LASzip
#include <laszip/laszip_api.h>

// System
#include <memory>

class ccPointCloud;

/**
 * @brief LAS file saver.
 *
 * Saves point clouds to LAS/LAZ files.
 */
class LasSaver
{
  public:
	/**
	 * @brief Save parameters.
	 */
	struct Parameters
	{
		//! Standard scalar fields.
		std::vector<LasScalarField> standardFields;

		//! Extra scalar fields.
		std::vector<LasExtraScalarField> extraFields;

		//! Save RGB colors.
		bool shouldSaveRGB{false};

		//! Save waveforms.
		bool shouldSaveWaveform{false};

		//! Save normals as extra scalar field.
		bool shouldSaveNormalsAsExtraScalarField{false};

		//! Version major.
		uint8_t versionMajor{1};

		//! Version minor.
		uint8_t versionMinor{0};

		//! Point format.
		uint8_t pointFormat{0};

		//! LAS scale.
		CCVector3d lasScale;

		//! LAS offset.
		CCVector3d lasOffset;
	};

	/**
	 * @brief Create saver.
	 *
	 * @param[in] cloud Point cloud to save.
	 * @param[in] parameters Save parameters.
	 */
	LasSaver(ccPointCloud& cloud, Parameters parameters);

	/**
	 * @brief Destructor.
	 */
	~LasSaver() noexcept;

	/**
	 * @brief Open file for writing.
	 *
	 * @param[in] filePath Output file path.
	 *
	 * @return Error code.
	 */
	CC_FILE_ERROR open(const QString filePath);

	/**
	 * @brief Save next point.
	 *
	 * @return Error code.
	 */
	CC_FILE_ERROR saveNextPoint();

	/**
	 * @brief Check if waveforms can be saved.
	 *
	 * @return true if supported.
	 */
	bool canSaveWaveforms() const;

	/**
	 * @brief Get last error message.
	 *
	 * @return Error string.
	 */
	QString getLastError() const;

  private:
	/**
	 * @brief Initialize LASzip header.
	 *
	 * @param[in] parameters Parameters.
	 */
	void initLaszipHeader(const Parameters& parameters);

  private:
	//! Current point index.
	unsigned m_currentPointIndex{0};

	//! Point cloud.
	ccPointCloud& m_cloudToSave;

	//! LASzip header.
	laszip_header m_laszipHeader{};

	//! LASzip writer.
	laszip_POINTER m_laszipWriter{nullptr};

	//! Fields saver.
	LasScalarFieldSaver m_fieldsSaver;

	//! Save RGB flag.
	bool m_shouldSaveRGB{false};

	//! Waveform saver.
	std::unique_ptr<LasWaveformSaver> m_waveformSaver{nullptr};

	//! LASzip point.
	laszip_point* m_laszipPoint{nullptr};

	//! Original scalar field index.
	int m_originallySelectedScalarField = -1;

	//! Normal dim temporary export flags.
	bool m_normalDimWasTemporarillyExported[3] = {false, false, false};
};
