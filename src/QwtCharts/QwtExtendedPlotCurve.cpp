#include "QwtCharts/QwtExtendedPlotCurve.h"
#include "QtUtil/QtPaintAttributes.h"

#include <QBrush>

#include <iostream>
#include <assert.h>


USING_STD

USING_UTIL



QwtExtendedPlotCurve::QwtExtendedPlotCurve (const QString& name)
	: QwtPlotCurve (name)
{
}	// QwtExtendedPlotCurve::QwtExtendedPlotCurve


QwtExtendedPlotCurve::QwtExtendedPlotCurve (
						const QString& name, const IN_UTIL PaintAttributes& pa)
	: QwtPlotCurve (name)
{
	setPaintAttributes (pa);
}	// QwtExtendedPlotCurve::QwtExtendedPlotCurve


QwtExtendedPlotCurve::QwtExtendedPlotCurve (const QwtExtendedPlotCurve&)
	: QwtPlotCurve ("")
{
	assert (0 && "QwtExtendedPlotCurve copy constructor is not allowed.");
}	// QwtExtendedPlotCurve::QwtExtendedPlotCurve


QwtExtendedPlotCurve& QwtExtendedPlotCurve::operator = (
													const QwtExtendedPlotCurve&)
{
	assert (0 && "QwtExtendedPlotCurve operator = is not allowed.");
	return *this;
}	// QwtExtendedPlotCurve::QwtExtendedPlotCurve


QwtExtendedPlotCurve::~QwtExtendedPlotCurve ( )
{
}	// QwtExtendedPlotCurve::~QwtExtendedPlotCurve


IN_UTIL PaintAttributes QwtExtendedPlotCurve::getPaintAttributes ( ) const
{
	return QtPaintAttributes (pen ( )).getPaintAttributes ( );
}	// QwtExtendedPlotCurve::getPaintAttributes


void QwtExtendedPlotCurve::setPaintAttributes (
											const IN_UTIL PaintAttributes& pa)
{	
	if (getPaintAttributes ( ) != pa)
	{
		QtPaintAttributes	qpa (pa);
		setPen (qpa.getPen ( ));
	}	// if (getPaintAttributes ( ) != pa)
}	// QwtExtendedPlotCurve::setPaintAttributes


void QwtExtendedPlotCurve::setPenAttributes (const QPen& p)
{
	if (pen ( ) != p)
		setPen (p);
}	// QwtExtendedPlotCurve::setPenAttributes




