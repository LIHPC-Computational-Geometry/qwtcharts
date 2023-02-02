#include "QtCurvesMainWindow.h"
#include "QwtCharts/QwtChartsManager.h"
#include <QtUtil/QtPaintAttributes.h>
#include <QtUtil/QtUnicodeHelper.h>
#include <TkUtil/AutoArrayPtr.h>
#include <QApplication>
#include <QContextMenuEvent>
#include <QMenuBar>
#include <QLayout>

#include <stdlib.h>		// rand

// Pour tests alphabet grec :
#include <QtUtil/QtStringHelper.h>

using namespace std;
using namespace TkUtil;

static const TkUtil::Charset	charset ("àéèùô");

USE_ENCODING_AUTODETECTION



// ============================================================================
//                            FONCTIONS STATIQUES
// ============================================================================

static QwtPlotCurve* createCurve (
						QwtCurveChartPanel& panel, QtCurveView::CURVE_TYPE type)
{
	PaintAttributes	pa (PaintAttributes::create (type));
	QString	name;
	switch (type)
	{
		case QtCurveView::SIN	: name	= "y = 10 * sin (x)";	break;
		case QtCurveView::COS	: name	= "y = cos (x)";	break;
		case QtCurveView::TAN	: name	= "y = tan (x)";	break;
	};
	QwtExtendedPlotCurve&	curve	= panel.addCurve (name);
	curve.setPaintAttributes (pa);
	curve.setRenderHint (QwtPlotItem::RenderAntialiased);
//	QPen	pen (Qt::red);
//	curve.setPen (pen);

	AutoArrayPtr<double>	xData (720);
	AutoArrayPtr<double>	yData (720);
	for (int i = 0; i < 720; i++)
	{
		double	rad	= 2 * M_PI * (i - 360) / 360;
//double	rad	= i * 5. / 719.;	// Quart de cercle
		xData [i]	= rad;
		switch (type)
		{
			case QtCurveView::SIN	:
//yData [i]	= sqrt (25 - rad * rad);	// Quart de cercle
				yData [i]	= 10 * sin (rad);
				break;
			case QtCurveView::COS	:
				yData [i]	= cos (rad);
				break;
			case QtCurveView::TAN	:
				yData [i]	= tan (rad);
				break;
		}	// switch (type)
	}	// for (int i = 0; i < 720; i++)
	curve.setSamples (xData.get ( ), yData.get ( ), 720);
	// Mise à jour du graphique, notamment pour que la fonction d'annulation du
	// zoom fonctionne.
	panel.dataModified ( );
	if (QtCurveView::SIN == type)
	{
		panel.allowCurveEdition (true);
		panel.allowAbscissaTranslation (false);
	}

	return &curve;
}	// createCurve


static void createSerie (
		unsigned char seriesNum, unsigned short max, QVector<double>& values)
{
	values.resize (seriesNum);
	for (unsigned short j = 0; j < seriesNum; j++)
	{
		double	value	= (double)(rand( ) % max);
		values [j]	= value;
	}	// for (unsigned short j = 0; j < seriesNum; j++)
}	// createSerie



// ============================================================================
//                       LA CLASSE QtCurveView
// ============================================================================

QtCurveView::QtCurveView (QMdiArea* parent, CURVE_TYPE type)
	: QMdiSubWindow (parent), _type (type), _plotWidget (0), _contextMenu (0),
	  _dragging (false)
{
	_plotWidget	= new QwtCurveChartPanel (this);
	_plotWidget->allowAbscissaTranslation (false);
	_plotWidget->resize (1200, 800);
	layout ( )->addWidget (_plotWidget);
	connect (_plotWidget, SIGNAL (displayModeModified (QwtChartPanel*)), this,
	         SLOT (displayModeCallback (QwtChartPanel*)));
	_plotWidget->setAbscissaTitle (QwtText ("X"));
	_plotWidget->setOrdinateTitle (QwtText ("Y"));
#ifdef USE_EXTENDED_PLOT_RESCALER
	_plotWidget->setAspectRatio (1.);
	_plotWidget->useAspectRatio (true);
#endif	// USE_EXTENDED_PLOT_RESCALER
	if (COS == type)
		_plotWidget->enableDisplayModes (QwtChartPanel::CARTESIAN);
	switch (_type)
	{
		case	ALL	:
			createCurve (*_plotWidget, SIN);
			createCurve (*_plotWidget, COS);
			createCurve (*_plotWidget, TAN);
			break;
		default	: createCurve (*_plotWidget, _type);	break;
	};
}	// QtCurveView::QtCurveView


void QtCurveView::setContextMenu (QMenu* menu)
{
	_contextMenu	= menu;
}	// QtCurveView::setContextMenu


void QtCurveView::contextMenuEvent (QContextMenuEvent* event)
{
	if (0 != _contextMenu)
		_contextMenu->popup (event->globalPos ( ));
}	// QtCurveView::contextMenuEvent


void QtCurveView::displayModeCallback (QwtChartPanel* panel)
{	// On refait les courbes :
	assert (0 != panel);
	assert (0 != _plotWidget);
	while (0 != _plotWidget->getItemCount ( ))
		_plotWidget->removeCurve (&_plotWidget->getCurve (0));

	switch (_type)
	{
		case	ALL	:
			createCurve (*_plotWidget, SIN);
			createCurve (*_plotWidget, COS);
			createCurve (*_plotWidget, TAN);
			break;
		default	: createCurve (*_plotWidget, _type);	break;
	};

	if (QwtChartPanel::POLAR == panel->getDisplayMode ( ))
	{
		QwtCurveChartPanel*	curvePanel	=
								dynamic_cast<QwtCurveChartPanel*>(panel);
		const size_t	count	= curvePanel->getItemCount ( );
		for (size_t i = 0; i < count; i++)
		{
			QwtExtendedPlotCurve&	curve	= curvePanel->getCurve (i);
			AutoArrayPtr<double>	xData (curve.dataSize ( ) + 1);
			AutoArrayPtr<double>	yData (curve.dataSize ( ) + 1);
			for (size_t p = 0; p < curve.dataSize ( ); p++)
			{
				const double	x	= curve.sample (p).x ( );
				const double	y	= curve.sample (p).y ( );
				const double	r	= sqrt (x * x + y * y);
				double	a	= 0. == x ? 0. : atan (y / x);
				if (x < 0.)
					a	= M_PI + a;
				else if (y < 0.)
					a	= 2 * M_PI - a;
				xData [p]	= r * cos (a);
				yData [p]	= r * sin (a);
			}	// for (size_t p = 0; p < curve.dataSize ( ); p++)
			xData [curve.dataSize ( )]	= xData [0];
			yData [curve.dataSize ( )]	= yData [0];
			curve.setSamples(xData.get( ), yData.get( ), curve.dataSize( ) + 1);
		}	// for (size_t i = 0; i < count; i++)	
	}	// if (QwtChartPanel::POLAR == panel->getDisplayMode ( ))
}	// QtCurveView::displayModeCallback


void QtCurveView::mousePressEvent (QMouseEvent* mouseEvent)
{
	QPoint	p			= mouseEvent->pos ( );
	QRegion	childRegion	= childrenRegion ( );
	if ((false == childRegion.contains (p)) || (true == _dragging))
	{
		_dragging	= true;
		QMdiSubWindow::mousePressEvent (mouseEvent);
	}
	else
	{
		// Si on laisse faire cette opération de dragging dans la légende
		// va finir par provoquer un redimensionnement de la fenêtre.
		// On mémorise qu'on est en cours de dragging ammorcé en dehors de la
		// bordure car sinon si le curseur passe dessus on aura alors
		// true == childRect.contains (p) et on laissera faire.
		_dragging	= false;
		mouseEvent->ignore ( );
	}
}	// QtCurveView::mousePressEvent


void QtCurveView::mouseReleaseEvent (QMouseEvent* mouseEvent)
{
	if (true == _dragging)
	{
		_dragging	= false;
		QMdiSubWindow::mouseReleaseEvent (mouseEvent);
	}
}	// QtCurveView::mouseReleaseEvent


void QtCurveView::mouseMoveEvent (QMouseEvent* mouseEvent)
{
	QPoint	p			= mouseEvent->pos ( );
	QRegion	childRegion	= childrenRegion ( );
	if ((true == _dragging) || (false == childRegion.contains (p)))
		QMdiSubWindow::mouseMoveEvent (mouseEvent);
	else
		_dragging	= false;
}	// QtCurveView::mouseMoveEvent


// ============================================================================
//                       LA CLASSE QtHistogramView
// ============================================================================

QtHistogramView::QtHistogramView (QMdiArea* parent, unsigned char series)
	: QMdiSubWindow (parent), _plotWidget (0), _contextMenu (0)
{
	_plotWidget	= new QwtHistogramChartPanel (this);
	_plotWidget->resize (1200, 800);
	layout ( )->addWidget (_plotWidget);
	_plotWidget->setAbscissaTitle (QwtText ("X"));
	_plotWidget->setOrdinateTitle (QwtText ("Y"));
//#ifdef USE_EXTENDED_PLOT_RESCALER
//	_plotWidget->setAspectRatio (1.);
//	_plotWidget->useAspectRatio (true);
//#endif	// USE_EXTENDED_PLOT_RESCALER
//
	if ((0 == series) || (10 < series))
	{
		cout << "createHistogram, invalid number of series (" << series
		<< "). Series should be comprise between 1 and 10 included."
		<< endl;
		series  = 1;
	}

	const unsigned short    classNum    = 10;
	const unsigned short    max         = 1000;

	QList<QwtText>					seriesNames;
	QVector<double>					classValues;
	QVector< QVector <double> >		seriesValues;

	for (unsigned char n  = 0; n < series; n++)
	{
		UTF8String   name (charset);
		name << "Série " << (unsigned long)n;
		seriesNames.append (QwtText (UTF8TOQSTRING (name)));
	}	// for (unsigned char n  = 0; n < series; n++)

	for (unsigned char i = 0; i < classNum; i++)
	{
		classValues.push_back (i * 0.05);

		QVector<double>	values;
		createSerie (series, max, values);
		seriesValues.push_back (values);
	}	// for (unsigned char i = 0; i < classNum; i++)

	_plotWidget->setSeries (seriesNames, classValues, seriesValues);
	_plotWidget->dataModified ( );
}	// QtHistogramView::QtHistogramView


void QtHistogramView::setContextMenu (QMenu* menu)
{
	_contextMenu	= menu;
}	// QtHistogramView::setContextMenu


void QtHistogramView::contextMenuEvent (QContextMenuEvent* event)
{
	if (0 != _contextMenu)
		_contextMenu->popup (event->globalPos ( ));
}	// QtHistogramView::contextMenuEvent


// ============================================================================
//                       LA CLASSE QtChartsMainWindow
// ============================================================================
//
QtChartsMainWindow::QtChartsMainWindow (QWidget* parent)
	: QMainWindow (parent), _mdiArea (0), _windowMenu (0)
{
	setWindowTitle ("Courbes");
	_mdiArea	= new QMdiArea (this);
	// Les panneaux graphiques ne reçoivent pas l'évènement focusInEvent
	// car c'est un ancêtre (QtCurveView) qui le recevra => s'informer du
	// changement de fenêtre active afin d'en faire part au gestionnaire
	// de graphiques :
	connect (_mdiArea, SIGNAL (subWindowActivated (QMdiSubWindow*)), this,
	         SLOT (subWindowActivated (QMdiSubWindow*)));
	setCentralWidget (_mdiArea);

	for (size_t i = 0; i < (size_t)QtCurveView::ALL + 1; i++)
		_curveViews [i]	= 0;
	for (size_t h = 0; h < 3; h++)
		_histogramViews [h]	= 0;

	// Création du menu :
	QAction*	action	= 0;
	QMenuBar*	bar	= menuBar ( );
	QMenu*		menu	= bar->addMenu ("Application");
	menu->addAction ("Inverser le sens de rotation pour le zoom", this,
	                 SLOT (invertZoomRotationCallback ( )));
	menu->addAction ("Quitter", this, SLOT (exitCallback ( )));
	_windowMenu	= bar->addMenu (QSTR ("Fenêtre"));
	_windowMenu->addAction ("Cascade", this, SLOT (cascadeCallback ( )));
	_windowMenu->addAction ("Tuiles", this, SLOT (tileCallback ( )));
	_windowMenu->addSeparator ( );
	_windowMenu->addAction (QwtChartsManager::getPrintAction ( ));
	QwtChartsManager::getPrintAction ( )->setShortcutContext (
													Qt::ApplicationShortcut);
	_windowMenu->addSeparator ( );
	_windowMenu->addAction ("sin (x)", this, SLOT (sinCallback ( )));
	_windowMenu->addAction ("cos (x)", this, SLOT (cosCallback ( )));
	_windowMenu->addAction ("tan (x)", this, SLOT (tanCallback ( )));
	_windowMenu->addAction ("sin/cos/tan (x)", this, SLOT (allCallback ( )));
	_windowMenu->addSeparator ( );
	_windowMenu->addAction (QSTR ("Histogramme 1 série"), this,
	                        SLOT (histogram1Callback ( )));
	_windowMenu->addAction (QSTR ("Histogramme 3 séries"), this,
	                        SLOT (histogram3Callback ( )));
	_windowMenu->addAction (QSTR ("Histogramme 5 séries"), this,
	                        SLOT (histogram5Callback ( )));

	// Utilisation de la barre d'outils par défaut de QwtChartsManager :
	addToolBar (Qt::LeftToolBarArea, &QwtChartsManager::getToolBar ( ));
}	// QtChartsMainWindow::QtChartsMainWindow


void QtChartsMainWindow::invertZoomRotationCallback ( )
{
	QwtChartPanel::mouseZoomUp	= !QwtChartPanel::mouseZoomUp;
}	// QtChartsMainWindow::invertZoomRotationCallback


void QtChartsMainWindow::exitCallback ( )
{
	QApplication::exit ( );
}	// QtChartsMainWindow::exitCallback


void QtChartsMainWindow::tileCallback ( )
{
	_mdiArea->tileSubWindows ( );
}	// QtChartsMainWindow::tileCallback


void QtChartsMainWindow::cascadeCallback ( )
{
	_mdiArea->cascadeSubWindows ( );
}	// QtChartsMainWindow::cascadeCallback


void QtChartsMainWindow::sinCallback ( )
{
	QtCurveView*	view	= _curveViews [QtCurveView::SIN];
	if (0 != view)
	{
		view->showNormal ( );
		_mdiArea->setActiveSubWindow (view);
	}
	else
		createCurveView (QtCurveView::SIN);
}	// QtChartsMainWindow::sinCallback


void QtChartsMainWindow::cosCallback ( )
{
	QtCurveView*	view	= _curveViews [QtCurveView::COS];
	if (0 != view)
	{
		view->showNormal ( );
		_mdiArea->setActiveSubWindow (view);
	}
	else
		createCurveView (QtCurveView::COS);
}	// QtChartsMainWindow::cosCallback


void QtChartsMainWindow::tanCallback ( )
{
	QtCurveView*	view	= _curveViews [QtCurveView::TAN];
	if (0 != view)
	{
		view->showNormal ( );
		_mdiArea->setActiveSubWindow (view);
	}
	else
		createCurveView (QtCurveView::TAN);
}	// QtChartsMainWindow::tanCallback


void QtChartsMainWindow::allCallback ( )
{
	QtCurveView*	view	= _curveViews [QtCurveView::ALL];
	if (0 != view)
	{
		view->showNormal ( );
		_mdiArea->setActiveSubWindow (view);
	}
	else
		createCurveView (QtCurveView::ALL);
}	// QtChartsMainWindow::allCallback


void QtChartsMainWindow::histogram1Callback ( )
{
	QtHistogramView*	view	= _histogramViews [0];
	if (0 == view)
	{
		createHistogramView (0);
	}
	else
	{
		view->showNormal ( );
		_mdiArea->setActiveSubWindow (view);
	}
}	// QtChartsMainWindow::histogram1Callback


void QtChartsMainWindow::histogram3Callback ( )
{
	QtHistogramView*	view	= _histogramViews [1];
	if (0 == view)
	{
		createHistogramView (1);
	}
	else
	{
		view->showNormal ( );
		_mdiArea->setActiveSubWindow (view);
	}
}	// QtChartsMainWindow::histogram3Callback


void QtChartsMainWindow::histogram5Callback ( )
{
	QtHistogramView*	view	= _histogramViews [2];
	if (0 == view)
	{
		createHistogramView (2);
	}
	else
	{
		view->showNormal ( );
		_mdiArea->setActiveSubWindow (view);
	}
}	// QtChartsMainWindow::histogram5Callback


QtCurveView* QtChartsMainWindow::createCurveView (QtCurveView::CURVE_TYPE type)
{
	QtCurveView*	view	= new QtCurveView (_mdiArea, type);
	_mdiArea->addSubWindow (view);
	view->show ( );
	QMenu*	menu	= new QMenu (view);
	menu->addMenu (_windowMenu);
	QMenu*	graphicMenu	= QwtChartsManager::createMenu (0, "Graphique ...");
	menu->addMenu (graphicMenu);	
	view->setContextMenu (menu);
	_curveViews [type]	= view;

	return view;
}	// QtChartsMainWindow::createCurveView


QtHistogramView* QtChartsMainWindow::createHistogramView (unsigned char num)
{
	if (0 != _histogramViews [num])
		return _histogramViews [num];
	unsigned char	seriesNum	= 0;
	switch (num)
	{
		case	0	:	seriesNum	= 1;	break;
		case	1	:	seriesNum	= 3;	break;
		case	2	:	seriesNum	= 5;	break;
	}	// switch (num)
	QtHistogramView*	view	= new QtHistogramView (_mdiArea, seriesNum);
	_mdiArea->addSubWindow (view);
	view->show ( );
	QMenu*	menu	= new QMenu (view);
	menu->addMenu (_windowMenu);
	QMenu*	graphicMenu	= QwtChartsManager::createMenu (0, "Graphique ...");
	menu->addMenu (graphicMenu);	
	view->setContextMenu (menu);
	_histogramViews [num]	= view;

	return view;
}	// QtChartsMainWindow::createHistogramView


void QtChartsMainWindow::subWindowActivated (QMdiSubWindow* window)
{
	QtCurveView*		curveView	= dynamic_cast<QtCurveView*>(window);
	QtHistogramView*	histView	= dynamic_cast<QtHistogramView*>(window);
	QwtChartPanel*	panel		= 0;
	if (0 != curveView)
		panel	= &curveView->getChartPanel ( );
	else if (0 != histView)
		panel	= &histView->getChartPanel ( );

	QwtChartsManager::getManager ( ).focusChanged (panel);
}	// QtChartsMainWindow::subWindowActivated
