#include "QwtCharts/QwtHistogramChartPanel.h"

#include "QwtCharts/QwtChartsManager.h"
#include "QwtCharts/QwtHistogramOptionsPanel.h"
#include "QwtCharts/QwtHistogramCoordinatesPicker.h"
#include "QwtCharts/QwtUtilities.h"
#include <QtUtil/QtUnicodeHelper.h>
#include <QtUtil/QtPaintAttributes.h>
#include <TkUtil/Exception.h>
#include <TkUtil/MemoryError.h>
#include <TkUtil/InformationLog.h>

#include <qwt_column_symbol.h>

#include <assert.h>
#include <values.h>


USING_UTIL
USING_STD

static const Charset	charset ("àéèùô");
USE_ENCODING_AUTODETECTION

#define BEGIN_TRY_CATCH_BLOCK                                              \
	bool			hasError	= false;                                   \
	UTF8String		errorMsg (charset);                                    \
	try {

#define COMPLETE_TRY_CATCH_BLOCK                                           \
	}                                                                      \
	catch (const IN_UTIL Exception& exc)                                   \
	{                                                                      \
		errorMsg << exc.getFullMessage ( );                                \
		hasError	= true;                                                \
	}                                                                      \
	catch (const IN_STD exception& stdExc) /* IN_STD : pour OSF */         \
	{                                                                      \
		errorMsg << stdExc.what ( );                                       \
		hasError	= true;                                                \
	}                                                                      \
	catch (...)                                                            \
	{                                                                      \
		errorMsg << "erreur non documentée.";                              \
		hasError	= true;                                                \
	}


// ============================================================================
//                            FONCTIONS STATIQUES
// ============================================================================



// ============================================================================
//                     LA CLASSE QwtHistogramChartPanel
// ============================================================================


QwtHistogramChartPanel::QwtHistogramChartPanel (QWidget* parent)
	: QwtChartPanel (parent),
	  _barChart (0)
{
	_barChart	= new QwtExtendedMultiBarChart ("Mon histogramme");
	_barChart->setLayoutPolicy (QwtExtendedMultiBarChart::AutoAdjustSamples);
	_barChart->setSpacing (20);
	_barChart->setMargin (5);
	addPlotItem (*_barChart);

	if (0 != getGrid ( ))
	{
		getGrid ( )->enableX (false);
		getGrid ( )->enableXMin (false);
		getGrid ( )->enableYMin (false);
	}	// if (0 != getGrid ( ))

	QwtHistogramCoordinatesPicker*	picker	=
		new QwtHistogramCoordinatesPicker (
			QwtPlot::xBottom, QwtPlot::yLeft, 0, QwtPlotPicker::RectRubberBand,
			QwtPicker::AlwaysOn,  &getPlot ( ));
	CHECK_NULL_PTR_ERROR (picker)
	picker->setChartPanel (this);
	setCoordinatesPicker (*picker);

	try
	{
		setSelectionMode (QwtChartsManager::getSelectionMode ( ));
//		setHistogramStyle (QwtExtendedMultiBarChart::STYLE_GROUPED);
		setHistogramStyle (QwtExtendedMultiBarChart::STYLE_OVERLAPPED);
	}
	catch (...)
	{
	}
}	// QwtHistogramChartPanel::QwtHistogramChartPanel


QwtHistogramChartPanel::QwtHistogramChartPanel (
											const QwtHistogramChartPanel& view)
	: QwtChartPanel (0),
	  _barChart (0)
{
	assert (0 && "QwtHistogramChartPanel copy constructor is not allowed.");
}	// QwtHistogramChartPanel::QwtHistogramChartPanel (const QwtHistogramChartPanel& view)


QwtHistogramChartPanel& QwtHistogramChartPanel::operator = (
											const QwtHistogramChartPanel& view)
{
	assert (0 && "QwtHistogramChartPanel::operator = is not allowed.");
	return *this;
}	// QwtHistogramChartPanel::QwtHistogramChartPanel (const QwtHistogramChartPanel& view)


QwtHistogramChartPanel::~QwtHistogramChartPanel ( )
{
}	// QwtHistogramChartPanel::~QwtHistogramChartPanel


QwtExtendedMultiBarChart& QwtHistogramChartPanel::getHistogram ( )
{
	CHECK_NULL_PTR_ERROR (_barChart)
	return *_barChart;
}	// QwtHistogramChartPanel::getHistogram


const QwtExtendedMultiBarChart& QwtHistogramChartPanel::getHistogram ( ) const
{
	CHECK_NULL_PTR_ERROR (_barChart)
	return *_barChart;
}	// QwtHistogramChartPanel::getHistogram


void QwtHistogramChartPanel::dataModified ( )
{
	getCoordinatesPicker ( ).updateFromPlot ( );

	QwtChartPanel::dataModified ( );
}	// QwtHistogramChartPanel::dataModified


void QwtHistogramChartPanel::setSeries (
		const QList<QwtText>& seriesNames, const QVector<double>& classValues,
		const QVector< QVector <double> >& seriesValues)
{
	if (classValues.size ( ) != seriesValues.size ( ))
	{
		UTF8String	message (charset);
		message << "Nombre de classes différents ("
		        << (unsigned long)classValues.size ( )
		        << "/" << (unsigned long)seriesValues.size ( )
		        << " dans les arguments de QwtHistogramChartPanel::setSeries.";
		throw Exception (message);
	}	// if (classValues.size ( ) != seriesValues.size ( ))

	const QwtColumnSymbol::Style		columnStyle	= QwtColumnSymbol::Box;
	const QwtColumnSymbol::FrameStyle	frameStyle	= QwtColumnSymbol::Raised;
	CHECK_NULL_PTR_ERROR (_barChart)
	_barChart->setBarTitles (seriesNames);
	unsigned short	s	= 0;
	for (s = 0; s < seriesNames.size ( ); s++)
	{
		QwtColumnSymbol*	symbol	= new QwtColumnSymbol (columnStyle);
		symbol->setLineWidth (5);
		symbol->setFrameStyle (frameStyle);
		QtPaintAttributes	qpa	= QtPaintAttributes(PaintAttributes::create(s));
		symbol->setPalette (qpa.getPen ( ).color ( ));
		_barChart->setSymbol (s, symbol);
	}	// for (s = 0; s < seriesNames.size ( ); s++)

	QVector< QwtSetSample >	series;
	for (size_t i = 0; i < classValues.size ( ); i++)
	{
		QwtSetSample	sample (classValues [i], seriesValues [i]);
		series.push_back (sample);
	}	// for (size_t i = 0; i < classValues.size ( ); i++)

	_barChart->setSamples (series);

	// Provoquer une éventuelle mise à jour des axes et autres :
	dataModified ( );
}	// QwtHistogramChartPanel::setSeries


vector<QwtChartPanel::QwtChartEditionPanel*>
				QwtHistogramChartPanel::createEditionPanels (QWidget* parent)
{
	vector<QwtChartPanel::QwtChartEditionPanel*>	list	=
								QwtChartPanel::createEditionPanels (parent);
	list.push_back (new QwtHistogramOptionsPanel (parent, this));

	return list;
}	// QwtHistogramChartPanel::createEditionPanels


vector<QwtChartPanel::DataPaintAttributes>
					QwtHistogramChartPanel::getDataPaintAttributes ( ) const
{
	CHECK_NULL_PTR_ERROR (_barChart)
	vector<QwtChartPanel::DataPaintAttributes>	attributes;

	QList<QwtText>		names	= _barChart->barTitles ( );
	size_t				index	= 0;
	for (QList<QwtText>::iterator it = names.begin ( );
		 names.end ( ) != it; it++, index++)
	{
		const QwtColumnSymbol*	symbol	=
				((const QwtExtendedMultiBarChart*)_barChart)->symbol (index);
		CHECK_NULL_PTR_ERROR (symbol)
		QColor	fg	= symbol->palette ( ).color (QPalette::WindowText);
		QColor	bg	= symbol->palette ( ).color (QPalette::Window);
		
		QPen	pen (fg);
		pen.setBrush (bg);
		pen.setWidth (symbol->lineWidth ( ));
		QtPaintAttributes	qpa (pen);
		DataPaintAttributes	pa (
					(*it).text ( ).toStdString ( ), qpa.getPaintAttributes ( ));
		attributes.push_back (pa);
	}   // for (vector<QwtPlotItem*>::const_iterator it = items.begin ( ); ...

	return attributes;
}	// QwtHistogramChartPanel::getDataPaintAttributes


void QwtHistogramChartPanel::setDataPaintAttributes (
        size_t index, const QwtChartPanel::DataPaintAttributes& paintAttributes)
{
	CHECK_NULL_PTR_ERROR (_barChart)
	const QwtColumnSymbol*	symbol	=
				((const QwtExtendedMultiBarChart*)_barChart)->symbol (index);
	if (0 == symbol)
	{
		UTF8String	message (charset);
		message << "L'histogramme n'a pas de " << index << "-ème série."
		        << " Erreur survenue dans "
		        << "QwtHistogramChartPanel::setDataPaintAttributes.";
		throw Exception (message);
	}	// if (0 == symbol)

	QtPaintAttributes			qpa (paintAttributes._pen);
	QwtColumnSymbol*			sym	= QwtUtilities::cloneSymbol (*symbol);
	CHECK_NULL_PTR_ERROR (sym)
	if ((unsigned short)-1 != paintAttributes._pen.getLineWidth ( ))
		sym->setLineWidth (paintAttributes._pen.getLineWidth ( ));
	QPalette	p (sym->palette ( ));
	QColor	fg	= sym->palette ( ).color (QPalette::WindowText);
	QColor	bg	= sym->palette ( ).color (QPalette::Window);
//	if (true == paintAttributes._fgValid)
//		p.setColor (QPalette::WindowText, qpa.getPen ( ).color ( ));
//	if (true == paintAttributes._bgValid)
//		p.setColor (QPalette::Window, qpa.getPen ( ).brush ( ).color ( ));
// Version courrante : on laisse Qt calculer les ombrages :
	if (true == paintAttributes._bgValid)
		p	= QPalette (qpa.getPen ( ).brush ( ).color ( ));
	sym->setPalette (p);
	_barChart->setSymbol (index, sym);
}   // QwtHistogramChartPanel::setDataPaintAttributes


QwtExtendedMultiBarChart::HISTOGRAM_STYLE
							QwtHistogramChartPanel::getHistogramStyle ( ) const
{
	CHECK_NULL_PTR_ERROR (_barChart)
	return _barChart->getHistogramStyle ( );
}	// QwtHistogramChartPanel::getHistogramStyle


void QwtHistogramChartPanel::setHistogramStyle (
								QwtExtendedMultiBarChart::HISTOGRAM_STYLE style)
{
	CHECK_NULL_PTR_ERROR (_barChart)
	_barChart->setHistogramStyle (style);
}	// QwtHistogramChartPanel::setHistogramStyle


bool QwtHistogramChartPanel::displayElementsNum ( ) const
{
	CHECK_NULL_PTR_ERROR (_barChart)
	return _barChart->displayElementsNum ( );
}	// QwtHistogramChartPanel::displayElementsNum


void QwtHistogramChartPanel::displayElementsNum (bool display)
{
	CHECK_NULL_PTR_ERROR (_barChart)
	return _barChart->displayElementsNum (display);
}	// QwtHistogramChartPanel::displayElementsNum


QwtExtendedMultiBarChart::POSITION
							QwtHistogramChartPanel::getElementNumPos ( ) const
{
	CHECK_NULL_PTR_ERROR (_barChart)
	return _barChart->getElementNumPos ( );
}	// QwtHistogramChartPanel::getElementNumPos


void QwtHistogramChartPanel::setElementNumPos (
										QwtExtendedMultiBarChart::POSITION pos)
{
	CHECK_NULL_PTR_ERROR (_barChart)
	_barChart->setElementNumPos (pos);
}	// QwtHistogramChartPanel::setElementNumPos


void QwtHistogramChartPanel::legendCheckedCallback (
								const QVariant& itemInfo, bool on, int index)
{
	CHECK_NULL_PTR_ERROR (_barChart)
	if (on != _barChart->isVisible (index))
	{
		_barChart->setVisible (index, on);
		getPlot ( ).replot ( );
	}	// if (on != _barChart->isVisible (index))
}	// QwtHistogramChartPanel::legendCheckedCallback


