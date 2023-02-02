#include "QwtCharts/QwtPlotCurveCoordinatesPicker.h"
#include "QwtCharts/QwtUtilities.h"
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

static const Charset	charset ("àéèùô");


double	QwtPlotCurveCoordinatesPicker::_selectionDistance	= 10;


// --------------------------------------------------------------------------
//                    LA CLASSE QwtPlotCurveCoordinatesPicker
// --------------------------------------------------------------------------


QwtPlotCurveCoordinatesPicker::QwtPlotCurveCoordinatesPicker (
			int xAxis, int yAxis, int selectionFlags, RubberBand rubberBand,
			DisplayMode trackerMode, QwtPlot* plot)
	: QwtPlotCoordinatesPicker (
				xAxis, yAxis, selectionFlags, rubberBand, trackerMode, plot),
	  _displayMode (QwtChartPanel::CARTESIAN),
	  _userPointIndexes ( ), _selectionRepresentation (0),
	  _originalCurve (0), _originalData (0)
{
	setSelectionMode (CURVE);
}	// QwtPlotCurveCoordinatesPicker::QwtPlotCurveCoordinatesPicker


QwtPlotCurveCoordinatesPicker::QwtPlotCurveCoordinatesPicker (
									const QwtPlotCurveCoordinatesPicker& picker)
	: QwtPlotCoordinatesPicker (0, 0 , 0, QwtPicker::NoRubberBand, QwtPicker::AlwaysOff,0),
	  _displayMode (QwtChartPanel::CARTESIAN),
	  _userPointIndexes ( ), _selectionRepresentation (0),
	  _originalCurve (0), _originalData (0)
{
	assert (0 && "QwtPlotCurveCoordinatesPicker copy constructor is not allowed.");
}	// QwtPlotCurveCoordinatesPicker::QwtPlotCurveCoordinatesPicker (const QwtPlotCurveCoordinatesPicker& view)


QwtPlotCurveCoordinatesPicker& QwtPlotCurveCoordinatesPicker::operator = (
									const QwtPlotCurveCoordinatesPicker& picker)
{
	assert (0 && "QwtPlotCurveCoordinatesPicker::operator = is not allowed.");
	return *this;
}	// QwtPlotCurveCoordinatesPicker::QwtPlotCurveCoordinatesPicker (const QwtPlotCurveCoordinatesPicker& view)


QwtPlotCurveCoordinatesPicker::~QwtPlotCurveCoordinatesPicker ( )
{
	delete _originalCurve;					_originalCurve				= 0;
	delete _selectionRepresentation;		_selectionRepresentation	= 0;
	delete _originalData;					_originalData				= 0;
}	// QwtPlotCurveCoordinatesPicker::~QwtPlotCurveCoordinatesPicker


void QwtPlotCurveCoordinatesPicker::setDisplayMode (
											QwtChartPanel::DISPLAY_MODE mode)
{
	_displayMode	= mode;
}	// QwtPlotCurveCoordinatesPicker::setDisplayMode


void QwtPlotCurveCoordinatesPicker::setSelectionMode (SELECTION_MODE mode)
{
	const Selection		oldSelection	= getSelection ( );
	QwtPlotCurve*		curve			= getSelectedCurve ( );

	if (LINEAR_INTERPOLATION != mode)
		clearSelection ( );
	else
	{	// v 0.47.0 : on s'assure d'avoir une sélection valide.
		if (false == getSelection ( ).hasSelection)
			throw Exception (UTF8String ("Mode interpolation linéaire : une courbe doit être préalablement sélectionnée.", charset));
		CHECK_NULL_PTR_ERROR (curve)

		clearSelection ( );
		getSelection ( )	= oldSelection;
		_originalData				= new QwtPointSeriesData ( );
		const QwtSeriesData<QPointF>*	array	= curve->data ( );
		if (0 != array)
			QwtUtilities::copySeriesData (*array, *_originalData);
		else
		{	// Ca bogue
			delete _originalData;
			_originalData	= 0;
		}
		_userPointIndexes.clear ( );
		getSelection ( ).selectedPointIndexes.clear ( );
		createSelectionRepresentation ( );
	}
	getSelection ( ).mode	= mode;
	int							flags		= 0;
	QwtPlotPicker::RubberBand	rubberBand	= QwtPlotPicker::NoRubberBand;
	Qt::KeyboardModifier		selectModifier	= Qt::NoModifier;
	Qt::MouseButton				selectButton	= Qt::LeftButton;
	Qt::KeyboardModifier		completeModifier= Qt::NoModifier;
	Qt::MouseButton				completeButton	= Qt::LeftButton;
	QwtPickerMachine*	machine	= 0;
	switch (mode)
	{
		case CURVE			:
		case POINT			:
			machine		= new QwtPickerClickPointMachine ( );
			rubberBand	= QwtPicker::RectRubberBand;
			break;
		case LINEAR_INTERPOLATION	:
			machine		= new QwtPickerClickPointMachine ( );
			rubberBand	= QwtPicker::RectRubberBand;
			selectModifier	= Qt::ControlModifier;
			break;
		case RUBBER_BAND	:
			machine		= new QwtPickerClickRectMachine ( );
			rubberBand	= QwtPicker::RectRubberBand;
			break;
	}	// switch (mode)
	CHECK_NULL_PTR_ERROR (machine)
	setStateMachine (machine);
	setRubberBand (rubberBand);
}	// QwtPlotCurveCoordinatesPicker::setSelectionMode


void QwtPlotCurveCoordinatesPicker::clearSelection ( )
{
	_userPointIndexes.clear ( );
	getViewportSelectionArea ( ).setCoords (-1000, -1000, -1000, -1000);
	delete _originalData;		_originalData	= 0;
	delete _originalCurve;		_originalCurve	= 0;
	if (0 != _selectionRepresentation)
	{
		delete _selectionRepresentation;
		_selectionRepresentation	= 0;
	}	// if (0 != _selectionRepresentation)

	QwtPlotCoordinatesPicker::clearSelection ( );
}	// QwtPlotCurveCoordinatesPicker::clearSelection


void QwtPlotCurveCoordinatesPicker::undoLastModifiedPoint ( )
{
	if ((LINEAR_INTERPOLATION != getSelectionMode ( )) ||
	    (false == allowCurveEdition ( )))
		return;

	if (0 != getSelectedCurve ( ))
	{
		if (1 > _userPointIndexes.size ( ))
			return;

		_userPointIndexes.pop_back ( );
		const size_t	lastIndex	=
			0 == _userPointIndexes.size ( ) ? 0 : _userPointIndexes.back ( );
		if (0 == _userPointIndexes.size ( ))
			getSelection ( ).selectedPointIndexes.clear ( );
		else
		{
			vector<size_t>::iterator	it	=
							getSelection ( ).selectedPointIndexes.begin ( );
			for ( ; (getSelection ( ).selectedPointIndexes.end ( ) != it) &&
			         (*it <= lastIndex); it++);
			if (getSelection ( ).selectedPointIndexes.end ( ) != it)
				getSelection ( ).selectedPointIndexes.erase (
							it, getSelection ( ).selectedPointIndexes.end ( ));
		}

		QwtArray<double>	xData (_originalData->size ( ));
		QwtArray<double>	yData (_originalData->size ( ));
		// Recopie des points modifiés :
		size_t	i	= 0;
		if (0 != _userPointIndexes.size ( ))
		{
			for (i = 0; i <= lastIndex; i++)
			{
				xData [i]	= getSelectedCurve ( )->sample (i).x ( );
				yData [i]	= getSelectedCurve ( )->sample (i).y ( );
			}	// for (i = 0; i <= lastIndex; i++)
		}	// if (0 != _userPointIndexes.size ( ))
		// Recopie des points originaux :
		// i commence à 0 ou lastIndex + 1 selon le cas :
		for ( ; i < _originalData->size( ); i++)
		{	
			xData [i]	= _originalData->sample (i).x ( );
			yData [i]	= _originalData->sample (i).y ( );
		}	// for (i = lastIndex + 1; i < ...
		getSelectedCurve ( )->setSamples (xData, yData);
		delete _selectionRepresentation;	_selectionRepresentation	= 0;
		createSelectionRepresentation ( );
	}	// if (0 != getSelectedCurve ( ))
}	// QwtPlotCurveCoordinatesPicker::undoLastModifiedPoint


void QwtPlotCurveCoordinatesPicker::updateFromPlot ( )
{
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
				getViewportSelectionArea ( )	= area;
				getViewportSelectionArea ( ).adjust (-100, 100, 100, -100);
			}	// if (true == rect.isValid ( ))
		}	// if (0 != _selectionRepresentation)

// coverity : needReplot vaut forcément false
//		if ((0 != getPlot ( )) && (true == needReplot))
//			getPlot ( )->replot ( );
	}	// if (0 != _selectionRepresentation)
}	// QwtPlotCurveCoordinatesPicker::updateFromPlot


size_t QwtPlotCurveCoordinatesPicker::pointAtX (
									const QwtPlotCurve& curve, double x) const
{
	CHECK_NULL_PTR_ERROR (curve.data ( ))
	const QwtSeriesData<QPointF>&	data	= *curve.data ( );
	size_t			best	= (size_t)-1;
	double			dist	= MAXDOUBLE;

	for (size_t i = 0; i < data.size ( ); i++)
	{
		double	d	= data.sample (i).x ( ) - x;
		d			*= d;
		if (d < dist)
		{
			best	= i;
			dist	= d;
		}	// if (d < dist)
	}	// for (size_t i = 0; i < data.size ( ); i++)

	return best;
}	// QwtPlotCurveCoordinatesPicker::pointAtX


QwtPlotCurve* QwtPlotCurveCoordinatesPicker::getSelectionRepresentation ( )
{
	return _selectionRepresentation;
}	// QwtPlotCurveCoordinatesPicker::getSelectionRepresentation


const QwtPlotCurve*	
			QwtPlotCurveCoordinatesPicker::getSelectionRepresentation ( ) const
{
	return _selectionRepresentation;
}	// QwtPlotCurveCoordinatesPicker::getSelectionRepresentation


void QwtPlotCurveCoordinatesPicker::widgetMousePressEvent (QMouseEvent* event)
{
	assert (0 != event);
	QwtPlotPicker::widgetMousePressEvent (event);

	if ((LINEAR_INTERPOLATION != getSelectionMode ( )) ||
	    (false == allowCurveEdition ( )))
		return;

	QPoint	previousPos	= getCursorPos ( );
	getCursorPos ( )	= event->pos ( );

	if ((true == controlKeyPressed ( )) && (0 != getSelectedCurve ( )))
	{
		if (0 == _originalCurve)
		{
			UTF8String	name (charset);
			name << "original_"
			     << getSelectedCurve ( )->title ( ).text ( ).toStdString ( );
			_originalCurve	=
					QwtUtilities::cloneCurve (*getSelectedCurve ( ), name);
			_originalCurve->setPen (QPen (Qt::black));
			_originalCurve->attach (getPlot ( ));
		}	// if (0 == _originalCurve)
		QPoint	previousPos	= getCursorPos ( );
		getCursorPos ( )	= event->pos ( );
		QwtDoublePoint	p			= invTransform (getCursorPos ( ));
		const size_t	index		= pointAtX (*getSelectedCurve ( ), p.x ( ));
		const size_t	lastIndex	=
						0 == getSelection ( ).selectedPointIndexes.size ( ) ?
						(size_t)-1 : getSelection ( ).selectedPointIndexes.back ( );
		if ((0 != getSelection ( ).selectedPointIndexes.size ( )) &&
		    (index <= lastIndex))
			return;

		QwtArray<double>	xData (getSelectedCurve ( )->dataSize ( ));
		QwtArray<double>	yData (getSelectedCurve ( )->dataSize ( ));
		// Recopie :
		for (size_t i = 0; i < getSelectedCurve ( )->dataSize ( ); i++)
		{
			xData [i]	= getSelectedCurve ( )->sample (i).x ( );
			yData [i]	= getSelectedCurve ( )->sample (i).y ( );
		}	// for (i = 0; i < getSelectedCurve ( )->dataSize ( ); i++)

		if (((size_t)-1 != lastIndex) && (lastIndex + 1 < index))
		{	// Ajouter/déplacer l'intervalle à la sélection courrante :
			CHECK_NULL_PTR_ERROR (getSelectedCurve ( ))
			CHECK_NULL_PTR_ERROR (getSelectedCurve ( )->data ( ))
			QwtSeriesData<QPointF>&	data= *(getSelectedCurve ( )->data ( ));
			const double	dx	= p.x ( ) - data.sample (lastIndex).x ( );
			const double	a	= dx < 1E-12 ?
							MAXDOUBLE :
								(p.y ( ) - data.sample (lastIndex).y ( )) / dx;
			const double	b	= 0.5 *
				(p.y ( ) + data.sample (lastIndex).y ( ) -
								a * (p.x ( ) + data.sample (lastIndex).x ( )));
			for (size_t i = lastIndex + 1; i < index; i++)
			{
				yData [i]	= a * xData [i] + b;
				getSelection ( ).selectedPointIndexes.push_back (i);
			}	// for (size_t i = lastIndex + 1; i < index; i++)
		}	// if ((size_t)-1 != lastIndex)
		yData [index]	= p.y ( );
		_userPointIndexes.push_back (index);
		getSelection ( ).selectedPointIndexes.push_back (index);
		getSelectedCurve ( )->setSamples (xData, yData);
		delete _selectionRepresentation;	_selectionRepresentation	= 0;
		createSelectionRepresentation ( );
		for (vector<size_t>::const_iterator it =
									getSelection ( ).selectedPointIndexes.begin( );
		     getSelection ( ).selectedPointIndexes.end ( ) != it; it++)
			emit pointModified (getSelection ( ).selectedCurveIndex, *it);
	}	// if ((true == controlKeyPressed ( )) && (0 != getSelectedCurve ( ))
}	// QwtPlotCurveCoordinatesPicker::widgetMousePressEvent


void QwtPlotCurveCoordinatesPicker::widgetMouseReleaseEvent (QMouseEvent* event)
{
	assert (0 != event);

	if (false == allowCurveEdition ( ))
	{
		QwtPlotPicker::widgetMouseReleaseEvent (event);	
		return;
	}	// if (false == allowCurveEdition ( ))

	if ((size_t)-1 != getSelection ( ).selectedCurveIndex)	// sélection en cours
	{
		event->accept ( );
		return;
	}

	QwtPlotPicker::widgetMouseReleaseEvent (event);
}	// QwtPlotCurveCoordinatesPicker::widgetMouseReleaseEvent


void QwtPlotCurveCoordinatesPicker::widgetMouseMoveEvent (QMouseEvent* event)
{
	assert (0 != event);
	QPoint	previousPos	= getCursorPos ( );
	getCursorPos ( )	= event->pos ( );

	if (false == allowCurveEdition ( ))
	{
		QwtPlotPicker::widgetMouseMoveEvent (event);	
		return;
	}	// if (false == allowCurveEdition ( ))

	// On ne change le curseur que si l'évènement n'est pas déjà traité par
	// ailleurs :
	if (false == event->isAccepted ( ))	// v 0.11.0
		selectCursor (event->pos ( ));
	if ((size_t)-1 != getSelection ( ).selectedCurveIndex)	// sélection en cours
	{
		Qt::MouseButtons	buttons	= event->buttons ( );
		if ((0 != (Qt::LeftButton & buttons)) &&
		    (true == inSelectionArea (
							getCursorPos ( ).x ( ), getCursorPos ( ).y ( ))))
		{
			event->accept ( );	// v 0.11.0
			moveSelection (getCursorPos ( ).x ( ) - previousPos.x ( ),
			               getCursorPos ( ).y ( ) - previousPos.y ( ));
		}
		else
			selectCursor (event->pos ( ));	// v 0.11.0
	}	// if ((size_t)-1 != _selectedCurveIndex)

	QwtPlotPicker::widgetMouseMoveEvent (event);
}	// QwtPlotCurveCoordinatesPicker::widgetMouseMoveEvent


void QwtPlotCurveCoordinatesPicker::begin ( )
{
	getInitialCursorPos ( )	= getCursorPos ( );
	if (false == inSelectionArea (
							getCursorPos ( ).x ( ), getCursorPos ( ).y ( )))
		if (LINEAR_INTERPOLATION != getSelectionMode ( ))
			clearSelection ( );
		else
		{
			_userPointIndexes.clear ( );
			getSelection ( ).selectedPointIndexes.clear ( );
		}
	QwtPlotPicker::begin ( );
}	// QwtPlotCurveCoordinatesPicker::begin


bool QwtPlotCurveCoordinatesPicker::end (bool ok)
{
	if (false == allowCurveEdition ( ))
		return QwtPlotPicker::end (ok);

	if (false == ok)
	{
		clearSelection ( );
		return false;
	}	// if (false == ok)

	if (((size_t)-1 != getSelection ( ).selectedCurveIndex) &&
	    (LINEAR_INTERPOLATION != getSelectionMode ( )))
		return QwtPlotPicker::end (ok);

	switch (getSelectionMode ( ))
	{
		case CURVE					: return curveEnd (ok);
		case POINT					: return pointEnd (ok);
		case RUBBER_BAND			: return rectEnd (ok);
		case LINEAR_INTERPOLATION	: return linearInterpolationEnd (ok);
	}	// switch (getSelectionMode ( ))

	assert (0 && "QwtPlotCurveCoordinatesPicker::end : cas non géré.");
}	// QwtPlotCurveCoordinatesPicker::end


bool QwtPlotCurveCoordinatesPicker::curveEnd (bool ok)
{
	QwtPlot*	plot	= getPlot ( );
	if ((0 == plot) || (0 == plot->canvas ( )))
		return QwtPlotPicker::end (ok);
	
	QwtPlotCurve*			selectedCurve	= 0;
	const QwtPlotItemList&	itemList		= plot->itemList ( );
	size_t					curveIndex		= (size_t)-1;
	double					distance		= _selectionDistance;
	for (QwtPlotItemIterator it = itemList.begin ( ); it != itemList.end( );
	     it++)
	{
		if ((*it)->rtti ( ) == QwtPlotItem::Rtti_PlotCurve)
		{
			curveIndex++;
			QwtPlotCurve*	c	= dynamic_cast<QwtPlotCurve*>(*it);
			assert (0 != c);
			if (false == c->isVisible ( ))
				continue;

			double	d	= 0.;
			int		index	= c->closestPoint (getCursorPos ( ), &d);
			if (d < distance)
			{
				distance						= d;
				selectedCurve					= c;
				getSelection ( ).selectedCurveIndex	= curveIndex;
				getSelection ( ).hasSelection			= true;
			}	// if (d < distance)
		}	// if ((*it)->rtti ( ) == QwtPlotItem::Rtti_PlotCurve)
	}	// for (QwtPlotItemIterator it = itemList.begin ( ); ...

	_userPointIndexes.clear ( );
	getSelection ( ).selectedPointIndexes.clear ( );
	if (0 != selectedCurve)
	{
		if (0 == _originalData)
			_originalData			= new QwtPointSeriesData ( );
		const QwtSeriesData<QPointF>*	array	= selectedCurve->data ( );
		if (0 != array)
		{
			QwtUtilities::copySeriesData (*array, *_originalData);
		}
		else
		{	// Y'a un pb, mais on est dans un callback ...
			delete _originalData;
			_originalData	= 0;
		}
		const int	number	= selectedCurve->dataSize ( );
		for (int i = 0; i < number; i++)
			getSelection ( ).selectedPointIndexes.push_back (i);
		getSelection ( ).x1	= selectedCurve->minXValue ( );
		getSelection ( ).y1	= selectedCurve->minYValue ( );
		getSelection ( ).x2	= selectedCurve->maxXValue ( );
		getSelection ( ).y2	= selectedCurve->maxYValue ( );
		getViewportSelectionArea ( ).setCoords (
				selectedCurve->minXValue ( ) - (int)_selectionDistance,
				selectedCurve->minYValue ( ) - (int)_selectionDistance,
				selectedCurve->maxXValue ( ) + (int)_selectionDistance,
				selectedCurve->maxYValue ( ) + (int)_selectionDistance);
		plot->canvas ( )->setCursor (Qt::ClosedHandCursor);
		createSelectionRepresentation ( );
	}
	else
		clearSelection ( );

	return QwtPlotPicker::end (ok);
}	// QwtPlotCurveCoordinatesPicker::curveEnd


bool QwtPlotCurveCoordinatesPicker::pointEnd (bool ok)
{
	QwtPlot*	plot	= getPlot ( );
	if ((0 == plot) || (0 == plot->canvas ( )))
		return QwtPlotPicker::end (ok);
	
	QwtPlotCurve*			selectedCurve	= 0;
	const QwtPlotItemList&	itemList		= plot->itemList ( );
	size_t					curveIndex		= (size_t)-1;
	size_t					pointIndex		= (size_t)-1;
	double					distance		= _selectionDistance;
	for (QwtPlotItemIterator it = itemList.begin ( ); it != itemList.end( );
	     it++)
	{
		if ((*it)->rtti ( ) == QwtPlotItem::Rtti_PlotCurve)
		{
			curveIndex++;
			QwtPlotCurve*	c	= dynamic_cast<QwtPlotCurve*>(*it);
			assert (0 != c);
			if (false == c->isVisible ( ))
				continue;

			double	d	= 0.;
			int		index	= c->closestPoint (getCursorPos ( ), &d);
			if (d < distance)
			{
				distance						= d;
				selectedCurve					= c;
				pointIndex						= index;
				getSelection ( ).selectedCurveIndex	= curveIndex;
				getSelection ( ).hasSelection			= true;
			}	// if (d < distance)
		}	// if ((*it)->rtti ( ) == QwtPlotItem::Rtti_PlotCurve)
	}	// for (QwtPlotItemIterator it = itemList.begin ( ); ...

	getSelection ( ).selectedPointIndexes.clear ( );
	if (0 != selectedCurve)
	{
		getSelection ( ).selectedPointIndexes.push_back (pointIndex);
		QPoint	point	= transform (
				QwtDoublePoint (selectedCurve->sample (pointIndex).x ( ),
								selectedCurve->sample (pointIndex).y ( )));
		getViewportSelectionArea ( ).setCoords (
									point.x ( ) - (int)_selectionDistance,
									point.y ( ) - (int)_selectionDistance,
									point.x ( ) + (int)_selectionDistance,
									point.y ( ) + (int)_selectionDistance);
		getSelection ( ).x1	= getSelection ( ).x2	= selectedCurve->sample(pointIndex).x();
		getSelection ( ).y1	= getSelection ( ).y2	= selectedCurve->sample(pointIndex).y();
		plot->canvas ( )->setCursor (Qt::ClosedHandCursor);
		createSelectionRepresentation ( );
	}
	else
		clearSelection ( );

	return QwtPlotPicker::end (ok);
}	// QwtPlotCurveCoordinatesPicker::pointEnd


bool QwtPlotCurveCoordinatesPicker::rectEnd (bool ok)
{
	QwtPlot*	plot	= getPlot ( );
	if ((0 == plot) || (0 == plot->canvas ( )))
		return QwtPlotPicker::end (ok);
	
	getSelection ( ).hasSelection			= false;
	getSelection ( ).selectedCurveIndex	= -1;
	getSelection ( ).selectedPointIndexes.clear ( );

	QwtPlotCurve*			selectedCurve	= 0;
	const QwtPlotItemList&	itemList		= plot->itemList ( );
	size_t					curveIndex		= (size_t)-1;
	size_t					pointIndex		= (size_t)-1;
	int	x	= getCursorPos ( ).x ( ) < getInitialCursorPos ( ).x ( ) ?
			  getCursorPos ( ).x ( ) : getInitialCursorPos ( ).x ( );
	int	y	= getCursorPos ( ).y ( ) < getInitialCursorPos ( ).y ( ) ?
			  getCursorPos ( ).y ( ) : getInitialCursorPos ( ).y ( );
	int	dx	= getCursorPos ( ).x ( ) - getInitialCursorPos ( ).x ( );
	int	dy	= getCursorPos ( ).y ( ) - getInitialCursorPos ( ).y ( );
	dx	= dx < 0 ? -dx : dx;
	dy	= dy < 0 ? -dy : dy;
	QRect	rectangle (x, y, dx, dy);
	for (QwtPlotItemIterator it = itemList.begin ( ); it != itemList.end( );
	     it++)
	{
		if ((*it)->rtti ( ) == QwtPlotItem::Rtti_PlotCurve)
		{
			curveIndex++;
			QwtPlotCurve*	c	= dynamic_cast<QwtPlotCurve*>(*it);
			assert (0 != c);
			if (false == c->isVisible ( ))
				continue;

			const int	number	= c->dataSize ( );
			for (int i = 0; i < number; i++)
			{
				QPoint	point	= transform (
										QwtDoublePoint (c->sample (i).x ( ),
										                c->sample (i).y ( )));
				if (true == rectangle.contains (point.x ( ), point.y ( )))
				{
					getSelection ( ).selectedPointIndexes.push_back (i);
					getSelection ( ).selectedCurveIndex	= curveIndex;
					getSelection ( ).hasSelection			= true;
				}	// if (true == rectangle.contains (...
			}	// for (int i = 0; i < number; i++)

		}	// if ((*it)->rtti ( ) == QwtPlotItem::Rtti_PlotCurve)

		// On ne gère la sélection que dans une seule courbe :
		if ((size_t)-1 != getSelection ( ).selectedCurveIndex)
			break;
	}	// for (QwtPlotItemIterator it = itemList.begin ( ); ...

	if ((size_t)-1 != getSelection ( ).selectedCurveIndex)
	{
		plot->canvas ( )->setCursor (Qt::ClosedHandCursor);
		QwtDoublePoint	dPoint	=
						invTransform (QPoint (rectangle.x ( ), rectangle.y( )));
		getSelection ( ).x1	= dPoint.x ( );
		getSelection ( ).y1	= dPoint.y ( );
		dPoint	= invTransform (QPoint (rectangle.x ( ) + rectangle.width ( ),
				                        rectangle.y ( ) + rectangle.height( )));
		getSelection ( ).x2	= dPoint.x ( );
		getSelection ( ).y2	= dPoint.y ( );
		getViewportSelectionArea ( )	= rectangle;
		createSelectionRepresentation ( );
	}
	else
	{
		clearSelection ( );
	}

	return QwtPlotPicker::end (ok);
}	// QwtPlotCurveCoordinatesPicker::rectEnd


bool QwtPlotCurveCoordinatesPicker::linearInterpolationEnd (bool ok)
{
	delete _originalCurve;				_originalCurve				= 0;
	delete _selectionRepresentation;	_selectionRepresentation	= 0;
	QwtPlot*	plot	= getPlot ( );
	if ((0 == plot) || (0 == plot->canvas ( )))
		return QwtPlotPicker::end (ok);

	if ((size_t)-1 != getSelection ( ).selectedCurveIndex)
	{
		plot->canvas ( )->setCursor (Qt::ClosedHandCursor);
		createSelectionRepresentation ( );
	}
	else
	{
		clearSelection ( );
	}

	return QwtPlotPicker::end (ok);
}	// QwtPlotCurveCoordinatesPicker::linearInterpolationEnd


QwtText QwtPlotCurveCoordinatesPicker::trackerText (
												const QwtDoublePoint& pos) const
{
	QString		label;

	switch (getDisplayMode ( ))
	{
		case QwtChartPanel::CARTESIAN	:
			label.sprintf ("%.4g %.4g", pos.x ( ), pos.y ( ));
			break;
		case QwtChartPanel::POLAR		:
		{
			const double	radius	=
						sqrt (pos.x ( ) * pos.x ( ) + pos.y ( ) * pos.y ( ));
			double	angle	= 0. == pos.x ( ) ?
						0 : atan (pos.y ( ) / pos.x ( )) * 180. / M_PI;
			if (pos.x ( ) < 0.)
				angle	= 180. + angle;
			else
				if (pos.y ( ) < 0.)
					angle	= 360. + angle;
			label.sprintf ("%.4g %.4g", angle, radius);
			break;
		}	// case POLAR
	}	// switch (getDisplayMode ( ))

	return label;
}	// QwtPlotCurveCoordinatesPicker::trackerText


void QwtPlotCurveCoordinatesPicker::updateSelection ( )
{
	if (false == getSelection ( ).hasSelection)
		return;

	try
	{
		size_t				i				= 0;
		Selection			oldSelection	= getSelection ( );
		const QwtPlotCurve*	curve			= getSelectedCurve ( );
		CHECK_NULL_PTR_ERROR (curve)
		switch (oldSelection.mode)
		{
			case CURVE					:
				// On refait la sélection complète (y a-t-il eu
				// addition/suppression de points ?
				getSelection ( ).selectedPointIndexes.clear ( );
				for (i = 0; i < curve->dataSize ( ); i++)
					getSelection ( ).selectedPointIndexes.push_back (i);
				break;
			case POINT					:
				if ((1 != getSelection ( ).selectedPointIndexes.size ( )) ||
				    ((0 != getSelection ( ).selectedPointIndexes.size ( )) &&
				     (getSelection ( ).selectedPointIndexes[0] >= curve->dataSize())))
					clearSelection ( );

				// On conserve la sélection : on ne sait pas s'il y a eu
				// addition/suppression de points, et/ou déplacement de points.
			break;
			case RUBBER_BAND			:
			{
				// On refait la sélection complète (y a-t-il eu
				// addition/suppression et/ou déplacement de points ?
				getSelection ( ).selectedPointIndexes.clear ( );
				QRectF	selectionArea (getSelection ( ).x1, getSelection ( ).y1,
						               getSelection ( ).x2 - getSelection ( ).x1,
						               getSelection ( ).y2 - getSelection ( ).y1);;
				for (i = 0; i < curve->dataSize ( ); i++)
				{
					if (true == selectionArea.contains (
							curve->sample (i).x ( ), curve->sample (i).y ( )))
						getSelection ( ).selectedPointIndexes.push_back (i);
				}
			}
			break;
			case LINEAR_INTERPOLATION	:
				// Si sélection il y a, elle a toute les chances d'être devenue
				// fausse.
				clearSelection ( );
			break;
		}	// switch (oldSelection.mode)
	}
	catch (const Exception& exc)
	{
		cout << "Erreur interne dans QwtPlotCurveCoordinatesPicker::updateSelection "
		     << "(" << __FILE__ << ' ' << __LINE__ << ") : "
		     << exc.getFullMessage ( ) << endl;
	}
	catch (...)
	{
		cout << "Erreur interne dans QwtPlotCurveCoordinatesPicker::updateSelection "
		     << "(" << __FILE__ << ' ' << __LINE__ << ") : "
		     << "Erreur non documentée." << endl;
	}
}	// QwtPlotCurveCoordinatesPicker::updateSelection


bool QwtPlotCurveCoordinatesPicker::inSelectionArea (int x, int y) const
{
	if (false == allowCurveEdition ( ))
		return false;

	return getViewportSelectionArea ( ).contains (x, y);
}	// QwtPlotCurveCoordinatesPicker::inSelectionArea


void QwtPlotCurveCoordinatesPicker::moveSelection (int dx, int dy)
{
	if (((0 == dx) && (0 == dy)) ||
	    (0 == getSelection ( ).selectedPointIndexes.size ( )))
		return;
	assert (0 != _selectionRepresentation);

	QwtPlot*		plot	= getPlot ( );
	QwtPlotCurve*	curve	= getSelectedCurve ( );
	if ((0 == curve) || (0 == plot))
		return;

	QwtArray<double>	xData (curve->dataSize ( ));
	QwtArray<double>	yData (curve->dataSize ( ));
	QwtArray<double>	xSelData (_selectionRepresentation->dataSize ( ));
	QwtArray<double>	ySelData (_selectionRepresentation->dataSize ( ));
	size_t	i	= 0, sel	= 0;
	// Recopie :
	for (i = 0; i < curve->dataSize ( ); i++)
	{
		xData [i]	= curve->sample (i).x ( );
		yData [i]	= curve->sample (i).y ( );
	}	// for (i = 0; i < curve->dataSize ( ); i++)

	// Déplacement des points sélectionnés :
	int	xMin	= INT_MAX, xMax	= INT_MIN;
	int	yMin	= INT_MAX, yMax	= INT_MIN;
	for (vector<size_t>::const_iterator it =
							getSelection ( ).selectedPointIndexes.begin ( );
	     getSelection ( ).selectedPointIndexes.end ( ) != it; it++, sel++)
	{
		QPoint	point	= transform (
							QwtDoublePoint (curve->sample (*it).x ( ),
							                curve->sample (*it).y ( )));
		point.setX (point.x ( ) + dx);
		point.setY (point.y ( ) + dy);
		QwtDoublePoint	dPoint	= invTransform (point);
		if (true == allowAbscissaTranslation ( ))
			xData [*it]	= xSelData [sel]	= dPoint.x ( );
		else
		{
			xData [*it]	= xSelData [sel]	= curve->sample (*it).x ( );
		}
		yData [*it]	= ySelData [sel]	= dPoint.y ( );
		xMin	= xMin < point.x ( ) + dx ? xMin : point.x ( ) + dx;
		xMax	= xMax > point.x ( ) + dx ? xMax : point.x ( ) + dx;
		yMin	= yMin < point.y ( ) + dy ? yMin : point.y ( ) + dy;
		yMax	= yMax > point.y ( ) + dy ? yMax : point.y ( ) + dy;
	}	// for (vector<size_t>::const_iterator it = ...

	curve->setSamples (xData, yData);
	_selectionRepresentation->setSamples (xSelData, ySelData);
	plot->replot ( );
	// +- 100 : on s'accorde une marge pour les déplacements rapides de souris.
	getViewportSelectionArea ( ).setCoords (
							xMin - 100, yMin - 100, xMax + 100, yMax + 100);
	for (vector<size_t>::const_iterator it =
							getSelection ( ).selectedPointIndexes.begin ( );
		   getSelection ( ).selectedPointIndexes.end ( ) != it; it++, sel++)
	{
		emit pointModified (getSelection ( ).selectedCurveIndex, *it);
	}	// for (vector<size_t>::const_iterator it = ...
}	// QwtPlotCurveCoordinatesPicker::moveSelection 


QwtPlotCurve* QwtPlotCurveCoordinatesPicker::getSelectedCurve ( )
{
	if (((size_t)-1 == getSelection ( ).selectedCurveIndex) ||
	    (0 == getPlot ( )))
		return 0;

	const QwtPlotItemList&	itemList	= getPlot ( )->itemList ( );
	size_t					index		= 0;
	if (getSelection ( ).selectedCurveIndex >= itemList.size ( ))
	{
		UTF8String	mess (charset);
		mess << "Erreur interne en " << __FILE__ << ' ' << (unsigned long)__LINE__
		     << " QwtPlotCurveCoordinatesPicker::getSelectedCurve. "
		     << "Incohérence entre le numéro de courbe sélectionnée et le "
		     << "contenu du graphique.";
		ConsoleOutput::cerr ( ) << mess << co_endl;
		return 0;
	}	// if (getSelection ( ).selectedCurveIndex >= itemList.size ( ))
	for (QwtPlotItemIterator it = itemList.begin ( ); it != itemList.end( );
	     it++)
	{
		if ((*it)->rtti ( ) == QwtPlotItem::Rtti_PlotCurve)
		{
			QwtPlotCurve*	c	= dynamic_cast<QwtPlotCurve*>(*it);
			assert (0 != c);

			if (index == getSelection ( ).selectedCurveIndex)
				return c;

			index++;
		}	// if ((*it)->rtti ( ) == QwtPlotItem::Rtti_PlotCurve)
	}	// for (QwtPlotItemIterator it = itemList.begin ( ); ...

	return 0;
}	// QwtPlotCurveCoordinatesPicker::getSelectedCurve


const QwtPlotCurve* QwtPlotCurveCoordinatesPicker::getSelectedCurve ( ) const
{
	if (((size_t)-1 == getSelection ( ).selectedCurveIndex) ||
	    (0 == getPlot ( )))
		return 0;

	const QwtPlotItemList&	itemList	= getPlot ( )->itemList ( );
	size_t					index		= 0;
	if (getSelection ( ).selectedCurveIndex >= itemList.size ( ))
	{
		UTF8String	mess (charset);
		mess << "Erreur interne en " << __FILE__ << ' ' << (unsigned long)__LINE__
		     << " QwtPlotCurveCoordinatesPicker::getSelectedCurve. "
		     << "Incohérence entre le numéro de courbe sélectionnée et le "
		     << "contenu du graphique.";
		ConsoleOutput::cerr ( ) << mess << co_endl;
		return 0;
	}	// if (getSelection ( ).selectedCurveIndex >= itemList.size ( ))
	for (QwtPlotItemIterator it = itemList.begin ( ); it != itemList.end( );
	     it++)
	{
		if ((*it)->rtti ( ) == QwtPlotItem::Rtti_PlotCurve)
		{
			QwtPlotCurve*	c	= dynamic_cast<QwtPlotCurve*>(*it);
			assert (0 != c);

			if (index == getSelection ( ).selectedCurveIndex)
				return c;

			index++;
		}	// if ((*it)->rtti ( ) == QwtPlotItem::Rtti_PlotCurve)
	}	// for (QwtPlotItemIterator it = itemList.begin ( ); ...

	return 0;
}	// QwtPlotCurveCoordinatesPicker::getSelectedCurve


void QwtPlotCurveCoordinatesPicker::getSelectionArea (
						double& x1, double& y1, double& x2, double& y2) const
{
	if (0 == _selectionRepresentation)
		throw Exception ("QwtPlotCurveCoordinatesPicker::getSelectionArea : absence de sélection.");

	const size_t	pointCount	= _selectionRepresentation->dataSize ( );
	if (0 == pointCount)
	{ x1	= x2	= y1	= y2	= 0.; }
	else
	{
		x1	= _selectionRepresentation->minXValue ( );
		y1	= _selectionRepresentation->minYValue ( );
		x2	= _selectionRepresentation->maxXValue ( );
		y2	= _selectionRepresentation->maxYValue ( );
	}
}	// QwtPlotCurveCoordinatesPicker::getSelectionArea


void QwtPlotCurveCoordinatesPicker::getSelectionExtrema (
				double& xmin, double& ymin, double& xmax, double& ymax) const
{
	if (0 == _selectionRepresentation)
		throw Exception ("QwtPlotCurveCoordinatesPicker::getSelectionExtrema : absence de sélection.");

	const size_t	pointCount	= _selectionRepresentation->dataSize ( );
	if (0 == pointCount)
	{ xmin	= xmax	= ymin	= ymax	= 0.; }
	else
	{
		CHECK_NULL_PTR_ERROR (_selectionRepresentation->data ( ))
		const QwtSeriesData<QPointF>&	data	=
										*_selectionRepresentation->data ( );
		const QwtDoubleRect	rect	= data.boundingRect ( );
		xmin	= rect.left ( );
		ymin	= rect.top ( );
		xmax	= rect.right ( );
		ymax	= rect.bottom ( );
	}
}	// QwtPlotCurveCoordinatesPicker::getSelectionExtrema


void QwtPlotCurveCoordinatesPicker::createSelectionRepresentation ( )
{
	assert (0 == _selectionRepresentation);
	if (0 == getSelectedCurve ( ))
		return;
	QColor	selectionColor (255, 125, 0);
	_selectionRepresentation	= new QwtPlotCurve ("Selection");
	_selectionRepresentation->setItemAttribute (
			QwtPlotItem::Legend, getChartPanel ( )->isSelectionLegendShown ( ));
	_selectionRepresentation->attach (getPlot ( ));
	_selectionRepresentation->setPen (selectionColor);
	AutoArrayPtr<double>	xData (getSelection ( ).selectedPointIndexes.size ( )),
							yData (getSelection ( ).selectedPointIndexes.size ( ));
	for (size_t i = 0; i < getSelection ( ).selectedPointIndexes.size ( ); i++)
	{
		xData [i]	= getSelectedCurve ( )->sample (
							getSelection ( ).selectedPointIndexes [i]).x ( );
		yData [i]	= getSelectedCurve ( )->sample (
							getSelection ( ).selectedPointIndexes [i]).y ( );
	}	// for (size_t i = 0; i < getSelection ( ).selectedPointIndexes.size ( ); i++)
	_selectionRepresentation->setSamples (xData.get ( ), yData.get ( ),
                                 getSelection ( ).selectedPointIndexes.size( ));

	const QwtSymbol*	sym		= getSelectedCurve ( )->symbol ( );
	QwtSymbol*			symbol	= new QwtSymbol ( );
	if (0 != sym)
		QwtUtilities::copySymbol (*sym, *symbol);
	// Ecraser le symbol de la courbe originelle :
	QSize	size	= 0 == sym ? QSize (3, 3) : sym->size ( );
	size	= size + QSize (4, 4);
	QwtUtilities::setSymbolAttributes (
			*symbol, QwtSymbol::Triangle, size, selectionColor, selectionColor);

	_selectionRepresentation->setSymbol (symbol);
	getPlot ( )->replot ();
}	// QwtPlotCurveCoordinatesPicker::createSelectionRepresentation



