#include "QwtCharts/QwtHistogramOptionsPanel.h"

#include <QtUtil/QtConfiguration.h>
#include <QtUtil/QtUnicodeHelper.h>
#include <TkUtil/Exception.h>
#include <TkUtil/InternalError.h>
#include <TkUtil/MemoryError.h>

#include <QLabel>
#include <QLayout>
#include <QGroupBox>

#include <assert.h>


USING_UTIL
USING_STD

static const TkUtil::Charset	charset ("àéèùô");

USE_ENCODING_AUTODETECTION


QwtHistogramOptionsPanel::QwtHistogramOptionsPanel (
						QWidget* parent, QwtHistogramChartPanel* histogramPanel)
	: QwtHistogramChartPanel::QwtChartEditionPanel (parent, histogramPanel),
	  _histogramPanel (histogramPanel),
	  _styleComboBox (0), _elementsNumCheckBox (0), _elementsNumPosComboBox (0)
{
	setWindowTitle ("Histogramme");
	QVBoxLayout*	mainLayout	= new QVBoxLayout (this);
	mainLayout->setSizeConstraint (QLayout::SetMinimumSize);

	// Style :
	QHBoxLayout*	hLayout	=  new QHBoxLayout ( );
	mainLayout->addLayout (hLayout);
	QLabel*			label	= new QLabel ("Style : ", this);
	hLayout->addWidget (label);
	_styleComboBox	= new QComboBox (this);
	hLayout->addWidget (_styleComboBox);
	_styleComboBox->addItem (QSTR ("Barres groupées"));
	_styleComboBox->addItem (QSTR ("Barres empilées"));
	_styleComboBox->addItem (QSTR ("Barres chevauchantes"));
	hLayout->addStretch (10);
	_styleComboBox->setCurrentIndex (
		0 == _histogramPanel ? 0 : (int)_histogramPanel->getHistogramStyle ( ));

	// Nombre d'éléments par classe :
	hLayout	=  new QHBoxLayout ( );
	mainLayout->addLayout (hLayout);
	_elementsNumCheckBox	= new QCheckBox (
			QSTR ("Afficher le nombre d'éléments par classe"), this);
	hLayout->addWidget (_elementsNumCheckBox);
	if (0 != _histogramPanel)
		_elementsNumCheckBox->setCheckState (
						true == _histogramPanel->displayElementsNum ( ) ?
						Qt::Checked : Qt::Unchecked);
	_elementsNumPosComboBox	= new QComboBox (this);
	hLayout->addWidget (_elementsNumPosComboBox);
	_elementsNumPosComboBox->addItem ("Au dessus");
	_elementsNumPosComboBox->addItem ("Au milieu");
	_elementsNumPosComboBox->addItem ("Au dessous");
	hLayout->addStretch (10);
	_elementsNumPosComboBox->setCurrentIndex (
		 0 == _histogramPanel ? 0 : (int)_histogramPanel->getElementNumPos ( ));

	// Vide :
	mainLayout->addStretch (10);
	
	setLayout (mainLayout);
}	// QwtHistogramOptionsPanel::QwtHistogramOptionsPanel


QwtHistogramOptionsPanel::QwtHistogramOptionsPanel (
									const QwtHistogramOptionsPanel& view)
	: QwtHistogramChartPanel::QwtChartEditionPanel (0, 0),
	  _histogramPanel (0),
	  _styleComboBox (0), _elementsNumCheckBox (0), _elementsNumPosComboBox (0)
{
	assert (0 && "QwtHistogramOptionsPanel copy constructor is not allowed.");
}	// QwtHistogramOptionsPanel::QwtHistogramOptionsPanel (const QwtHistogramOptionsPanel& view)


QwtHistogramOptionsPanel& QwtHistogramOptionsPanel::operator = (
										const QwtHistogramOptionsPanel& view)
{
	assert (0 && "QwtHistogramOptionsPanel::operator = is not allowed.");
	return *this;
}	// QwtHistogramOptionsPanel::QwtHistogramOptionsPanel (const QwtHistogramOptionsPanel& view)


QwtHistogramOptionsPanel::~QwtHistogramOptionsPanel ( )
{
}	// QwtHistogramOptionsPanel::~QwtHistogramOptionsPanel


void QwtHistogramOptionsPanel::apply ( )
{
	getHistogramPanel ( ).setElementNumPos (getElementNumPos ( ));
	getHistogramPanel ( ).displayElementsNum (displayElementsNum ( ));
	getHistogramPanel ( ).setHistogramStyle (getHistogramStyle ( ));
}	// QwtHistogramOptionsPanel::apply


QwtHistogramChartPanel& QwtHistogramOptionsPanel::getHistogramPanel ( )
{
	if (0 == _histogramPanel)
	{
		INTERNAL_ERROR (exc, "Absence de panneau graphique associé.", "QwtHistogramOptionsPanel::getHistogramPanel")
		throw (exc);
	}	// if (0 == _histogramPanel)

	return *_histogramPanel;
}	// QwtHistogramOptionsPanel::getHistogramPanel


const QwtHistogramChartPanel&
						QwtHistogramOptionsPanel::getHistogramPanel ( ) const
{
	if (0 == _histogramPanel)
	{
		INTERNAL_ERROR (exc, "Absence de panneau graphique associé.", "QwtHistogramOptionsPanel::getHistogramPanel")
		throw (exc);
	}	// if (0 == _histogramPanel)

	return *_histogramPanel;
}	// QwtHistogramOptionsPanel::getHistogramPanel


QwtExtendedMultiBarChart::HISTOGRAM_STYLE
						QwtHistogramOptionsPanel::getHistogramStyle ( ) const
{
	CHECK_NULL_PTR_ERROR (_styleComboBox)
	return (QwtExtendedMultiBarChart::HISTOGRAM_STYLE)
										_styleComboBox->currentIndex ( );
}	// QwtHistogramOptionsPanel::getHistogramStyle


void QwtHistogramOptionsPanel::setHistogramStyle (
								QwtExtendedMultiBarChart::HISTOGRAM_STYLE style)
{
	CHECK_NULL_PTR_ERROR (_styleComboBox)
	if (style >= _styleComboBox->count ( ))
	{
		UTF8String	message (charset);
		message << "Type de style non supporté (" << (unsigned long)style
		        << "). Type max : "
		        << (unsigned long)QwtExtendedMultiBarChart::STYLE_OVERLAPPED
		        << ". Erreur survenue dans "
		        << "QwtHistogramOptionsPanel::setHistogramStyle.";
		throw Exception (message);
	}	// if (style >= _styleComboBox->count ( ))

	_styleComboBox->setCurrentIndex (style);
}	// QwtHistogramOptionsPanel::setHistogramStyle


bool QwtHistogramOptionsPanel::displayElementsNum ( ) const
{
	CHECK_NULL_PTR_ERROR (_elementsNumCheckBox)
	return _elementsNumCheckBox->isChecked ( );
}	// QwtHistogramOptionsPanel::displayElementsNum


void QwtHistogramOptionsPanel::displayElementsNum (bool display)
{
	CHECK_NULL_PTR_ERROR (_elementsNumCheckBox)
	_elementsNumCheckBox->setCheckState (
								true == display ? Qt::Checked : Qt::Unchecked);
}	// QwtHistogramOptionsPanel::displayElementsNum


QwtExtendedMultiBarChart::POSITION
							QwtHistogramOptionsPanel::getElementNumPos ( ) const
{
	CHECK_NULL_PTR_ERROR (_elementsNumPosComboBox)
	return (QwtExtendedMultiBarChart::POSITION)
									_elementsNumPosComboBox->currentIndex ( );
}	// QwtHistogramOptionsPanel::getElementNumPos


void QwtHistogramOptionsPanel::setElementNumPos (
										QwtExtendedMultiBarChart::POSITION pos)
{
	CHECK_NULL_PTR_ERROR (_elementsNumPosComboBox)
	if (pos >= _elementsNumPosComboBox->count ( ))
	{
		UTF8String	message (charset);
		message << "Position non supportée (" << (unsigned long)pos
		        << "). Position max : "
		        << (unsigned long)QwtExtendedMultiBarChart::BOTTOM
		        << ". Erreur survenue dans "
		        << "QwtHistogramOptionsPanel::setElementNumPos.";
		throw Exception (message);
	}	// if (pos >= _elementsNumPosComboBox->count ( ))

	_elementsNumPosComboBox->setCurrentIndex (pos);
}	// QwtHistogramOptionsPanel::setElementNumPos

