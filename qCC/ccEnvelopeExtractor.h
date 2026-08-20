#pragma once

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

/**
 * @file ccEnvelopeExtractor.h
 *
 * @brief Envelope extractor for point clouds.
 *
 * @details Extracts envelope (concave hull) polylines from point clouds.
 *
 * An envelope is a boundary that follows the shape of a point cloud,
 * as opposed to a convex hull which always forms the outermost boundary.
 * Envelopes can be:
 * - **Upper**: Points on the top surface
 * - **Lower**: Points on the bottom surface
 * - **Full**: All boundary points
 *
 * The algorithm uses alpha-shapes and concave hull techniques
 * to produce natural-looking boundaries.
 *
 * Used for:
 * - Terrain boundary extraction
 * - Building footprint extraction
 * - Point cloud outline generation
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 *
 * @see CCCoreLib::PointProjectionTools::ExtractConcaveHull2D
 */

// qCC_db
#include <ccPolyline.h>

// CCCoreLib
#include <PointProjectionTools.h>

/**
 * @brief Extract envelopes from point clouds.
 *
 * @details Provides static methods for extracting envelope (concave hull)
 * polylines from point clouds.
 *
 * The extraction projects points onto a best-fit plane and then
 * computes the envelope in 2D. Multi-pass mode allows for better
 * results with complex shapes.
 *
 * Usage:
 * @code
 * ccPolyline* envelope = ccEnvelopeExtractor::ExtractFlatEnvelope(
 *     cloud,
 *     false,                          // allowMultiPass
 *     maxEdgeLength,
 *     nullptr,                         // preferredNormDim
 *     nullptr,                         // preferredUpDir
 *     ccEnvelopeExtractor::FULL);
 * @endcode
 */
class ccEnvelopeExtractor
{
  public:
	/**
	 * @brief Envelope types.
	 */
	enum EnvelopeType
	{
		LOWER, //!< Lower envelope (bottom surface)
		UPPER, //!< Upper envelope (top surface)
		FULL   //!< Full envelope (all boundaries)
	};

	/**
	 * @brief Extract a unique closed envelope polyline.
	 *
	 * @param[in] points Point cloud to extract from.
	 * @param[in] allowMultiPass Allow multi-pass for better results.
	 * @param[in] maxEdgeLength Maximum edge length (0 = convex hull).
	 * @param[in] preferredNormDim Preferred normal direction.
	 * @param[in] preferredUpDir Preferred up direction.
	 * @param[in] envelopeType Type of envelope to extract.
	 * @param[out] originalPointIndexes Indexes of output vertices.
	 * @param[in] enableVisualDebugMode Show debug window.
	 * @param[in] maxAngleDeg Maximum angle between segments.
	 *
	 * @return Envelope polyline, or nullptr on error.
	 *
	 * @note The returned polyline must be deleted by caller.
	 */
	static ccPolyline* ExtractFlatEnvelope(CCCoreLib::GenericIndexedCloudPersist* points,
	                                       bool                                   allowMultiPass,
	                                       PointCoordinateType                    maxEdgeLength         = 0,
	                                       const PointCoordinateType*             preferredNormDim      = nullptr,
	                                       const PointCoordinateType*             preferredUpDir        = nullptr,
	                                       EnvelopeType                           envelopeType          = FULL,
	                                       std::vector<unsigned>*                 originalPointIndexes  = nullptr,
	                                       bool                                   enableVisualDebugMode = false,
	                                       double                                 maxAngleDeg           = 0.0);

	/**
	 * @brief Extract multiple envelope polyline parts.
	 *
	 * @param[in] points Point cloud to extract from.
	 * @param[in] allowMultiPass Allow multi-pass processing.
	 * @param[in] maxEdgeLength Maximum edge length.
	 * @param[out] parts Output polyline parts.
	 * @param[in] envelopeType Type of envelope.
	 * @param[in] allowSplitting Allow splitting of long segments.
	 * @param[in] preferredNormDim Preferred normal direction.
	 * @param[in] preferredUpDir Preferred up direction.
	 * @param[in] enableVisualDebugMode Show debug window.
	 *
	 * @return true on success.
	 *
	 * @note The caller owns the returned polylines.
	 */
	static bool ExtractFlatEnvelope(CCCoreLib::GenericIndexedCloudPersist* points,
	                                bool                                   allowMultiPass,
	                                PointCoordinateType                    maxEdgeLength,
	                                std::vector<ccPolyline*>&              parts,
	                                EnvelopeType                           envelopeType          = FULL,
	                                bool                                   allowSplitting        = true,
	                                const PointCoordinateType*             preferredNormDim      = nullptr,
	                                const PointCoordinateType*             preferredUpDir        = nullptr,
	                                bool                                   enableVisualDebugMode = false);

  protected:
	/**
	 * @brief Extract concave hull in 2D.
	 *
	 * @param[in] points Input points.
	 * @param[out] hullPoints Hull points.
	 * @param[in] envelopeType Type of envelope.
	 * @param[in] allowMultiPass Allow multi-pass.
	 * @param[in] maxSquareLength Maximum squared edge length.
	 * @param[in] enableVisualDebugMode Show debug window.
	 * @param[in] maxAngleDeg Maximum angle between segments.
	 *
	 * @return true on success.
	 *
	 * @note Algorithm based on Park & Oh, "A New Concave Hull Algorithm
	 * and Concaveness Measure for n-dimensional Datasets", 2012.
	 */
	static bool ExtractConcaveHull2D(std::vector<CCCoreLib::PointProjectionTools::IndexedCCVector2>& points,
	                                 std::list<CCCoreLib::PointProjectionTools::IndexedCCVector2*>&  hullPoints,
	                                 EnvelopeType                                                    envelopeType,
	                                 bool                                                            allowMultiPass,
	                                 PointCoordinateType                                             maxSquareLength       = 0,
	                                 bool                                                            enableVisualDebugMode = false,
	                                 double                                                          maxAngleDeg           = 90.0);
};
