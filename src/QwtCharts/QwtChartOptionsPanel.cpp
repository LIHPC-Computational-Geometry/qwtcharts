#include "QwtCharts/QwtChartOptionsPanel.h"
#include <QtUtil/QtUnicodeHelper.h>
#include <QtUtil/QtConfiguration.h>
#include <TkUtil/Exception.h>
#include <TkUtil/InternalError.h>
#include <TkUtil/MemoryError.h>

#include <QVBoxLayout>

#include <qwt_scale_widget.h>

#include <QtUtil/CharToLatinQString.h>


#include <assert.h>


USING_UTIL
USING_STD

static const TkUtil::Charset	charset ("àéèùô");

USE_ENCODING_AUTODETECTION


QwtChartOptionsPanel::QwtChartOptionsPanel (
									QWidget* parent, QwtChartPanel* chartPanel)
	: QWidget (parent),
	  _tabWidget (0), _chartPanel (chartPanel),
	  _generalOptionsPanel (0), _axisOptionsPanel (0), _gridOptionsPanel (0),
	  _legendOptionsPanel (0), _curvesOptionsPanel (0), _editionPanels ( )
{
	QVBoxLayout*	layout	= new QVBoxLayout (this);
	layout->setSizeConstraint (QLayout::SetMinimumSize);
	setLayout (layout);

	// Onglets :
	_tabWidget			= new QTabWidget (this);
	layout->addWidget (_tabWidget);

	// Options générales :
	_generalOptionsPanel	=
					new QwtChartGeneralOptionsPanel (_tabWidget, _chartPanel);
	_tabWidget->addTab (_generalOptionsPanel, QSTR ("Général"));
	_editionPanels.push_back (_generalOptionsPanel);

	// Axes :
	_axisOptionsPanel	= new QwtAxisOptionsPanel (_tabWidget, _chartPanel);
	_tabWidget->addTab (_axisOptionsPanel, "Axes");
	_editionPanels.push_back (_axisOptionsPanel);

	// Grille :
	_gridOptionsPanel	= new QwtGridOptionsPanel (_tabWidget, _chartPanel);
	_tabWidget->addTab (_gridOptionsPanel, "Grille");
	_editionPanels.push_back (_gridOptionsPanel);

	// Légende :
	_legendOptionsPanel	= new QwtLegendOptionsPanel (_tabWidget, _chartPanel);
	_tabWidget->addTab (_legendOptionsPanel, QSTR ("Légende"));
	_editionPanels.push_back (_legendOptionsPanel);

	// Courbes :
	_curvesOptionsPanel	= new QwtCurvesOptionsPanel (_tabWidget, _chartPanel);
	_tabWidget->addTab (_curvesOptionsPanel, "Courbes");
	_editionPanels.push_back (_curvesOptionsPanel);

	// Panneaux spécifiques au graphique représenté :
	if (0 != _chartPanel)
	{
		vector<QwtChartPanel::QwtChartEditionPanel*>	specificPanels	 =
								_chartPanel->createEditionPanels  (_tabWidget);
		for (vector<QwtChartPanel::QwtChartEditionPanel*>::iterator
				it = specificPanels.begin ( ); specificPanels.end ( ) != it;
				it++)
			_tabWidget->addTab (*it, (*it)->windowTitle ( ));
		_editionPanels.insert (_editionPanels.end ( ), specificPanels.begin ( ),
		                       specificPanels.end ( ));
	}	// if (0 != _chartPanel)
}	// QwtChartOptionsPanel::QwtChartOptionsPanel


QwtChartOptionsPanel::QwtChartOptionsPanel (const QwtChartOptionsPanel& view)
	: QWidget (0), _tabWidget (0), _chartPanel (0), 
	  _generalOptionsPanel (0), _axisOptionsPanel (0), _gridOptionsPanel (0),
	   _legendOptionsPanel (0)
{
	assert (0 && "QwtChartOptionsPanel copy constructor is not allowed.");
}	// QwtChartOptionsPanel::QwtChartOptionsPanel (const QwtChartOptionsPanel& view)


QwtChartOptionsPanel& QwtChartOptionsPanel::operator = (
											const QwtChartOptionsPanel& view)
{
	assert (0 && "QwtChartOptionsPanel::operator = is not allowed.");
	return *this;
}	// QwtChartOptionsPanel::QwtChartOptionsPanel (const QwtChartOptionsPanel& view)


QwtChartOptionsPanel::~QwtChartOptionsPanel ( )
{
}	// QwtChartOptionsPanel::~QwtChartOptionsPanel


void QwtChartOptionsPanel::apply ( )
{
/*
	getGeneralPanel ( ).apply ( );
	getAxisPanel ( ).apply ( );
	getGridPanel ( ).apply ( );
	getLegendPanel ( ).apply ( );
	getCurvesPanel ( ).apply ( );

	// Panneaux spécifiques au graphique représenté :
	if (0 != _chartPanel)
	{
		for (vector<QwtChartPanel::QwtChartEditionPanel*>::iterator
				it = _editionPanels.begin ( ); _editionPanels.end ( ) != it;
				it++)
			(*it)->apply ( );
	}	// if (0 != _chartPanel)
*/
	getCurrentPanel ( ).apply ( );

	if (0 != _chartPanel)
	{
		// Si un axe est modifié Qwt 5.0.2 ne le remet pas à jour de lui-même avec
		// replot => on l'aide un peu ...
		for (int i = QwtPlot::yLeft; i < QwtPlot::axisCnt; i++)
			if (0 != _chartPanel->getPlot ( ).axisWidget (i))
				_chartPanel->getPlot ( ).axisWidget (i)->update ( );
		_chartPanel->getPlot ( ).replot ( );
	}	// if (0 != _chartPanel)
}	// QwtChartOptionsPanel::apply


QwtChartPanel& QwtChartOptionsPanel::getChartPanel ( )
{
	if (0 == _chartPanel)
	{
		INTERNAL_ERROR (exc, "Absence de panneau graphique associé.", "QwtChartOptionsPanel::getChartPanel")
		throw (exc);
	}	// if (0 == _chartPanel)

	return *_chartPanel;
}	// QwtChartOptionsPanel::getChartPanel


const QwtChartPanel& QwtChartOptionsPanel::getChartPanel ( ) const
{
	if (0 == _chartPanel)
	{
		INTERNAL_ERROR (exc, "Absence de panneau graphique associé.", "QwtChartOptionsPanel::getChartPanel")
		throw (exc);
	}	// if (0 == _chartPanel)

	return *_chartPanel;
}	// QwtChartOptionsPanel::getChartPanel


QwtChartPanel::QwtChartEditionPanel& QwtChartOptionsPanel::getCurrentPanel ( )
{
	const int	index	= getTabWidget ( ).currentIndex ( );

	if (index >= _editionPanels.size ( ))
	{
		INTERNAL_ERROR (exc, "Erreur de gestion des panneaux d'édition des paramètres du graphique.", "QwtChartOptionsPanel::getCurrentPanel")
		throw exc;
	}	// if (index >= _editionPanels.size ( ))

	return *_editionPanels [index];
}	// QwtChartOptionsPanel::getCurrentPanel


const QwtChartPanel::QwtChartEditionPanel&
								QwtChartOptionsPanel::getCurrentPanel ( ) const
{
	const int	index	= getTabWidget ( ).currentIndex ( );

	if (index >= _editionPanels.size ( ))
	{
		INTERNAL_ERROR (exc, "Erreur de gestion des panneaux d'édition des paramètres du graphique.", "QwtChartOptionsPanel::getCurrentPanel")
		throw exc;
	}	// if (index >= _editionPanels.size ( ))

	return *_editionPanels [index];
}	// QwtChartOptionsPanel::getCurrentPanel


QwtChartGeneralOptionsPanel& QwtChartOptionsPanel::getGeneralPanel ( )
{
	assert (0 != _generalOptionsPanel);
	return *_generalOptionsPanel;
}	// QwtChartOptionsPanel::getGeneralPanel


const QwtChartGeneralOptionsPanel& QwtChartOptionsPanel::getGeneralPanel ( ) const
{
	assert (0 != _generalOptionsPanel);
	return *_generalOptionsPanel;
}	// QwtChartOptionsPanel::getGeneralPanel


QwtAxisOptionsPanel& QwtChartOptionsPanel::getAxisPanel ( )
{
	assert (0 != _axisOptionsPanel);
	return *_axisOptionsPanel;
}	// QwtChartOptionsPanel::getAxisPanel


const QwtAxisOptionsPanel& QwtChartOptionsPanel::getAxisPanel ( ) const
{
	assert (0 != _axisOptionsPanel);
	return *_axisOptionsPanel;
}	// QwtChartOptionsPanel::getAxisPanel


QwtGridOptionsPanel& QwtChartOptionsPanel::getGridPanel ( )
{
	assert (0 != _gridOptionsPanel);
	return *_gridOptionsPanel;
}	// QwtChartOptionsPanel::getGridPanel


const QwtGridOptionsPanel& QwtChartOptionsPanel::getGridPanel ( ) const
{
	assert (0 != _gridOptionsPanel);
	return *_gridOptionsPanel;
}	// QwtChartOptionsPanel::getGridPanel


QwtLegendOptionsPanel& QwtChartOptionsPanel::getLegendPanel ( )
{
	assert (0 != _legendOptionsPanel);
	return *_legendOptionsPanel;
}	// QwtChartOptionsPanel::getLegendPanel


const QwtLegendOptionsPanel& QwtChartOptionsPanel::getLegendPanel ( ) const
{
	assert (0 != _legendOptionsPanel);
	return *_legendOptionsPanel;
}	// QwtChartOptionsPanel::getLegendPanel


QwtCurvesOptionsPanel& QwtChartOptionsPanel::getCurvesPanel ( )
{
	assert (0 != _curvesOptionsPanel);
	return *_curvesOptionsPanel;
}	// QwtChartOptionsPanel::getCurvesPanel


const QwtCurvesOptionsPanel& QwtChartOptionsPanel::getCurvesPanel ( ) const
{
	assert (0 != _curvesOptionsPanel);
	return *_curvesOptionsPanel;
}	// QwtChartOptionsPanel::getCurvesPanel


void QwtChartOptionsPanel::curveAdded (QwtExtendedPlotCurve* curve)
{
	CHECK_NULL_PTR_ERROR (curve)

	// On notifie les panneaux d'édition du graphique représenté :
	if (0 != _chartPanel)
	{
		for (vector<QwtChartPanel::QwtChartEditionPanel*>::iterator
				it = _editionPanels.begin ( ); _editionPanels.end ( ) != it;
				it++)
			(*it)->curveAdded (curve);
	}	// if (0 != _chartPanel)
}	// QwtChartOptionsPanel::curveAdded


void QwtChartOptionsPanel::curveRemoved (QwtExtendedPlotCurve* curve)
{
	CHECK_NULL_PTR_ERROR (curve)

	// On notifie les panneaux d'édition du graphique représenté :
	if (0 != _chartPanel)
	{
		for (vector<QwtChartPanel::QwtChartEditionPanel*>::iterator
				it = _editionPanels.begin ( ); _editionPanels.end ( ) != it;
				it++)
			(*it)->curveRemoved (curve);
	}	// if (0 != _chartPanel)
}	// QwtChartOptionsPanel::curveRemoved

QTabWidget& QwtChartOptionsPanel::getTabWidget ( )
{
	assert (0 != _tabWidget);
	return *_tabWidget;
}	// QwtChartOptionsPanel::getTabWidget


const QTabWidget& QwtChartOptionsPanel::getTabWidget ( ) const
{
	assert (0 != _tabWidget);
	return *_tabWidget;
}	// QwtChartOptionsPanel::getTabWidget



