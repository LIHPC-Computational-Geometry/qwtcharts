#include "QwtCharts/QwtExtendedPlotZoomer.h"

#include "QwtCharts/QwtChartPanel.h"

#include <QWheelEvent>

#include <assert.h>
#include <iostream>
using namespace std;


QwtExtendedPlotZoomer::QwtExtendedPlotZoomer (
									QwtPlotCanvas* canvas, bool doReplot)
	: QwtPlotZoomer (canvas, doReplot), _chartPanel (0)
{
	// Qwt 6 : initMousePattern (2) requis pour éviter que le zoomer réagisse à
	// un évènement de type MouseRelease sur le bouton 2, ce qui a pour effet
	// d'annuler un éventuel zoom.
	// Or ce type d'évènement survient lorsque l'on vient de faire du "panning"
	// (déplacement de la vue au bouton du milieu de la souris) : le panning
	// annule le zoom et perd de ce fait tout son intérêt.
	initMousePattern (2);
}	// QwtExtendedPlotZoomer::QwtExtendedPlotZoomer


QwtExtendedPlotZoomer::QwtExtendedPlotZoomer (
				int xAxis, int yAxis, QwtPlotCanvas* canvas, bool doReplot)
	: QwtPlotZoomer (xAxis, yAxis, canvas, doReplot), _chartPanel (0)
{
	// Qwt 6 : initMousePattern (2) requis pour éviter que le zoomer réagisse à
	// un évènement de type MouseRelease sur le bouton 2, ce qui a pour effet
	// d'annuler un éventuel zoom.
	// Or ce type d'évènement survient lorsque l'on vient de faire du "panning"
	// (déplacement de la vue au bouton du milieu de la souris) : le panning
	// annule le zoom et perd de ce fait tout son intérêt.
	initMousePattern (2);
}	// QwtExtendedPlotZoomer::QwtExtendedPlotZoomer


QwtExtendedPlotZoomer::QwtExtendedPlotZoomer (const QwtExtendedPlotZoomer&)
	: QwtPlotZoomer (0), _chartPanel (0)
{
	assert (0 && "QwtExtendedPlotZoomer copy constructor is not allowed.");
}	// QwtExtendedPlotZoomer::QwtExtendedPlotZoomer


QwtExtendedPlotZoomer& QwtExtendedPlotZoomer::operator = (
												const QwtExtendedPlotZoomer& pr)
{
	assert (0 && "QwtExtendedPlotZoomer::operator = is not allowed.");

	return *this;
}	// QwtExtendedPlotZoomer::::operator =


QwtExtendedPlotZoomer::~QwtExtendedPlotZoomer ( )
{
}	// QwtExtendedPlotZoomer::~QwtExtendedPlotZoomer


void QwtExtendedPlotZoomer::widgetWheelEvent (QWheelEvent* event)
{
	assert (0 != event);
	if (0 != getChartPanel ( ))
		getChartPanel ( )->zoom (*event);
}   // QwtExtendedPlotZoomer::widgetWheelEvent


void QwtExtendedPlotZoomer::rescale ( )
{	// Code issu de la version 5.2.0 de QwtPlot::replot
	if (0 == getChartPanel ( ))
	{
		QwtPlotZoomer::rescale ( );
		return;
	}	// if (0 == getChartPanel ( ))
	QwtPlot*	managedPlot	= plot ( );
	if (0 == managedPlot)
		return;

	const QwtDoubleRect&	rect	= zoomRect ( );
	if (rect == scaleRect ( ))
		return;

	const bool	doReplot	= managedPlot->autoReplot ( );
	managedPlot->setAutoReplot (false);
	double	x1	= rect.left ( );
	double	x2	= rect.right ( );
	double	y1	= rect.top ( );
	double	y2	= rect.bottom ( );
	if (managedPlot->axisScaleDiv (xAxis ( )).lowerBound ( ) > managedPlot->axisScaleDiv (xAxis ( )).upperBound ( ))
		qSwap (x1, x2);
	if (managedPlot->axisScaleDiv (yAxis ( )).lowerBound ( ) > managedPlot->axisScaleDiv (yAxis ( )).upperBound ( ))
		qSwap (y1, y2);
	getChartPanel ( )->setAbscissaScale (x1, x2, false, getChartPanel ( )->getAbscissaStep ( ));
	getChartPanel ( )->setOrdinateScale (y1, y2, false, getChartPanel ( )->getOrdinateStep ( ));
	managedPlot->setAutoReplot (doReplot);
	managedPlot->replot ( );
}	// void QwtExtendedPlotZoomer::replot ( )


