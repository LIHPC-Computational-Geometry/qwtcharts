#include "QwtCharts/QwtChartGeneralOptionsPanel.h"
#include <QtUtil/QtUnicodeHelper.h>
#include <QtUtil/QtConfiguration.h>
#include <QtUtil/QtScientificTextDialog.h>
#include <TkUtil/Exception.h>
#include <TkUtil/InternalError.h>
#include <TkUtil/MemoryError.h>

#include <qwt_plot_layout.h>

#include <QButtonGroup>
#include <QColorDialog>
#include <QLabel>
#include <QLayout>
#include <QGroupBox>
#include <QPushButton>

#include <assert.h>


USING_UTIL
USING_STD

static const TkUtil::Charset	charset ("àéèùô");

USE_ENCODING_AUTODETECTION


QwtChartGeneralOptionsPanel::QwtChartGeneralOptionsPanel (
									QWidget* parent, QwtChartPanel* chartPanel)
	: QwtChartPanel::QwtChartEditionPanel (parent, chartPanel),
	  _chartPanel (chartPanel),
	  _backgroundColor (Qt::white),
	  _titleTextField (0), _titleFontPanel (0),
	  _cartesianButton (0), _polarButton (0),
	  _marginsTextField (0),
	  _aspectRatioCheckbox (0), _aspectRatioTextField (0)
{
	if (0 != _chartPanel)
		_backgroundColor	= _chartPanel->getBackgroundColor ( );

	QVBoxLayout*	mainLayout	= new QVBoxLayout (this);
	mainLayout->setSizeConstraint (QLayout::SetMinimumSize);

	// Titre :
	QGroupBox*		group		= new QGroupBox ("Titre", this);
	QVBoxLayout*	vLayout		= new QVBoxLayout ( );
	group->setLayout (vLayout);
	mainLayout->addWidget (group);
	QHBoxLayout*	hLayout	= new QHBoxLayout ( );
	vLayout->addLayout (hLayout);
	QLabel*	label	= new QLabel ("Titre :", group);
	hLayout->addWidget (label);
	QString	title	=
			0 == _chartPanel ? "" : _chartPanel->getPlot ( ).title ( ).text ( );
	_titleTextField	= new QtTextField (title, group);
	hLayout->addWidget (_titleTextField);
	QPushButton*	button	= new QPushButton ("Modifier ...", group);
	hLayout->addWidget (button);
	connect (button, SIGNAL (clicked ( )), this, SLOT (editTitleCallback ( )));
	QFont	font	= 0 == _chartPanel ?
			QApplication::font( ) : _chartPanel->getPlot ( ).title ( ).font ( );
	QColor	color	= 0 == _chartPanel ?
			Qt::black :  _chartPanel->getPlot ( ).title ( ).color ( );
	_titleFontPanel	=
		new QtFontPanel (this, font, color, QtFontPanel::HORIZONTAL_PANEL);
	vLayout->addWidget (_titleFontPanel);
	vLayout->addStretch (2);

	// Type de représentation :
	QButtonGroup*	buttonGroup	= new QButtonGroup (this);
	buttonGroup->setExclusive (true);
	group	= new QGroupBox (QSTR ("Représentation"), this);
	hLayout	= new QHBoxLayout ( );
	group->setLayout (hLayout);
	mainLayout->addWidget (group);
	_cartesianButton	=
					new QRadioButton (QSTR ("Cartésienne"), group);
	hLayout->addWidget (_cartesianButton);
	if (QwtChartPanel::CARTESIAN == chartPanel->getDisplayMode ( ))
		_cartesianButton->setChecked (true);
	_cartesianButton->setEnabled (
				chartPanel->isDisplayModeAllowed (QwtChartPanel::CARTESIAN));
	buttonGroup->addButton (_cartesianButton);
	_polarButton	= new QRadioButton ("Polaire", group);
	buttonGroup->addButton (_polarButton);
	hLayout->addWidget (_polarButton);
	if (QwtChartPanel::POLAR == chartPanel->getDisplayMode ( ))
		_polarButton->setChecked (true);
	_polarButton->setEnabled (
				chartPanel->isDisplayModeAllowed (QwtChartPanel::POLAR));
	hLayout->addStretch (200);

	// Fond :
	group	= new QGroupBox ("Zone graphique", this);
	hLayout	= new QHBoxLayout ( );
	group->setLayout (hLayout);
	mainLayout->addWidget (group);
	label	= new QLabel ("Fond :", group);
	hLayout->addWidget (label);
	button	= new QPushButton ("Couleur ...", group);
	hLayout->addWidget (button);
	connect (button, SIGNAL (clicked ( )), this,
	         SLOT (backgroundColorCallback ( )));
	hLayout->addStretch (2);

	// Marges :
	group	= new QGroupBox ("Marges", this);
	hLayout	= new QHBoxLayout ( );
	group->setLayout (hLayout);	
	mainLayout->addWidget (group);
	label	= new QLabel ("Marges du graphique complet :", group);
	hLayout->addWidget (label);
	int	margin	= 0;
	if (0 != _chartPanel)
		_chartPanel->getPlot ( ).getContentsMargins (&margin, &margin, &margin, &margin);
	_marginsTextField	= new QtIntTextField (margin, group);
	_marginsTextField->setRange (0, 100);
	_marginsTextField->setVisibleColumns (3);
	hLayout->addWidget (_marginsTextField);
	hLayout->addStretch (2);

	group		= new QGroupBox ("Rapport d'aspect", this);
	hLayout		= new QHBoxLayout ( );
	group->setLayout (hLayout);
	mainLayout->addWidget (group);
	_aspectRatioCheckbox	=
					new QCheckBox ("Utiliser le rapport d'aspect", group);
	if (0 != _chartPanel)
		_aspectRatioCheckbox->setChecked (_chartPanel->useAspectRatio ( ));
	hLayout->addWidget (_aspectRatioCheckbox);
	label	= new QLabel ("Rapport d'aspect :", group);
	hLayout->addWidget (label);
	_aspectRatioTextField	=
		new QtDoubleTextField (0 == _chartPanel ?
		                       1. : _chartPanel->getAspectRatio ( ), group);
	_aspectRatioTextField->setRange (1E-6, 1E6);
	hLayout->addWidget (_aspectRatioTextField);
	hLayout->addStretch (2);

	mainLayout->addStretch (2);
	setLayout (mainLayout);
}	// QwtChartGeneralOptionsPanel::QwtChartGeneralOptionsPanel


QwtChartGeneralOptionsPanel::QwtChartGeneralOptionsPanel (
									const QwtChartGeneralOptionsPanel& view)
	: QwtChartPanel::QwtChartEditionPanel (0, 0), _chartPanel (0),
	  _backgroundColor (Qt::white), _titleTextField (0), _titleFontPanel (0),
	  _cartesianButton (0), _polarButton (0),
	  _marginsTextField (0),
	  _aspectRatioCheckbox (0), _aspectRatioTextField (0)
{
	assert (0 && "QwtChartGeneralOptionsPanel copy constructor is not allowed.");
}	// QwtChartGeneralOptionsPanel::QwtChartGeneralOptionsPanel (const QwtChartGeneralOptionsPanel& view)


QwtChartGeneralOptionsPanel& QwtChartGeneralOptionsPanel::operator = (
										const QwtChartGeneralOptionsPanel& view)
{
	assert (0 && "QwtChartGeneralOptionsPanel::operator = is not allowed.");
	return *this;
}	// QwtChartGeneralOptionsPanel::QwtChartGeneralOptionsPanel (const QwtChartGeneralOptionsPanel& view)


QwtChartGeneralOptionsPanel::~QwtChartGeneralOptionsPanel ( )
{
}	// QwtChartGeneralOptionsPanel::~QwtChartGeneralOptionsPanel


QwtText QwtChartGeneralOptionsPanel::getTitle ( ) const
{
	assert (0 != _titleTextField);
	assert (0 != _titleFontPanel);
	QwtText	title (_titleTextField->text ( ));
	title.setFont (_titleFontPanel->getFont ( ));
	title.setColor (_titleFontPanel->getColor ( ));

	return title;
}	// QwtChartGeneralOptionsPanel::getTitle


void QwtChartGeneralOptionsPanel::setTitle (const QwtText& title) 
{
	assert (0 != _titleTextField);
	assert (0 != _titleFontPanel);
	_titleTextField->setText (title.text ( ));
	_titleFontPanel->setFont (title.font ( ));
	_titleFontPanel->setColor (title.color ( ));
}	// QwtChartGeneralOptionsPanel::setTitle


QwtChartPanel::DISPLAY_MODE
						QwtChartGeneralOptionsPanel::getDisplayMode ( ) const
{
	assert (0 != _cartesianButton);
	assert (0 != _polarButton);
	if (true == _cartesianButton->isChecked ( ))
		return QwtChartPanel::CARTESIAN;
	if (true == _polarButton->isChecked ( ))
		return QwtChartPanel::POLAR;

	INTERNAL_ERROR (exc, "Mode de représentation (cartésienne/polaire) indéfini.", "QwtChartGeneralOptionsPanel::getDisplayMode")
	throw exc;
}	// QwtChartGeneralOptionsPanel::getDisplayMode


void QwtChartGeneralOptionsPanel::setDisplayMode (
											QwtChartPanel::DISPLAY_MODE mode)
{
	assert (0 != _cartesianButton);
	assert (0 != _polarButton);

	switch (mode)
	{
		case QwtChartPanel::CARTESIAN	:
			_cartesianButton->setChecked (true);	break;
		case QwtChartPanel::POLAR		:
			_polarButton->setChecked (true);	break;
		default							:
		{
			INTERNAL_ERROR (exc, "Mode de représentation (cartésienne/polaire) indéfini.", "QwtChartGeneralOptionsPanel::setDisplayMode")
			throw exc;
		}
	}	// switch (mode)
}	// QwtChartGeneralOptionsPanel::setDisplayMode


QColor QwtChartGeneralOptionsPanel::getBackgroundColor ( ) const
{
	return _backgroundColor;
}	// QwtChartGeneralOptionsPanel::getBackgroundColor


void QwtChartGeneralOptionsPanel::setBackgroundColor (const QColor& color)
{
	_backgroundColor	= color;
}	// QwtChartGeneralOptionsPanel::setBackgroundColor


unsigned int QwtChartGeneralOptionsPanel::getMargins ( ) const
{
	assert (0 != _marginsTextField);
	if (0 > _marginsTextField->getValue ( ))
	{
		UTF8String	msg (charset);
		msg << "La valeur " << (unsigned long)_marginsTextField->getValue ( )
		    << " est négative alors qu'elle devrait être positive ou nulle.";
		INTERNAL_ERROR (exc, msg, "QwtChartGeneralOptionsPanel::getMargins")
		throw exc;
	}	// if (0 > _marginsTextField->getValue ( ))

	return (unsigned int)_marginsTextField->getValue ( );
}	// QwtChartGeneralOptionsPanel::getMargins


void QwtChartGeneralOptionsPanel::setMargins (unsigned int margins)
{
	assert (0 != _marginsTextField);
	_marginsTextField->setValue (margins);
}	// QwtChartGeneralOptionsPanel::setMargins


bool QwtChartGeneralOptionsPanel::useAspectRatio ( ) const
{
	assert (0 != _aspectRatioCheckbox);
	return _aspectRatioCheckbox->isChecked ( );
}	// QwtChartGeneralOptionsPanel::useAspectRatio


void QwtChartGeneralOptionsPanel::useAspectRatio (bool use)
{
	assert (0 != _aspectRatioCheckbox);
	_aspectRatioCheckbox->setChecked (use);
}	// QwtChartGeneralOptionsPanel::useAspectRatio


double QwtChartGeneralOptionsPanel::getAspectRatio ( ) const
{
	assert (0 != _aspectRatioTextField);
	return _aspectRatioTextField->getValue ( );
}	// QwtChartGeneralOptionsPanel::getAspectRatio


void QwtChartGeneralOptionsPanel::setAspectRatio (double ar)
{
	assert (0 != _aspectRatioTextField);
	_aspectRatioTextField->setValue (ar);
}	// QwtChartGeneralOptionsPanel::setAspectRatio


void QwtChartGeneralOptionsPanel::apply ( )
{
	if (true == hasChartPanel ( ))
	{
		getChartPanel ( ).setTitle (getTitle ( ));
		getChartPanel ( ).setDisplayMode (getDisplayMode ( ));
		getChartPanel ( ).setBackgroundColor (getBackgroundColor ( ));
		getChartPanel ( ).setMargin (getMargins ( ));
		getChartPanel ( ).useAspectRatio (useAspectRatio ( ));
		getChartPanel ( ).setAspectRatio (getAspectRatio ( ));
	}	// if (true == hasChartPanel ( ))
}	// QwtChartGeneralOptionsPanel::apply


QwtChartPanel& QwtChartGeneralOptionsPanel::getChartPanel ( )
{
	if (0 == _chartPanel)
	{
		INTERNAL_ERROR (exc, "Absence de panneau graphique associé.", "QwtChartGeneralOptionsPanel::getChartPanel")
		throw (exc);
	}	// if (0 == _chartPanel)

	return *_chartPanel;
}	// QwtChartGeneralOptionsPanel::getChartPanel


const QwtChartPanel& QwtChartGeneralOptionsPanel::getChartPanel ( ) const
{
	if (0 == _chartPanel)
	{
		INTERNAL_ERROR (exc, "Absence de panneau graphique associé.", "QwtChartGeneralOptionsPanel::getChartPanel")
		throw (exc);
	}	// if (0 == _chartPanel)

	return *_chartPanel;
}	// QwtChartGeneralOptionsPanel::getChartPanel



void QwtChartGeneralOptionsPanel::editTitleCallback ( )
{
	assert (0 != _titleTextField);
	assert (0 != _titleFontPanel);

	try
	{
		UTF8String		title (
			QtUnicodeHelper::qstringToUTF8String (_titleTextField->text( )));
		QFont			font		= _titleFontPanel->getFont ( );
		QColor			textColor	= _titleFontPanel->getColor ( );
		QColor			background	= getBackgroundColor ( );
		QtScientificTextDialog	dialog (
					this, "Titre du graphique", title, true, font,
					textColor, background);
		if (QDialog::Accepted != dialog.exec ( ))
			return;

		_titleTextField->setText (dialog.getQText ( ));
	}
	catch (const Exception& exc)
	{
		UTF8String	mess (charset);
		cout << "Erreur interne dans QwtChartGeneralOptionsPanel::editTitleCallback ("
		     << __FILE__ << ' ' << (unsigned long)__LINE__ << ") : " << exc.getFullMessage ( );
		ConsoleOutput::cout ( ) << mess << co_endl;
	}
	catch (...)
	{
		UTF8String	mess (charset);
		cout << "Erreur interne dans QwtChartGeneralOptionsPanel::editTitleCallback ("
		     << __FILE__ << ' ' << (unsigned long)__LINE__ << ") : erreur non documentée.";
		ConsoleOutput::cout ( ) << mess << co_endl;
	}
}	// QwtChartGeneralOptionsPanel::editTitleCallback


void QwtChartGeneralOptionsPanel::backgroundColorCallback ( )
{
	QColor	color	= QColorDialog::getColor (_backgroundColor, this, QSTR ("Couleur du fond"), QColorDialog::DontUseNativeDialog);
	if (true == color.isValid ( ))
		_backgroundColor	= color;
}	// QwtChartGeneralOptionsPanel::backgroundColorCallback

