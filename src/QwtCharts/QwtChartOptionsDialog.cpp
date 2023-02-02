#include "QwtCharts/QwtChartOptionsDialog.h"
#include <QtUtil/QtAutoWaitingCursor.h>
#include <QtUtil/QtUnicodeHelper.h>
#include <QtUtil/QtConfiguration.h>
#include <QtUtil/QtMessageBox.h>
#include <TkUtil/Exception.h>
#include <TkUtil/InternalError.h>
#include <TkUtil/MemoryError.h>

#include <QVBoxLayout>
#include <QLabel>

#include <assert.h>


USING_UTIL
USING_STD

static const TkUtil::Charset	charset ("àéèùô");

USE_ENCODING_AUTODETECTION


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


QwtChartOptionsDialog::QwtChartOptionsDialog (
					QWidget* parent, QwtChartPanel* chartPanel, bool modal,
					const string& helpURL, const string& helpTag)
	: QDialog (parent),
	  _chartOptionsPanel (0), _dlgClosurePanel (0)
{
	setWindowTitle (QSTR ("Editions des propriétés du graphique"));
	setModal (modal);
	if (true == modal)
		setWindowModality (Qt::WindowModal);

	QVBoxLayout*	layout	= new QVBoxLayout (this);
	layout->setMargin (QtConfiguration::margin);
	layout->setSizeConstraint (QLayout::SetMinimumSize);
	_chartOptionsPanel	= new QwtChartOptionsPanel (this, chartPanel);
	layout->addWidget (_chartOptionsPanel);

	layout->addWidget (new QLabel (" ", this));

	_dlgClosurePanel	=
				new QtDlgClosurePanel (this, true, "Appliquer", "Fermer", "",
				                       helpURL, helpTag);
	layout->addWidget (_dlgClosurePanel);
	_dlgClosurePanel->setMinimumSize (_dlgClosurePanel->sizeHint ( ));
	connect (_dlgClosurePanel->getApplyButton ( ), SIGNAL(clicked ( )), this,
	         SLOT (applyCallback ( )));
	connect (_dlgClosurePanel->getCloseButton ( ), SIGNAL(clicked ( )), this,
	         SLOT (accept ( )));
	_dlgClosurePanel->getApplyButton ( )->setAutoDefault (false);
	_dlgClosurePanel->getApplyButton ( )->setDefault (false);
	_dlgClosurePanel->getCloseButton ( )->setAutoDefault (false);
	_dlgClosurePanel->getCloseButton ( )->setDefault (false);

	layout->activate ( );
	setMinimumSize (layout->sizeHint ( ));

	// Etre informé d'éventuelles modifications du graphique :
	if (0 != chartPanel)
		chartPanel->setEditionDialog (this);
}	// QwtChartOptionsDialog::QwtChartOptionsDialog


QwtChartOptionsDialog::QwtChartOptionsDialog (const QwtChartOptionsDialog& view)
	: QDialog (0), _chartOptionsPanel (0), _dlgClosurePanel (0)
{
	assert (0 && "QwtChartOptionsDialog copy constructor is not allowed.");
}	// QwtChartOptionsDialog::QwtChartOptionsDialog (const QwtChartOptionsDialog& view)


QwtChartOptionsDialog& QwtChartOptionsDialog::operator = (
											const QwtChartOptionsDialog& view)
{
	assert (0 && "QwtChartOptionsDialog::operator = is not allowed.");
	return *this;
}	// QwtChartOptionsDialog::QwtChartOptionsDialog (const QwtChartOptionsDialog& view)


QwtChartOptionsDialog::~QwtChartOptionsDialog ( )
{
	if (0 != _chartOptionsPanel)
		_chartOptionsPanel->getChartPanel ( ).setEditionDialog (0);
}	// QwtChartOptionsDialog::~QwtChartOptionsDialog


QwtChartOptionsPanel& QwtChartOptionsDialog::operator -> ( )
{
	assert (0 != _chartOptionsPanel);
	return *_chartOptionsPanel;
}	// QwtChartOptionsDialog::operator


const QwtChartOptionsPanel& QwtChartOptionsDialog::operator -> ( ) const
{
	assert (0 != _chartOptionsPanel);
	return *_chartOptionsPanel;
}	// QwtChartOptionsDialog::operator


void QwtChartOptionsDialog::curveAdded (QwtExtendedPlotCurve* curve)
{
	assert (0 != _chartOptionsPanel);
	BEGIN_TRY_CATCH_BLOCK

	QtAutoWaitingCursor	cursor (true);
	_chartOptionsPanel->curveAdded (curve);

	COMPLETE_TRY_CATCH_BLOCK

	if (true == hasError)
	{
		QtMessageBox::displayErrorMessage (this, "Graphique", errorMsg);
	}
}	// QwtChartOptionsDialog::curveAdded


void QwtChartOptionsDialog::curveRemoved (QwtExtendedPlotCurve* curve)
{
	assert (0 != _chartOptionsPanel);
	BEGIN_TRY_CATCH_BLOCK

	QtAutoWaitingCursor	cursor (true);
	_chartOptionsPanel->curveRemoved (curve);

	COMPLETE_TRY_CATCH_BLOCK

	if (true == hasError)
	{
		QtMessageBox::displayErrorMessage (this, "Graphique", errorMsg);
	}
}	// QwtChartOptionsDialog::curveRemoved


void QwtChartOptionsDialog::applyCallback ( )
{
	assert (0 != _chartOptionsPanel);
	BEGIN_TRY_CATCH_BLOCK

	QtAutoWaitingCursor	cursor (true);
	_chartOptionsPanel->apply ( );

	COMPLETE_TRY_CATCH_BLOCK

	if (true == hasError)
	{
		QtMessageBox::displayErrorMessage (this, "Graphique", errorMsg);
	}
}	// QwtChartOptionsDialog::applyCallback



