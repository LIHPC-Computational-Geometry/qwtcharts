#include "QwtCharts/QwtChartPanel.h"
#include "QwtCharts/QwtChartOptionsDialog.h"
#include "QwtCharts/QwtChartsManager.h"
#include "QwtCharts/QwtExtendedPlotCurve.h"
#include "QwtCharts/QwtExtendedPlotZoomer.h"
#include "QwtCharts/QwtExtendedScaleDraw.h"
#include "QwtCharts/QwtPlotCurveCoordinatesPicker.h"	// For ugly code
#include <QtUtil/QtMessageBox.h>
#include <QtUtil/QtUnicodeHelper.h>
#include <QtUtil/QtConfiguration.h>
#include <QtUtil/QtPaintAttributes.h>
#include <TkUtil/Exception.h>
#include <TkUtil/File.h>
#include <TkUtil/InternalError.h>
#include <TkUtil/MemoryError.h>

#include <QEvent>
#include <QLayout>
#include <QMdiSubWindow>
#include <QPaintEngine>
#include <QPainter>
#include <QWheelEvent>

#include <qwt_plot_multi_barchart.h>
#include <qwt_legend_label.h>
#include <qwt_plot_legenditem.h>
#include <qwt_picker_machine.h>
#include <qwt_plot_renderer.h>
#include <qwt_scale_widget.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_item.h>
#include <qwt_plot_layout.h>
#include <qwt_painter.h>

#include "QwtCharts/QwtExtendedPlot.h"

#include <memory>
#include <assert.h>
#include <values.h>


//#include <math.h>	// isnormal (norme C 99)
#include <cmath>

static bool isValid (const double& d)
{
	switch (std::fpclassify (d))
	{
		case FP_NORMAL		:
		case FP_ZERO		: return true;
		case FP_NAN			:
		case FP_INFINITE	:
		case FP_SUBNORMAL	:
		default				: return false;
	}	// switch (fpclassify (d))
}	// isValid


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



static void absExtrema (
		const QwtSeriesData<QwtSetSample>* data, double& min, double& max)
{
	if (0 == data)
		return;

	const size_t	count	= data->size ( );
	for (size_t i = 0; i < count; i++)
	{
		QwtSetSample	sample	= data->sample (i);
		min	= sample.value < min ? sample.value : min;
		max	= sample.value > max ? sample.value : max;
	}	// for (size_t i = 0; i < count; i++)
}	// absExtrema


static void ordExtrema (
		const QwtSeriesData<QwtSetSample>* data, double& min, double& max)
{
	if (0 == data)
		return;

	const size_t	count	= data->size ( );
	for (size_t i = 0; i < count; i++)
	{
		QwtSetSample	sample	= data->sample (i);
		if (0 == sample.set.size ( ))
			continue;
		double			value	= sample.set [0];
		min	= value < min ? value : min;
		max	= value > max ? value : max;
	}	// for (size_t i = 0; i < count; i++)
}	// ordExtrema


// ---------------------------------------------------------------------------
//               LA CLASSE QwtChartPanel::QwtChartEditionPanel
// ---------------------------------------------------------------------------

QwtChartPanel::QwtChartEditionPanel::QwtChartEditionPanel (
										QWidget* parent, QwtChartPanel* panel)
	: QWidget (parent), _chartPanel (panel)
{
}	// QwtChartEditionPanel::QwtChartEditionPanel


QwtChartPanel::QwtChartEditionPanel::QwtChartEditionPanel (
									const QwtChartPanel::QwtChartEditionPanel&)
	: QWidget (0), _chartPanel (0)
{
	assert (0 && "QwtChartEditionPanel copy constructor is not allowed.");
}	// QwtChartEditionPanel::QwtChartEditionPanel


QwtChartPanel::QwtChartEditionPanel&
						QwtChartPanel::QwtChartEditionPanel::operator = (
									const QwtChartPanel::QwtChartEditionPanel&)
{
	assert (0 && "QwtChartEditionPanel operator = is not allowed.");
	return *this;
}	// QwtChartEditionPanel::QwtChartEditionPanel


void QwtChartPanel::QwtChartEditionPanel::apply ( )
{
}	// QwtChartPanel::QwtChartEditionPanel::apply


const QwtChartPanel& QwtChartPanel::QwtChartEditionPanel::getChartPanel( ) const
{
	CHECK_NULL_PTR_ERROR (_chartPanel)
	return *_chartPanel;
}	// QwtChartPanel::QwtChartEditionPanel::getChartPanel


QwtChartPanel& QwtChartPanel::QwtChartEditionPanel::getChartPanel ( )
{
	CHECK_NULL_PTR_ERROR (_chartPanel)
	return *_chartPanel;
}	// QwtChartPanel::QwtChartEditionPanel::getChartPanel


// ---------------------------------------------------------------------------
//               LA CLASSE QwtChartPanel::QwtChartSpecificPanelCreator
// ---------------------------------------------------------------------------

QwtChartPanel::QwtChartSpecificPanelCreator::QwtChartSpecificPanelCreator (
														QwtChartPanel* panel)
	: _chartPanel (panel)
{
}	// QwtChartSpecificPanelCreator::QwtChartSpecificPanelCreator


QwtChartPanel::QwtChartSpecificPanelCreator::QwtChartSpecificPanelCreator (
							const QwtChartPanel::QwtChartSpecificPanelCreator&)
	: _chartPanel (0)
{
	assert (0 && "QwtChartSpecificPanelCreator copy constructor is not allowed.");
}	// QwtChartSpecificPanelCreator::QwtChartSpecificPanelCreator


QwtChartPanel::QwtChartSpecificPanelCreator&
				QwtChartPanel::QwtChartSpecificPanelCreator::operator = (
							const QwtChartPanel::QwtChartSpecificPanelCreator&)
{
	assert (0 && "QwtChartSpecificPanelCreator operator = is not allowed.");
	return *this;
}	// QwtChartSpecificPanelCreator::QwtChartSpecificPanelCreator


vector<QwtChartPanel::QwtChartEditionPanel*>
	QwtChartPanel::QwtChartSpecificPanelCreator::createEditionPanels (QWidget*)
{
	vector<QwtChartPanel::QwtChartEditionPanel*>	list;
	return list;
}	// QwtChartPanel::QwtChartSpecificPanelCreator::createEditionPanels


const QwtChartPanel& QwtChartPanel::QwtChartSpecificPanelCreator::getChartPanel(
																		) const
{
	CHECK_NULL_PTR_ERROR (_chartPanel)
	return *_chartPanel;
}	// QwtChartPanel::QwtChartSpecificPanelCreator::getChartPanel


QwtChartPanel& QwtChartPanel::QwtChartSpecificPanelCreator::getChartPanel ( )
{
	CHECK_NULL_PTR_ERROR (_chartPanel)
	return *_chartPanel;
}	// QwtChartPanel::QwtChartSpecificPanelCreator::getChartPanel



// ---------------------------------------------------------------------------
//               LA CLASSE QwtChartPanel::DataPaintAttributes
// ---------------------------------------------------------------------------

QwtChartPanel::DataPaintAttributes::DataPaintAttributes (
							QwtPlotItem* plotItem, const PaintAttributes& pen)
	: _plotItem (plotItem), _name ( ), _pen (pen),
	  _fgValid (true), _bgValid (true)
{
	if (0 != _plotItem)
		_name	= _plotItem->title ( ).text ( ).toStdString ( );
}	// DataPaintAttributes::DataPaintAttributes


QwtChartPanel::DataPaintAttributes::DataPaintAttributes (
							const string& name, const PaintAttributes& pen)
	: _plotItem (0), _name (name), _pen (pen),
	  _fgValid (true), _bgValid (true)
{
}	// DataPaintAttributes::DataPaintAttributes


QwtChartPanel::DataPaintAttributes::DataPaintAttributes (
							const QwtChartPanel::DataPaintAttributes& copied)
	: _plotItem (copied._plotItem), _name (copied._name), _pen (copied._pen),
	  _fgValid (copied._fgValid), _bgValid (copied._bgValid)
{
}	// DataPaintAttributes::DataPaintAttributes


QwtChartPanel::DataPaintAttributes&
				QwtChartPanel::DataPaintAttributes::operator = (
							const QwtChartPanel::DataPaintAttributes& copied)
{
	if (&copied != this)
	{
		_plotItem	= copied._plotItem;
		_name		= copied._name;
		_pen		= copied._pen;
		_fgValid	= copied._fgValid;
		_bgValid	= copied._bgValid;
	}	// if (&copied != this)

	return *this;
}	// DataPaintAttributes::DataPaintAttributes



// ---------------------------------------------------------------------------
//                     LA CLASSE QwtChartPanel::AxisScale
// ---------------------------------------------------------------------------

QwtChartPanel::AxisScale::AxisScale (
		double min, double max, bool autoScale, double tickStep, bool updated)
	: _min (min), _max (max), _auto (autoScale), _updated (updated),
	  _tickStep (tickStep)
{
}	// AxisScale::AxisScale


QwtChartPanel::AxisScale::AxisScale (const QwtChartPanel::AxisScale& as)
	: _min (as._min), _max (as._max), _auto (as._auto), _updated (as._updated),
	  _tickStep (as._tickStep)
{
}	// AxisScale::AxisScale (const QwtChartPanel::AxisScale&)


QwtChartPanel::AxisScale& QwtChartPanel::AxisScale::operator = (
										const QwtChartPanel::AxisScale& as)
{
	if (&as != this)
	{
		_min		= as._min;
		_max		= as._max;
		_auto		= as._auto;
		_updated	= as._updated;
		_tickStep	= as._tickStep;
	}

	return *this;
}	// AxisScale::operator = 


// ---------------------------------------------------------------------------
//                           LA CLASSE QwtChartPanel
// ---------------------------------------------------------------------------


bool						QwtChartPanel::drawHorMajorLines	= true;
bool						QwtChartPanel::drawHorMinorLines	= false;
bool						QwtChartPanel::drawVerMajorLines	= true;
bool						QwtChartPanel::drawVerMinorLines	= false;
QwtPlot::LegendPosition		QwtChartPanel::legendPosition		=	
											QwtPlot::RightLegend;
QwtLegendData::Mode			QwtChartPanel::legendCheckable	=
											QwtLegendData::ReadOnly;
int							QwtChartPanel::legendCurveWidth		= 3;
unsigned char				QwtChartPanel::scaleLabelPrecision	= 10;
char						QwtChartPanel::scaleLabelFormat		= 'g';
int							QwtChartPanel::imageSizeMin			= 1000;
bool						QwtChartPanel::mouseZoomUp			= true;
bool						QwtChartPanel::displaySelectionLegend= false;



QwtChartPanel::QwtChartPanel (QWidget* parent)
	: QWidget (parent),
	  _displayMode (CARTESIAN), _allowedDisplayModes (CARTESIAN | POLAR),
	  _abscissaScale ( ), _ordinateScale ( ),
	  _plotWidget (0), _plotItems ( ), _modificationList ( ),
	  _grid (0),
	  _legendCheckable(legendCheckable), _legendPosition(QwtPlot::RightLegend),
	  _showSelectionLegend (displaySelectionLegend),
	  _legendCurveWidth (legendCurveWidth),
	  _coordsPicker (0), _zoomer (0),
	  _rescaler (0),
	  _panner (0), _logStream (0), _specificPanelCreator (0), _editionDialog (0)
{

	QHBoxLayout*	mainLayout	= new QHBoxLayout (this);
	mainLayout->setSizeConstraint (QLayout::SetMinimumSize);

	_plotWidget	= new QwtExtendedPlot (this);
	CHECK_NULL_PTR_ERROR (_plotWidget)
	CHECK_NULL_PTR_ERROR (_plotWidget->canvas ( ))
	QFrame*	frame	= dynamic_cast<QFrame*>(_plotWidget->canvas ( ));
	if (0 != frame)
		frame->setFrameStyle(QFrame::NoFrame);
	_grid		= new QwtPlotGrid ( );
	CHECK_NULL_PTR_ERROR (_grid)
	_grid->enableX (drawHorMajorLines);		// Lignes verticales majeures
	_grid->enableXMin (drawHorMinorLines);	// Lignes verticales mineures
	_grid->enableY (drawVerMajorLines);		// Lignes horizontales majeures
	_grid->enableYMin (drawVerMinorLines);	// Lignes horizontales mineures
	_grid->attach (_plotWidget);
	
	setBackgroundColor (Qt::white);
	showLegend (true);
	_plotWidget->setMinimumSize (300, 200);
	_plotWidget->setContentsMargins (15, 15, 15, 15);
	for (int i = QwtPlot::yLeft; i < QwtPlot::axisCnt; i++)
	{
		_plotWidget->setAxisScaleDraw (
			i, new QwtExtendedScaleDraw(scaleLabelPrecision, scaleLabelFormat));
	}	// for (int i = QwtPlot::yLeft; i < QwtPlot::axisCnt; i++)

	mainLayout->addWidget (_plotWidget);
	setLayout (mainLayout);

	_panner	= new QwtPlotPanner (_plotWidget->canvas ( ));
	CHECK_NULL_PTR_ERROR (_panner)
	_panner->setMouseButton(Qt::MidButton);
	_panner->setCursor (Qt::ClosedHandCursor);
	_panner->setEnabled (true);

	// ATTENTION : le picker doit être créé après le panner, sinon problème de
	// curseur.
/* v 0.47.0 : fait dans les classes dérivées.
  	_coordsPicker	= new QwtPlotCoordinatesPicker (
				QwtPlot::xBottom, QwtPlot::yLeft,
				0,
//				QwtPlotPicker::CrossRubberBand, QwtPicker::AlwaysOn,
				QwtPlotPicker::RectRubberBand, QwtPicker::AlwaysOn,
				_plotWidget);
	CHECK_NULL_PTR_ERROR (_coordsPicker)
	_coordsPicker->setChartPanel (this); */

	QwtPlotCanvas*	canvas	=
						dynamic_cast<QwtPlotCanvas*>(_plotWidget->canvas ( ));
	_zoomer	= new QwtExtendedPlotZoomer (
						QwtPlot::xBottom, QwtPlot::yLeft, canvas);
	CHECK_NULL_PTR_ERROR (_zoomer)
	_zoomer->setRubberBand (QwtPicker::RectRubberBand);
	_zoomer->setRubberBandPen (QColor (Qt::green));
	_zoomer->setTrackerMode (QwtPicker::AlwaysOn);
	_zoomer->setTrackerPen (QColor (Qt::black));	// Couleur coords affichees
	_zoomer->setStateMachine (new QwtPickerDragRectMachine ( ));
//	_zoomer->setMousePattern (QwtEventPattern::MouseSelect2, Qt::RightButton,
//						Qt::ControlModifier);
//	_zoomer->setMousePattern (QwtEventPattern::MouseSelect3, Qt::RightButton);
	_zoomer->setEnabled (false);
	connect (_zoomer, SIGNAL (zoomed (const QRectF&)), this,
	         SLOT (zoomCallback_Qwt6 (const QRectF&)));

	// QwtPlotRescaler::Fitting : le graphique utilise tout l'espace, et 
	// utilise la politique d'expansion => tout le graphique n'est pas
	// nécessairement visible sauf si l'on joue avec setIntervalHint.
	// QwtPlotRescaler::Expanding : le redimensionnement des graphiques n'est
	// pas flagrant ...
	// QwtPlotRescaler::Fixed : idem Fitting mais ne prend pas en compte
	// setIntervalHint.

	// On créé ici un rescaler qui a pour vocation d'afficher en toutes
	// circonstances la totalité des courbes, sauf spécification contraire de
	// l'utilisateur.
	// Ce rescaler est inactivé par défaut.
	_rescaler	= new QwtPlotRescaler (
		_plotWidget->canvas( ), QwtPlot::xBottom, QwtPlotRescaler::Fitting);
	_rescaler->setExpandingDirection (QwtPlotRescaler::ExpandBoth);
	_rescaler->setAspectRatio (1.);
	_rescaler->setEnabled (false);
	_zoomer->setChartPanel (this);

	if (true == QwtChartsManager::isInitialized ( ))
		QwtChartsManager::registerChartPanel (*this);

	setFocusPolicy (Qt::StrongFocus);

	// Bogue dans Qwt 5.0.2 ? La fonte utilisée par défaut est celle du widget,
	// nom celle retournée par title ( ).font ( ).
	// On force donc l'affectation de la fonte du QwtText à celle réllement
	// utilisée par défaut :
	if (0 != _plotWidget->titleLabel ( ))
	{
		QwtText	title	= _plotWidget->title ( );
		_plotWidget->setTitle (title);
	}	// if (0 != _plotWidget->titleLabel ( ))
}	// QwtChartPanel::QwtChartPanel


QwtChartPanel::QwtChartPanel (const QwtChartPanel& view)
	: QWidget (0),
	  _displayMode (CARTESIAN), _allowedDisplayModes (CARTESIAN | POLAR),
	  _abscissaScale ( ), _ordinateScale ( ),
	  _plotItems ( ), _modificationList ( ),
	  _grid (0),
	  _legendCheckable (legendCheckable), _legendPosition (legendPosition),
	  _showSelectionLegend (displaySelectionLegend),
	  _legendCurveWidth (legendCurveWidth),
	  _coordsPicker (0), _zoomer (0),
	  _panner (0), _logStream (0), _specificPanelCreator (0), _editionDialog (0)
{
	assert (0 && "QwtChartPanel copy constructor is not allowed.");
}	// QwtChartPanel::QwtChartPanel (const QwtChartPanel& view)


QwtChartPanel& QwtChartPanel::operator = (const QwtChartPanel& view)
{
	assert (0 && "QwtChartPanel::operator = is not allowed.");
	return *this;
}	// QwtChartPanel::QwtChartPanel (const QwtChartPanel& view)


QwtChartPanel::~QwtChartPanel ( )
{
	delete _specificPanelCreator;
	if (true == QwtChartsManager::isInitialized ( ))
		QwtChartsManager::unregisterChartPanel (*this);
	delete _coordsPicker;
	_zoomer->setChartPanel (0);
	delete _zoomer;
	delete _panner;
}	// QwtChartPanel::~QwtChartPanel


void QwtChartPanel::setLogOutputStream (LogOutputStream* stream)
{
	_logStream	= stream;
}	// QwtChartPanel::setLogOutputStream


size_t QwtChartPanel::getItemCount ( ) const
{
	return _plotItems.size ( );
}


void QwtChartPanel::setDisplayMode (DISPLAY_MODE mode)
{
	if (mode == _displayMode)
		return;

	if (false == isDisplayModeAllowed (mode))
	{
		UTF8String	msg ("Mode de représentation interdit : ", charset);
		switch (mode)
		{
			case CARTESIAN	: msg << "représentation cartésienne.";	break;
			case POLAR		: msg << "représentation polaire.";		break;
			default			: msg << "représentation indéfinie.";	break;
		}	// switch (mode)
		throw Exception (msg);
	}	// if (false == isDisplayModeAllowed (mode))

	// Ugly code, to redefine :
	QwtPlotCurveCoordinatesPicker*	curvePicker	=
		dynamic_cast<QwtPlotCurveCoordinatesPicker*>(&getCoordinatesPicker ( ));
	if (0 != curvePicker)
		curvePicker->setDisplayMode ((QwtChartPanel::DISPLAY_MODE)mode);

	_displayMode	= mode;
	emit displayModeModified (this);
}	// QwtChartPanel::setDisplayMode


bool QwtChartPanel::isDisplayModeAllowed (DISPLAY_MODE mode) const
{
	return 0 == (_allowedDisplayModes & (size_t)mode) ? false : true;
}	// QwtChartPanel::isDisplayModeAllowed


void QwtChartPanel::enableDisplayModes (size_t mask)
{
	_allowedDisplayModes	= mask;
}	// QwtChartPanel::enableDisplayModes


const QColor& QwtChartPanel::getBackgroundColor ( ) const
{
	return getPlot ( ).canvasBackground ( ).color ( );
}	// QwtChartPanel::getBackgroundColor


void QwtChartPanel::setBackgroundColor (const QColor& bg)
{
	QPalette	p (palette ( ));
	setAutoFillBackground (true);
	p.setColor (QPalette::Active, QPalette::Background, bg);
	p.setColor (QPalette::Inactive, QPalette::Background, bg);
	setPalette (p);
	getPlot ( ).setCanvasBackground (bg);
	getPlot ( ).replot ( );
}	// QwtChartPanel::setBackgroundColor


int QwtChartPanel::getMargin ( ) const
{
	int	margin	= 0;
	getContentsMargins (&margin, &margin, &margin, &margin);
	return margin;
}	// QwtChartPanel::getMargin


void QwtChartPanel::setMargin (int margin)
{
	if (0 > margin)
		throw Exception ("Il n'est pas possible d'appliquer à un graphique des marges de valeur négative.");

	getPlot ( ).setContentsMargins (margin, margin, margin, margin);
}	// QwtChartPanel::setMargin


QwtText QwtChartPanel::getTitle ( ) const
{
	return getPlot ( ).title ( );
}	// QwtChartPanel::getTitle


void QwtChartPanel::setTitle (const QwtText& title)
{
	getPlot ( ).setTitle (title);
}	// QwtChartPanel::setTitle


void QwtChartPanel::setAbscissaTitle (const QwtText& title)
{
	getPlot ( ).setAxisTitle (QwtPlot::xBottom, title);
}	// QwtChartPanel::setAbscissaTitle


void QwtChartPanel::setOrdinateTitle (const QwtText& title)
{
	getPlot ( ).setAxisTitle (QwtPlot::yLeft, title);
}	// QwtChartPanel::setOrdinateTitle


bool QwtChartPanel::getAbscissaScale (double& min, double& max, bool& autoScale)
{
	max	= -MAXDOUBLE;
	min	= MAXDOUBLE;
	if ((false == _abscissaScale._auto) || (false == _abscissaScale._updated))
	{	// Essayer néanmoins d'obtenir l'info :
//		if (0 != getPlot ( ).axisScaleDiv (QwtPlot::xBottom))
		if (false == _abscissaScale._updated)
		{
/*
			min	= getPlot ( ).axisScaleDiv (QwtPlot::xBottom)->lowerBound ( );
			max	= getPlot ( ).axisScaleDiv (QwtPlot::xBottom)->upperBound ( );
*/
			// On va à la pêche : Qwt ne stocke pas ces infos.
			vector<QwtPlotItem*>::iterator	iti	= _plotItems.begin ( );
			for ( ; _plotItems.end ( ) != iti; iti++)
			{
				QwtPlotCurve*				curve		=
								dynamic_cast<QwtPlotCurve*>(*iti);
				QwtPlotMultiBarChart*	barChart	=
								dynamic_cast<QwtPlotMultiBarChart*>(*iti);
				if (0 != curve)
				{
					double	mn	= curve->minXValue ( );
					double	mx	= curve->maxXValue ( );
					min	= min < mn ? min : mn;
					max	= max > mx ? max : mx;
				}	// if (0 != curve)
				else if (0 != barChart)
				{
					const QwtSeriesData<QwtSetSample>*	data	=
														barChart->data ( );
					absExtrema (data, min, max);
				}	// else if (0 != barChart)
			}	// for ( ; _plotItems.end ( ) != iti; iti++)

			_abscissaScale._min		= min;
			_abscissaScale._max		= max;
			_abscissaScale._updated	= true;
//			autoScale	= false;	// v 0.47.0
			autoScale	= _abscissaScale._auto;	// v 0.47.0
			return true;
		}	// if (0 != getPlot ( ).axisScaleDiv (QwtPlot::xBottom))
	}	// if ((false == _abscissaScale._auto) || ...

	min			= _abscissaScale._min;
	max			= _abscissaScale._max;
	autoScale	= _abscissaScale._auto;

	return _abscissaScale._auto;
}	// QwtChartPanel::getAbscissaScale


bool QwtChartPanel::getVisibleAbscissaScale  (double& min, double& max)
{
	bool	ok	= false;
	max			= -MAXDOUBLE;
	min			= MAXDOUBLE;

	// On va à la pêche : Qwt ne stocke pas ces infos.
	vector<QwtPlotItem*>::iterator	iti	= _plotItems.begin ( );
	for ( ; _plotItems.end ( ) != iti; iti++)
	{
		if (false == (*iti)->isVisible ( ))
			continue;

		QwtPlotCurve*				curve		=
								dynamic_cast<QwtPlotCurve*>(*iti);
		QwtPlotMultiBarChart*	barChart	=
								dynamic_cast<QwtPlotMultiBarChart*>(*iti);
		if (0 != curve)
		{
			ok	= true;
			double	mn	= curve->minXValue ( );
			double	mx	= curve->maxXValue ( );
			min	= min < mn ? min : mn;
			max	= max > mx ? max : mx;
		}	// if (0 != curve)
		else if (0 != barChart)
		{
			ok	= true;
			const QwtSeriesData<QwtSetSample>*	data	= barChart->data ( );
			absExtrema (data, min, max);
		}	// else if (0 != barChart)
	}	// for ( ; _plotItems.end ( ) != iti; iti++)

	return ok;
}	// QwtChartPanel::getVisibleAbscissaScale


void QwtChartPanel::setAbscissaScale (
						double min, double max, bool autoScale, double tickStep)
{
	_abscissaScale._auto		= autoScale;
	_abscissaScale._tickStep	= tickStep;
	if (false == autoScale)
	{
		_abscissaScale._min			= min;
		_abscissaScale._max			= max;
		getPlot ( ).setAxisScale (
					QwtPlot::xBottom, _abscissaScale._min, _abscissaScale._max,
					tickStep);
		getPlot ( ).setAxisScale (
					QwtPlot::xTop, _abscissaScale._min, _abscissaScale._max,
					tickStep);
		_abscissaScale._updated	= true;
	}	// if (false == autoScale)
	else
	{
		getPlot ( ).setAxisAutoScale (QwtPlot::xBottom);
		getPlot ( ).setAxisAutoScale (QwtPlot::xTop);
		_abscissaScale._updated	= false;
	}	// else if (false == auto)

	adjustChart ( );
}	// QwtChartPanel::setAbscissaScale


bool QwtChartPanel::getOrdinateScale (double& min, double& max, bool& autoScale)
{
	max	= -MAXDOUBLE;
	min	= MAXDOUBLE;
	if ((false == _ordinateScale._auto) || (false == _ordinateScale._updated))
	{	// Essayer néanmoins d'obtenir l'info :
		if (false == _ordinateScale._updated)
//		if (0 != getPlot ( ).axisScaleDiv (QwtPlot::yLeft))
		{
/*
			min	= getPlot ( ).axisScaleDiv (QwtPlot::yLeft)->lowerBound ( );
			max	= getPlot ( ).axisScaleDiv (QwtPlot::yLeft)->upperBound ( );
*/
			// On va à la pêche : Qwt ne stocke pas ces infos.
			vector<QwtPlotItem*>::iterator	iti	= _plotItems.begin ( );
			for ( ; _plotItems.end ( ) != iti; iti++)
			{
				QwtPlotCurve*			curve	=
								dynamic_cast<QwtPlotCurve*>(*iti);
				QwtPlotMultiBarChart*	barChart	=
								dynamic_cast<QwtPlotMultiBarChart*>(*iti);
				if (0 != curve)
				{
					double	mn	= curve->minYValue ( );
					double	mx	= curve->maxYValue ( );
					min	= min < mn ? min : mn;
					max	= max > mx ? max : mx;
				}	// if (0 != curve)
				else if (0 != barChart)
				{
					const QwtSeriesData<QwtSetSample>*	data	=
														barChart->data ( );
					ordExtrema (data, min, max);
				}	// else if (0 != barChart)
			}	// for ( ; _plotItems.end ( ) != iti; iti++)

			_ordinateScale._min		= min;
			_ordinateScale._max		= max;
			_ordinateScale._updated	= true;
//			autoScale	= false;	// v 0.47.0
			autoScale	= _ordinateScale._auto;	// v 0.47.0
			return true;
		}	// if (0 != getPlot ( ).axisScaleDiv (QwtPlot::yLeft))
	}	// if ((false == _ordinateScale._auto) || ...
	min			= _ordinateScale._min;
	max			= _ordinateScale._max;
	autoScale	= _ordinateScale._auto;

	return _ordinateScale._auto;
}	// QwtChartPanel::getOrdinateScale


bool QwtChartPanel::getVisibleOrdinateScale  (double& min, double& max)
{
	bool	ok	= false;
	max			= -MAXDOUBLE;
	min			= MAXDOUBLE;

	// On va à la pêche : Qwt ne stocke pas ces infos.
	vector<QwtPlotItem*>::iterator	iti	= _plotItems.begin ( );
	for ( ; _plotItems.end ( ) != iti; iti++)
	{
		if (false == (*iti)->isVisible ( ))
			continue;

		QwtPlotCurve*				curve		=
								dynamic_cast<QwtPlotCurve*>(*iti);
		QwtPlotMultiBarChart*	barChart	=
								dynamic_cast<QwtPlotMultiBarChart*>(*iti);
		if (0 != curve)
		{
			ok	= true;
			double	mn	= curve->minYValue ( );
			double	mx	= curve->maxYValue ( );
			min	= min < mn ? min : mn;
			max	= max > mx ? max : mx;
		}	// if (0 != curve)
		else if (0 != barChart)
		{
			ok	= true;
			const QwtSeriesData<QwtSetSample>*	data	= barChart->data ( );
			ordExtrema (data, min, max);
		}	// else if (0 != barChart)
	}	// for ( ; _plotItems.end ( ) != iti; iti++)

	return ok;
}	// QwtChartPanel::getVisibleOrdinateScale


void QwtChartPanel::setOrdinateScale (
						double min, double max, bool autoScale, double tickStep)
{
	_ordinateScale._auto		= autoScale;
	_ordinateScale._tickStep	= tickStep;
	if (false == autoScale)
	{
		_ordinateScale._min			= min;
		_ordinateScale._max			= max;
		getPlot ( ).setAxisScale (
					QwtPlot::yLeft, _ordinateScale._min, _ordinateScale._max,
					tickStep);
		getPlot ( ).setAxisScale (
					QwtPlot::yRight, _ordinateScale._min, _ordinateScale._max,
					tickStep);
		_ordinateScale._updated	= true;
	}	// if (false == autoScale)
	else
	{
		getPlot ( ).setAxisAutoScale (QwtPlot::yLeft);
		getPlot ( ).setAxisAutoScale (QwtPlot::yRight);
		_ordinateScale._updated	= false;
	}	// else if (false == auto)

	adjustChart ( );
}	// QwtChartPanel::setOrdinateScale


bool QwtChartPanel::areHorMajorLinesDisplayed ( ) const
{
	assert (0 != _grid);
	return _grid->yEnabled ( );
}	// QwtChartPanel::areHorMajorLinesDisplayed


void QwtChartPanel::displayHorMajorLines (bool display) 
{
	assert (0 != _grid);
	_grid->enableY (display);
	getPlot ( ).replot ( );
}	// QwtChartPanel::displayHorMajorLines


bool QwtChartPanel::areHorMinorLinesDisplayed ( ) const
{
	assert (0 != _grid);
	return _grid->yMinEnabled ( );
}	// QwtChartPanel::areHorMinorLinesDisplayed


void QwtChartPanel::displayHorMinorLines (bool display) 
{
	assert (0 != _grid);
	_grid->enableYMin (display);
	getPlot ( ).replot ( );
}	// QwtChartPanel::displayHorMinorLines


bool QwtChartPanel::areVerMajorLinesDisplayed ( ) const
{
	assert (0 != _grid);
	return _grid->xEnabled ( );
}	// QwtChartPanel::areVerMajorLinesDisplayed


void QwtChartPanel::displayVerMajorLines (bool display) 
{
	assert (0 != _grid);
	_grid->enableX (display);
	getPlot ( ).replot ( );
}	// QwtChartPanel::displayVerMajorLines


bool QwtChartPanel::areVerMinorLinesDisplayed ( ) const
{
	assert (0 != _grid);
	return _grid->xMinEnabled ( );
}	// QwtChartPanel::areVerMinorLinesDisplayed


void QwtChartPanel::displayVerMinorLines (bool display) 
{
	assert (0 != _grid);
	_grid->enableXMin (display);
	getPlot ( ).replot ( );
}	// QwtChartPanel::displayVerMinorLines


void QwtChartPanel::showLegend (bool show, QwtPlot::LegendPosition pos)
{
	if (false == show)
	{
		if (0 != getPlot ( ).legend ( ))
			getPlot ( ).insertLegend (0);
		return;
	}	// if (false == show)

	if ((pos != _legendPosition) || (false == hasLegend ( )))
	{
		_legendPosition	= pos;
		QwtLegend*	legend	= new QwtLegend ( );	
		legend->setFrameStyle (QFrame::Box | QFrame::Plain);
		legend->setLineWidth (0);
		legend->setMidLineWidth (0);
		connect (legend, SIGNAL (checked  (const QVariant&, bool, int)),
				this, SLOT (legendCheckedCallback(const QVariant&, bool, int)));
		legend->setDefaultItemMode 	(_legendCheckable);
		// Nous spécifions tous les paramètres de légende :
//		legend->setDisplayPolicy (QwtLegend::NoIdentifier,
		// On insère la légende en premier afin que itemToInfo fonctionne.
		getPlot ( ).insertLegend (legend, pos);
		const QwtPlotItemList&	items	=
						getPlot ( ).itemList (QwtPlotItem::Rtti_PlotCurve);
		for (QwtPlotItemIterator it = items.begin( ); items.end( ) != it; it++)
		{
			CHECK_NULL_PTR_ERROR (*it)
			QWidget*		widget	=
							legend->legendWidget (getPlot ( ).itemToInfo (*it));
			QwtLegendLabel*	label	= dynamic_cast<QwtLegendLabel*>(widget);
			CHECK_NULL_PTR_ERROR (label)
			label->setChecked ((*it)->isVisible ( ));
		}	// for (QwtPlotItemIterator it = items.begin( ); ...
	}	// if ((pos != _legendPosition) || (false == hasLegend ( )))
}	// QwtChartPanel::showLegend


int QwtChartPanel::getLegendFrameWidth ( ) const
{
	return 0 == getLegend ( ) ? 0 : getLegend ( )->lineWidth ( );
}	// QwtChartPanel::getLegendFrameWidth


void QwtChartPanel::setLegendFrameWidth (int width)
{
	if (0 > width)
		throw Exception ("Modification de l'épaisseur du cadre d'une légende : épaisseur négative.");

	if (0 == getLegend ( ))
		throw Exception ("Modification de l'épaisseur du cadre d'une légende : absence de légende.");

	getLegend ( )->setLineWidth (width);
}	// QwtChartPanel::setLegendFrameWidth


int QwtChartPanel::getLegendMargin ( ) const
{
	const QWidget*		widget	=
					0 == getLegend ( ) ?  0 : getLegend ( )->contentsWidget( );
	const QLayout*		layout	= 0 == widget ? 0 : widget->layout ( );

	int	left	= 0, top	= 0, right	= 0, bottom	= 0;
	if (0 != layout)
		layout->getContentsMargins (&left, &top, &right, &bottom);

	return top;
}	// QwtChartPanel::getLegendMargin


void QwtChartPanel::setLegendMargin (int margin)
{
	if (0 > margin)
		throw Exception ("Modification des marges d'une légende : marge négative.");

	if (0 == getLegend ( ))
		throw Exception ("Modification des marges d'une légende : absence de légende.");
	QWidget*	widget	= 0 == getLegend ( ) ?
						  0 : getLegend ( )->contentsWidget ( );
	QLayout*	layout	= 0 == widget ? 0 : widget->layout ( );

	if (0 != layout)
	{
		layout->setContentsMargins (margin, margin, margin, margin);
		getPlot ( ).replot ( );
	}
}	// QwtChartPanel::setLegendMargin


bool QwtChartPanel::hasLegend ( ) const
{
	return 0 != getLegend ( ) ? true : false;
}	// QwtChartPanel::hasLegend


const QwtLegend* QwtChartPanel::getLegend ( ) const
{
	return dynamic_cast<const QwtLegend*>(getPlot ( ).legend ( ));
}	// QwtChartPanel::hasLegend


QwtLegend* QwtChartPanel::getLegend ( )
{
	return dynamic_cast<QwtLegend*>(getPlot ( ).legend ( ));
}	// QwtChartPanel::hasLegend


bool QwtChartPanel::isLegendCheckable ( ) const
{
	assert (QwtLegendData::Clickable != _legendCheckable);
	return QwtLegendData::Checkable == _legendCheckable ? true : false;
}	// QwtChartPanel::isLegendCheckable


void QwtChartPanel::setLegendCheckable (bool checkable)
{
	if (checkable == _legendCheckable)
		return;
	_legendCheckable	= true == checkable ?
						  QwtLegendData::Checkable : QwtLegendData::ReadOnly;
	if (0 != getLegend ( ))
	{
		// Changement de mode pour les items : nécessite de reconstruire la
		// légende.
		QwtPlot::LegendPosition	position	= getLegendPosition ( );
		showLegend (false, position);
		showLegend (true, position);
		getLegend ( )->setDefaultItemMode (_legendCheckable);
	}	// if (0 != getLegend ( ))
}	// QwtChartPanel::setLegendCheckable


bool QwtChartPanel::isSelectionLegendShown ( ) const
{
	return _showSelectionLegend;
}	// QwtChartPanel::isSelectionLegendShown


void QwtChartPanel::showSelectionLegend (bool show)
{
	if (show == _showSelectionLegend)
		return;

	_showSelectionLegend	 = show;
	QwtPlotCurveCoordinatesPicker*	picker	=
					dynamic_cast<QwtPlotCurveCoordinatesPicker*>(_coordsPicker);
	if ((0 != picker) && (0 != picker->getSelectionRepresentation ( )))
	{
		QwtPlot*	plot	= picker->getSelectionRepresentation ( )->plot ( );
		picker->getSelectionRepresentation ( )->detach ( );
		picker->getSelectionRepresentation ( )->setItemAttribute (
													QwtPlotItem::Legend, show);
		if (0 != plot)
		{
			picker->getSelectionRepresentation ( )->attach (plot);
			plot->replot ( );
		}	// if (0 != plot)
	}	// if ((0 != picker) && (0 != ...
}	// QwtChartPanel::showSelectionLegend


int QwtChartPanel::getLegendCurveWidth ( ) const
{
	return _legendCurveWidth;
}	// QwtChartPanel::getLegendCurveWidth


void QwtChartPanel::setLegendCurveWidth (int width)
{
	if (width != _legendCurveWidth)
	{
		_legendCurveWidth	= width;

		if (0 != getLegend ( ))
		{
			QList<QWidget*>		items;
			QwtPlotItemList	list	=
						getPlot ( ).itemList (QwtPlotItem::Rtti_PlotCurve);
			for (int i = 0; i < list.size ( ); i++)
				items.push_back (getLegend ( )->legendWidget (
										getPlot ( ).itemToInfo (list [i])));
/*
			for (size_t i = 0; i < items.size ( ); i++)
			{
				QwtPlotLegendItem* item	=
								dynamic_cast<QwtPlotLegendItem*>(items [i]);
				if (0 != item)
				{
				}	// if (0 != item)
			}	// for (size_t i = 0; i < items.size ( ); i++)
*/
		}	// if (0 != getLegend ( ))
	}	// if (width != _legendCurveWidth)
}	// QwtChartPanel::setLegendCurveWidth


unsigned char QwtChartPanel::getScaleLabelPrecision ( ) const
{
	const QwtExtendedScaleDraw*	scaleDraw	=
		dynamic_cast<const QwtExtendedScaleDraw*>(getPlot ( ).axisScaleDraw(0));

	if (0 == scaleDraw)
		throw Exception ("Précision utilisée lors des conversions valeur -> texte inaccessible, la classe utilisée n'est pas du type QwtExtendedScaleDraw.");

	return scaleDraw->getPrecision ( );
}	// QwtChartPanel::getScaleLabelPrecision


void QwtChartPanel::setScaleLabelPrecision (unsigned char precision)
{
	for (int i = QwtPlot::yLeft; i < QwtPlot::axisCnt; i++)
	{
		QwtExtendedScaleDraw*	scaleDraw	=
			dynamic_cast<QwtExtendedScaleDraw*>(getPlot ( ).axisScaleDraw (i));

		if (0 == scaleDraw)
			throw Exception ("Précision utilisée lors des conversions valeur -> texte inaccessible, la classe utilisée n'est pas du type QwtExtendedScaleDraw.");

		scaleDraw->setPrecision (precision);
		getPlot ( ).axisWidget (i)->repaint ( );
	}	// for (int i = QwtPlot::yLeft; i < QwtPlot::axisCnt; i++)
}	// QwtChartPanel::setScaleLabelPrecision


char QwtChartPanel::getScaleLabelFormat ( ) const
{
	const QwtExtendedScaleDraw*	scaleDraw	=
		dynamic_cast<const QwtExtendedScaleDraw*>(getPlot ( ).axisScaleDraw(0));

	if (0 == scaleDraw)
		throw Exception ("Format utilisée lors des conversions valeur -> texte inaccessible, la classe utilisée n'est pas du type QwtExtendedScaleDraw.");

	return scaleDraw->getFormat ( );
}	// QwtChartPanel::getScaleLabelFormat


void QwtChartPanel::setScaleLabelFormat (char format)
{
	for (int i = QwtPlot::yLeft; i < QwtPlot::axisCnt; i++)
	{
		QwtExtendedScaleDraw*	scaleDraw	=
			dynamic_cast<QwtExtendedScaleDraw*>(getPlot ( ).axisScaleDraw (i));

		if (0 == scaleDraw)
			throw Exception ("Format utilisée lors des conversions valeur -> texte inaccessible, la classe utilisée n'est pas du type QwtExtendedScaleDraw.");

		scaleDraw->setFormat (format);
		getPlot ( ).axisWidget (i)->repaint ( );
	}	// for (int i = QwtPlot::yLeft; i < QwtPlot::axisCnt; i++)
}	// QwtChartPanel::setScaleLabelFormat


/**
 * <P>Calcule le rectangle englobant la zone de tracé pour des impressions 
 * type postscript (imprimante, PS, PDF).</P>
 * <P>Par défaut Qt (4.3.2) a tendance à occuper toute la page, et par là même à
 * déformer la vision que l'on a du graphique. Cette fonction calcule le
 * rectangle englobant la zone d'impression en s'efforçant de maintenir le
 * rapport d'aspect largeur/hauteur observé à l'écran. </P>
 * </P>
 * @param		Paramétrage de la sortie
 * @param		Graphique à imprimer
 * @param		Zone de tracé à utiliser dans l'imprimante.
 * @return		<I>true</I> si cette fonction estime que l'impression devrait
 *				être bonne, <I>false</I> si elle estime qu'elle risque d'être
 *				médiocre, voire même de provoquer des
 *				problèmes d'affichage au niveau de la fenêtre graphique (cas
 *				rencontré avec des graphiques très étirés). L'image risque
 *				d'être médiocre dès lors que le texte occupe une part importante
 *				d'une dimension du graphique. Un bogue au niveau de l'affichage
 *				est possible au delà de cette zone à risque.
 */
static bool computeRect (
				const QPrinter& printer, const QwtPlot& plot, QRect& rect)
{
	CHECK_NULL_PTR_ERROR (plot.canvas ( ))
	if ((printer.width ( ) < 100) || (printer.width ( ) > 1E6) ||
	    (printer.height ( ) < 100) || (printer.height ( ) > 1E6))
	{
		UTF8String	msg (charset);
		msg << "Impression impossible, dimensions de l'imprimantes invalides ("
		    << (unsigned long)printer.width ( ) << ", " 
		    << (unsigned long)printer.height ( ) << ").";
		throw Exception (msg);
	}	// if ((printer.width ( ) < 100) || (printer.width ( ) > 1E6) || ...
	if ((plot.canvas ( )->width ( ) < 100) ||
	    (plot.canvas ( )->width ( ) > 1E6) ||
	    (plot.canvas ( )->height ( ) < 100) ||
	    (plot.canvas ( )->height ( ) > 1E6))
	{
		UTF8String	msg (charset);
		msg << "Impression impossible, dimensions du graphique invalides ("
		    << (unsigned long)plot.canvas ( )->width ( )<< ", " 
		    << (unsigned long)plot.canvas ( )->height ( ) << ").";
		throw Exception (msg);
	}	// if ((plot.canvas ( )->width ( ) < 100) || ...

	bool	qualityPb	= false;
	rect.setCoords (0, 0, printer.width ( ), printer.height ( ));

	// Le rapport hauteur / largeur du graphique, à conserver :
	double	ratio	= plot.canvas ( )->height ( ) /
					  (double)plot.canvas ( )->width ( );

	if (printer.width ( ) >= printer.height ( ))	// Paysage
	{
		if (rect.width ( ) * ratio <= printer.height ( ))
		{	// Il faut ratio >= 0.3
			rect.setHeight ((int)(rect.width ( ) * ratio));
			if (ratio < 0.3)
				qualityPb	= true;
		}
		else
		{	// Il faut ratio <= 1.5
			rect.setWidth ((int)(printer.height ( ) / ratio));
			if (ratio > 1.5)
				qualityPb	= true;
		}
	}
	else
	{
		if (rect.height ( ) / ratio <= printer.width ( ))
		{	// Il faut ratio <= 2
			rect.setWidth ((int)(printer.height ( ) / ratio));
			if (ratio > 2)
				qualityPb	= true;
		}
		else
		{	// Il faut ratio >= 0.6
			rect.setHeight ((int)(rect.width ( ) * ratio));
			if (ratio <= 0.6)
				qualityPb	= true;
		}
	}	// else if (printer.width ( ) >= printer.height ( ))

	rect.setX ((printer.width ( ) - rect.width ( )) / 2);
	rect.setY ((printer.height ( ) - rect.height ( )) / 2);

	return !qualityPb;
}	// computeRect


bool QwtChartPanel::print (QPrinter& printer)
{
	/*
	 * Dans le cas d'une impression dans un fichier Qt (4.3.2) créé un fichier
	 * postscript (même si l'extension est jpg, png ou autre) de résolution très
	 * faible. Dans un tel cas on utilise l'API QImage, qui, couplée aux
	 * paramètres du QPrinter, donne une résolution très (trop) élevée 
	 * (environ 100 millions de pixels). On réduit donc ici cette taille à
	 * environ 1 million de pixels (paramétrables, jouer sur la variable
	 * statique publique imageSizeMin.
	 */
	BEGIN_TRY_CATCH_BLOCK

	const bool	useImage	= useQImage (printer);

	QwtPlotRenderer	renderer;
	if (QPrinter::GrayScale == printer.colorMode ( ))
	{
		renderer.setDiscardFlag (QwtPlotRenderer::DiscardBackground);
		renderer.setDiscardFlag (QwtPlotRenderer::DiscardCanvasBackground);
		renderer.setDiscardFlag (QwtPlotRenderer::DiscardCanvasFrame);
		renderer.setLayoutFlag (QwtPlotRenderer::FrameWithScales);
	}	// if (QPrinter::GrayScale == printer.colorMode ( ))

	if (false == useImage)	// => imprimante, PS ou PDF	
	{
		// La zone de tracé.
		// On recontre ici différents problèmes :
		// - Si on ne fait une impression par défaut le graphique occupera toute
		// la place de la page, aux marges près. Dans le cas d'un graphique
		// étiré le rapport d'aspect observé à l'écran n'est pas conservé, loin
		// s'en faut. On précise donc ici le rectangle d'impression, définit à
		// partir de la taille de la fenêtre graphique.
		// - Malgré cela on observe des différences de rapports d'aspect pour
		// une raison encore inconnue, mais il semblerait que les textes
		// interviennent (taille du titre, ...), surtout si leurs proportions 
		// dans le graphique sont importantes.
		// Lors d'une impression "par défaut", pour un graphique voisin du
		// format A4, la hauteur du graphique est augmentée d'environ 10% par
		// rapport à la largeur sur une imprimante HP 4650.
		// Rem : la résolution du QPrinter est toujours annoncée à 1200x1200
		// (imprimante, pdf, jpg, ...)i, ce qui laisse supposer qu'il ne se
		// soucis peut être pas réellement du périphérique de sortie.
		// La zone de tracé :
		QRect	painterRect;
		if (false == computeRect (printer, getPlot ( ), painterRect))
		{
			UTF8String		question (charset);
			question << "Le graphique est très étiré, et l'impression risque "
			         << "d'être de médiocre qualité. De plus, l'affichage "
			         << "du graphique risque d'être fortement altéré." << "\n"
			         << "Souhaitez-vous :" << "\n"
			         << "- (Imprimer) en modifiant le rapport d'aspect, "
			         << "\n"
			         << "- (Poursuivre) avec ce même rapport d'aspect,"
			         << "\n"
			         << "- (Annuler) l'impression ?";
			switch (QtMessageBox::displayWarningMessage (
					&getPlot ( ), "Impression : risque d'erreur", question,
					100, "Imprimer", "Poursuivre", "Annuler"))
			{
				case	0	: 
					painterRect.setCoords (
								0, 0, printer.width ( ), printer.height ( ));
					break;
				case	1	: break;
				default	: return false;
			}	// switch (QtMessageBox::displayWarningMessage (
		}	// if (false == computeRect (printer, getPlot ( ), painterRect))
		renderer.renderTo (&getPlot ( ), printer);
	}	// if (false == useImage)
	else
	{	// Impression dans un fichier autre que PS ou PDF
		CHECK_NULL_PTR_ERROR (getPlot ( ).canvas ( ))
		// La surface de tracé :
		int	docWidth	= getPlot ( ).canvas ( )->width ( );
		int	docHeight	= getPlot ( ).canvas ( )->height ( );
		// On ajuste la taille de l'image :
		getImageSize (printer, docWidth, docHeight);
		int	pdwidth		= docWidth;
		int	pdheight	= docHeight;
		unique_ptr<QImage>	image (new QImage (pdwidth, pdheight, QImage::QImage::Format_RGB32));
		// On trace un fond blanc :
		QRect	r (0, 0, pdwidth, pdheight);
		QPainter	p (image.get ( ));
		p.setBrush (Qt::white);
		p.drawRect (0, 0, pdwidth, pdheight);
		p.end ( );

		// On trace le graphique :
		QPainter	painter ((QPaintDevice*)image.get( ));
		QRect	painterRect (0, 0, pdwidth, pdheight);
		renderer.render (&getPlot ( ), &painter, QRect (0,0,pdwidth,pdheight));
		image->save (printer.outputFileName ( ), 0, 100);
	}	// else if (false == useImage)

	COMPLETE_TRY_CATCH_BLOCK

	if (true == hasError)
		throw Exception (errorMsg);

	return true;
}	// QwtChartPanel::print


/* =====================================  Méthode gardée juste pour information

void QwtChartPanel::print (QPrinter& printer, bool aspectRatio)
{
	BEGIN_TRY_CATCH_BLOCK

//	if (false == aspectRatio)
	{
		print (printer);
		return;
	}	// if (false == aspectRatio)

	const	int			w	= printer.width ( ), h	= printer.height ( );
	// La surface de tracé :
	int	docWidth	= printer.width ( );
	int	docHeight	= printer.height ( );
	getImageSize (printer, docWidth, docHeight);
	int	pdwidth		= docWidth;
	int	pdheight	= docHeight;
	unique_ptr<QImage>	image (false == useQImage (printer) ?
			0 : new QImage (pdwidth, pdheight, QImage::QImage::Format_RGB32));
	if (0 != image.get ( ))
	{
		QRect	r (0, 0, pdwidth, pdheight);
		QPainter	p (image.get ( ));
		p.setBrush (Qt::white);
		p.drawRect (0, 0, pdwidth, pdheight);
	}	// if (0 != image.get ( ))

	if ((false == allowAspectRatio ( )) ||
	    (0 == getPlot ( ).axisScaleDiv (QwtPlot::xBottom)) ||
	    (0 == getPlot ( ).axisScaleDiv (QwtPlot::yLeft)))
		throw Exception ("Maintien du rapport d'aspect à 1 impossible pour ce graphique.");

	QwtPlotPrintFilter	filter;
	if (printer.colorMode ( ) == QPrinter::GrayScale)
		filter.setOptions (QwtPlotPrintFilter::PrintAll &
		                   ~QwtPlotPrintFilter::PrintCanvasBackground);
	else filter.setOptions (QwtPlotPrintFilter::PrintAll);

	// Ca n'est pas parfait.
	// On enlève de la surface d'impression toutes les légendes, titres et
	// autres. On diminue alors le plus grand de largeur/hauteur de manière
	// à avoir un rapport d'aspect de 1.
	// Le rapport d'aspect :
	double	xRange	= getPlot ( ).axisScaleDiv (QwtPlot::xBottom)->range ( );
	double	yRange	= getPlot ( ).axisScaleDiv (QwtPlot::yLeft)->range ( );

	// Issu de QwtPlot::print de Qwt 5.0.2 :
	QPainter	painter (true == useQImage(printer) ?
				         (QPaintDevice*)image.get( ) : (QPaintDevice*)&printer);
	QRect		painterRect (0, 0, pdwidth, pdheight);
	QwtPainter::setMetricsMap (&getPlot ( ), painter.device ( ));
	const QwtMetricsMap &metricsMap	= QwtPainter::metricsMap ( );
	filter.apply (&getPlot ( ));
	getPlot ( ).plotLayout()->activate (&getPlot ( ),
							metricsMap.deviceToLayout (painterRect), 0);
	// Le nombre de pixels utilisés pour les tracés "hors graphique" :
	int	legendWidth	= 0, legendHeight = 0;
	if ((0 != getPlot ( ).legend ( )) &&
	    (false == getPlot ( ).legend ( )->isEmpty ( )))
	{
		if ((QwtPlot::LeftLegend == _legendPosition) ||
		    (QwtPlot::RightLegend == _legendPosition))
			legendWidth	+= metricsMap.layoutToDeviceX (
									getPlot ( ).legend ( )->width ( ));
		if ((QwtPlot::BottomLegend == _legendPosition) ||
		    (QwtPlot::TopLegend == _legendPosition))
			legendHeight	+= metricsMap.layoutToDeviceY (
									getPlot ( ).legend ( )->height ( ));
	}	// if ((0 != getPlot ( ).legend ( )) && ...
	if ((0 != getPlot ( ).axisWidget (QwtPlot::yLeft)) &&
	    (true == getPlot ( ).axisWidget (QwtPlot::yLeft)->isVisible ( )))
		legendWidth	+= metricsMap.layoutToDeviceX (
						getPlot ( ).axisWidget (QwtPlot::yLeft)->width ( ));
	if ((0 != getPlot ( ).axisWidget (QwtPlot::yRight)) &&
	    (true == getPlot ( ).axisWidget (QwtPlot::yRight)->isVisible ( )))
		legendWidth	+= metricsMap.layoutToDeviceX (
						getPlot ( ).axisWidget (QwtPlot::yRight)->width ( ));
	if ((0 != getPlot ( ).axisWidget (QwtPlot::xBottom)) &&
	    (true == getPlot ( ).axisWidget (QwtPlot::xBottom)->isVisible ( )))
		legendHeight	+= metricsMap.layoutToDeviceY (
						getPlot ( ).axisWidget (QwtPlot::xBottom)->height ( ));
	if ((0 != getPlot ( ).axisWidget (QwtPlot::xTop)) &&
	    (true == getPlot ( ).axisWidget (QwtPlot::xTop)->isVisible ( )))
		legendHeight	+= metricsMap.layoutToDeviceY (
						getPlot ( ).axisWidget (QwtPlot::xTop)->height ( ));
	if ((0 != getPlot ( ).titleLabel ( )) &&
	    (false == getPlot ( ).titleLabel ( )->text ( ).isEmpty ( )))
		legendHeight	+= metricsMap.layoutToDeviceY (
						 getPlot ( ).titleLabel ( )->height ( ));
	// C'est pas encore parfait, il faudrait voir précisemment combien de
	// "marges" (espaces) il y a dans les 2 sens ...
//	legendWidth	+= metricsMap.layoutToDeviceX (2 * getPlot ( ).margin ( ));
//	legendHeight+= metricsMap.layoutToDeviceX (2 * getPlot ( ).margin ( ));
	pdwidth		-= legendWidth;
	pdheight	-= legendHeight;

	if (pdwidth > pdheight)	// Diminuer la largeur si nécessaire
	{
		pdwidth	= (int)(xRange * (double)pdheight / yRange);
	}	// if (pdwidth > pdheight)
	else
		pdheight	= (int)(yRange * (double)pdwidth / xRange);
	pdwidth		+= legendWidth;
	pdheight	+= legendHeight;
	int	x	= (docWidth - pdwidth) / 2;
	int	y	= (docHeight - pdheight) / 2;
	QRect	rect (x, y, pdwidth, pdheight);
	if (0 == image.get ( ))
		getPlot ( ).print (&painter, rect, filter);
	else
	{
		getPlot ( ).print (&painter, rect, filter);
		image->save (printer.outputFileName ( ), 0, 100);
	}	// else if (0 == image.get ( ))

	COMPLETE_TRY_CATCH_BLOCK

	if (true == hasError)
		throw Exception (errorMsg);
}	// QwtChartPanel::print

========================================================================= */


void QwtChartPanel::enableZoom (bool enable)
{
	CHECK_NULL_PTR_ERROR (_zoomer)
	if (0 != _coordsPicker)
		_coordsPicker->setEnabled (!enable);
	_zoomer->setEnabled (enable);
}	// QwtChartPanel::enableZoom


bool QwtChartPanel::zoomEnabled ( ) const
{
	CHECK_NULL_PTR_ERROR (_zoomer)
	return _zoomer->isEnabled ( );
}	// QwtChartPanel::zoomEnabled


void QwtChartPanel::zoom (QWheelEvent& event)
{
	const double	sign	= true == mouseZoomUp ? -1. : 1.;
	const bool		enabled	= zoomEnabled ( );
	enableZoom (true);

	zoom (1. + sign * (double)event.delta ( ) / 8. / 100.);

	event.accept ( );
	enableZoom (enabled);
}	// QwtChartPanel::zoom


void QwtChartPanel::zoom (double factor)
{
	const QwtScaleDiv&	absScaleDiv	= getPlot( ).axisScaleDiv(QwtPlot::xBottom);
	const QwtScaleDiv&	ordScaleDiv	= getPlot( ).axisScaleDiv(QwtPlot::yLeft);
	assert (0 != _zoomer);

	if (false == zoomEnabled ( ))
		return;

	if (1. == factor)
		return;

	// Le rectangle englobant de la future partie visible :
	QwtDoubleRect	rect	= _zoomer->zoomRect ( );
	const double	width	= rect.width ( );
	const double	height	= rect.height ( );
	const double	dx		= (factor - 1.) * width;
	const double	dy		= (factor - 1.) * height;
	rect.setLeft (rect.left ( ) - dx / 2.);
	rect.setBottom (rect.bottom ( ) - dx / 2.);
	rect.setWidth (width + dx);
	rect.setHeight (height + dy);

	// Le QwtPlotZoomer n'est pas notifié des éventuelles translations de la
	// vue, type panning (on n'est plus centré sur le milieu des courbes,
	// l'utilisateur a effectué une translation de la vue au bouton central de
	// la souris).
	// => On récupère les coordonnées du centre de la vue afin de centrer la
	// future vue sur ce même point.
	const double	xMin	= absScaleDiv.lowerBound ( );
	const double	xMax	= absScaleDiv.upperBound ( );
	const double	yMin	= ordScaleDiv.lowerBound ( );
	const double	yMax	= ordScaleDiv.upperBound ( );
	const double	x		= xMin + (xMax - xMin) / 2.;
	const double	y		= yMin + (yMax - yMin) / 2.;
	rect.moveCenter (QwtDoublePoint (x, y));
	_zoomer->zoom (rect);
}	// QwtChartPanel::zoom


void QwtChartPanel::enablePanning (bool enable)
{
	CHECK_NULL_PTR_ERROR (_panner)
	_panner->setEnabled (enable);
}	// QwtChartPanel::enablePanning


bool QwtChartPanel::panningEnabled ( ) const
{
	CHECK_NULL_PTR_ERROR (_panner)
	return _panner->isEnabled ( );
}	// QwtChartPanel::panningEnabled


bool QwtChartPanel::zoomCancelable ( ) const
{
	CHECK_NULL_PTR_ERROR (_zoomer)
	// _zoomer->zoomStack ( )[0] : situation initiale, sans zoom
	return 1 >= _zoomer->zoomStack ( ).size ( ) ? false : true;
}	// QwtChartPanel::zoomCancelable


void QwtChartPanel::cancelZoom ( )
{
	CHECK_NULL_PTR_ERROR (_zoomer)
	_zoomer->zoom (0);
	emit zoomModified (this);
}	// QwtChartPanel::cancelZoom


void QwtChartPanel::adjustScales ( )
{
	CHECK_NULL_PTR_ERROR (_zoomer)
	CHECK_NULL_PTR_ERROR (_rescaler)
	double	minx	= 0.,	maxx	= 0.;
	double	miny	= 0.,	maxy	= 0.;
	if ((false == getVisibleAbscissaScale (minx, maxx)) ||
	    (false == getVisibleOrdinateScale (miny, maxy)))
		return;

	if (true == _rescaler->isEnabled ( ))
	{
		QwtDoubleInterval	intervalx (minx, maxx);
		QwtDoubleInterval	intervaly (miny, maxy);
		_rescaler->setIntervalHint (QwtPlot::xBottom, intervalx);
		_rescaler->setIntervalHint (QwtPlot::xTop, intervalx);
		_rescaler->setIntervalHint (QwtPlot::yLeft, intervaly);
		_rescaler->setIntervalHint (QwtPlot::yRight, intervaly);
		_rescaler->rescale ( );
	}	// if (true == _rescaler->isEnabled ( ))
	else
	{
		const double	width	= maxx - minx;
		const double	height	= maxy - miny;
		QwtDoubleRect	rect (minx, miny, width, height);
		_zoomer->zoom (rect);
	}
}	// QwtChartPanel::adjustScales


bool QwtChartPanel::zoomUndoable ( ) const
{
	CHECK_NULL_PTR_ERROR (_zoomer)
	if ((1 < _zoomer->zoomStack ( ).size ( )) &&
	    (0 != _zoomer->zoomRectIndex ( )))
		return true;

	return false;
}	// QwtChartPanel::zoomUndoable


void QwtChartPanel::undoZoom ( )
{
	CHECK_NULL_PTR_ERROR (_zoomer)
	_zoomer->zoom (-1);
	emit zoomModified (this);
}	// QwtChartPanel::undoZoom ( )


bool QwtChartPanel::zoomRedoable ( ) const
{
	CHECK_NULL_PTR_ERROR (_zoomer)
	if ((1 < _zoomer->zoomStack ( ).size ( )) &&
	    (_zoomer->zoomStack ( ).size ( ) != (_zoomer->zoomRectIndex ( ) + 1)))
		return true;

	return false;
}	// QwtChartPanel::zoomRedoable


void QwtChartPanel::redoZoom ( )
{
	CHECK_NULL_PTR_ERROR (_zoomer)
	_zoomer->zoom (1);
	emit zoomModified (this);
}	// QwtChartPanel::redoZoom ( )


bool QwtChartPanel::isModified ( ) const
{
	for (vector<bool>::const_iterator	itm	= _modificationList.begin ( );
	     _modificationList.end ( ) != itm; itm++)
		if (true == *itm)
			return true;

	return false;
}	// QwtChartPanel::isModified


bool QwtChartPanel::isModified (size_t item) const
{
	if (_modificationList.size ( ) <= item)
		throw Exception ("QwtChartPanel::isModified : indice hors du domaine.");

	return _modificationList [item];
}	// QwtChartPanel::isModified


void QwtChartPanel::setModified (size_t item, bool modified)
{
	if (_modificationList.size ( ) <= item)
		throw Exception("QwtChartPanel::setModified : indice hors du domaine.");

	_modificationList [item]	= modified;
}	// QwtChartPanel::setModified


void QwtChartPanel::dataModified ( )
{
	CHECK_NULL_PTR_ERROR (_zoomer)
	getCoordinatesPicker ( ).updateFromPlot ( );
	_abscissaScale._updated	= false;
	_ordinateScale._updated	= false;
	adjustChart  ( );
	getPlot ( ).replot ( );
// Versions 0.22.0 et antérieures : si dataModified est appelé alors qu'on est
// en plein zoom, alors on perd tout zoom et on ne peut donc plus revenir (via
// les fonctions de zoom) en arrière (setZoomBase fait perdre l'historique).
//	_zoomer->setZoomBase ( );
// Versions 0.23.0 et ultérieures : on recré l'éventuelle pile de zooms. On
// donne pour rectangle initial les dimensions max du graphique.
	const QStack<QwtDoubleRect>&	stack	= _zoomer->zoomStack ( );
	QStack<QwtDoubleRect>			newStack;
	bool	as	= false;
	double	x1 = 0., x2 = 0., y1 = 0., y2 = 0.;
	getAbscissaScale (x1, x2, as);
	getOrdinateScale (y1, y2, as);
	// La nouvelle reference (etat non zoome) :
	if ((0 != isValid (x1)) && (0 != isValid (x2)) &&
	    (0 != isValid (y1)) && (0 != isValid (y2)))
		newStack.push (QwtDoubleRect (x1, y1, x2 - x1, y2 - y1));
	else
		newStack.push (QwtDoubleRect (0., 0., 1., 1.));

	// On conserve le reste de l'ancienne pile :
	for (size_t i = 1; i < stack.size ( ); i++)
		newStack.push (stack [i]);

	_zoomer->setZoomStack (newStack, _zoomer->zoomRectIndex ( ));
}	// QwtChartPanel::dataModified


void QwtChartPanel::adjustChart ( )
{
	CHECK_NULL_PTR_ERROR (_rescaler)
	double	mn	= 0.,	mx	= 0.;
	bool	as	= false;
	getAbscissaScale (mn, mx, as);
	QwtDoubleInterval	interval (mn, mx);
	_rescaler->setIntervalHint (QwtPlot::xBottom, interval);
	_rescaler->setIntervalHint (QwtPlot::xTop, interval);
	getOrdinateScale (mn, mx, as);
	interval	= QwtDoubleInterval (mn, mx);
	_rescaler->setIntervalHint (QwtPlot::yLeft, interval);
	_rescaler->setIntervalHint (QwtPlot::yRight, interval);
	if (true == _rescaler->isEnabled ( ))
		_rescaler->rescale ( );
}	// QwtChartPanel::adjustChart


void QwtChartPanel::legendCheckedCallback (QwtPlotItem* item, bool checked)
{
	assert (0 != item);
	item->setVisible (checked);
	getPlot ( ).replot ( );
}	// QwtChartPanel::legendCheckedCallback


void QwtChartPanel::legendCheckedCallback (
							const QVariant& itemInfo, bool checked, int index)
{
	QwtPlotItem*	item	= getPlot ( ).infoToItem (itemInfo);
	if (0 == item)
		return;

	legendCheckedCallback (item, checked);
}	// legendCheckedCallback


void QwtChartPanel::zoomCallback_Qwt5 (const QwtDoubleRect& r)
{
	CHECK_NULL_PTR_ERROR (_zoomer)
	double xmin	= getPlot ( ).axisScaleDiv (QwtPlot::xBottom).lowerBound ( );
	double xmax	= getPlot ( ).axisScaleDiv (QwtPlot::xBottom).upperBound ( );
	double ymin	= getPlot ( ).axisScaleDiv (QwtPlot::yLeft).lowerBound ( );
	double ymax	= getPlot ( ).axisScaleDiv (QwtPlot::yLeft).upperBound ( );
	QStack<QwtDoubleRect>	stack	= _zoomer->zoomStack ( );
	QwtDoubleRect&			rect	= stack [_zoomer->zoomRectIndex ( )];
	rect.setRect (xmin, ymin, xmax - xmin, ymax - ymin);
	_zoomer->setZoomStack (stack, _zoomer->zoomRectIndex ( ));
	if (0 != _coordsPicker)
	{
		_coordsPicker->updateFromPlot ( );
		emit zoomModified (this);
	}	// if (0 != _coordsPicker)
}	// QwtChartPanel::zoomCallback_Qwt6


void QwtChartPanel::zoomCallback_Qwt6 (const QRectF& r)
{
	zoomCallback_Qwt5 (r);
}	// QwtChartPanel::zoomCallback_Qwt6


bool QwtChartPanel::useQImage (const QPrinter& printer)
{
	if (0 == printer.outputFileName ( ).size ( ))
		return false;

	File	file (printer.outputFileName ( ).toStdString ( ));
	if ((0 == strcasecmp (file.getExtension ( ).c_str ( ), "ps")) ||
	    (0 == strcasecmp (file.getExtension ( ).c_str ( ), "pdf")))
		return false;

	return true;
}	// QwtChartPanel::useQImage


void QwtChartPanel::getImageSize (
							const QPrinter& printer, int& width, int& height)
{
/*
	const int	printerWidth	= printer.width ( );
	const int	printerHeight	= printer.height ( );
	const int	min				= printerWidth < printerHeight ?
								  printerWidth : printerHeight;
	if ((min < imageSizeMin) || (0 >= imageSizeMin))
	{
		width	= printerWidth;
		height	= printerHeight;
		return;
	}	// if ((min < imageSizeMin) || (0 >= imageSizeMin))
*/
	const int	min	= width < height ? width : height;
//if ((min < imageSizeMin) || (0 >= imageSizeMin))
	if ((min >= imageSizeMin) || (0 >= imageSizeMin))
		return;

	const double	ratio	= imageSizeMin / (double)min;
//	width	= (int)(ratio * printerWidth);
//	height	= (int)(ratio * printerHeight);
	width	= (int)(width * ratio);
	height	= (int)(height * ratio);
}	// QwtChartPanel::getImageSize


void QwtChartPanel::focusInEvent (QFocusEvent* event)
{
	BEGIN_TRY_CATCH_BLOCK
	QWidget::focusInEvent (event);

	if (true == QwtChartsManager::isInitialized ( ))
		QwtChartsManager::getManager ( ).focusChanged (this);

	COMPLETE_TRY_CATCH_BLOCK
}	// QwtChartPanel::focusInEvent


void QwtChartPanel::log (const Log& log)
{
	if (0 != _logStream)
		_logStream->log (log);
}	// QwtChartPanel::log


QwtPlot& QwtChartPanel::getPlot ( )
{
	if (0 == _plotWidget)
	{
		INTERNAL_ERROR (exc, "Absence de graphique.", "QwtChartPanel::getPlot")
		throw (exc);
	}	// if (0 == _plotWidget)

	return *_plotWidget;
}	// QwtChartPanel::getPlot


bool QwtChartPanel::useAspectRatio ( ) const
{
	CHECK_NULL_PTR_ERROR (_rescaler)
	return _rescaler->isEnabled ( );
}	// QwtChartPanel::useAspectRatio


void QwtChartPanel::useAspectRatio (bool use)
{
	CHECK_NULL_PTR_ERROR (_rescaler)
	if (useAspectRatio ( ) != use)
	{
		_rescaler->setEnabled (use);
		if (true == use)
			_rescaler->rescale ( );
	}	// if (useAspectRatio ( ) != use)
}	// QwtChartPanel::useAspectRatio


double QwtChartPanel::getAspectRatio ( ) const
{
	CHECK_NULL_PTR_ERROR (_rescaler)
	return _rescaler->aspectRatio (QwtPlot::yLeft);
}	// QwtChartPanel::getAspectRatio


void QwtChartPanel::setAspectRatio (double ar)
{
	CHECK_NULL_PTR_ERROR (_rescaler)
	if (getAspectRatio ( ) != ar)
	{
		_rescaler->setAspectRatio (ar);
		if (true == _rescaler->isEnabled ( ))
			_rescaler->rescale ( );
	}	// if (getAspectRatio ( ) != ar)
}	// QwtChartPanel::setAspectRatio


QwtPlotCoordinatesPicker::SELECTION_MODE
									QwtChartPanel::getSelectionMode ( ) const
{
	return getCoordinatesPicker ( ).getSelectionMode ( );
}	// QwtChartPanel::getSelectionMode


void QwtChartPanel::setSelectionMode (
								QwtPlotCoordinatesPicker::SELECTION_MODE mode)
{
	getCoordinatesPicker ( ).setSelectionMode (mode);
}	// QwtChartPanel::setSelectionMode


QwtPlotCurve* QwtChartPanel::getSelectedCurve ( )
{
	return getCoordinatesPicker ( ).getSelectedCurve ( );
}	// QwtChartPanel::getSelectedCurve


const QwtPlotCurve* QwtChartPanel::getSelectedCurve ( ) const
{
	return getCoordinatesPicker ( ).getSelectedCurve ( );
}	// QwtChartPanel::getSelectedCurve


void QwtChartPanel::clearSelection ( )
{
	getCoordinatesPicker ( ).clearSelection ( );
}	// QwtChartPanel::clearSelection


void QwtChartPanel::getSelectionArea (
						double& x1, double& y1, double& x2, double& y2) const
{
	return getCoordinatesPicker ( ).getSelectionArea (x1, y1, x2, y2);
}	// QwtChartPanel::getSelectionArea


void QwtChartPanel::getSelectionExtrema (
				double& xmin, double& ymin, double& xmax, double& ymax) const
{
	return getCoordinatesPicker ( ).getSelectionExtrema(xmin, ymin, xmax, ymax);
}	// QwtChartPanel::getSelectionExtrema


vector<QwtChartPanel::QwtChartEditionPanel*>
							QwtChartPanel::createEditionPanels (QWidget* parent)
{
	vector<QwtChartPanel::QwtChartEditionPanel*>	panels;

	if (0 != _specificPanelCreator)
		panels	= _specificPanelCreator->createEditionPanels (parent);

	return panels;
}	// QwtChartPanel::createEditionPanels


vector<QwtChartPanel::DataPaintAttributes>
								QwtChartPanel::getDataPaintAttributes ( ) const
{
	throw Exception ("QwtChartPanel::getDataPaintAttributes should be verloaded.");
}	// QwtChartPanel::getDataPaintAttributes


void QwtChartPanel::setDataPaintAttributes (
		size_t index, const QwtChartPanel::DataPaintAttributes& paintAttributes)
{
	throw Exception ("QwtChartPanel::setDataPaintAttributes should be verloaded.");
}	// QwtChartPanel::setDataPaintAttributes


void QwtChartPanel::setSpecificsPanelsCreator (
										QwtChartSpecificPanelCreator* creator)
{	
	delete _specificPanelCreator;
	_specificPanelCreator	= creator;
}	// QwtChartPanel::setSpecificsPanelsCreator


void QwtChartPanel::setEditionDialog (QwtChartOptionsDialog* dialog)
{
	_editionDialog	= dialog;
}	// QwtChartPanel::setEditionDialog


const QwtPlot& QwtChartPanel::getPlot ( ) const
{
	if (0 == _plotWidget)
	{
		INTERNAL_ERROR (exc, "Absence de graphique.", "QwtChartPanel::getPlot")
		throw (exc);
	}	// if (0 == _plotWidget)

	return *_plotWidget;
}	// QwtChartPanel::getPlot


void QwtChartPanel::addPlotItem (QwtPlotItem& plotItem)
{
	if (0 != getEditionDialog ( ))
	{
		QwtExtendedPlotCurve*	curve	=
							dynamic_cast<QwtExtendedPlotCurve*>(&plotItem);
		if (0 != curve)
			getEditionDialog ( )->curveAdded (curve);
	}	// if (0 != getEditionDialog ( ))

	plotItem.attach (&getPlot ( ));
	_plotItems.push_back (&plotItem);
	_modificationList.push_back (false);

	// V 1.0.1 : bouton enfoncé de la légende :
	QwtLegend*	legend	= dynamic_cast<QwtLegend*>(getPlot ( ).legend ( ));
	if (0 != legend)
	{
		QWidget*	widget	=
					legend->legendWidget (getPlot ( ).itemToInfo (&plotItem));
		QwtLegendLabel*	label	= dynamic_cast<QwtLegendLabel*>(widget);
		if (0 != label)
			label->setChecked (plotItem.isVisible ( ));
	}	// if (0 != legend)

	QwtPlotCurve*	curve	= dynamic_cast<QwtPlotCurve*>(&plotItem);
	if (0 != curve)
		adjustChart ( );
}	// QwtChartPanel::addPlotItem


void QwtChartPanel::removePlotItem (QwtPlotItem& plotItem)
{
	if (0 != getEditionDialog ( ))
	{
		QwtExtendedPlotCurve*	curve	=
							dynamic_cast<QwtExtendedPlotCurve*>(&plotItem);
		if (0 != curve)
			getEditionDialog ( )->curveRemoved (curve);
	}	// if (0 != getEditionDialog ( ))

	vector<QwtPlotItem*>::iterator	iti	= _plotItems.begin ( );
	vector<bool>::iterator			itm	= _modificationList.begin ( );
	for ( ; _plotItems.end ( ) != iti; iti++, itm++)
	{
		assert (_modificationList.end ( ) != itm);
		if (&plotItem == *iti)
		{
			_plotItems.erase (iti);
			_modificationList.erase (itm);
			return;
		}	// if (&plotItem == *iti)
	}	// for ( ; &plotItem != *iti; iti++, itm++)

	throw Exception ("QwtChartPanel::removePlotItem : série non trouvée.");
}	// QwtChartPanel::removePlotItem


QwtPlotCoordinatesPicker& QwtChartPanel::getCoordinatesPicker ( )
{
	CHECK_NULL_PTR_ERROR (_coordsPicker)
	return *_coordsPicker;
}	// QwtChartPanel::getCoordinatesPicker


const QwtPlotCoordinatesPicker& QwtChartPanel::getCoordinatesPicker ( ) const
{
	CHECK_NULL_PTR_ERROR (_coordsPicker)
	return *_coordsPicker;
}	// QwtChartPanel::getCoordinatesPicker


void QwtChartPanel::setCoordinatesPicker (QwtPlotCoordinatesPicker& picker)
{
	CHECK_NULL_PTR_ERROR (_zoomer)
	bool	enabled	= 0 == _coordsPicker ?
					  !_zoomer->isEnabled ( ) : _coordsPicker->isEnabled ( );
	delete _coordsPicker;
	_coordsPicker	= &picker;
	picker.setEnabled (enabled);

	if (true == QwtChartsManager::isInitialized ( ))
		QwtChartsManager::registerChartPanel (*this);
}	// QwtChartPanel::setCoordinatesPicker


