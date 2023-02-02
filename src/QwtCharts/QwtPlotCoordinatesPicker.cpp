#include "QwtCharts/QwtPlotCoordinatesPicker.h"
#include "QwtCharts/QwtChartPanel.h"
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
//              LA CLASSE QwtPlotCoordinatesPicker::Selection
// --------------------------------------------------------------------------

QwtPlotCoordinatesPicker::Selection::Selection ( )
	: hasSelection (false), mode (QwtPlotCoordinatesPicker::CURVE),
	  x1 (0.), y1 (0.), x2 (0.), y2 (0.),
	  selectedCurveIndex ((size_t)-1), selectedPointIndexes ( )
{
}	// Selection::Selection


QwtPlotCoordinatesPicker::Selection::Selection (
								const QwtPlotCoordinatesPicker::Selection& s)
	: hasSelection (s.hasSelection), mode (s.mode),
	  x1 (s.x1), y1 (s.y1), x2 (s.x2), y2 (s.y2),
	  selectedCurveIndex (s.selectedCurveIndex),
	  selectedPointIndexes (s.selectedPointIndexes)
{
}	// Selection::Selection (const Selection&)


QwtPlotCoordinatesPicker::Selection&
			QwtPlotCoordinatesPicker::Selection::operator = (
								const QwtPlotCoordinatesPicker::Selection& s)
{
	if (&s != this)
	{
		hasSelection			= s.hasSelection;
		mode					= s.mode;
		x1						= s.x1;
		y1						= s.y1;
		x2						= s.x2;
		y2						= s.y2;
		selectedCurveIndex		= s.selectedCurveIndex;
		selectedPointIndexes	= s.selectedPointIndexes;
	}	// if (&s != this)

	return *this;
}	// Selection::operator =


void QwtPlotCoordinatesPicker::Selection::clear ( )
{
	hasSelection		= false;
	x1					= 0.;
	y1					= 0.;
	x2					= 0.;
	y2					= 0.;
	selectedCurveIndex	= (size_t)-1;
	selectedPointIndexes.clear ( );
}	// Selection::clear


// --------------------------------------------------------------------------
//                    LA CLASSE QwtPlotCoordinatesPicker
// --------------------------------------------------------------------------


QwtPlotCoordinatesPicker::QwtPlotCoordinatesPicker (
			int xAxis, int yAxis, int selectionFlags, RubberBand rubberBand,
			DisplayMode trackerMode, QwtPlot* plot)
	: QwtPlotPicker (xAxis, yAxis, rubberBand, trackerMode,
	                 0 == plot ? 0 : plot->canvas ( )),
	  _plot (plot), _chartPanel (0),
	  _allowCurveEdition (false), _allowAbscissaTranslation (true),
	  _selection ( ),
	  _cursorPos (0, 0), _initialCursorPos (0, 0),
	  _initialCursorShape (Qt::ArrowCursor), _controlPressed (false)
{
	if ((0 != _plot) && (0 != _plot->canvas ( )))
		_initialCursorShape	= _plot->canvas ( )->cursor ( ).shape ( );
	setSelectionMode (CURVE);
}	// QwtPlotCoordinatesPicker::QwtPlotCoordinatesPicker


QwtPlotCoordinatesPicker::QwtPlotCoordinatesPicker (
										const QwtPlotCoordinatesPicker& picker)
	: QwtPlotPicker (0, 0 , QwtPicker::NoRubberBand, QwtPicker::AlwaysOff,0),
	  _plot (0), _chartPanel (0),
	  _allowCurveEdition (false), _allowAbscissaTranslation (true),
	  _selection (picker._selection),
	  _cursorPos (0, 0), _initialCursorPos (0, 0),
	  _initialCursorShape (Qt::ArrowCursor), _controlPressed (false)
{
	assert (0 && "QwtPlotCoordinatesPicker copy constructor is not allowed.");
}	// QwtPlotCoordinatesPicker::QwtPlotCoordinatesPicker (const QwtPlotCoordinatesPicker& view)


QwtPlotCoordinatesPicker& QwtPlotCoordinatesPicker::operator = (
										const QwtPlotCoordinatesPicker& picker)
{
	assert (0 && "QwtPlotCoordinatesPicker::operator = is not allowed.");
	return *this;
}	// QwtPlotCoordinatesPicker::QwtPlotCoordinatesPicker (const QwtPlotCoordinatesPicker& view)


QwtPlotCoordinatesPicker::~QwtPlotCoordinatesPicker ( )
{
}	// QwtPlotCoordinatesPicker::~QwtPlotCoordinatesPicker


void QwtPlotCoordinatesPicker::allowCurveEdition (bool allow)
{	
	_allowCurveEdition	= allow;
}	// QwtPlotCoordinatesPicker::allowCurveEdition


void QwtPlotCoordinatesPicker::allowAbscissaTranslation (bool allow)
{	
	_allowAbscissaTranslation	= allow;
}	// QwtPlotCoordinatesPicker::allowAbscissaTranslation


QwtPlotCoordinatesPicker::Selection& QwtPlotCoordinatesPicker::getSelection ( )
{
	return _selection;
}	// QwtPlotCoordinatesPicker::getSelection


const QwtPlotCoordinatesPicker::Selection&
								QwtPlotCoordinatesPicker::getSelection ( ) const
{
	return _selection;
}	// QwtPlotCoordinatesPicker::getSelection


void QwtPlotCoordinatesPicker::setSelectionMode (SELECTION_MODE mode)
{
	if (getSelectionMode ( ) == mode)
		return;

	clearSelection ( );
	_selection.mode	= mode;
}	// QwtPlotCoordinatesPicker::setSelectionMode


QwtPlotCurve* QwtPlotCoordinatesPicker::getSelectedCurve ( )
{
	return 0;
}	// QwtPlotCoordinatesPicker::getSelectedCurve


const QwtPlotCurve* QwtPlotCoordinatesPicker::getSelectedCurve ( ) const
{
	return 0;
}	// QwtPlotCoordinatesPicker::getSelectedCurve


void QwtPlotCoordinatesPicker::clearSelection ( )
{
	_selection.clear ( );
	if (0 != _plot)
		_plot->replot ( );
}	// QwtPlotCoordinatesPicker::clearSelection


void QwtPlotCoordinatesPicker::updateFromPlot ( )
{
}	// QwtPlotCoordinatesPicker::updateFromPlot


void QwtPlotCoordinatesPicker::widgetMousePressEvent (QMouseEvent* event)
{
	assert (0 != event);
	QwtPlotPicker::widgetMousePressEvent (event);
	_cursorPos	= event->pos ( );
}	// QwtPlotCoordinatesPicker::widgetMousePressEvent


void QwtPlotCoordinatesPicker::widgetMouseMoveEvent (QMouseEvent* event)
{
	assert (0 != event);
	_cursorPos			= event->pos ( );
	QwtPlotPicker::widgetMouseMoveEvent (event);
}	// QwtPlotCoordinatesPicker::widgetMouseMoveEvent


void QwtPlotCoordinatesPicker::widgetWheelEvent (QWheelEvent* event)
{
	if (0 != getChartPanel ( ))
		getChartPanel ( )->zoom (*event);
}	// QwtPlotCoordinatesPicker::widgetWheelEvent


void QwtPlotCoordinatesPicker::widgetKeyPressEvent (QKeyEvent* event)
{
	QwtPlotPicker::widgetKeyPressEvent (event);

	if (Qt::Key_Control == event->key ( ))
		_controlPressed	= true;
}	// QwtPlotCoordinatesPicker::widgetKeyPressEvent


void QwtPlotCoordinatesPicker::widgetKeyReleaseEvent (QKeyEvent* event)
{
//	QwtPlotPicker::widgetKeyPressEvent (event);		// v 0.47.0
	QwtPlotPicker::widgetKeyReleaseEvent (event);	// v 0.47.0

	if (Qt::Key_Control == event->key ( ))
		_controlPressed	= false;
}	// QwtPlotCoordinatesPicker::widgetKeyReleaseEvent


void QwtPlotCoordinatesPicker::selectCursor (const QPoint& point)
{
	QwtPlot*	plot	= getPlot ( );
	if ((0 == plot) || (0 == plot->canvas ( )))
		return;

	if (true == inSelectionArea (point.x ( ), point.y ( )))
		plot->canvas ( )->setCursor (Qt::PointingHandCursor);
	else
		plot->canvas ( )->setCursor (_initialCursorShape);
}	// QwtPlotCoordinatesPicker::selectCursor


void QwtPlotCoordinatesPicker::getSelectionArea (
						double& x1, double& y1, double& x2, double& y2) const
{
	x1	= x2	= y1	= y2	= 0.;
}	// QwtPlotCoordinatesPicker::getSelectionArea


void QwtPlotCoordinatesPicker::getSelectionExtrema (
				double& xmin, double& ymin, double& xmax, double& ymax) const
{
	xmin	= xmax	= ymin	= ymax	= 0.;
}	// QwtPlotCoordinatesPicker::getSelectionExtrema


bool QwtPlotCoordinatesPicker::inSelectionArea (int x, int y) const
{
	return false;
}	// QwtPlotCoordinatesPicker::inSelectionArea



