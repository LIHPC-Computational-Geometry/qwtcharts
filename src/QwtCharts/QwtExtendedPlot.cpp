#include "QwtCharts/QwtExtendedPlot.h"

#include <QEvent>
#include <QApplication>


#include <assert.h>
#include <iostream>
using namespace std;


QwtExtendedPlot::QwtExtendedPlot (QWidget* parent)
	: QwtPlot (parent), _lockCanvas (false)
{
}	// QwtExtendedPlot::QwtExtendedPlot


QwtExtendedPlot::QwtExtendedPlot (const QwtText& title, QWidget* parent)
	: QwtPlot (title, parent), _lockCanvas (false)
{
}	// QwtExtendedPlot::QwtExtendedPlot


QwtExtendedPlot::QwtExtendedPlot (const QwtExtendedPlot&)
	: QwtPlot (0)
{
	assert (0 && "QwtExtendedPlot copy constructor is not allowed.");
}	// QwtExtendedPlot::QwtExtendedPlot


QwtExtendedPlot& QwtExtendedPlot::operator = (const QwtExtendedPlot& pr)
{
	assert (0 && "QwtExtendedPlot::operator = is not allowed.");

	return *this;
}	// QwtExtendedPlot::::operator =


QwtExtendedPlot::~QwtExtendedPlot ( )
{
}	// QwtExtendedPlot::~QwtExtendedPlot


void QwtExtendedPlot::replot ( )
{	// Code de la version 5.2.0 de QwtPlot::replot
	if (true == _lockCanvas)
	{
		bool doAutoReplot = autoReplot();
		setAutoReplot(false);

		updateAxes();

    /*
      Maybe the layout needs to be updated, because of changed
      axes labels. We need to process them here before painting
      to avoid that scales and canvas get out of sync.
     */
#if QT_VERSION >= 0x040000
		QApplication::sendPostedEvents(this, QEvent::LayoutRequest);
#else
		QApplication::sendPostedEvents(this, QEvent::LayoutHint);
#endif

	// Ligne à l'origine des problèmes de raffraichissement :
//	d_data->canvas->replot();

		setAutoReplot(doAutoReplot);
	}
	else
		QwtPlot::replot ( );
}	// void QwtExtendedPlot::replot ( )

