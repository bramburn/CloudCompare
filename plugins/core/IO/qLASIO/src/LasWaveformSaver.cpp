// ##########################################################################
// #                                                                        #
// #                CLOUDCOMPARE PLUGIN: LAS-IO Plugin                      #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  it under the terms of the GNU General Public License as published by  #
// #  the Free Software Foundation; version 2 of the License.               #
// #                                                                        #
// #  This program is distributed in the hope that it will be useful,       #
// #  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
// #  GNU General Public License for more details.                          #
// #                                                                        #
// #                   COPYRIGHT: Thomas Montaigu                           #
// #                                                                        #
// ##########################################################################

/**
 * @file LasWaveformSaver.cpp
 *
 * @brief LAS waveform data writer
 *
 * Writes waveform packet data from ccWaveform into a laszip_point
 * during LAS export. Waveform data consists of:
 * - Descriptor ID: identifies the waveform format
 * - Data offset: byte offset in the EVLR data
 * - Byte count: number of bytes
 * - Echo time: pulse return time in picoseconds
 * - Beam direction: unit vector (X, Y, Z)
 *
 * The waveform packet is written as a 29-byte struct into the
 * laszip_point::wave_packet field.
 *
 * @see LasWaveformLoader.cpp for the read-side counterpart
 */

#include "LasWaveformSaver.h"

#include "LasDetails.h"

#include <ccPointCloud.h>

/**
 * @brief Construct the waveform saver
 *
 * @param[in] pointCloud Source cloud (must have waveform data)
 */
LasWaveformSaver::LasWaveformSaver(const ccPointCloud& pointCloud) noexcept
    : m_array(29, '\0')
    , m_pointCloud(pointCloud)
{
}

/**
 * @brief Write one point's waveform data
 *
 * Extracts waveform metadata from ccWaveform and writes it as
 * a 29-byte wave_packet into the laszip_point:
 * Bytes 0-3: descriptor ID (uint32)
 * Bytes 4-11: data offset (uint64)
 * Bytes 12-15: byte count (uint32)
 * Bytes 16-19: echo time (float32, picoseconds)
 * Bytes 20-27: beam direction (3 × float32)
 * Byte 28: flags (empty)
 *
 * @param[in] index Point index
 * @param[out] point Target laszip point
 */
void LasWaveformSaver::handlePoint(size_t index, laszip_point& point)
{
	assert(index < m_pointCloud.size());
	const ccWaveform& w = m_pointCloud.waveforms().at(index);

	{
		QDataStream stream(&m_array, QIODevice::WriteOnly);
		stream.setByteOrder(QDataStream::ByteOrder::LittleEndian);
		stream << w.descriptorID();
		stream << static_cast<quint64>(w.dataOffset() + LasDetails::EvlrHeader::SIZE);
		stream << w.byteCount();

		float array[4];

		array[0] = w.echoTime_ps();
		array[1] = w.beamDir().x;
		array[2] = w.beamDir().y;
		array[3] = w.beamDir().z;

		memcpy(&m_array.data()[13], array, 4 * 4);
	}

	memcpy(point.wave_packet, m_array.constData(), 29);
}
