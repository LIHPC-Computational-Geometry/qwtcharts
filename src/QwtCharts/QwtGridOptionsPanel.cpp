#include "QwtCharts/QwtGridOptionsPanel.h"
#include "QtUtil/QtUnicodeHelper.h"
#include "QtUtil/QtConfiguration.h"
#include "TkUtil/Exception.h"
#include "TkUtil/InternalError.h"
#include "TkUtil/MemoryError.h"

#include <QLayout>
#include <QGroupBox>

#include <assert.h>


USING_UTIL
USING_STD


QwtGridOptionsPanel::QwtGridOptionsPanel (
									QWidget* parent, QwtChartPanel* chartPanel)
	: QwtChartEditionPanel (parent, chartPanel),
	  _chartPanel (chartPanel),
	  _horMajCheckbox (0), _horMinCheckbox (0),
	  _verMajCheckbox (0), _verMinCheckbox (0),
	  _majorPenPanel (0), _minorPenPanel (0)
{

	QHBoxLayout*	mainLayout	= new QHBoxLayout (this);
	mainLayout->setSizeConstraint (QLayout::SetMinimumSize);

	QVBoxLayout*	vLayout	= new QVBoxLayout ( );
	mainLayout->addLayout (vLayout);
	_horMajCheckbox	= new QCheckBox ("Lignes horizontales majeures", this);
	vLayout->addWidget (_horMajCheckbox);
	_horMinCheckbox	= new QCheckBox ("Lignes horizontales mineures", this);
	vLayout->addWidget (_horMinCheckbox);
	_verMajCheckbox	= new QCheckBox ("Lignes verticales majeures", this);
	vLayout->addWidget (_verMajCheckbox);
	_verMinCheckbox	= new QCheckBox ("Lignes verticales mineures", this);
	vLayout->addWidget (_verMinCheckbox);
	if (0 != chartPanel)
	{
		_horMajCheckbox->setChecked (chartPanel->areHorMajorLinesDisplayed ( ));
		_horMinCheckbox->setChecked (chartPanel->areHorMinorLinesDisplayed ( ));
		_verMajCheckbox->setChecked (chartPanel->areVerMajorLinesDisplayed ( ));
		_verMinCheckbox->setChecked (chartPanel->areVerMinorLinesDisplayed ( ));
	}	// if (0 != chartPanel)

	// REM : si grid->majPen ( ).width ( ) est nul on prend 1 pour valeur.
	// cf. doc Qt sur setWidth : une valeur de 0 vaut en fait 1 mais est
	// indépendant d'une éventuelle transformation appliquée au painter.
	// Ici c'est le même traitement dans tous les cas.
	vLayout	= new QVBoxLayout ( );
	mainLayout->addLayout (vLayout);
	QGroupBox*	group	= new QGroupBox ("Lignes majeures", this);
	QVBoxLayout*	groupLayout	= new QVBoxLayout ( );
	group->setLayout (groupLayout);
	vLayout->addWidget (group);
	QwtPlotGrid*	grid	= 0 != chartPanel ? chartPanel->getGrid ( ) : 0;
	QColor	color	= 0 != grid ? grid->majorPen ( ).color ( ) : Qt::black;
	uint	width	= 0 != grid ? grid->majorPen ( ).width ( ) : 1;
	_majorPenPanel	=
		new QtPenPanel (group, Qt::Vertical, false, color, 
		                0 == width ? 1 : width);
	_majorPenPanel->setStyle (
					0 != grid ? grid->majorPen ( ).style ( ) : Qt::SolidLine);
	groupLayout->addWidget (_majorPenPanel);

	group	= new QGroupBox ("Lignes mineures", this);
	groupLayout	= new QVBoxLayout ( );
	group->setLayout (groupLayout);
	vLayout->addWidget (group);
	color	= 0 != grid ? grid->minorPen ( ).color ( ) : Qt::black;
	width	= 0 != grid ? grid->minorPen ( ).width ( ) : 1;
	_minorPenPanel	=
		new QtPenPanel (group, Qt::Vertical, false, color,
		                0 == width ? 1 : width);
	_minorPenPanel->setStyle (
					0 != grid ? grid->minorPen ( ).style ( ) : Qt::SolidLine);
	groupLayout->addWidget (_minorPenPanel);

	setLayout (mainLayout);
}	// QwtGridOptionsPanel::QwtGridOptionsPanel


QwtGridOptionsPanel::QwtGridOptionsPanel (const QwtGridOptionsPanel& view)
	: QwtChartPanel::QwtChartEditionPanel (0, 0), _chartPanel (0),
	  _horMajCheckbox (0), _horMinCheckbox (0),
	  _verMajCheckbox (0), _verMinCheckbox (0)
{
	assert (0 && "QwtGridOptionsPanel copy constructor is not allowed.");
}	// QwtGridOptionsPanel::QwtGridOptionsPanel (const QwtGridOptionsPanel& view)


QwtGridOptionsPanel& QwtGridOptionsPanel::operator = (const QwtGridOptionsPanel& view)
{
	assert (0 && "QwtGridOptionsPanel::operator = is not allowed.");
	return *this;
}	// QwtGridOptionsPanel::QwtGridOptionsPanel (const QwtGridOptionsPanel& view)


QwtGridOptionsPanel::~QwtGridOptionsPanel ( )
{
}	// QwtGridOptionsPanel::~QwtGridOptionsPanel


bool QwtGridOptionsPanel::areHorMajorLinesDisplayed ( ) const
{
	assert (0 != _horMajCheckbox);
	return _horMajCheckbox->isChecked ( );
}	// QwtGridOptionsPanel::areHorMajorLinesDisplayed


void QwtGridOptionsPanel::displayHorMajorLines (bool display) 
{
	assert (0 != _horMajCheckbox);
	_horMajCheckbox->setChecked (display);
}	// QwtGridOptionsPanel::displayHorMajorLines


bool QwtGridOptionsPanel::areHorMinorLinesDisplayed ( ) const
{
	assert (0 != _horMinCheckbox);
	return _horMinCheckbox->isChecked ( );
}	// QwtGridOptionsPanel::areHorMinorLinesDisplayed


void QwtGridOptionsPanel::displayHorMinorLines (bool display) 
{
	assert (0 != _horMinCheckbox);
	_horMinCheckbox->setChecked (display);
}	// QwtGridOptionsPanel::displayHorMinorLines


bool QwtGridOptionsPanel::areVerMajorLinesDisplayed ( ) const
{
	assert (0 != _verMajCheckbox);
	return _verMajCheckbox->isChecked ( );
}	// QwtGridOptionsPanel::areVerMajorLinesDisplayed


void QwtGridOptionsPanel::displayVerMajorLines (bool display) 
{
	assert (0 != _verMajCheckbox);
	_verMajCheckbox->setChecked (display);
}	// QwtGridOptionsPanel::displayVerMajorLines


bool QwtGridOptionsPanel::areVerMinorLinesDisplayed ( ) const
{
	assert (0 != _verMinCheckbox);
	return _verMinCheckbox->isChecked ( );
}	// QwtGridOptionsPanel::areVerMinorLinesDisplayed


void QwtGridOptionsPanel::displayVerMinorLines (bool display) 
{
	assert (0 != _verMinCheckbox);
	_verMinCheckbox->setChecked (display);
}	// QwtGridOptionsPanel::displayVerMinorLines


QPen QwtGridOptionsPanel::getMajorPen ( ) const
{
	assert (0 != _majorPenPanel);
	return _majorPenPanel->getPen ( );
}	// QwtGridOptionsPanel::getMajorPen


void QwtGridOptionsPanel::setMajorPen (const QPen& pen)
{
	assert (0 != _majorPenPanel);
	_majorPenPanel->setPen (pen);
}	// QwtGridOptionsPanel::setMajorPen


QPen QwtGridOptionsPanel::getMinorPen ( ) const
{
	assert (0 != _minorPenPanel);
	return _minorPenPanel->getPen ( );
}	// QwtGridOptionsPanel::getMinorPen


void QwtGridOptionsPanel::setMinorPen (const QPen& pen)
{
	assert (0 != _minorPenPanel);
	_minorPenPanel->setPen (pen);
}	// QwtGridOptionsPanel::setMinorPen


void QwtGridOptionsPanel::apply ( )
{
	if (true == hasChartPanel ( ))
	{	
		getChartPanel ( ).displayHorMajorLines (areHorMajorLinesDisplayed ( ));
		getChartPanel ( ).displayHorMinorLines (areHorMinorLinesDisplayed ( ));
		getChartPanel ( ).displayVerMajorLines (areVerMajorLinesDisplayed ( ));
		getChartPanel ( ).displayVerMinorLines (areVerMinorLinesDisplayed ( ));
		QwtPlotGrid*	grid	= getChartPanel ( ).getGrid ( );
		if (0 != grid)
		{
			grid->setMajorPen (getMajorPen ( ));
			grid->setMinorPen (getMinorPen ( ));
		}	// if (0 != grid)
	}	// if (true == hasChartPanel ( ))
}	// QwtGridOptionsPanel::apply


QwtChartPanel& QwtGridOptionsPanel::getChartPanel ( )
{
	if (0 == _chartPanel)
	{
		INTERNAL_ERROR (exc, "Absence de panneau graphique associé.", "QwtGridOptionsPanel::getChartPanel")
		throw (exc);
	}	// if (0 == _chartPanel)

	return *_chartPanel;
}	// QwtGridOptionsPanel::getChartPanel


const QwtChartPanel& QwtGridOptionsPanel::getChartPanel ( ) const
{
	if (0 == _chartPanel)
	{
		INTERNAL_ERROR (exc, "Absence de panneau graphique associé.", "QwtGridOptionsPanel::getChartPanel")
		throw (exc);
	}	// if (0 == _chartPanel)

	return *_chartPanel;
}	// QwtGridOptionsPanel::getChartPanel



