#include "QwtCharts/QwtHistogramCoordinatesPicker.h"

#include "QwtCharts/QwtHistogramChartPanel.h"
#include <TkUtil/AutoArrayPtr.h>
#include <TkUtil/Exception.h>
#include <TkUtil/MemoryError.h>
#include <QMouseEvent>
#include <qwt_symbol.h>
#include <qwt_picker_machine.h>

#include <values.h>	// MAXDOUBLE
#include <assert.h>


USING_UTIL
using namespace std;


// --------------------------------------------------------------------------
//                    LA CLASSE QwtHistogramCoordinatesPicker
// --------------------------------------------------------------------------


QwtHistogramCoordinatesPicker::QwtHistogramCoordinatesPicker (
			int xAxis, int yAxis, int selectionFlags, RubberBand rubberBand, DisplayMode trackerMode, QwtPlot* plot)
	: QwtPlotCoordinatesPicker (xAxis, yAxis, selectionFlags, rubberBand, trackerMode, plot)
{
}	// QwtHistogramCoordinatesPicker::QwtHistogramCoordinatesPicker


QwtHistogramCoordinatesPicker::QwtHistogramCoordinatesPicker (const QwtHistogramCoordinatesPicker& picker)
	: QwtPlotCoordinatesPicker (0, 0 , 0, QwtPicker::NoRubberBand, QwtPicker::AlwaysOff,0)
{
	assert (0 && "QwtHistogramCoordinatesPicker copy constructor is not allowed.");
}	// QwtHistogramCoordinatesPicker::QwtHistogramCoordinatesPicker (const QwtHistogramCoordinatesPicker& view)


QwtHistogramCoordinatesPicker& QwtHistogramCoordinatesPicker::operator = (const QwtHistogramCoordinatesPicker& picker)
{
	assert (0 && "QwtHistogramCoordinatesPicker::operator = is not allowed.");
	return *this;
}	// QwtHistogramCoordinatesPicker::QwtHistogramCoordinatesPicker (const QwtHistogramCoordinatesPicker& view)


QwtHistogramCoordinatesPicker::~QwtHistogramCoordinatesPicker ( )
{
}	// QwtHistogramCoordinatesPicker::~QwtHistogramCoordinatesPicker


QwtHistogramChartPanel* QwtHistogramCoordinatesPicker::getHistogramPanel ( )
{
	return dynamic_cast<QwtHistogramChartPanel*>(getChartPanel ( ));
}	// QwtHistogramCoordinatesPicker::getHistogramPanel


const QwtHistogramChartPanel* QwtHistogramCoordinatesPicker::getHistogramPanel ( ) const
{
	return dynamic_cast<const QwtHistogramChartPanel*>(getChartPanel ( ));
}	// QwtHistogramCoordinatesPicker::getHistogramPanel


QwtExtendedMultiBarChart* QwtHistogramCoordinatesPicker::getHistogram ( )
{
	QwtHistogramChartPanel*	panel	= getHistogramPanel ( );

	return 0 == panel ? 0 : &(panel->getHistogram ( ));
}	// QwtHistogramCoordinatesPicker::getHistogram


const QwtExtendedMultiBarChart* QwtHistogramCoordinatesPicker::getHistogram ( ) const
{
	const QwtHistogramChartPanel*	panel	= getHistogramPanel ( );

	return 0 == panel ? 0 : &(panel->getHistogram ( ));
}	// QwtHistogramCoordinatesPicker::getHistogram


void QwtHistogramCoordinatesPicker::setSelectionMode (SELECTION_MODE mode)
{
	const Selection		oldSelection	= getSelection ( );
//	QwtPlotCurve*		curve			= getSelectedCurve ( );

	clearSelection ( );
	getSelection ( ).mode	= mode;	// for rollback if necessary
	QwtPlotPicker::RubberBand	rubberBand	= QwtPlotPicker::NoRubberBand;
	Qt::KeyboardModifier		selectModifier	= Qt::NoModifier;
	Qt::MouseButton				selectButton	= Qt::LeftButton;
//	Qt::KeyboardModifier		completeModifier= Qt::NoModifier;
	Qt::KeyboardModifier		completeModifier= Qt::ControlModifier;
	Qt::MouseButton				completeButton	= Qt::LeftButton;
	QwtPickerMachine*	machine	= 0;
	switch (mode)
	{
		case CURVE			:
		case POINT			:
		case LINEAR_INTERPOLATION	:
			machine		= new QwtPickerClickPointMachine ( );
			rubberBand	= QwtPicker::RectRubberBand;
			break;
		case RUBBER_BAND	:
			machine		= new QwtPickerClickRectMachine ( );
			rubberBand	= QwtPicker::RectRubberBand;
			break;
	}	// switch (mode)
	CHECK_NULL_PTR_ERROR (machine)
	setStateMachine (machine);
	setRubberBand (rubberBand);
	setMousePattern (QwtEventPattern::MouseSelect1, Qt::LeftButton);
	setMousePattern (QwtEventPattern::MouseSelect2, Qt::LeftButton , Qt::ControlModifier);
}	// QwtHistogramCoordinatesPicker::setSelectionMode


void QwtHistogramCoordinatesPicker::clearSelection ( )
{
	 QwtPlotCoordinatesPicker::clearSelection ( );
}	// QwtHistogramCoordinatesPicker::clearSelection


void QwtHistogramCoordinatesPicker::updateFromPlot ( )
{
/*
	updateSelection ( );

	bool	needReplot	= false;
	if (0 != _selectionRepresentation)
	{
		// On recréé la représentation pour mise à jour des données qui ont
		// peut être été modifiées par ailleurs :
		delete _selectionRepresentation;
		_selectionRepresentation	= 0;
		createSelectionRepresentation ( );	// provoque replot si création
		// Mise à jour de l'aire contenant la sélection :
		if (0 != _selectionRepresentation)
		{
			QwtDoubleRect	rect	= _selectionRepresentation->boundingRect( );
			if (true == rect.isValid ( ))
			{
				QRect	area	= transform (rect);
				_viewportSelectionArea	= area;
				_viewportSelectionArea.adjust (-100, 100, 100, -100);
			}	// if (true == rect.isValid ( ))
		}	// if (0 != _selectionRepresentation)

		if ((0 != _plot) && (true == needReplot))
			_plot->replot ( );
	}	// if (0 != _selectionRepresentation)
*/
}	// QwtHistogramCoordinatesPicker::updateFromPlot


void QwtHistogramCoordinatesPicker::widgetMouseReleaseEvent (QMouseEvent* event)
{
	assert (0 != event);
	if ((true == mouseMatch (MouseSelect1, event)) ||
	    (true == mouseMatch (MouseSelect2, event)))
	{
		const bool	extend	= mouseMatch (MouseSelect2, event);
		QPoint	pos		= getCursorPos ( );		// Coordonnées en pixels
		QPointF	couple	= invTransform (pos);	// Point sur le graphique.

		QwtExtendedMultiBarChart*			barChart= getHistogram ( );
		if (0 == barChart)
			return;

		const QwtSeriesData<QwtSetSample>*	data	= barChart->data ( );
		const vector<pair<size_t, size_t> >	classes	=
							barChart->intersectedClasses (pos.x ( ), pos.y ( ));
		if (false == extend)
			barChart->clearSelection ( );
		if (0 != classes.size ( ))
		{
			if (false == barChart->isSelected (classes[0].first, classes[0].second))
				barChart->addToSelection (classes);
			else
				barChart->removeFromSelection (classes);
		}	// if (0 != classes.size ( ))
		if (0 != getPlot ( ))
			getPlot ( )->replot ( );
	}

/*
	if (false == allowCurveEdition ( ))
	{
		QwtPlotPicker::widgetMouseReleaseEvent (event);	
		return;
	}	// if (false == allowCurveEdition ( ))

	if ((size_t)-1 != _selection.selectedCurveIndex)	// sélection en cours
	{
		event->accept ( );
		return;
	}
*/

	QwtPlotCoordinatesPicker::widgetMouseReleaseEvent (event);
}	// QwtHistogramCoordinatesPicker::widgetMouseReleaseEvent


void QwtHistogramCoordinatesPicker::begin ( )
{
	QwtPlotCoordinatesPicker::begin ( );

	getInitialCursorPos ( )	= getCursorPos ( );
	clearSelection ( );

	QwtPlotPicker::begin ( );
}	// QwtHistogramCoordinatesPicker::begin


bool QwtHistogramCoordinatesPicker::end (bool ok)
{
	if (false == ok)
	{
		clearSelection ( );
		return false;
	}	// if (false == ok)

	switch (getSelectionMode ( ))
	{
		case CURVE					:
		case POINT					:
		case LINEAR_INTERPOLATION	: 
		case RUBBER_BAND			: return QwtPlotPicker::end (ok);
	}	// switch (getSelectionMode ( ))

	assert (0 && "QwtHistogramCoordinatesPicker::end : cas non géré.");
}	// QwtHistogramCoordinatesPicker::end


QwtText QwtHistogramCoordinatesPicker::trackerTextF (const QwtDoublePoint& p) const
{
	QPoint	pos	= getCursorPos ( );
	const QwtExtendedMultiBarChart*	histogram	= getHistogram ( );
	if (0 != histogram)
	{
		QString	text;
		double	c	= 0.;
		size_t	num	= 0;
		if (true == histogram->intersects (pos.x ( ), pos.y ( ), c, num))
			text.sprintf ("%.4f, %lu", c, num);	// v 4.2.1 : %u -> %lu

		return text;
	}	// if (0 != histogram)

	return QwtPlotCoordinatesPicker::trackerTextF (pos);
}	// QwtHistogramCoordinatesPicker::trackerTextF


void QwtHistogramCoordinatesPicker::getSelectionArea (double& x1, double& y1, double& x2, double& y2) const
{
	x1	= x2	= y1	= y2	= 0.;
}	// QwtHistogramCoordinatesPicker::getSelectionArea


void QwtHistogramCoordinatesPicker::getSelectionExtrema (double& xmin, double& ymin, double& xmax, double& ymax) const
{
	xmin	= xmax	= ymin	= ymax	= 0.;
}	// QwtHistogramCoordinatesPicker::getSelectionExtrema


bool QwtHistogramCoordinatesPicker::inSelectionArea (int x, int y) const
{
	return false;
}	// QwtHistogramCoordinatesPicker::inSelectionArea

