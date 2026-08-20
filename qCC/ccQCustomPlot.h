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
 * @file ccQCustomPlot.h
 *
 * @brief Custom QCustomPlot widgets for CloudCompare.
 *
 * @details Extension classes for QCustomPlot charting library.
 *
 * Classes:
 * - **QCPBarsWithText**: Bar chart with text labels
 * - **QCPColoredBars**: Bar chart with per-bar colors
 * - **QCPSelectableCursor**: Selectable cursor/indicator
 * - **QCPHiddenArea**: Greyed-out selectable region
 * - **QCPArrow**: Selectable arrow handle
 *
 * These are used primarily by ccHistogramWindow for interactive
 * histogram visualization with draggable range selectors.
 *
 * @author EDF R&D / TELECOM ParisTech (ENST-TSI)
 */

#ifndef CC_QCUSTOMPLOT_HEADER
#define CC_QCUSTOMPLOT_HEADER

// QCustomPlot
#ifdef _MSC_VER
// To get rid of the really annoying warnings about unsafe methods
#pragma warning(push)
#pragma warning(disable : 4996)
#endif
#include <qcustomplot.h>
#ifdef _MSC_VER
#pragma warning(pop)
#endif

// System
#include <assert.h>

/**
 * @class QCPBarsWithText
 *
 * @brief Bar chart with optional text labels.
 *
 * @extends QCPBars
 */
class QCPBarsWithText : public QCPBars
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the bar with text.
	 *
	 * @param[in] keyAxis Key axis.
	 * @param[in] valueAxis Value axis.
	 */
	QCPBarsWithText(QCPAxis* keyAxis, QCPAxis* valueAxis)
	    : QCPBars(keyAxis, valueAxis)
	    , m_textOnTheLeft(false)
	{
	}

	/**
	 * @brief Set the bar text.
	 * @param[in] text Text to display.
	 */
	void setText(QString text)
	{
		m_text = QStringList(text);
	}

	/**
	 * @brief Append text line.
	 * @param[in] text Text to append.
	 */
	void appendText(QString text)
	{
		m_text.append(text);
	}

	/**
	 * @brief Set text alignment.
	 * @param[in] left Align text to the left.
	 */
	void setTextAlignment(bool left)
	{
		m_textOnTheLeft = left;
	}

  protected:
	//! Text lines
	QStringList m_text;

	//! Text on the left side
	bool m_textOnTheLeft;

	// reimplemented virtual draw method
	virtual void draw(QCPPainter* painter) override
	{
		if (!mKeyAxis || !mValueAxis)
		{
			qDebug() << Q_FUNC_INFO << "invalid key or value axis";
			return;
		}

		// switch to standard display
		QCPBars::draw(painter);

		int fontHeight = painter->fontMetrics().height();

		if (!data()->isEmpty())
		{
			double& key   = data()->begin()->key;
			double& value = data()->begin()->value;
			QPointF P     = coordsToPixels(key, value);
			// apply a small shift
			int margin = 5; // in pixels
			if (m_textOnTheLeft)
				margin = -margin;
			P.setX(P.x() + margin);
			// we draw at the 'base' line
			P.setY(P.y() + fontHeight);

			for (int i = 0; i < m_text.size(); ++i)
			{
				QPointF Pstart = P;
				if (m_textOnTheLeft)
					Pstart.setX(P.x() - painter->fontMetrics().horizontalAdvance(m_text[i]));
				painter->drawText(Pstart, m_text[i]);
				P.setY(P.y() + fontHeight);
			}
		}
	}
};

/**
 * @class QCPColoredBars
 *
 * @brief Bar chart with per-bar custom colors.
 *
 * @extends QCPBars
 */
class QCPColoredBars : public QCPBars
{
	Q_OBJECT

  public:
	/**
	 * @brief Colored bar data.
	 */
	class QCPColoredBarData : public QCPBarsData
	{
	  public:
		QCPColoredBarData()
		    : color(Qt::blue)
		{
		}

		QColor color; //!< Bar color
	};

	//! Colored bar data map
	typedef QMultiMap<double, QCPColoredBarData> QCPColoredBarDataMap;

	/**
	 * @brief Construct colored bars.
	 *
	 * @param[in] keyAxis Key axis.
	 * @param[in] valueAxis Value axis.
	 */
	QCPColoredBars(QCPAxis* keyAxis, QCPAxis* valueAxis)
	    : QCPBars(keyAxis, valueAxis)
	{
	}

	/**
	 * @brief Set data without colors.
	 *
	 * @param[in] key Key values.
	 * @param[in] value Value values.
	 */
	void setData(const QVector<double>& key, const QVector<double>& value)
	{
		// no colors? we switch to the standard QCPBars object
		m_coloredData.clear();
		QCPBars::setData(key, value);
	}

	/**
	 * @brief Set data with colors.
	 *
	 * @param[in] key Key values.
	 * @param[in] value Value values.
	 * @param[in] colors Per-bar colors.
	 */
	void setData(const QVector<double>& key, const QVector<double>& value, const QVector<QColor>& colors)
	{
		Q_ASSERT(colors.size() == key.size());

		data()->clear(); // we duplicate the structures so that other stuff in QCPBarData works!

		int n = qMin(key.size(), value.size());

		for (int i = 0; i < n; ++i)
		{
			QCPColoredBarData newData;
			newData.key   = key[i];
			newData.value = value[i];
			if (colors.size() > i)
				newData.color = colors[i];
			m_coloredData.insertMulti(newData.key, newData);
			QCPBars::addData(newData.key, newData.value);
		}
	}

	/**
	 * @brief Get clip rectangle.
	 */
	inline QRect rect() const
	{
		return clipRect();
	}

	// reimplemented virtual methods:
	/**
	 * @brief Clear all data.
	 */
	virtual void clearData() override
	{
		QCPBars::data().clear();
		m_coloredData.clear();
	}

  protected:
	// reimplemented virtual draw method
	virtual void draw(QCPPainter* painter) override
	{
		// no colors?
		if (m_coloredData.empty())
		{
			// switch to standard display
			QCPBars::draw(painter);
		}

		if (!mKeyAxis || !mValueAxis)
		{
			qDebug() << Q_FUNC_INFO << "invalid key or value axis";
			return;
		}

		QCPColoredBarDataMap::const_iterator it;
		for (it = m_coloredData.constBegin(); it != m_coloredData.constEnd(); ++it)
		{
			// skip bar if not visible in key axis range:
			if (it.key() + mWidth * 0.5 < mKeyAxis.data()->range().lower || it.key() - mWidth * 0.5 > mKeyAxis.data()->range().upper)
				continue;

			QRectF barRect = getBarRect(it.key(), it.value().value);
			// draw bar fill:
			if (brush().style() != Qt::NoBrush && brush().color().alpha() != 0)
			{
				QBrush theBrush = brush();
				theBrush.setColor(it.value().color);

				applyFillAntialiasingHint(painter);
				painter->setPen(Qt::NoPen);
				painter->setBrush(theBrush);
				painter->drawRect(barRect);
			}
			// draw bar line:
			if (pen().style() != Qt::NoPen && pen().color().alpha() != 0)
			{
				QPen thePen = pen();
				thePen.setColor(it.value().color);

				applyDefaultAntialiasingHint(painter);
				painter->setPen(thePen);
				painter->setBrush(Qt::NoBrush);
				painter->drawPolyline(barRect);
			}
		}
	}

	//! Colored data map
	QCPColoredBarDataMap m_coloredData;
};

/**
 * @class QCPSelectableCursor
 *
 * @brief Selectable cursor interface for interactive charts.
 *
 * @extends QCPAbstractPlottable
 */
class QCPSelectableCursor : public QCPAbstractPlottable
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct the selectable cursor.
	 *
	 * @param[in] keyAxis Key axis.
	 * @param[in] valueAxis Value axis.
	 */
	explicit QCPSelectableCursor(QCPAxis* keyAxis, QCPAxis* valueAxis)
	    : QCPAbstractPlottable(keyAxis, valueAxis)
	    , mCurrentVal(0)
	    , mMinVal(0)
	    , mMaxVal(0)
	    , mLastPos(-1, -1)
	    , mLastRadius(0)
	{
	}

	/**
	 * @brief Check if selectable at position.
	 *
	 * @param[in] click Click position.
	 * @return true if selectable.
	 */
	inline virtual bool isSelectable(QPoint click) const
	{
		if (mLastPos.x() < 0 || mLastPos.y() < 0)
			return false;
		QPoint d = mLastPos - click;
		return (d.x() * d.x() + d.y() * d.y() <= mLastRadius * mLastRadius);
	}

	// getters
	/**
	 * @brief Get current value.
	 */
	inline double currentVal() const { return mCurrentVal; }

	/**
	 * @brief Get minimum value.
	 */
	inline double minVal() const { return mMinVal; }

	/**
	 * @brief Get maximum value.
	 */
	inline double maxVal() const { return mMaxVal; }

	/**
	 * @brief Get range.
	 *
	 * @param[out] minVal Minimum.
	 * @param[out] maxVal Maximum.
	 */
	inline void range(double& minVal, double& maxVal) const
	{
		minVal = mMinVal;
		maxVal = mMaxVal;
	}

	// setters
	/**
	 * @brief Set current value.
	 *
	 * @param[in] val Current value.
	 */
	inline void setCurrentVal(double val)
	{
		mCurrentVal = std::max(std::min(val, mMaxVal), mMinVal);
	}

	/**
	 * @brief Set range.
	 *
	 * @param[in] minVal Minimum.
	 * @param[in] maxVal Maximum.
	 */
	inline void setRange(double minVal, double maxVal)
	{
		mMinVal = minVal;
		mMaxVal = maxVal;
	}

	/**
	 * @brief Convert pixel X to key.
	 *
	 * @param[in] pixX Pixel X coordinate.
	 * @return Key value.
	 */
	inline double pixelToKey(int pixX) const
	{
		return keyAxis() ? keyAxis()->pixelToCoord(pixX) : 0;
	}

	/**
	 * @brief Convert pixel Y to value.
	 *
	 * @param[in] pixY Pixel Y coordinate.
	 * @return Value.
	 */
	inline double pixelToValue(int pixY) const
	{
		return valueAxis() ? valueAxis()->pixelToCoord(pixY) : 0;
	}

	// reimplemented virtual methods:
	/**
	 * @brief Clear data.
	 */
	virtual void clearData() override {}

	/**
	 * @brief Selection test.
	 */
	double selectTest(const QPointF& pos, bool onlySelectable, QVariant* details = nullptr) const override
	{
		return -1;
	} // we don't use the QCP internal selection mechanism!

  protected:
	// reimplemented virtual methods:
	/**
	 * @brief Draw legend icon.
	 */
	void drawLegendIcon(QCPPainter* painter, const QRectF& rect) const override {}

	/**
	 * @brief Get key range.
	 */
	QCPRange getKeyRange(bool& foundRange, QCP::SignDomain inSignDomain = QCP::sdBoth) const override
	{
		foundRange = false;
		return QCPRange();
	}

	/**
	 * @brief Get value range.
	 */
	QCPRange getValueRange(bool& foundRange, QCP::SignDomain inSignDomain = QCP::sdBoth, const QCPRange& inKeyRange = QCPRange()) const override
	{
		foundRange = false;
		return QCPRange();
	}

	// property members:
	//! Current value
	double mCurrentVal;

	//! Range
	double mMinVal, mMaxVal;

	//! Last position
	QPoint mLastPos;

	//! Last radius
	int mLastRadius;
};

/**
 * @class QCPHiddenArea
 *
 * @brief Greyed-out selectable region for range hiding.
 *
 * @extends QCPSelectableCursor
 */
class QCPHiddenArea : public QCPSelectableCursor
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct hidden area.
	 *
	 * @param[in] leftSide Whether on the left side.
	 * @param[in] keyAxis Key axis.
	 * @param[in] valueAxis Value axis.
	 */
	explicit QCPHiddenArea(bool leftSide, QCPAxis* keyAxis, QCPAxis* valueAxis)
	    : QCPSelectableCursor(keyAxis, valueAxis)
	    , mLeftSide(leftSide)
	{
		mPen = QPen(QColor(80, 80, 80), Qt::SolidLine); // dark grey
		mPen.setWidth(2);
		setPen(mPen);

		mBrush = QBrush(Qt::white, Qt::SolidPattern); // white
		setBrush(mBrush);
	}

  protected:
	// reimplemented virtual methods:
	/**
	 * @brief Draw the hidden area.
	 */
	virtual void draw(QCPPainter* painter) override
	{
		if (!keyAxis())
			return;

		QRect rect = clipRect();

		double currentPosd = keyAxis()->coordToPixel(mCurrentVal);
		if (mLeftSide)
		{
			int x2 = static_cast<int>(ceil(currentPosd));
			if (x2 < rect.x())
				return;
			rect.setWidth(x2 - rect.x());
		}
		else
		{
			int x1 = static_cast<int>(floor(currentPosd));
			if (x1 < rect.x())
				return;
			int newWidth = rect.width() - (x1 - rect.x());
			rect.setX(x1);
			rect.setWidth(newWidth);
		}

		// draw greyed rect
		if ((mLeftSide && mCurrentVal > mMinVal)
		    || (!mLeftSide && mCurrentVal < mMaxVal))
		{
			applyFillAntialiasingHint(painter);
			painter->setPen(Qt::NoPen);
			painter->setBrush(QBrush(QColor(128, 128, 128, 128), Qt::SolidPattern)); // semi-transparent grey
			painter->drawPolygon(rect);
		}

		// draw circle (handle)
		if (pen().style() != Qt::NoPen && pen().color().alpha() != 0)
		{
			// circle
			QPoint C(mLeftSide ? rect.x() + rect.width() : rect.x(), rect.y() + rect.height() / 2);
			int    r = rect.height() / 10;

			painter->setPen(pen());
			painter->setBrush(brush());
			painter->drawEllipse(C, r, r);

			painter->setPen(QPen(QColor(128, 128, 128, 128), Qt::SolidLine)); // semi-transparent grey
			painter->drawLine(C + QPoint(0, r), C - QPoint(0, r));

			// save last circle position
			mLastPos    = C;
			mLastRadius = r;
		}
		else
		{
			// no circle
			mLastPos    = QPoint(-1, -1);
			mLastRadius = 0;
		}
	}

	//! Whether on left side
	bool mLeftSide;
};

/**
 * @class QCPArrow
 *
 * @brief Selectable arrow handle for range manipulation.
 *
 * @extends QCPSelectableCursor
 */
class QCPArrow : public QCPSelectableCursor
{
	Q_OBJECT

  public:
	/**
	 * @brief Construct arrow.
	 *
	 * @param[in] keyAxis Key axis.
	 * @param[in] valueAxis Value axis.
	 */
	explicit QCPArrow(QCPAxis* keyAxis, QCPAxis* valueAxis)
	    : QCPSelectableCursor(keyAxis, valueAxis)
	{
		mPen.setColor(QColor(128, 128, 0)); // dark yellow
		mPen.setStyle(Qt::SolidLine);
		mPen.setWidth(2);
		setPen(mPen);

		mBrush.setColor(QColor(255, 255, 0, 196)); // semi-transparent yellow
		mBrush.setStyle(Qt::SolidPattern);
		setBrush(mBrush);
	}

	/**
	 * @brief Set arrow color.
	 *
	 * @param[in] r Red.
	 * @param[in] g Green.
	 * @param[in] b Blue.
	 */
	void setColor(int r, int g, int b)
	{
		mBrush.setColor(QColor(r, g, b, 196)); // semi-transparent color
		setBrush(mBrush);
	}

  protected:
	// reimplemented virtual methods:
	/**
	 * @brief Draw the arrow.
	 */
	virtual void draw(QCPPainter* painter) override
	{
		if (!keyAxis())
			return;

		QRect rect = clipRect();

		int currentPos = static_cast<int>(keyAxis()->coordToPixel(mCurrentVal));

		int r = rect.height() / 10;

		// draw dashed line
		{
			QPen pen(QColor(128, 128, 128, 128), Qt::DashLine);
			pen.setWidth(1);
			painter->setPen(pen); // semi-transparent grey
			painter->drawLine(QPoint(currentPos, rect.y() + 2 * r), QPoint(currentPos, rect.y() + rect.height()));
		}

		// draw triangle(handle)
		if (pen().style() != Qt::NoPen && pen().color().alpha() != 0)
		{
			QPoint O(currentPos, rect.y() + r);
			QPoint T[3] = {O + QPoint(0, r), O - QPoint(r, r), O - QPoint(-r, r)};

			painter->setPen(pen());
			painter->setBrush(brush());
			painter->drawPolygon(T, 3);

			// save last circle position
			mLastPos    = O;
			mLastRadius = r;
		}
		else
		{
			// no circle
			mLastPos    = QPoint(-1, -1);
			mLastRadius = 0;
		}
	}
};

#endif // CC_QCUSTOMPLOT_HEADER
