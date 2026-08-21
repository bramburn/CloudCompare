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
 * @file ccSORFilterDlg.cpp
 *
 * @brief SOR filter dialog implementation
 *
 * Dialog for configuring Statistical Outlier Removal (SOR) filter.
 *
 * ## SOR Algorithm
 *
 * For each point, find K nearest neighbors. Compute mean distance
 * to all neighbors. If a point's mean distance exceeds the global
 * mean by more than n*sigma (standard deviation), mark it as an outlier.
 *
 * ## Parameters
 *
 * - **KNN (K)**: number of nearest neighbors to consider
 * - **n sigma**: threshold multiplier for outlier detection
 * - **Max threads**: parallel processing threads
 *
 * @see ccSORFilterDlg.h
 */

#include "ccSORFilterDlg.h"

#include "ui_sorFilterDlg.h"

#include <QThread>

ccSORFilterDlg::ccSORFilterDlg(QWidget* parent /*=nullptr*/)
    : QDialog(parent, Qt::Tool)
    , m_ui(new Ui::SorFilterDialog)
{
	m_ui->setupUi(this);

	static const int MaxThreadCount = QThread::idealThreadCount();
	m_ui->maxThreadCountSpinBox->setRange(1, MaxThreadCount);
	m_ui->maxThreadCountSpinBox->setSuffix(QString(" / %1").arg(MaxThreadCount));
}

ccSORFilterDlg::~ccSORFilterDlg()
{
	delete m_ui;
}

int ccSORFilterDlg::KNN() const
{
	return m_ui->knnSpinBox->value();
}

void ccSORFilterDlg::setKNN(int knn)
{
	m_ui->knnSpinBox->setValue(knn);
}

double ccSORFilterDlg::nSigma() const
{
	return m_ui->nSigmaDoubleSpinBox->value();
}

void ccSORFilterDlg::setNSigma(double nSigma)
{
	m_ui->nSigmaDoubleSpinBox->setValue(nSigma);
}

void ccSORFilterDlg::setMaxThreadCount(int count)
{
	m_ui->maxThreadCountSpinBox->setValue(count);
}

int ccSORFilterDlg::maxThreadCount() const
{
	return m_ui->maxThreadCountSpinBox->value();
}
