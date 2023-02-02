#include "QwtCharts/QwtLegendOptionsPanel.h"
#include <QtUtil/QtUnicodeHelper.h>
#include <QtUtil/QtConfiguration.h>
#include <TkUtil/Exception.h>
#include <TkUtil/InternalError.h>
#include <TkUtil/MemoryError.h>

#include <qwt_scale_widget.h>

#include <QButtonGroup>
#include <QLayout>
#include <QGroupBox>
#include <QLabel>

#include <assert.h>


USING_UTIL
USING_STD

static const TkUtil::Charset	charset ("àéèùô");

USE_ENCODING_AUTODETECTION


QwtLegendOptionsPanel::QwtLegendOptionsPanel (
									QWidget* parent, QwtChartPanel* chartPanel)
	: QwtChartPanel::QwtChartEditionPanel (parent, chartPanel),
	  _chartPanel (chartPanel),
	  _showLegendCheckbox (0), _showSelectionLegendCheckbox (0),
	  _itemsCheckableCheckbox (0),
	  _frameWidthTextField (0), _marginTextField (0), _curveWidthTextField (0),
	  _leftButton (0), _rightButton (0), _topButton (0), _bottomButton (0)
{
	QVBoxLayout*	mainLayout	= new QVBoxLayout (this);
	mainLayout->setSizeConstraint (QLayout::SetMinimumSize);

	// Affichage / Position :
	QHBoxLayout*	hlayout	= new QHBoxLayout ( );
	mainLayout->addLayout (hlayout);
	QVBoxLayout*	vlayout	= new QVBoxLayout ( );
	hlayout->addLayout (vlayout);
	_showLegendCheckbox	= new QCheckBox ("Afficher", this);
	_showLegendCheckbox->setChecked (
				0 == _chartPanel ? false : _chartPanel->hasLegend ( ));
	vlayout->addWidget (_showLegendCheckbox);
	_itemsCheckableCheckbox	=
				new QCheckBox (QSTR ("Eléments cochables"), this);
	_itemsCheckableCheckbox->setChecked (
				0 == _chartPanel ? false : _chartPanel->isLegendCheckable ( ));
	vlayout->addWidget (_itemsCheckableCheckbox);
	_showSelectionLegendCheckbox	=	new QCheckBox (
					QSTR ("Afficher la légende de sélection"), this);
	_showSelectionLegendCheckbox->setChecked (
									QwtChartPanel::displaySelectionLegend);
	vlayout->addWidget (_showSelectionLegendCheckbox);
	vlayout->addStretch (2);
	QGroupBox*		group		= new QGroupBox ("Position", this);
	QGridLayout*	gridLayout	= new QGridLayout ( );
	group->setLayout (gridLayout);
	hlayout->addWidget (group);
	QButtonGroup*	positionButtonGroup	= new QButtonGroup ( );
	positionButtonGroup->setExclusive (true);
	_leftButton	= new QRadioButton ("Gauche", group);
	gridLayout->addWidget (_leftButton, 0, 0);
	_rightButton	= new QRadioButton ("Droite", group);
	gridLayout->addWidget (_rightButton, 1, 0);
	_topButton	= new QRadioButton ("Haut", group);
	gridLayout->addWidget (_topButton, 0, 1);
	_bottomButton	= new QRadioButton ("Bas", group);
	gridLayout->addWidget (_bottomButton, 1, 1);

	// Bordure :
	group	= new QGroupBox ("Bordure", this);
	hlayout	= new QHBoxLayout ( );
	mainLayout->addWidget (group);
	group->setLayout (hlayout);
	QLabel*	label	= new QLabel ("Epaisseur :", group);
	hlayout->addWidget (label);
	_frameWidthTextField	=
		new QtIntTextField (
			0 == _chartPanel ? 1 : _chartPanel->getLegendFrameWidth ( ), group);
	_frameWidthTextField->setRange (0, 50);
	hlayout->addWidget (_frameWidthTextField);

	// Marge :
	group	= new QGroupBox ("Marge", this);
	hlayout	= new QHBoxLayout ( );
	mainLayout->addWidget (group);
	group->setLayout (hlayout);
	label	= new QLabel ("Marge :", group);
	hlayout->addWidget (label);
	_marginTextField	=
		new QtIntTextField (
			0 == _chartPanel ? 1 : _chartPanel->getLegendMargin ( ), group);
	_marginTextField->setRange (0, 20);
	hlayout->addWidget (_marginTextField);

	// Epaisseur des courbes :
	group	= new QGroupBox ("Courbes", this);
	hlayout	= new QHBoxLayout ( );
	mainLayout->addWidget (group);
	group->setLayout (hlayout);
	label	= new QLabel ("Epaisseur :", group);
	hlayout->addWidget (label);
	_curveWidthTextField	=
		new QtIntTextField (
			0 == _chartPanel ? 1 : _chartPanel->getLegendCurveWidth ( ), group);
	_curveWidthTextField->setRange (0, 20);
	hlayout->addWidget (_curveWidthTextField);
	
	mainLayout->addStretch (2);
	setLayout (mainLayout);

	setLegendPosition (0 == _chartPanel ?
	                   QwtPlot::RightLegend : _chartPanel->getLegendPosition());
}	// QwtLegendOptionsPanel::QwtLegendOptionsPanel


QwtLegendOptionsPanel::QwtLegendOptionsPanel (const QwtLegendOptionsPanel& view)
	: QwtChartPanel::QwtChartEditionPanel (0, 0),
	  _chartPanel (0),
	  _showLegendCheckbox (0), _showSelectionLegendCheckbox (0),
	  _itemsCheckableCheckbox (0),
	  _frameWidthTextField (0),_marginTextField (0), _curveWidthTextField (0),
	  _leftButton (0), _rightButton (0), _topButton (0), _bottomButton (0)
{
	assert (0 && "QwtLegendOptionsPanel copy constructor is not allowed.");
}	// QwtLegendOptionsPanel::QwtLegendOptionsPanel (const QwtLegendOptionsPanel& view)


QwtLegendOptionsPanel& QwtLegendOptionsPanel::operator = (
											const QwtLegendOptionsPanel& view)
{
	assert (0 && "QwtLegendOptionsPanel::operator = is not allowed.");
	return *this;
}	// QwtLegendOptionsPanel::QwtLegendOptionsPanel (const QwtLegendOptionsPanel& view)


QwtLegendOptionsPanel::~QwtLegendOptionsPanel ( )
{
}	// QwtLegendOptionsPanel::~QwtLegendOptionsPanel


bool QwtLegendOptionsPanel::isLegendShown ( ) const
{
	assert (0 != _showLegendCheckbox);
	return _showLegendCheckbox->isChecked ( );
}	// QwtLegendOptionsPanel::isLegendShown


void QwtLegendOptionsPanel::showLegend (
								bool show, QwtPlot::LegendPosition position)
{
	assert (0 != _showLegendCheckbox);
	_showLegendCheckbox->setChecked (show);
	setLegendPosition (position);
}	// QwtLegendOptionsPanel::showLegend


QwtPlot::LegendPosition QwtLegendOptionsPanel::getLegendPosition ( ) const
{
	assert (0 != _leftButton);
	assert (0 != _rightButton);
	assert (0 != _topButton);
	assert (0 != _bottomButton);
	if (true == _leftButton->isChecked ( ))
		return QwtPlot::LeftLegend;
	else if (true == _rightButton->isChecked ( ))
		return QwtPlot::RightLegend;
	else if (true == _topButton->isChecked ( ))
		return QwtPlot::TopLegend;

	return QwtPlot::BottomLegend;
}	// QwtLegendOptionsPanel::getLegendPosition


void QwtLegendOptionsPanel::setLegendPosition (QwtPlot::LegendPosition position)
{
	assert (0 != _leftButton);
	assert (0 != _rightButton);
	assert (0 != _topButton);
	assert (0 != _bottomButton);
	switch (position)
	{
		case QwtPlot::LeftLegend	:
			_leftButton->setChecked (true);		break;
		case QwtPlot::RightLegend	:
			_rightButton->setChecked (true);	break;
		case QwtPlot::TopLegend	:
			_topButton->setChecked (true);		break;
		case QwtPlot::BottomLegend	:
			_bottomButton->setChecked (true);	break;
	}	// switch (position)
}	// QwtLegendOptionsPanel::setLegendPosition


bool QwtLegendOptionsPanel::areItemsCheckable ( ) const
{
	assert (0 != _itemsCheckableCheckbox);
	return _itemsCheckableCheckbox->isChecked ( );
}	// QwtLegendOptionsPanel::areItemsCheckable


void QwtLegendOptionsPanel::setItemsCheckable (bool checkable) 
{
	assert (0 != _itemsCheckableCheckbox);
	_itemsCheckableCheckbox->setChecked (checkable);
}	// QwtLegendOptionsPanel::setItemsCheckable


bool QwtLegendOptionsPanel::isSelectionLegendShown ( ) const
{
	assert (0 != _showSelectionLegendCheckbox);
	return _showSelectionLegendCheckbox->isChecked ( );
}	// QwtLegendOptionsPanel::isSelectionLegendShown


void QwtLegendOptionsPanel::showSelectionLegend (bool show)
{
	assert (0 != _showSelectionLegendCheckbox);
	_showSelectionLegendCheckbox->setChecked (show);
}	// QwtLegendOptionsPanel::showSelectionLegend


int QwtLegendOptionsPanel::getFrameWidth ( ) const
{
	assert (0 != _frameWidthTextField);
	return _frameWidthTextField->getValue ( );
}	// QwtLegendOptionsPanel::getFrameWidth


void QwtLegendOptionsPanel::setFrameWidth (int width)
{
	assert (0 != _frameWidthTextField);
	_frameWidthTextField->setValue (width);
}	// QwtLegendOptionsPanel::setFrameWidth


int QwtLegendOptionsPanel::getMargin ( ) const
{
	assert (0 != _marginTextField);
	return _marginTextField->getValue ( );
}	// QwtLegendOptionsPanel::getMargin


void QwtLegendOptionsPanel::setMargin (int margin)
{
	assert (0 != _marginTextField);
	_marginTextField->setValue (margin);
}	// QwtLegendOptionsPanel::setMargin


int QwtLegendOptionsPanel::getCurveWidth ( ) const
{
	assert (0 != _curveWidthTextField);
	return _curveWidthTextField->getValue ( );
}	// QwtLegendOptionsPanel::getCurveWidth


void QwtLegendOptionsPanel::setCurveWidth (int width)
{
	assert (0 != _curveWidthTextField);
	_curveWidthTextField->setValue (width);
}	// QwtLegendOptionsPanel::setCurveWidth


void QwtLegendOptionsPanel::apply ( )
{
	if (true == hasChartPanel ( ))
	{
		// Caractère cochable des items : nécessite une reconstruction de la 
		// légende :
		if (areItemsCheckable ( ) != getChartPanel ( ).isLegendCheckable ( ))
			getChartPanel ( ).setLegendCheckable (areItemsCheckable ( ));
		getChartPanel ( ).showLegend (isLegendShown ( ), getLegendPosition ( ));
		getChartPanel ( ).showSelectionLegend (isSelectionLegendShown ( ));
		if (true == getChartPanel ( ).hasLegend ( ))
		{
			getChartPanel ( ).setLegendFrameWidth (getFrameWidth ( ));
			getChartPanel ( ).setLegendMargin (getMargin ( ));
			getChartPanel ( ).setLegendCurveWidth (getCurveWidth ( ));
		}	// if (true == getChartPanel ( ).hasLegend ( ))
	}	// if (true == hasChartPanel ( ))
}	// QwtLegendOptionsPanel::apply


QwtChartPanel& QwtLegendOptionsPanel::getChartPanel ( )
{
	if (0 == _chartPanel)
	{
		INTERNAL_ERROR (exc, "Absence de panneau graphique associé.", "QwtLegendOptionsPanel::getChartPanel")
		throw (exc);
	}	// if (0 == _chartPanel)

	return *_chartPanel;
}	// QwtLegendOptionsPanel::getChartPanel


const QwtChartPanel& QwtLegendOptionsPanel::getChartPanel ( ) const
{
	if (0 == _chartPanel)
	{
		INTERNAL_ERROR (exc, "Absence de panneau graphique associé.", "QwtLegendOptionsPanel::getChartPanel")
		throw (exc);
	}	// if (0 == _chartPanel)

	return *_chartPanel;
}	// QwtLegendOptionsPanel::getChartPanel





