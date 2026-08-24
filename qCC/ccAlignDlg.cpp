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
 * @file ccAlignDlg.cpp
 * @brief Implementation of point cloud alignment dialog
 * @details Provides UI for fine-tuning alignment between two point clouds
 * using various sampling methods and ICP-like algorithms.
 * @see ccAlignDlg
 */

#include "ccAlignDlg.h"

#include "mainwindow.h"
#include "ui_alignDlg.h"

// common
#include <ccQtHelpers.h>

// CCCoreLib
#include <CloudSamplingTools.h>
#include <GeometricalAnalysisTools.h>

// qCC_db
#include <ccGenericPointCloud.h>
#include <ccProgressDialog.h>

/**
 * @brief Constructor
 * @param data Data point cloud to align
 * @param model Reference/model point cloud
 * @param parent Parent widget
 * @details Initializes the dialog with sampling options and sets up
 * signal/slot connections for interactive parameter adjustment.
 */
ccAlignDlg::ccAlignDlg(ccGenericPointCloud* data, ccGenericPointCloud* model, QWidget* parent)
    : QDialog(parent, Qt::Tool)
    , m_ui(new Ui::AlignDialog)
{
	m_ui->setupUi(this);

	m_ui->samplingMethod->addItem(tr("None"));
	m_ui->samplingMethod->addItem(tr("Random"));
	m_ui->samplingMethod->addItem(tr("Space"));
	m_ui->samplingMethod->addItem(tr("Octree"));
	m_ui->samplingMethod->setCurrentIndex(NONE);

	ccQtHelpers::SetButtonColor(m_ui->dataColorButton, Qt::red);
	ccQtHelpers::SetButtonColor(m_ui->modelColorButton, Qt::yellow);

	dataObject = data;
	modelObject = model;
	setColorsAndLabels();

	changeSamplingMethod(m_ui->samplingMethod->currentIndex());
	toggleNbMaxCandidates(m_ui->isNbCandLimited->isChecked());

	connect(m_ui->swapButton, &QPushButton::clicked, this, &ccAlignDlg::swapModelAndData);
	connect(m_ui->modelSample, &QSlider::sliderReleased, this, &ccAlignDlg::modelSliderReleased);
	connect(m_ui->dataSample, &QSlider::sliderReleased, this, &ccAlignDlg::dataSliderReleased);
	connect(m_ui->modelSamplingRate, qOverload<double>(&QDoubleSpinBox::valueChanged), this, &ccAlignDlg::modelSamplingRateChanged);
	connect(m_ui->dataSamplingRate, qOverload<double>(&QDoubleSpinBox::valueChanged), this, &ccAlignDlg::dataSamplingRateChanged);
	connect(m_ui->deltaEstimation, &QPushButton::clicked, this, &ccAlignDlg::estimateDelta);
	connect(m_ui->samplingMethod, qOverload<int>(&QComboBox::currentIndexChanged), this, &ccAlignDlg::changeSamplingMethod);
	connect(m_ui->isNbCandLimited, &QCheckBox::toggled, this, &ccAlignDlg::toggleNbMaxCandidates);
}

/**
 * @brief Destructor
 * @details Disables temporary colors and cleans up UI.
 */
ccAlignDlg::~ccAlignDlg()
{
	modelObject->enableTempColor(false);
	dataObject->enableTempColor(false);

	delete m_ui;
}

/**
 * @brief Gets the number of alignment tries
 * @return Number of tries for the ICP algorithm
 */
unsigned ccAlignDlg::getNbTries()
{
	return m_ui->nbTries->value();
}

/**
 * @brief Gets the expected overlap percentage
 * @return Overlap value between 0 and 100
 */
double ccAlignDlg::getOverlap()
{
	return m_ui->overlap->value();
}

/**
 * @brief Gets the delta parameter
 * @return Delta value for distance threshold
 */
double ccAlignDlg::getDelta()
{
	return m_ui->delta->value();
}

/**
 * @brief Gets the model/reference cloud
 * @return Pointer to the model point cloud
 */
ccGenericPointCloud* ccAlignDlg::getModelObject()
{
	return modelObject;
}

/**
 * @brief Gets the data cloud to align
 * @return Pointer to the data point cloud
 */
ccGenericPointCloud* ccAlignDlg::getDataObject()
{
	return dataObject;
}

/**
 * @brief Gets the selected sampling method
 * @return Current sampling method (NONE, RANDOM, SPACE, or OCTREE)
 */
ccAlignDlg::CC_SAMPLING_METHOD ccAlignDlg::getSamplingMethod()
{
	return (CC_SAMPLING_METHOD)m_ui->samplingMethod->currentIndex();
}

/**
 * @brief Checks if candidate count is limited
 * @return true if max candidates is enabled
 */
bool ccAlignDlg::isNumberOfCandidatesLimited()
{
	return m_ui->isNbCandLimited->isChecked();
}

/**
 * @brief Gets the maximum number of candidates
 * @return Maximum number of candidates for ICP
 */
unsigned ccAlignDlg::getMaxNumberOfCandidates()
{
	return m_ui->nbMaxCandidates->value();
}

/**
 * @brief Gets a sampled version of the model cloud
 * @return Sampled reference cloud, or nullptr on failure
 * @details Respects the current sampling method setting and rate.
 * Caller is responsible for deleting the returned cloud.
 */
CCCoreLib::ReferenceCloud* ccAlignDlg::getSampledModel()
{
	CCCoreLib::ReferenceCloud* sampledCloud = nullptr;

	switch (getSamplingMethod())
	{
	case SPACE:
	{
		CCCoreLib::CloudSamplingTools::SFModulationParams modParams(false);
		sampledCloud = CCCoreLib::CloudSamplingTools::resampleCloudSpatially(modelObject,
		                                                                     static_cast<PointCoordinateType>(m_ui->modelSamplingRate->value()),
		                                                                     modParams);
	}
	break;
	case OCTREE:
		if (modelObject->getOctree())
		{
			sampledCloud = CCCoreLib::CloudSamplingTools::subsampleCloudWithOctreeAtLevel(modelObject,
			                                                                              static_cast<unsigned char>(m_ui->modelSamplingRate->value()),
			                                                                              CCCoreLib::CloudSamplingTools::NEAREST_POINT_TO_CELL_CENTER,
			                                                                              nullptr,
			                                                                              modelObject->getOctree().data());
		}
		else
		{
			ccLog::Error("[ccAlignDlg::getSampledModel] Failed to get/compute model octree!");
		}
		break;
	case RANDOM:
	{
		sampledCloud = CCCoreLib::CloudSamplingTools::subsampleCloudRandomly(modelObject,
		                                                                     static_cast<unsigned>(m_ui->modelSamplingRate->value()));
	}
	break;
	default:
	{
		sampledCloud = new CCCoreLib::ReferenceCloud(modelObject);
		if (!sampledCloud->addPointIndex(0, modelObject->size()))
		{
			delete sampledCloud;
			sampledCloud = nullptr;
			ccLog::Error("[ccAlignDlg::getSampledModel] Not enough memory!");
		}
	}
	break;
	}

	return sampledCloud;
}

/**
 * @brief Gets a sampled version of the data cloud
 * @return Sampled reference cloud, or nullptr on failure
 * @details Respects the current sampling method setting and rate.
 * Caller is responsible for deleting the returned cloud.
 */
CCCoreLib::ReferenceCloud* ccAlignDlg::getSampledData()
{
	CCCoreLib::ReferenceCloud* sampledCloud = nullptr;

	switch (getSamplingMethod())
	{
	case SPACE:
	{
		CCCoreLib::CloudSamplingTools::SFModulationParams modParams(false);
		sampledCloud = CCCoreLib::CloudSamplingTools::resampleCloudSpatially(dataObject,
		                                                                     static_cast<PointCoordinateType>(m_ui->dataSamplingRate->value()),
		                                                                     modParams);
	}
	break;
	case OCTREE:
		if (dataObject->getOctree())
		{
			sampledCloud = CCCoreLib::CloudSamplingTools::subsampleCloudWithOctreeAtLevel(dataObject,
			                                                                              static_cast<unsigned char>(m_ui->dataSamplingRate->value()),
			                                                                              CCCoreLib::CloudSamplingTools::NEAREST_POINT_TO_CELL_CENTER,
			                                                                              nullptr,
			                                                                              dataObject->getOctree().data());
		}
		else
		{
			ccLog::Error("[ccAlignDlg::getSampledData] Failed to get/compute data octree!");
		}
		break;
	case RANDOM:
	{
		sampledCloud = CCCoreLib::CloudSamplingTools::subsampleCloudRandomly(dataObject, (unsigned)(m_ui->dataSamplingRate->value()));
	}
	break;
	default:
	{
		sampledCloud = new CCCoreLib::ReferenceCloud(dataObject);
		if (!sampledCloud->addPointIndex(0, dataObject->size()))
		{
			delete sampledCloud;
			sampledCloud = nullptr;
			ccLog::Error("[ccAlignDlg::getSampledData] Not enough memory!");
		}
	}
	break;
	}

	return sampledCloud;
}

/**
 * @brief Updates UI labels and temporary colors for clouds
 * @details Sets model to red, data to yellow, and updates text labels.
 */
void ccAlignDlg::setColorsAndLabels()
{
	if (!modelObject || !dataObject)
		return;

	m_ui->modelCloud->setText(modelObject->getName());
	modelObject->setVisible(true);
	modelObject->setTempColor(ccColor::red);
	modelObject->prepareDisplayForRefresh_recursive();

	m_ui->dataCloud->setText(dataObject->getName());
	dataObject->setVisible(true);
	dataObject->setTempColor(ccColor::yellow);
	dataObject->prepareDisplayForRefresh_recursive();

	MainWindow::RefreshAllGLWindow(false);
}

/**
 * @brief Swaps model and data clouds
 * @details Allows user to switch which cloud is considered the reference.
 */
void ccAlignDlg::swapModelAndData()
{
	std::swap(dataObject, modelObject);
	setColorsAndLabels();
	changeSamplingMethod(m_ui->samplingMethod->currentIndex());
}

/**
 * @brief Handles model sampling slider release
 * @details Converts slider position to sampling rate and updates the UI.
 */
void ccAlignDlg::modelSliderReleased()
{
	double rate = static_cast<double>(m_ui->modelSample->sliderPosition()) / m_ui->modelSample->maximum();
	if (getSamplingMethod() == SPACE)
		rate = 1.0 - rate;
	rate *= m_ui->modelSamplingRate->maximum();
	m_ui->modelSamplingRate->setValue(rate);
	modelSamplingRateChanged(rate);
}

/**
 * @brief Handles data sampling slider release
 * @details Converts slider position to sampling rate and updates the UI.
 */
void ccAlignDlg::dataSliderReleased()
{
	double rate = static_cast<double>(m_ui->dataSample->sliderPosition()) / m_ui->dataSample->maximum();
	if (getSamplingMethod() == SPACE)
		rate = 1.0 - rate;
	rate *= m_ui->dataSamplingRate->maximum();
	m_ui->dataSamplingRate->setValue(rate);
	dataSamplingRateChanged(rate);
}

/**
 * @brief Handles model sampling rate spin box changes
 * @param value New sampling rate value
 * @details Updates the slider position and remaining points label.
 */
void ccAlignDlg::modelSamplingRateChanged(double value)
{
	QString message("An error occurred");

	CC_SAMPLING_METHOD method = getSamplingMethod();
	float rate = static_cast<float>(m_ui->modelSamplingRate->value()) / m_ui->modelSamplingRate->maximum();
	if (method == SPACE)
		rate = 1.0f - rate;
	m_ui->modelSample->setSliderPosition(static_cast<int>(rate * m_ui->modelSample->maximum()));

	switch (method)
	{
	case SPACE:
	{
		CCCoreLib::ReferenceCloud* tmpCloud = getSampledModel(); // DGM FIXME: wow! you generate a spatially sampled cloud just to display its size?!
		if (tmpCloud)
		{
			message = QString("distance units (%1 remaining points)").arg(tmpCloud->size());
			delete tmpCloud;
		}
	}
	break;
	case RANDOM:
	{
		message = QString("remaining points (%1%)").arg(rate * 100.0f, 0, 'f', 1);
	}
	break;
	case OCTREE:
	{
		CCCoreLib::ReferenceCloud* tmpCloud = getSampledModel(); // DGM FIXME: wow! you generate a spatially sampled cloud just to display its size?!
		if (tmpCloud)
		{
			message = QString("%1 remaining points").arg(tmpCloud->size());
			delete tmpCloud;
		}
	}
	break;
	default:
	{
		unsigned remaining = static_cast<unsigned>(rate * modelObject->size());
		message = QString("%1 remaining points").arg(remaining);
	}
	break;
	}

	m_ui->modelRemaining->setText(message);
}

/**
 * @brief Handles data sampling rate spin box changes
 * @param value New sampling rate value
 * @details Updates the slider position and remaining points label.
 */
void ccAlignDlg::dataSamplingRateChanged(double value)
{
	QString message("An error occurred");

	CC_SAMPLING_METHOD method = getSamplingMethod();
	double rate = static_cast<float>(m_ui->dataSamplingRate->value() / m_ui->dataSamplingRate->maximum());
	if (method == SPACE)
		rate = 1.0 - rate;
	m_ui->dataSample->setSliderPosition(static_cast<int>(rate * m_ui->dataSample->maximum()));

	switch (method)
	{
	case SPACE:
	{
		CCCoreLib::ReferenceCloud* tmpCloud = getSampledData(); // DGM FIXME: wow! you generate a spatially sampled cloud just to display its size?!
		if (tmpCloud)
		{
			message = QString("distance units (%1 remaining points)").arg(tmpCloud->size());
			delete tmpCloud;
		}
	}
	break;
	case RANDOM:
	{
		message = QString("remaining points (%1%)").arg(rate * 100.0, 0, 'f', 1);
	}
	break;
	case OCTREE:
	{
		CCCoreLib::ReferenceCloud* tmpCloud = getSampledData(); // DGM FIXME: wow! you generate a spatially sampled cloud just to display its size?!
		if (tmpCloud)
		{
			message = QString("%1 remaining points").arg(tmpCloud->size());
			delete tmpCloud;
		}
	}
	break;
	default:
	{
		unsigned remaining = static_cast<unsigned>(rate * dataObject->size());
		message = QString("%1 remaining points").arg(remaining);
	}
	break;
	}

	m_ui->dataRemaining->setText(message);
}

/**
 * @brief Automatically estimates delta using local density
 * @details Computes local density approximation and sets delta to
 * mean + variance for robust alignment initialization.
 */
void ccAlignDlg::estimateDelta()
{
	ccProgressDialog pDlg(false, this);

	CCCoreLib::ReferenceCloud* sampledData = getSampledData();

	// we have to work on a copy of the cloud in order to prevent the algorithms from modifying the original cloud.
	CCCoreLib::PointCloud cloud;
	{
		if (!cloud.reserve(sampledData->size()))
		{
			ccLog::Error("Not enough memory");
			return;
		}
		for (unsigned i = 0; i < sampledData->size(); i++)
		{
			cloud.addPoint(*sampledData->getPoint(i));
		}
		if (!cloud.enableScalarField())
		{
			ccLog::Error("Not enough memory");
			return;
		}
	}

	if (CCCoreLib::GeometricalAnalysisTools::ComputeLocalDensityApprox(&cloud, CCCoreLib::GeometricalAnalysisTools::DENSITY_KNN, &pDlg) != CCCoreLib::GeometricalAnalysisTools::NoError)
	{
		ccLog::Error("Failed to compute approx. density");
		return;
	}
	unsigned count = 0;
	double meanDensity = 0;
	double meanSqrDensity = 0;
	for (unsigned i = 0; i < cloud.size(); i++)
	{
		ScalarType value = cloud.getPointScalarValue(i);
		if (value == value)
		{
			meanDensity += value;
			meanSqrDensity += static_cast<double>(value) * value;
			count++;
		}
	}

	if (count)
	{
		meanDensity /= count;
		meanSqrDensity /= count;
	}
	double dev = meanSqrDensity - (meanDensity * meanDensity);

	m_ui->delta->setValue(meanDensity + dev);
	delete sampledData;
}

/**
 * @brief Handles sampling method change
 * @param index New sampling method index
 * @details Updates spin box ranges and limits based on the selected
 * sampling method (None, Random, Space, or Octree).
 */
void ccAlignDlg::changeSamplingMethod(int index)
{
	// Reste a changer les textes d'aide
	switch (index)
	{
	case SPACE:
	{
		// model
		{
			m_ui->modelSamplingRate->setDecimals(4);
			int oldSliderPos = m_ui->modelSample->sliderPosition();
			CCVector3 bbMin;
			CCVector3 bbMax;
			modelObject->getBoundingBox(bbMin, bbMax);
			double dist = (bbMin - bbMax).norm();
			m_ui->modelSamplingRate->setMaximum(dist);
			m_ui->modelSample->setSliderPosition(oldSliderPos);
			m_ui->modelSamplingRate->setSingleStep(0.01);
			m_ui->modelSamplingRate->setMinimum(0.);
		}
		// data
		{
			m_ui->dataSamplingRate->setDecimals(4);
			int oldSliderPos = m_ui->dataSample->sliderPosition();
			CCVector3 bbMin;
			CCVector3 bbMax;
			dataObject->getBoundingBox(bbMin, bbMax);
			double dist = (bbMin - bbMax).norm();
			m_ui->dataSamplingRate->setMaximum(dist);
			m_ui->dataSample->setSliderPosition(oldSliderPos);
			m_ui->dataSamplingRate->setSingleStep(0.01);
			m_ui->dataSamplingRate->setMinimum(0.);
		}
	}
	break;
	case RANDOM:
	{
		// model
		{
			m_ui->modelSamplingRate->setDecimals(0);
			m_ui->modelSamplingRate->setMaximum(static_cast<float>(modelObject->size()));
			m_ui->modelSamplingRate->setSingleStep(1.);
			m_ui->modelSamplingRate->setMinimum(0.);
		}
		// data
		{
			m_ui->dataSamplingRate->setDecimals(0);
			m_ui->dataSamplingRate->setMaximum(static_cast<float>(dataObject->size()));
			m_ui->dataSamplingRate->setSingleStep(1.);
			m_ui->dataSamplingRate->setMinimum(0.);
		}
	}
	break;
	case OCTREE:
	{
		// model
		{
			if (!modelObject->getOctree())
				modelObject->computeOctree();
			m_ui->modelSamplingRate->setDecimals(0);
			m_ui->modelSamplingRate->setMaximum(static_cast<double>(CCCoreLib::DgmOctree::MAX_OCTREE_LEVEL));
			m_ui->modelSamplingRate->setMinimum(1.);
			m_ui->modelSamplingRate->setSingleStep(1.);
		}
		// data
		{
			if (!dataObject->getOctree())
				dataObject->computeOctree();
			m_ui->dataSamplingRate->setDecimals(0);
			m_ui->dataSamplingRate->setMaximum(static_cast<double>(CCCoreLib::DgmOctree::MAX_OCTREE_LEVEL));
			m_ui->dataSamplingRate->setMinimum(1.);
			m_ui->dataSamplingRate->setSingleStep(1.);
		}
	}
	break;
	default:
	{
		// model
		{
			m_ui->modelSamplingRate->setDecimals(2);
			m_ui->modelSamplingRate->setMaximum(100.);
			m_ui->modelSamplingRate->setSingleStep(0.01);
			m_ui->modelSamplingRate->setMinimum(0.);
		}
		// data
		{
			m_ui->dataSamplingRate->setDecimals(2);
			m_ui->dataSamplingRate->setMaximum(100.);
			m_ui->dataSamplingRate->setSingleStep(0.01);
			m_ui->dataSamplingRate->setMinimum(0.);
		}
	}
	break;
	}

	if (index == NONE)
	{
		// model
		m_ui->modelSample->setSliderPosition(m_ui->modelSample->maximum());
		m_ui->modelSample->setEnabled(false);
		m_ui->modelSamplingRate->setEnabled(false);
		// data
		m_ui->dataSample->setSliderPosition(m_ui->dataSample->maximum());
		m_ui->dataSample->setEnabled(false);
		m_ui->dataSamplingRate->setEnabled(false);
	}
	else
	{
		// model
		m_ui->modelSample->setEnabled(true);
		m_ui->modelSamplingRate->setEnabled(true);
		// data
		m_ui->dataSample->setEnabled(true);
		m_ui->dataSamplingRate->setEnabled(true);
	}

	modelSliderReleased();
	dataSliderReleased();
}

/**
 * @brief Toggles max candidates spin box enabled state
 * @param activ true to enable, false to disable
 */
void ccAlignDlg::toggleNbMaxCandidates(bool activ)
{
	m_ui->nbMaxCandidates->setEnabled(activ);
}
