#include "QwtCharts/QwtCurvesOptionsPanel.h"
#include "QwtCharts/QwtExtendedPlotCurve.h"
#include <QtUtil/QtUnicodeHelper.h>
#include <QtUtil/QtConfiguration.h>
#include <QtUtil/QtPaintAttributes.h>
#include <TkUtil/Exception.h>
#include <TkUtil/InternalError.h>
#include <TkUtil/MemoryError.h>

#include <QColorDialog>
#include <QLabel>
#include <QLayout>
#include <QGroupBox>

#include <qwt_plot_legenditem.h>

#include <assert.h>


USING_UTIL
USING_STD

static const Charset	charset ("àéèùô");
USE_ENCODING_AUTODETECTION

QwtCurvesOptionsPanel::QwtCurvesOptionsPanel (
									QWidget* parent, QwtChartPanel* chartPanel)
	: QwtChartPanel::QwtChartEditionPanel (parent, chartPanel),
	  _chartPanel (chartPanel), _curvesList (0),
	  _penPanel (0)
{
	QVBoxLayout*	mainLayout	= new QVBoxLayout (this);
	mainLayout->setSizeConstraint (QLayout::SetMinimumSize);

	// Courbes :
	QGroupBox*		group	= new QGroupBox ("Courbes", this);
	QVBoxLayout*	vLayout	= new QVBoxLayout ( );
	group->setLayout (vLayout);
	mainLayout->addWidget (group);
	_curvesList	= new QListWidget (group);
	_curvesList->setSelectionMode (QListWidget::ExtendedSelection);
	vLayout->addWidget (_curvesList);

	// Traits :
	group	= new QGroupBox ("Traits", this);
	vLayout	= new QVBoxLayout ( );
	group->setLayout (vLayout);
	mainLayout->addWidget (group);
	_penPanel	= new QtPenPanel (
							group, Qt::Horizontal, true,
							QColor (0, 0, 0).convertTo(QColor::Invalid), 1);
	vLayout->addWidget (_penPanel);

	// Initialisation :
	vector<QwtChartPanel::DataPaintAttributes>	pa	=
								getChartPanel ( ).getDataPaintAttributes ( );
	bool						inited	= false;
	QColor						penColor=
									QColor (0, 0, 0).convertTo(QColor::Invalid);
	uint						penWidth	= 0;
	Qt::PenStyle				penStyle	= Qt::SolidLine;
	for (vector<QwtChartPanel::DataPaintAttributes>::iterator it = pa.begin ( );
	     pa.end ( ) != it; it++)
	{
		_curvesList->addItem ((*it)._name.c_str ( ));
		_curves.push_back ((*it)._plotItem);
		QtPaintAttributes	qpa ((*it)._pen);

		if (false == inited)
		{
			penColor	= qpa.getPen ( ).color ( );
			penWidth	= qpa.getPen ( ).width ( );
			penStyle	= qpa.getPen ( ).style ( );
				inited	= true;
		}	// if (false == inited)
		else
		{
			penColor	= qpa.getPen ( ).color ( ) == penColor ?
					penColor : QColor (0, 0, 0).convertTo(QColor::Invalid);
			penWidth	= qpa.getPen ( ).width ( ) == penWidth ?
						  penWidth : (uint)-1;
			penStyle	= qpa.getPen ( ).style ( ) == penStyle ?
						  penStyle : (Qt::PenStyle)-1;
		}	// else if (false == inited)
		
	}	// for (vector<QwtChartPanel::DataPaintAttributes>::iterator it = ...
/*
	const vector<QwtPlotItem*>	items	= getChartPanel ( ).getPlotItems ( );
	bool						inited	= false;
	QColor						penColor=
									QColor (0, 0, 0).convertTo(QColor::Invalid);
	uint						penWidth	= 0;
	Qt::PenStyle				penStyle	= Qt::SolidLine;
	for (vector<QwtPlotItem*>::const_iterator it = items.begin ( );
	     items.end ( ) != it; it++)
	{
		QwtExtendedPlotCurve*	curveItem	=
									dynamic_cast<QwtExtendedPlotCurve*>(*it);
		if (0 != curveItem)
		{
			_curves.push_back (curveItem);
			_curvesList->addItem (curveItem->title ( ).text ( ));

			if (false == inited)
			{
				penColor	= curveItem->pen ( ).color ( );
				penWidth	= curveItem->pen ( ).width ( );
				penStyle	= curveItem->pen ( ).style ( );
				inited	= true;
			}	// if (false == inited)
			else
			{
				penColor	= curveItem->pen ( ).color ( ) == penColor ?
						penColor : QColor (0, 0, 0).convertTo(QColor::Invalid);
				penWidth	= curveItem->pen ( ).width ( ) == penWidth ?
							  penWidth : (uint)-1;
				penStyle	= curveItem->pen ( ).style ( ) == penStyle ?
							  penStyle : (Qt::PenStyle)-1;
			}	// else if (false == inited)
		}	// if (0 != curveItem)
	}	// for (vector<QwtPlotItem*>::const_iterator it = items.begin ( ); ...
*/	// v 0.47.0
	_penPanel->setColor (penColor);
	_penPanel->setWidth (penWidth);
	_penPanel->setStyle (penStyle);
	
	setLayout (mainLayout);
}	// QwtCurvesOptionsPanel::QwtCurvesOptionsPanel


QwtCurvesOptionsPanel::QwtCurvesOptionsPanel (
									const QwtCurvesOptionsPanel& view)
	: QwtChartPanel::QwtChartEditionPanel (0, 0),
	  _chartPanel (0), _curvesList (0)
{
	assert (0 && "QwtCurvesOptionsPanel copy constructor is not allowed.");
}	// QwtCurvesOptionsPanel::QwtCurvesOptionsPanel (const QwtCurvesOptionsPanel& view)


QwtCurvesOptionsPanel& QwtCurvesOptionsPanel::operator = (
										const QwtCurvesOptionsPanel& view)
{
	assert (0 && "QwtCurvesOptionsPanel::operator = is not allowed.");
	return *this;
}	// QwtCurvesOptionsPanel::QwtCurvesOptionsPanel (const QwtCurvesOptionsPanel& view)


QwtCurvesOptionsPanel::~QwtCurvesOptionsPanel ( )
{
}	// QwtCurvesOptionsPanel::~QwtCurvesOptionsPanel


void QwtCurvesOptionsPanel::apply ( )
{
	CHECK_NULL_PTR_ERROR (_curvesList)
	CHECK_NULL_PTR_ERROR (_penPanel)

	const QColor		penColor	= _penPanel->getColor ( );
	const uint			penWidth	= _penPanel->getWidth ( );
	const Qt::PenStyle	penStyle	= _penPanel->getStyle ( );
	QtPaintAttributes	qpa (QPen (penColor, penWidth, penStyle));
	QwtChartPanel::DataPaintAttributes	pa (0, qpa.getPaintAttributes ( ));
	if (false == penColor.isValid ( ))
		pa._fgValid	= false;
	QwtLegend*			legend		=
		dynamic_cast<QwtLegend*>(getChartPanel ( ).getPlot ( ).legend ( ));

	int		row		= 0;
	for (vector<QwtPlotItem*>::iterator it = _curves.begin ( );
	     _curves.end ( ) != it; it++, row++)
	{
		if ((0 == _curvesList->item (row)) ||
		    (false == _curvesList->item (row)->isSelected ( )))
			continue;

		getChartPanel ( ).setDataPaintAttributes (row, pa);
	}	// for (vector<QwtPlotItem*>::const_iterator it = ...
}	// QwtCurvesOptionsPanel::apply


void QwtCurvesOptionsPanel::curveAdded (QwtExtendedPlotCurve* curve)
{
	CHECK_NULL_PTR_ERROR (curve)
	CHECK_NULL_PTR_ERROR (_curvesList)
	if (_curves.size ( ) != _curvesList->count ( ))
	{
		INTERNAL_ERROR (exc, "Erreur de gestion des listes de courbes.", "QwtCurvesOptionsPanel::curveAdded")
		throw exc;
	}	// if (_curves.size ( ) != _curvesList->count ( ))

	for (vector<QwtPlotItem*>::const_iterator it = _curves.begin ( );
	     _curves.end ( ) != it; it++)
		if (*it == curve)
		{
			UTF8String	msg (charset);
			msg << "La courbe " << curve->title ( ).text ( ).toStdString ( )
			    << " est déjà enregistrée par le panneau graphique d'édition "
			    << "des paramètres de représentation des courbes.";
			throw Exception (msg);
		}	// if (*it == curve)

	_curves.push_back (curve);
	_curvesList->addItem (curve->title ( ).text ( ));
}	// QwtCurvesOptionsPanel::curveAdded


void QwtCurvesOptionsPanel::curveRemoved (QwtExtendedPlotCurve* curve)
{
	CHECK_NULL_PTR_ERROR (curve)
	CHECK_NULL_PTR_ERROR (_curvesList)
	if (_curves.size ( ) != _curvesList->count ( ))
	{
		INTERNAL_ERROR (exc, "Erreur de gestion des listes de courbes.", "QwtCurvesOptionsPanel::curveRemoved")
		throw exc;
	}	// if (_curves.size ( ) != _curvesList->count ( ))

	int	row	= 0;
	for (vector<QwtPlotItem*>::iterator it = _curves.begin ( );
	     _curves.end ( ) != it; it++, row)
	{
		if (*it == curve)
		{
			QListWidgetItem*	item	= _curvesList->takeItem (row);
			delete item;
			_curves.erase (it);
			return;
		}	// if (*it == curve)
	}	// for (vector<QwtPlotItem*>::const_iterator it = items.begin ( ); ...

	UTF8String	msg (charset);
	msg << "La courbe " << curve->title ( ).text ( ).toStdString ( )
	    << " n'est pas enregistrée par le panneau graphique d'édition "
	    << "des paramètres de représentation des courbes.";
	throw Exception (msg);
}	// QwtCurvesOptionsPanel::curveRemoved


QwtChartPanel& QwtCurvesOptionsPanel::getChartPanel ( )
{
	if (0 == _chartPanel)
	{
		INTERNAL_ERROR (exc, "Absence de panneau graphique associé.", "QwtCurvesOptionsPanel::getChartPanel")
		throw (exc);
	}	// if (0 == _chartPanel)

	return *_chartPanel;
}	// QwtCurvesOptionsPanel::getChartPanel


const QwtChartPanel& QwtCurvesOptionsPanel::getChartPanel ( ) const
{
	if (0 == _chartPanel)
	{
		INTERNAL_ERROR (exc, "Absence de panneau graphique associé.", "QwtCurvesOptionsPanel::getChartPanel")
		throw (exc);
	}	// if (0 == _chartPanel)

	return *_chartPanel;
}	// QwtCurvesOptionsPanel::getChartPanel



