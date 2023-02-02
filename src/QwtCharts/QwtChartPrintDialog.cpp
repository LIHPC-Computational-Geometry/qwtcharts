#include "QwtCharts/QwtChartPrintDialog.h"

#include <QLayout>

#include <assert.h>
#include <iostream>


USING_STD


QwtChartPrintDialog::QwtChartPrintDialog (QPrinter* printer, QWidget* parent)
	: QPrintDialog (printer, parent)
//	  , _aspectRatioCheckBox (0)
{
	// On essaye de retrouver le parent de la case à cocher "Rapport d'aspect".
	// C'est assez hideux :
/* ============================================= Gardé juste pour info ...
	QWidget*	general	= findChild <QWidget*>("generalWidget");
	QWidget*	other	= 0 == general ?
						  0 : general->findChild <QWidget*>("gbOther");
	if (0 != other)
	{
		QLayout*	layout	= other->layout ( );
		_aspectRatioCheckBox	=
					new QCheckBox ("Maintient du rapport d'aspect", other);
		_aspectRatioCheckBox->setChecked (keepAspectRatio);
		_aspectRatioCheckBox->setEnabled (enableAspectRatio);
		if (0 != layout)
			layout->addWidget (_aspectRatioCheckBox);
	}	// if (0 != other)
	else
		cout << "QwtChartPrintDialog::QwtChartPrintDialog, ERREUR : "
		     << "le widget \"autres options\" est introuvable, impossibilité "
		     << "de proposer l'option \"Maintient du rapport d'aspect\"."
		     << endl;
============================================ */
}	// QwtChartPrintDialog::QwtChartPrintDialog


QwtChartPrintDialog::QwtChartPrintDialog (const QwtChartPrintDialog&)
	: QPrintDialog ((QPrinter*)0)
//	, _aspectRatioCheckBox (0)
{
	assert (0 && "QwtChartPrintDialog copy constructor is not allowed.");
}	// QwtChartPrintDialog::QwtChartPrintDialog


QwtChartPrintDialog& QwtChartPrintDialog::operator =(const QwtChartPrintDialog&)
{
	assert (0 && "QwtChartPrintDialog operator = is not allowed.");
	return *this;
}	// QwtChartPrintDialog::operator =


QwtChartPrintDialog::~QwtChartPrintDialog ( )
{
}	// QwtChartPrintDialog::~QwtChartPrintDialog

/*
bool QwtChartPrintDialog::keepAspectRatio ( ) const
{
	return 0 == _aspectRatioCheckBox ?
	       false : _aspectRatioCheckBox->isChecked ( );
}	// QwtChartPrintDialog::keepAspectRatio
*/

