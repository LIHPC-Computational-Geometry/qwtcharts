#include "QwtCharts/QwtCurveChartPanel.h"
#include "QwtCharts/QwtChartsManager.h"
#include "QwtCharts/QwtExtendedPlotCurve.h"
#include "QwtCharts/QwtPlotCurveCoordinatesPicker.h"
#include <QtUtil/QtPaintAttributes.h>
#include <QtUtil/QtUnicodeHelper.h>
#include <TkUtil/Exception.h>
#include <TkUtil/MemoryError.h>
#include <TkUtil/InformationLog.h>


#include <assert.h>
#include <values.h>


USING_UTIL
USING_STD

static const Charset	charset ("àéèùô");


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




QwtCurveChartPanel::QwtCurveChartPanel (QWidget* parent)
	: QwtChartPanel (parent),
	  _curves ( )
{
	QwtPlotCurveCoordinatesPicker*	picker	= new QwtPlotCurveCoordinatesPicker(
					QwtPlot::xBottom, QwtPlot::yLeft,
					0,
					QwtPlotPicker::RectRubberBand, QwtPicker::AlwaysOn,
					&getPlot ( ));
	CHECK_NULL_PTR_ERROR (picker)
	picker->setChartPanel (this);
	setCoordinatesPicker (*picker);

	try
	{
		setSelectionMode (QwtChartsManager::getSelectionMode ( ));
	}
	catch (...)
	{
	}

	connect (&getCoordinatesPicker ( ), SIGNAL (pointModified (int, int)),
	         this, SLOT (pointModifiedCallback (int, int)));
}	// QwtCurveChartPanel::QwtCurveChartPanel


QwtCurveChartPanel::QwtCurveChartPanel (const QwtCurveChartPanel& view)
	: QwtChartPanel (0),
	  _curves ( )
{
	assert (0 && "QwtCurveChartPanel copy constructor is not allowed.");
}	// QwtCurveChartPanel::QwtCurveChartPanel (const QwtCurveChartPanel& view)


QwtCurveChartPanel& QwtCurveChartPanel::operator = (
											const QwtCurveChartPanel& view)
{
	assert (0 && "QwtCurveChartPanel::operator = is not allowed.");
	return *this;
}	// QwtCurveChartPanel::QwtCurveChartPanel (const QwtCurveChartPanel& view)


QwtCurveChartPanel::~QwtCurveChartPanel ( )
{
	for (vector<QwtExtendedPlotCurve*>::iterator itCurves = _curves.begin ( );
	     _curves.end ( ) != itCurves; itCurves++)
		(*itCurves)->detach ( );
}	// QwtCurveChartPanel::~QwtCurveChartPanel


void QwtCurveChartPanel::dataModified ( )
{
	getCoordinatesPicker ( ).updateFromPlot ( );

	QwtChartPanel::dataModified ( );
}	// QwtCurveChartPanel::dataModified


QwtExtendedPlotCurve& QwtCurveChartPanel::addCurve (const QString& name)
{
	QwtExtendedPlotCurve*	curve	= instantiateCurve (name);
	CHECK_NULL_PTR_ERROR (curve)
	_curves.push_back (curve);
	addPlotItem (*curve);

	return *curve;
}	// QwtCurveChartPanel::addCurve


void QwtCurveChartPanel::removeCurve (QwtExtendedPlotCurve* curve)
{
	for (vector<QwtExtendedPlotCurve*>::iterator it  = _curves.begin ( );
	     _curves.end ( ) != it; it++)
	{
		if (curve == *it)
		{
			getCoordinatesPicker ( ).clearSelection ( );
			removePlotItem (*curve);
			_curves.erase (it);
			delete curve;
			break;
		}	// if (curve == *it)
	}	// for (vector<QwtExtendedPlotCurve*>::iterator it  = _curves.begin ( ); ...
}	// QwtCurveChartPanel::removeCurve


QwtExtendedPlotCurve& QwtCurveChartPanel::getCurve (size_t i)
{
	if (_curves.size ( ) > i)
		return *(_curves [i]);

	UTF8String	msg (charset);
	msg << "Impossibilité de fournir la " << (unsigned long)i
	    << "-ème courbe du graphique "
	    << getPlot ( ).title ( ).text ( ).toStdString ( )
	    << ". Ce graphique n'a que " << (unsigned long)_curves.size ( )
	    << " courbes.";
	throw Exception (msg);
}	// QwtCurveChartPanel::getCurve


const QwtExtendedPlotCurve& QwtCurveChartPanel::getCurve (size_t i) const
{
	if (_curves.size ( ) > i)
		return *(_curves [i]);

	UTF8String	msg (charset);
	msg << "Impossibilité de fournir la " << (unsigned long)i
	    << "-ème courbe du graphique "
	    << getPlot ( ).title ( ).text ( ).toStdString ( )
	    << ". Ce graphique n'a que " << (unsigned long)_curves.size ( )
	    << " courbes.";
	throw Exception (msg);
}	// QwtCurveChartPanel::getCurve


QwtExtendedPlotCurve& QwtCurveChartPanel::getCurve (const QString& name)
{
	for (vector<QwtExtendedPlotCurve*>::iterator it = _curves.begin ( );
	     _curves.end ( ) != it; it++)
		if ((*it)->title ( ).text ( ) == name)
			return **it;

	UTF8String	msg (charset);
	msg << "Impossibilité de fournir la courbe " << name.toStdString ( )
	    << " du graphique "
	    << getPlot ( ).title ( ).text ( ).toStdString ( )
	    << ". Ce graphique n'a que " << (unsigned long)_curves.size ( )
	    << " courbes.";
	throw Exception (msg);
}	// QwtCurveChartPanel::getCurve


const QwtExtendedPlotCurve& QwtCurveChartPanel::getCurve (
													const QString& name) const
{
	for (vector<QwtExtendedPlotCurve*>::const_iterator it = _curves.begin ( );
	     _curves.end ( ) != it; it++)
		if ((*it)->title ( ).text ( ) == name)
			return **it;

	UTF8String	msg (charset);
	msg << "Impossibilité de fournir la courbe " << name.toStdString ( )
	    << " du graphique "
	    << getPlot ( ).title ( ).text ( ).toStdString ( )
	    << ". Ce graphique n'a que " << (unsigned long)_curves.size ( )
	    << " courbes.";
	throw Exception (msg);
}	// QwtCurveChartPanel::getCurve


void QwtCurveChartPanel::adjustChart (bool adjustAbscissa, bool adjustOrdinates)
{
	if ((0 == _curves.size ( )) ||
	    ((false == adjustAbscissa) && (false == adjustOrdinates)))
		return;

	double	xMin	= MAXDOUBLE, xMax = -MAXDOUBLE;
	double	yMin	= MAXDOUBLE, yMax = -MAXDOUBLE;

	for (vector<QwtExtendedPlotCurve*>::const_iterator it = _curves.begin ( );
	     _curves.end ( ) != it; it++)
	{
		if (true == adjustAbscissa)
		{
			xMin = xMin <= (*it)->minXValue ( ) ? xMin : (*it)->minXValue ( );
			xMax = xMax >= (*it)->maxXValue ( ) ? xMax : (*it)->maxXValue ( );
		}	// if (true == adjustAbscissa)
		if (true == adjustOrdinates)
		{
			yMin = yMin <= (*it)->minYValue ( ) ? yMin : (*it)->minYValue ( );
			yMax = yMax >= (*it)->maxYValue ( ) ? yMax : (*it)->maxYValue ( );
		}	// if (true == adjustOrdinates)
	}	// for (vector<QwtExtendedPlotCurve*>::const_iterator it = ...

	// On cherche ici à éviter d'avoir un axe des ordonnées qui va par exemple
	// de 50 à 50. Ca arrive quand on a quelque chose de constant ou un max -
	// min très faible. Dans un tel cas on impose le min et le max.
	double	max		= xMax != 0. ? xMax : xMin;
	// relDiff : différence relative entre le min et le max. On évite une
	// division par zéro.
	double	relDiff	= fabs (max) <= 1e-12 ? 0. : (xMax - xMin) / fabs (max);
	if ((true == adjustAbscissa) && (relDiff < 1e-2))
	{
		if (fabs (xMax - xMin) <= 1e-12)
		{
			xMin	-= 1e-12;
			xMax	+= 1e-12;
		}	// if (fabs (xMax - xMin) <= 1e-12)
		else if (relDiff < 1e-2)
		{
			xMin	-= (xMax - xMin) / 2.;
			xMax	+= (xMax - xMin) / 2.;
		}	// else else if (relDiff < 1e-2)
	}	// if ((true == adjustAbscissa) && (relDiff < 1e-2))
	max		= yMax != 0. ? yMax : yMin;
	relDiff	= fabs (max) <= 1e-12 ? 0. : (yMax - yMin) / fabs (max);
	if ((true == adjustOrdinates) && (relDiff < 1e-2))
	{
		if (fabs (yMax - yMin) <= 1e-12)
		{
			yMin	-= 1e-12;
			yMax	+= 1e-12;
		}	// if (fabs (yMax - yMin) <= 1e-12)
		else if (relDiff < 1e-2)
		{
			yMin	-= (yMax - yMin) / 2.;
			yMax	+= (yMax - yMin) / 2.;
		}	// else if (relDiff < 1e-2)
	}	// if ((true == adjustOrdinates) && (relDiff < 1e-2))

	if (true == adjustAbscissa)
		setAbscissaScale (xMin, xMax, false, getAbscissaStep ( ));
	if (true == adjustOrdinates)
		setOrdinateScale (yMin, yMax, false, getOrdinateStep ( ));

	QwtChartPanel::adjustChart ( );
}	// QwtCurveChartPanel::adjustChart


void QwtCurveChartPanel::allowCurveEdition (bool allow)
{
	getCoordinatesPicker ( ).allowCurveEdition (allow);
}	// QwtCurveChartPanel::allowCurveEdition


bool QwtCurveChartPanel::allowCurveEdition ( ) const
{
	return getCoordinatesPicker ( ).allowCurveEdition ( );
}	// QwtCurveChartPanel::allowCurveEdition


void QwtCurveChartPanel::allowAbscissaTranslation (bool allow)
{
	getCoordinatesPicker ( ).allowAbscissaTranslation (allow);
}	// QwtCurveChartPanel::allowAbscissaTranslation


bool QwtCurveChartPanel::allowAbscissaTranslation ( ) const
{
	return getCoordinatesPicker ( ).allowAbscissaTranslation ( );
}	// QwtCurveChartPanel::allowAbscissaTranslation


QwtPlotCoordinatesPicker::SELECTION_MODE
								QwtCurveChartPanel::getSelectionMode ( ) const
{
	return getCoordinatesPicker ( ).getSelectionMode ( );
}	// QwtCurveChartPanel::getSelectionMode


void QwtCurveChartPanel::setSelectionMode (
								QwtPlotCoordinatesPicker::SELECTION_MODE mode)
{
	getCoordinatesPicker ( ).setSelectionMode (mode);
}	// QwtCurveChartPanel::setSelectionMode


void QwtCurveChartPanel::undoLastModifiedPoint ( )
{
	QwtPlotCurveCoordinatesPicker*	picker	=
		dynamic_cast<QwtPlotCurveCoordinatesPicker*>(&getCoordinatesPicker ( ));
	if (0 != picker)
		picker->undoLastModifiedPoint ( );
}	// QwtCurveChartPanel::undoLastModifiedPoint


vector<QwtChartPanel::DataPaintAttributes>
						QwtCurveChartPanel::getDataPaintAttributes ( ) const
{
	vector<QwtChartPanel::DataPaintAttributes>	attributes;

	const vector<QwtPlotItem*>	items	= getPlotItems ( );
	for (vector<QwtPlotItem*>::const_iterator it = items.begin ( );
		 items.end ( ) != it; it++)
	{
		QwtExtendedPlotCurve*	curveItem   =
									dynamic_cast<QwtExtendedPlotCurve*>(*it);
		if (0 != curveItem)
		{
			QtPaintAttributes	pen (curveItem->pen ( ));
			DataPaintAttributes	pa (curveItem, pen.getPaintAttributes ( ));
			attributes.push_back (pa);
		}   // if (0 != curveItem)
	}   // for (vector<QwtPlotItem*>::const_iterator it = items.begin ( ); ...

	return attributes;
}	// QwtCurveChartPanel::getDataPaintAttributes


void QwtCurveChartPanel::setDataPaintAttributes (
	size_t index,  const QwtChartPanel::DataPaintAttributes& paintAttributes)
{
	QwtExtendedPlotCurve&	curve	= getCurve (index);
	QPen					pen		= curve.pen ( );
	QtPaintAttributes		qpa (paintAttributes._pen);

	if ((true == paintAttributes._fgValid) &&
	    (true == qpa.getPen ( ).color ( ).isValid ( )))
		pen.setColor (qpa.getPen ( ).color ( ));
	if (-1 != qpa.getPen ( ).width ( ))
		pen.setWidth (qpa.getPen ( ).width ( ));
	if ((Qt::PenStyle)-1 != qpa.getPen ( ).style ( ))
		pen.setStyle (qpa.getPen ( ).style ( ));
	QtPaintAttributes	updated (pen);
	curve.setPaintAttributes (updated.getPaintAttributes ( ));
}	// QwtCurveChartPanel::setDataPaintAttributes


void QwtCurveChartPanel::pointModifiedCallback (int curve, int point)
{
	setModified (curve, true);
	emit pointModified (curve, point);
//	dataModified ( );

	QwtExtendedPlotCurve&	c	= getCurve (curve);
	const double			x	= c.sample (point).x ( );
	const double			y	= c.sample (point).y ( );
	UTF8String				msg (charset);
	msg << "Nouvelles coordonnées du " << (unsigned long)(point + 1)
	    << " point de la courbe " << c.title ( ).text ( ).toStdString ( )
	    << " : (" << x << ", " << y << ").";
	log (InformationLog (msg));
}	// QwtCurveChartPanel::pointModifiedCallback


QwtExtendedPlotCurve* QwtCurveChartPanel::instantiateCurve (const QString& name)
{
	QwtExtendedPlotCurve*	curve	= new QwtExtendedPlotCurve (name);
	CHECK_NULL_PTR_ERROR (curve)

	return curve;
}	// QwtCurveChartPanel::instantiateCurve


