#include "QwtCharts/QwtAxisOptionsPanel.h"
#include "QwtCharts/QwtExtendedScaleDraw.h"
#include <QtUtil/QtConfiguration.h>
#include <QtUtil/QtScientificTextDialog.h>
#include <QtUtil/QtUnicodeHelper.h>
#include <TkUtil/Exception.h>
#include <TkUtil/InternalError.h>
#include <TkUtil/MemoryError.h>

#include <qwt_scale_widget.h>

#include <QLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>

#include <assert.h>
#include <values.h>


USING_UTIL
USING_STD

static const TkUtil::Charset	charset ("àéèùô");

USE_ENCODING_AUTODETECTION


QwtAxisOptionsPanel::QwtAxisOptionsPanel (
									QWidget* parent, QwtChartPanel* chartPanel)
	: QwtChartPanel::QwtChartEditionPanel (parent, chartPanel),
	  _chartPanel (chartPanel),
	  _horTitleTextField (0), _verTitleTextField (0),
	  _horTitleFontPanel (0), _verTitleFontPanel (0),
	  _horAutoScaleCheckbox (0), _verAutoScaleCheckbox (0),
	  _horMinTextField (0), _horMaxTextField (0),
	  _verMinTextField (0), _verMaxTextField (0),
	  _horLabelFormatComboBox (0), _verLabelFormatComboBox (0),
	  _horNumberOfDecimalsCheckBox (0), _verNumberOfDecimalsCheckBox (0),
	  _horNumberOfDecimalsTextField (0), _verNumberOfDecimalsTextField (0),
	  _verMajorTickStepTextField (0), _horMajorTickStepTextField (0),
	  _axisFontPanel (0)
{
	QVBoxLayout*	mainLayout	= new QVBoxLayout (this);
	mainLayout->setSizeConstraint (QLayout::SetMinimumSize);

	// Axe horizontal :
	bool	autoScale	= true;
	double	min			= 0., max	= 0.,	step	= 0.;
	if (0 != _chartPanel)
	{
		_chartPanel->getAbscissaScale (min, max, autoScale);
		step	= _chartPanel->getAbscissaStep ( );
	}	// if (0 != _chartPanel)
	QGroupBox*	group	= new QGroupBox ("Axe horizontal", this);
	QVBoxLayout*	groupLayout	= new QVBoxLayout ( );
	group->setLayout (groupLayout);
	mainLayout->addWidget (group);
	QHBoxLayout*	hLayout	= new QHBoxLayout ( );
	groupLayout->addLayout (hLayout);
	QLabel*	label	= new QLabel ("Titre : ", group);
	hLayout->addWidget (label);
	QString	title	= 0 == _chartPanel ?
		  "" : _chartPanel->getPlot ( ).axisTitle (QwtPlot::xBottom).text ( );
	_horTitleTextField	= new QtTextField (title, group);
	hLayout->addWidget (_horTitleTextField);
	QPushButton*	button	= new QPushButton ("Modifier ...", group);
	hLayout->addWidget (button);
	connect (button, SIGNAL (clicked( )), this, SLOT (editHorTitleCallback( )));
	QFont	font	= 0 == _chartPanel ?
			QApplication::font( ) : _chartPanel->getPlot ( ).axisTitle (
													QwtPlot::xBottom).font ( );
	QColor	color	= 0 == _chartPanel ?
			Qt::black :  _chartPanel->getPlot ( ).axisTitle (
													QwtPlot::xBottom).color ( );
	_horTitleFontPanel	= 
			new QtFontPanel (this, font, color, QtFontPanel::HORIZONTAL_PANEL);
	groupLayout->addWidget (_horTitleFontPanel);
	_horAutoScaleCheckbox	= new QCheckBox ("Echelle automatique", group);
	_horAutoScaleCheckbox->setChecked (autoScale);
	connect (_horAutoScaleCheckbox, SIGNAL (stateChanged (int)), this,
	         SLOT (autoScaleCallback (int)));
	groupLayout->addWidget (_horAutoScaleCheckbox);
	hLayout	= new QHBoxLayout ( );
	groupLayout->addLayout (hLayout);
	label	= new QLabel ("Min :", group);
	hLayout->addWidget (label);
	_horMinTextField	= new QtDoubleTextField (min, group);
	_horMinTextField->setDecimals (15);
	_horMinTextField->setValue (min);
	hLayout->addWidget (_horMinTextField);
	label	= new QLabel ("Max :", group);
	hLayout->addWidget (label);
	_horMaxTextField	= new QtDoubleTextField (max, group);
	_horMaxTextField->setDecimals (15);
	_horMaxTextField->setValue (max);
	hLayout->addWidget (_horMaxTextField);
	hLayout	= new QHBoxLayout ( );
	groupLayout->addLayout (hLayout);
	label	= new QLabel ("Format d'affichage :", group);
	hLayout->addWidget (label);
	_horLabelFormatComboBox	= new QtExtStringNumberFormatComboBox (group, 'c');
	hLayout->addWidget (_horLabelFormatComboBox);
	hLayout	= new QHBoxLayout ( );
	groupLayout->addLayout (hLayout);
	_horNumberOfDecimalsCheckBox	=
		new QCheckBox (QSTR ("Fixer le nombre de décimales"), group);
	connect (_horNumberOfDecimalsCheckBox, SIGNAL (stateChanged (int)), this,
	         SLOT (enableNumberOfDecimalsCallback (int)));
	hLayout->addWidget (_horNumberOfDecimalsCheckBox);
	label	= new QLabel ("Nombre :", group);
	hLayout->addWidget (label);
	_horNumberOfDecimalsTextField	= new QtIntTextField (2, group);
	_horNumberOfDecimalsTextField->setRange (0, 15);
	_horNumberOfDecimalsTextField->setEnabled (
								_horNumberOfDecimalsCheckBox->isChecked ( ));
	hLayout->addWidget (_horNumberOfDecimalsTextField);

	hLayout	= new QHBoxLayout ( );
	groupLayout->addLayout (hLayout);
	label	 =
		new QLabel ("Distance entre 2 lignes majeures verticales :", group);
	hLayout->addWidget (label);
	_verMajorTickStepTextField	= new QtDoubleTextField (step, group);
	hLayout->addWidget (_verMajorTickStepTextField);
	_verMajorTickStepTextField->setRange (0., MAXDOUBLE);
	_verMajorTickStepTextField->setDecimals (15);

	// Axe vertical :
	if (0 != _chartPanel)
	{
		_chartPanel->getOrdinateScale (min, max, autoScale);
		step	= _chartPanel->getOrdinateStep ( );
	}	// if (0 != _chartPanel)
	group	= new QGroupBox ("Axe vertical", this);
	groupLayout	= new QVBoxLayout ( );
	group->setLayout (groupLayout);
	mainLayout->addWidget (group);
	hLayout	= new QHBoxLayout ( );
	groupLayout->addLayout (hLayout);
	label	= new QLabel ("Titre : ", group);
	hLayout->addWidget (label);
	title	= 0 == _chartPanel ?
		  "" : _chartPanel->getPlot ( ).axisTitle (QwtPlot::yLeft).text ( );
	_verTitleTextField	= new QtTextField (title, group);
	hLayout->addWidget (_verTitleTextField);
	button	= new QPushButton ("Modifier ...", group);
	hLayout->addWidget (button);
	connect (button, SIGNAL (clicked( )), this, SLOT (editVerTitleCallback( )));
	font	= 0 == _chartPanel ?
			QApplication::font( ) : _chartPanel->getPlot ( ).axisTitle (
													QwtPlot::yLeft).font ( );
	color	= 0 == _chartPanel ?
			Qt::black :  _chartPanel->getPlot ( ).axisTitle (
													QwtPlot::yLeft).color ( );
	_verTitleFontPanel	= 
			new QtFontPanel (this, font, color, QtFontPanel::HORIZONTAL_PANEL);
	groupLayout->addWidget (_verTitleFontPanel);
	_verAutoScaleCheckbox	= new QCheckBox ("Echelle automatique", group);
	_verAutoScaleCheckbox->setChecked (autoScale);
	connect (_verAutoScaleCheckbox, SIGNAL (stateChanged (int)), this,
	         SLOT (autoScaleCallback (int)));
	groupLayout->addWidget (_verAutoScaleCheckbox);
	hLayout	= new QHBoxLayout ( );
	groupLayout->addLayout (hLayout);
	label	= new QLabel ("Min :", group);
	hLayout->addWidget (label);
	_verMinTextField	= new QtDoubleTextField (min, group);
	_verMinTextField->setDecimals (15);
	_verMinTextField->setValue (min);
	hLayout->addWidget (_verMinTextField);
	label	= new QLabel ("Max :", group);
	hLayout->addWidget (label);
	_verMaxTextField	= new QtDoubleTextField (max, group);
	_verMaxTextField->setDecimals (15);
	_verMaxTextField->setValue (max);
	hLayout->addWidget (_verMaxTextField);
	hLayout	= new QHBoxLayout ( );
	groupLayout->addLayout (hLayout);
	label	= new QLabel ("Format d'affichage :", group);
	hLayout->addWidget (label);
	_verLabelFormatComboBox	= new QtExtStringNumberFormatComboBox (group, 'c');
	hLayout->addWidget (_verLabelFormatComboBox);
	hLayout	= new QHBoxLayout ( );
	groupLayout->addLayout (hLayout);
	_verNumberOfDecimalsCheckBox	=
		new QCheckBox (QSTR ("Fixer le nombre de décimales"), group);
	connect (_verNumberOfDecimalsCheckBox, SIGNAL (stateChanged (int)), this,
	         SLOT (enableNumberOfDecimalsCallback (int)));
	hLayout->addWidget (_verNumberOfDecimalsCheckBox);
	label	= new QLabel ("Nombre :", group);
	hLayout->addWidget (label);
	_verNumberOfDecimalsTextField	= new QtIntTextField (2, group);
	_verNumberOfDecimalsTextField->setRange (0, 15);
	_verNumberOfDecimalsTextField->setEnabled (
									_verNumberOfDecimalsCheckBox->isChecked ( ));
	hLayout->addWidget (_verNumberOfDecimalsTextField);
	hLayout	= new QHBoxLayout ( );
	groupLayout->addLayout (hLayout);
	label	 =
		new QLabel ("Distance entre 2 lignes majeures horizontales :", group);
	hLayout->addWidget (label);
	_horMajorTickStepTextField	= new QtDoubleTextField (step, group);
	hLayout->addWidget (_horMajorTickStepTextField);
	_horMajorTickStepTextField->setRange (0., MAXDOUBLE);
	_horMajorTickStepTextField->setDecimals (15);

	// Choix de la police de caractères :
	group	= new QGroupBox (
				QSTR ("Police de caractères des graduations"), this);
	groupLayout	= new QVBoxLayout ( );
	group->setLayout (groupLayout);
	mainLayout->addWidget (group);
	font 	= 0 == _chartPanel ? QApplication::font ( ) :
						  _chartPanel->getPlot( ).axisFont(QwtPlot::xBottom);
	const QColor	titleColor	= 0 == _chartPanel ?
								Qt::black : _chartPanel->getPlot (
									).axisTitle (QwtPlot::xBottom).color ( );
	_axisFontPanel	=
		new QtFontPanel (group, font, titleColor,QtFontPanel::HORIZONTAL_PANEL);
	groupLayout->addWidget (_axisFontPanel);

	// Quelques initialisations :
	if (0 != _chartPanel)
	{
		QwtExtendedScaleDraw*	sd	=
				dynamic_cast<QwtExtendedScaleDraw*>(
				getChartPanel ( ).getPlot ( ).axisScaleDraw (QwtPlot::xBottom));
		if (0 != sd)
		{
			_horNumberOfDecimalsCheckBox->setChecked (
											sd->isNumberOfDecimalsEnabled ( ));
			_horNumberOfDecimalsTextField->setValue (sd->getNumberOfDecimals ( ));
			_horLabelFormatComboBox->setFormat (sd->getFormat ( ));
		}	// if (0 != sd)
	}	// if (0 != _chartPanel)
	if (0 != _chartPanel)
	{
		QwtExtendedScaleDraw*	sd	=
				dynamic_cast<QwtExtendedScaleDraw*>(
					getChartPanel ( ).getPlot ( ).axisScaleDraw (QwtPlot::yLeft));
		if (0 != sd)
		{
			_verNumberOfDecimalsCheckBox->setChecked (
											sd->isNumberOfDecimalsEnabled ( ));
			_verNumberOfDecimalsTextField->setValue (sd->getNumberOfDecimals ( ));
			_verLabelFormatComboBox->setFormat (sd->getFormat ( ));
		}	// if (0 != sd)
	}	// if (0 != _chartPanel)


	setLayout (mainLayout);

	autoScaleCallback (true);
}	// QwtAxisOptionsPanel::QwtAxisOptionsPanel


QwtAxisOptionsPanel::QwtAxisOptionsPanel (const QwtAxisOptionsPanel& view)
	: QwtChartPanel::QwtChartEditionPanel (0, 0), _chartPanel (0),
	  _horTitleTextField (0), _verTitleTextField (0),
	  _horTitleFontPanel (0), _verTitleFontPanel (0),
	  _horAutoScaleCheckbox (0), _verAutoScaleCheckbox (0),
	  _horMinTextField (0), _horMaxTextField (0),
	  _verMinTextField (0), _verMaxTextField (0),
	  _horLabelFormatComboBox (0), _verLabelFormatComboBox (0),
	  _horNumberOfDecimalsCheckBox (0), _verNumberOfDecimalsCheckBox (0),
	  _horNumberOfDecimalsTextField (0), _verNumberOfDecimalsTextField (0),
	  _axisFontPanel (0)
{
	assert (0 && "QwtAxisOptionsPanel copy constructor is not allowed.");
}	// QwtAxisOptionsPanel::QwtAxisOptionsPanel (const QwtAxisOptionsPanel& view)


QwtAxisOptionsPanel& QwtAxisOptionsPanel::operator = (
											const QwtAxisOptionsPanel& view)
{
	assert (0 && "QwtAxisOptionsPanel::operator = is not allowed.");
	return *this;
}	// QwtAxisOptionsPanel::QwtAxisOptionsPanel (const QwtAxisOptionsPanel& view)


QwtAxisOptionsPanel::~QwtAxisOptionsPanel ( )
{
}	// QwtAxisOptionsPanel::~QwtAxisOptionsPanel


QwtText QwtAxisOptionsPanel::getHorTitle ( ) const
{
	assert (0 != _horTitleTextField);
	assert (0 != _horTitleFontPanel);
	QwtText	title (_horTitleTextField->text ( ));
	title.setFont (_horTitleFontPanel->getFont ( ));
	title.setColor (_horTitleFontPanel->getColor ( ));

	return title;
}	// QwtAxisOptionsPanel::getHorTitle


void QwtAxisOptionsPanel::setHorTitle (const QwtText& title)
{
	assert (0 != _horTitleTextField);
	assert (0 != _horTitleFontPanel);
	_horTitleTextField->setText (title.text ( ));
	_horTitleFontPanel->setFont (title.font ( ));
	_horTitleFontPanel->setColor (title.color ( ));
}	// QwtAxisOptionsPanel::setHorTitle


QwtText QwtAxisOptionsPanel::getVerTitle ( ) const
{
	assert (0 != _verTitleTextField);
	assert (0 != _verTitleFontPanel);
	QwtText	title (_verTitleTextField->text ( ));
	title.setFont (_verTitleFontPanel->getFont ( ));
	title.setColor (_verTitleFontPanel->getColor ( ));

	return title;
}	// QwtAxisOptionsPanel::getVerTitle


void QwtAxisOptionsPanel::setVerTitle (const QwtText& title)
{
	assert (0 != _verTitleTextField);
	assert (0 != _verTitleFontPanel);
	_verTitleTextField->setText (title.text ( ));
	_verTitleFontPanel->setFont (title.font ( ));
	_verTitleFontPanel->setColor (title.color ( ));
}	// QwtAxisOptionsPanel::setVerTitle


bool QwtAxisOptionsPanel::isHorAutoScale ( ) const
{
	assert (0 != _horAutoScaleCheckbox);
	return _horAutoScaleCheckbox->isChecked ( );
}	// QwtAxisOptionsPanel::isHorAutoScale


void QwtAxisOptionsPanel::enableHorAutoScale (bool enable) 
{
	assert (0 != _horAutoScaleCheckbox);
	_horAutoScaleCheckbox->setChecked (enable);
}	// QwtAxisOptionsPanel::enableHorAutoScale


bool QwtAxisOptionsPanel::isVerAutoScale ( ) const
{
	assert (0 != _verAutoScaleCheckbox);
	return _verAutoScaleCheckbox->isChecked ( );
}	// QwtAxisOptionsPanel::isVerAutoScale


void QwtAxisOptionsPanel::enableVerAutoScale (bool enable) 
{
	assert (0 != _verAutoScaleCheckbox);
	_verAutoScaleCheckbox->setChecked (enable);
}	// QwtAxisOptionsPanel::enableVerAutoScale


double QwtAxisOptionsPanel::getHorMin ( ) const
{
	assert (0 != _horMinTextField);
	return _horMinTextField->getValue ( );
}	// QwtAxisOptionsPanel::getHorMin


void QwtAxisOptionsPanel::setHorMin (double min)
{
	assert (0 != _horMinTextField);
	_horMinTextField->setValue (min);
}	// QwtAxisOptionsPanel::setHorMin


double QwtAxisOptionsPanel::getHorMax ( ) const
{
	assert (0 != _horMaxTextField);
	return _horMaxTextField->getValue ( );
}	// QwtAxisOptionsPanel::getHorMax


bool QwtAxisOptionsPanel::isHorNumberOfDecimalsEnabled ( ) const
{
	assert (0 != _horNumberOfDecimalsCheckBox);
	return _horNumberOfDecimalsCheckBox->isChecked ( );
}	// QwtAxisOptionsPanel::isHorNumberOfDecimalsEnabled


void QwtAxisOptionsPanel::enableHorNumberOfDecimals (bool enabled)
{
	assert (0 != _horNumberOfDecimalsCheckBox);
	assert (0 != _horNumberOfDecimalsTextField);
	_horNumberOfDecimalsCheckBox->setChecked (enabled);
	_horNumberOfDecimalsTextField->setEnabled (enabled);
}	// QwtAxisOptionsPanel::enableHorNumberOfDecimals


unsigned char QwtAxisOptionsPanel::getHorNumberOfDecimals ( ) const
{
	assert (0 != _horNumberOfDecimalsTextField);
	return _horNumberOfDecimalsTextField->getValue ( );
}	// QwtAxisOptionsPanel::getHorNumberOfDecimals


void QwtAxisOptionsPanel::setHorNumberOfDecimals (unsigned char num)
{
	assert (0 != _horNumberOfDecimalsTextField);
	_horNumberOfDecimalsTextField->setValue (num);
}	// QwtAxisOptionsPanel::setHorNumberOfDecimals


void QwtAxisOptionsPanel::setHorMax (double max)
{
	assert (0 != _horMaxTextField);
	_horMaxTextField->setValue (max);
}	// QwtAxisOptionsPanel::setHorMax


char QwtAxisOptionsPanel::getHorFormat ( ) const
{	
	assert (0 != _horLabelFormatComboBox);
	return _horLabelFormatComboBox->getFormat ( );
}	// QwtAxisOptionsPanel::getHorFormat


void QwtAxisOptionsPanel::setHorFormat (char format)
{	
	assert (0 != _horLabelFormatComboBox);
	_horLabelFormatComboBox->setFormat (format);
}	// QwtAxisOptionsPanel::setHorFormat


double QwtAxisOptionsPanel::getHorMajorTickStep ( ) const
{
	assert (0 != _verMajorTickStepTextField);
	return _verMajorTickStepTextField->getValue ( );
}	// QwtAxisOptionsPanel::setHorMax


void QwtAxisOptionsPanel::setHorMajorTickStep (double step)
{
	assert (0 != _verMajorTickStepTextField);
	if (0. > step)
	{
		UTF8String	message (charset);
		message << "Affectation d'une valeur négative (" << step
		        << ") entre deux lignes verticales du graphique.";
		throw Exception (message);
	}	// if (0. > step)

	_verMajorTickStepTextField->setValue (step);
}	// QwtAxisOptionsPanel::setHorMajorTickStep


double QwtAxisOptionsPanel::getVerMin ( ) const
{
	assert (0 != _verMinTextField);
	return _verMinTextField->getValue ( );
}	// QwtAxisOptionsPanel::getVerMin


void QwtAxisOptionsPanel::setVerMin (double min)
{
	assert (0 != _verMinTextField);
	_verMinTextField->setValue (min);
}	// QwtAxisOptionsPanel::setVerMin


double QwtAxisOptionsPanel::getVerMax ( ) const
{
	assert (0 != _verMaxTextField);
	return _verMaxTextField->getValue ( );
}	// QwtAxisOptionsPanel::getVerMax


void QwtAxisOptionsPanel::setVerMax (double max)
{
	assert (0 != _verMaxTextField);
	_verMaxTextField->setValue (max);
}	// QwtAxisOptionsPanel::setVerMax


char QwtAxisOptionsPanel::getVerFormat ( ) const
{	
	assert (0 != _verLabelFormatComboBox);
	return _verLabelFormatComboBox->getFormat ( );
}	// QwtAxisOptionsPanel::getVerFormat


void QwtAxisOptionsPanel::setVerFormat (char format)
{	
	assert (0 != _verLabelFormatComboBox);
	_verLabelFormatComboBox->setFormat (format);
}	// QwtAxisOptionsPanel::setVerFormat


bool QwtAxisOptionsPanel::isVerNumberOfDecimalsEnabled ( ) const
{
	assert (0 != _verNumberOfDecimalsCheckBox);
	return _verNumberOfDecimalsCheckBox->isChecked ( );
}	// QwtAxisOptionsPanel::isVerNumberOfDecimalsEnabled


void QwtAxisOptionsPanel::enableVerNumberOfDecimals (bool enabled)
{
	assert (0 != _verNumberOfDecimalsCheckBox);
	assert (0 != _verNumberOfDecimalsTextField);
	_verNumberOfDecimalsCheckBox->setChecked (enabled);
	_verNumberOfDecimalsTextField->setEnabled (enabled);
}	// QwtAxisOptionsPanel::enableVerNumberOfDecimals


unsigned char QwtAxisOptionsPanel::getVerNumberOfDecimals ( ) const
{
	assert (0 != _verNumberOfDecimalsTextField);
	return _verNumberOfDecimalsTextField->getValue ( );
}	// QwtAxisOptionsPanel::getVerNumberOfDecimals


void QwtAxisOptionsPanel::setVerNumberOfDecimals (unsigned char num)
{
	assert (0 != _verNumberOfDecimalsTextField);
	_verNumberOfDecimalsTextField->setValue (num);
}	// QwtAxisOptionsPanel::setVerNumberOfDecimals


double QwtAxisOptionsPanel::getVerMajorTickStep ( ) const
{
	assert (0 != _horMajorTickStepTextField);
	return _horMajorTickStepTextField->getValue ( );
}	// QwtAxisOptionsPanel::getVerMajorTickStep


void QwtAxisOptionsPanel::setVerMajorTickStep (double step)
{
	assert (0 != _horMajorTickStepTextField);
	if (0. > step)
	{
		UTF8String	message (charset);
		message << "Affectation d'une valeur négative (" << step
		        << ") entre deux lignes horizontales du graphique.";
		throw Exception (message);
	}	// if (0. > step)

	_horMajorTickStepTextField->setValue (step);
}	// QwtAxisOptionsPanel::setVerMajorTickStep


QFont QwtAxisOptionsPanel::getAxisFont ( ) const
{
	assert (0 != _axisFontPanel);
	return _axisFontPanel->getFont ( );
}	// QwtAxisOptionsPanel::getAxisFont


void QwtAxisOptionsPanel::setAxisFont (const QFont& font)
{
	assert (0 != _axisFontPanel);
	_axisFontPanel->setFont (font);
}	// QwtAxisOptionsPanel::setAxisFont


QColor QwtAxisOptionsPanel::getAxisColor ( ) const
{
	assert (0 != _axisFontPanel);
	return _axisFontPanel->getColor ( );
}	// QwtAxisOptionsPanel::getAxisColor


void QwtAxisOptionsPanel::setAxisColor (const QColor& color)
{
	assert (0 != _axisFontPanel);
	_axisFontPanel->setColor (color);
}	// QwtAxisOptionsPanel::setAxisColor


void QwtAxisOptionsPanel::apply ( )
{
	if (true == hasChartPanel ( ))
	{
		getChartPanel ( ).getPlot ( ).setAxisTitle (
											QwtPlot::xBottom, getHorTitle ( ));
		getChartPanel ( ).getPlot ( ).setAxisTitle (
											QwtPlot::yLeft, getVerTitle ( ));
		getChartPanel ( ).setAbscissaScale (
							getHorMin ( ), getHorMax ( ), isHorAutoScale ( ),
							getHorMajorTickStep ( ));
		getChartPanel ( ).setOrdinateScale (
							getVerMin ( ), getVerMax ( ), isVerAutoScale ( ),
							getVerMajorTickStep ( ));

		QwtExtendedScaleDraw*	sd	=
				dynamic_cast<QwtExtendedScaleDraw*>(
				getChartPanel ( ).getPlot ( ).axisScaleDraw (QwtPlot::xBottom));
		if (0 != sd)
		{
			sd->setNumberOfDecimals (
					isHorNumberOfDecimalsEnabled( ), getHorNumberOfDecimals( ));
			sd->setFormat (getHorFormat ( ));
		}
		sd	= dynamic_cast<QwtExtendedScaleDraw*>(
					getChartPanel ( ).getPlot( ).axisScaleDraw(QwtPlot::yLeft));
		if (0 != sd)
		{
			sd->setNumberOfDecimals (
					isVerNumberOfDecimalsEnabled( ), getVerNumberOfDecimals( ));
			sd->setFormat (getVerFormat ( ));
		}

		const QFont		font		= getAxisFont ( );
		const QColor	titleColor	= getAxisColor ( );
		for (int axis = QwtPlot::yLeft; axis < QwtPlot::axisCnt; axis++)
		{
			getChartPanel ( ).getPlot ( ).setAxisFont (axis, font);
			if (0 != getChartPanel ( ).getPlot ( ).axisWidget (axis))
			{
				QPalette	p	=
					getChartPanel ( ).getPlot ( ).axisWidget (axis)->palette( );
				// WindowText : couleur des axes avec Qwt 5.2.0
//				p.setColor (QPalette::WindowText, titleColor);
				p.setColor (QPalette::Text, titleColor);
				getChartPanel ( ).getPlot ( ).axisWidget (axis)->setPalette (p);
			}
		}	// for (int axis = QwtPlot::yLeft; axis < QwtPlot::axisCnt; axis++)
		getChartPanel ( ).adjustChart ( );
	}	// if (true == hasChartPanel ( ))
}	// QwtAxisOptionsPanel::apply


QwtChartPanel& QwtAxisOptionsPanel::getChartPanel ( )
{
	if (0 == _chartPanel)
	{
		INTERNAL_ERROR (exc, "Absence de panneau graphique associé.", "QwtAxisOptionsPanel::getChartPanel")
		throw (exc);
	}	// if (0 == _chartPanel)

	return *_chartPanel;
}	// QwtAxisOptionsPanel::getChartPanel


const QwtChartPanel& QwtAxisOptionsPanel::getChartPanel ( ) const
{
	if (0 == _chartPanel)
	{
		INTERNAL_ERROR (exc, "Absence de panneau graphique associé.", "QwtAxisOptionsPanel::getChartPanel")
		throw (exc);
	}	// if (0 == _chartPanel)

	return *_chartPanel;
}	// QwtAxisOptionsPanel::getChartPanel


void QwtAxisOptionsPanel::editHorTitleCallback ( )
{
	assert (0 != _horTitleTextField);
	assert (0 != _horTitleFontPanel);

	try
	{
		UTF8String		title (QtUnicodeHelper::qstringToUTF8String (
												_horTitleTextField->text( )));
		QFont			font		= _horTitleFontPanel->getFont ( );
		QColor			textColor	= _horTitleFontPanel->getColor ( );
		QColor			background	= 0 == hasChartPanel ( ) ?
						Qt::white : getChartPanel ( ).getBackgroundColor ( );
		QtScientificTextDialog	dialog (
					this, UTF8STR ("Titre de l'axe des abscisses du graphique"),
					title, true, font, textColor, background);
		if (QDialog::Accepted != dialog.exec ( ))
			return;

		_horTitleTextField->setText (dialog.getQText ( ));
	}
	catch (const Exception& exc)
	{
		UTF8String	mess (charset);
		mess << "Erreur interne dans QwtAxisOptionsPanel::editHorTitleCallback ("
		     << __FILE__ << ' ' << (unsigned long)__LINE__ << ") : " << exc.getFullMessage ( );
		ConsoleOutput::cout ( ) << mess << co_endl;
	}
	catch (...)
	{
		UTF8String	mess (charset);
		mess << "Erreur interne dans QwtAxisOptionsPanel::editHorTitleCallback ("
		     << __FILE__ << ' ' << (unsigned long)__LINE__ << ") : erreur non documentée.";
		ConsoleOutput::cout ( ) << mess << co_endl;
	}
}	// QwtAxisOptionsPanel::editHorTitleCallback


void QwtAxisOptionsPanel::editVerTitleCallback ( )
{
	assert (0 != _verTitleTextField);
	assert (0 != _verTitleFontPanel);

	try
	{
		UTF8String		title (QtUnicodeHelper::qstringToUTF8String (
												_verTitleTextField->text( )));
		QFont			font		= _verTitleFontPanel->getFont ( );
		QColor			textColor	= _verTitleFontPanel->getColor ( );
		QColor			background	= 0 == hasChartPanel ( ) ?
						Qt::white : getChartPanel ( ).getBackgroundColor ( );
		QtScientificTextDialog	dialog (
					this, UTF8STR ("Titre de l'axe des ordonnées du graphique"),
					title, true, font, textColor, background);
		if (QDialog::Accepted != dialog.exec ( ))
			return;

		_verTitleTextField->setText (dialog.getQText ( ));
	}
	catch (const Exception& exc)
	{
		UTF8String	mess (charset);
		mess << "Erreur interne dans QwtAxisOptionsPanel::editVerTitleCallback ("
		     << __FILE__ << ' ' << (unsigned long)__LINE__ << ") : " << exc.getFullMessage ( );
		ConsoleOutput::cout ( ) << mess << co_endl;
	}
	catch (...)
	{
		UTF8String	mess (charset);
		mess << "Erreur interne dans QwtAxisOptionsPanel::editVerTitleCallback ("
		     << __FILE__ << ' ' << (unsigned long)__LINE__ << ") : erreur non documentée.";
		ConsoleOutput::cout ( ) << mess << co_endl;
	}
}	// QwtAxisOptionsPanel::editVerTitleCallback


void QwtAxisOptionsPanel::autoScaleCallback (int)
{
	assert (0 != _horMinTextField);
	assert (0 != _horMaxTextField);
	assert (0 != _verMinTextField);
	assert (0 != _verMaxTextField);
	assert (0 != _verMajorTickStepTextField);
	assert (0 != _horMajorTickStepTextField);
	_horMinTextField->setEnabled (!isHorAutoScale ( ));
	_horMaxTextField->setEnabled (!isHorAutoScale ( ));
	_verMinTextField->setEnabled (!isVerAutoScale ( ));
	_verMaxTextField->setEnabled (!isVerAutoScale ( ));
	_verMajorTickStepTextField->setEnabled (!isHorAutoScale ( ));
	_horMajorTickStepTextField->setEnabled (!isVerAutoScale ( ));
}	// QwtAxisOptionsPanel::autoScaleCallback


void QwtAxisOptionsPanel::enableNumberOfDecimalsCallback (int)
{
	assert (0 != _horNumberOfDecimalsTextField);
	assert (0 != _verNumberOfDecimalsTextField);
	_horNumberOfDecimalsTextField->setEnabled (isHorNumberOfDecimalsEnabled ( ));
	_verNumberOfDecimalsTextField->setEnabled (isVerNumberOfDecimalsEnabled ( ));
}	// QwtAxisOptionsPanel::enableNumberOfDecimalsCallback



