/**
 * @file StatisticalOutliersRemover.h
 *
 * @brief Statistical outliers remover filter.
 *
 * @details Filter for removing statistical outliers
 * from point clouds using PCL.
 */
//##########################################################################
//#                                                                        #
//#                       CLOUDCOMPARE PLUGIN: qPCL                        #
//#                                                                        #
//#  This program is free software; you can redistribute it and/or modify  #
//#  it under the terms of the GNU General Public License as published by  #
//#  the Free Software Foundation; version 2 or later of the License.      #
//#                                                                        #
//#  This program is distributed in the hope that it will be useful,       #
//#  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
//#  GNU General Public License for more details.                          #
//#                                                                        #
//#                         COPYRIGHT: Luca Penasa                         #
//#                                                                        #
//##########################################################################

#include "BaseFilter.h"

class StatisticalOutliersRemover : public BaseFilter
{
public:
	StatisticalOutliersRemover();
	~StatisticalOutliersRemover() override;

protected:
	//inherited from BaseFilter
	int compute() override;
	int getParametersFromDialog() override;

protected: //members
	int m_kNN;
	float m_std;
};
