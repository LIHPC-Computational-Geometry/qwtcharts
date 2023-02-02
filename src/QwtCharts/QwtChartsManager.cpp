#include "QwtCharts/QwtChartsManager.h"
#include "QwtCharts/QwtCurveChartPanel.h"
#include "QwtCharts/QwtChartOptionsDialog.h"
#include <QtUtil/QtUnicodeHelper.h>
#include <TkUtil/Exception.h>
#include <TkUtil/InternalError.h>
#include <TkUtil/MemoryError.h>
#include <QtUtil/QtActionAutoLock.h>
#include <QtUtil/QtAutoWaitingCursor.h>
#include <QtUtil/QtMessageBox.h>
#include <QwtCharts/QwtChartPanel.h>

#include <QColorDialog>
#include <QInputDialog>
#include <QPrinter>
#include <QPrintEngine>
#include <QMdiSubWindow>
#include <QToolButton>


#include "QwtCharts/QwtChartPrintDialog.h"

#include <memory>
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
	}                                                                      \
                                                                           \
    if (true == hasError)                                                  \
		displayErrorMessage (window, errorMsg);


static QwtChartPanel*	currentPanel	= 0;


/**
 * @return		La première instance de <I>QwtChartPanel</I> retrouvée dans la
 *				descendance de <I>root</I> (<I>root</I> y compris), ou 0.
 */
static QwtChartPanel* getChartPanel (QWidget* root)
{
	if (0 == root)
		return 0;
	QwtChartPanel*	chartPanel	= dynamic_cast<QwtChartPanel*>(root);
	if (0 != chartPanel)
		return chartPanel;

	QObjectList	children	= root->children ( );
	for (QObjectList::iterator it = children.begin ( ); children.end ( ) != it;
	     it++)
	{
		chartPanel	= getChartPanel (dynamic_cast<QWidget*>(*it));
		if (0 != chartPanel)
			return chartPanel;
	}	// for (QObjectList::iterator it = children.begin ( ); ...

	return 0;
}	// getChartPanel


/**
 * @return		La première instance de <I>QwtChartPanel</I> retrouvée dans la
 *				descendance de la fenêtre de l'application ayant le focus.
 */


string					QwtChartsManager::editionDialogHelpURL;
string					QwtChartsManager::editionDialogHelpTag;
QwtChartsManager*		QwtChartsManager::_manager				= 0;
QToolBar*				QwtChartsManager::_toolBar				= 0;
vector<QwtChartPanel*>	QwtChartsManager::_panels;
bool					QwtChartsManager::_zoomMode				= false;
QwtPlotCoordinatesPicker::SELECTION_MODE QwtChartsManager::_selectionMode	=
												QwtPlotCoordinatesPicker::CURVE;
QAction*				QwtChartsManager::_zoomAction			= 0;
QAction*				QwtChartsManager::_cancelZoomAction		= 0;
QAction*				QwtChartsManager::_adjustScalesAction	= 0;
QAction*				QwtChartsManager::_undoZoomAction		= 0;
QAction*				QwtChartsManager::_redoZoomAction		= 0;
QtExclusiveActionGroup*	QwtChartsManager::_selectionModeAction	= 0;
QAction*				QwtChartsManager::_unselectAction		= 0;
QAction*				QwtChartsManager::_undoLastModifiedPointAction	= 0;
QAction*				QwtChartsManager::_printAction			= 0;
QAction*				QwtChartsManager::_editChartAction		= 0;
QAction*				QwtChartsManager::_backgroundAction		= 0;
QAction*				QwtChartsManager::_marginAction			= 0;
QAction*				QwtChartsManager::_showLegendAction		= 0;
QAction*				QwtChartsManager::_topLegendAction		= 0;
QAction*				QwtChartsManager::_bottomLegendAction	= 0;
QAction*				QwtChartsManager::_rightLegendAction	= 0;
QAction*				QwtChartsManager::_leftLegendAction		= 0;
QAction*				QwtChartsManager::_legendMarginAction	= 0;
QAction*				QwtChartsManager::_legendFrameWidthAction= 0;
QAction*				QwtChartsManager::_verMajorLinesAction	= 0;
QAction*				QwtChartsManager::_verMinorLinesAction	= 0;
QAction*				QwtChartsManager::_horMajorLinesAction	= 0;
QAction*				QwtChartsManager::_horMinorLinesAction	= 0;
const Version			QwtChartsManager::_version (QWT_CHARTS_VERSION);


QwtChartsManager::QwtChartsManager ( )
{
	assert (0 == _manager);
	if (0 != _toolBar)
	{
		INTERNAL_ERROR (exc, "Barre d'outil initialisée.", "QwtChartsManager::QwtChartsManager")
		throw exc;
	}	// if (0 != _toolBar)
}	// QwtChartsManager::QwtChartsManager


QwtChartsManager::QwtChartsManager (const QwtChartsManager& view)
{
	assert (0 && "QwtChartsManager copy constructor is not allowed.");
}	// QwtChartsManager::QwtChartsManager (const QwtChartsManager& view)


QwtChartsManager& QwtChartsManager::operator = (const QwtChartsManager& view)
{
	assert (0 && "QwtChartsManager::operator = is not allowed.");
	return *this;
}	// QwtChartsManager::QwtChartsManager (const QwtChartsManager& view)


QwtChartsManager::~QwtChartsManager ( )
{
	assert (this == _manager);
	_manager	= 0;
}	// QwtChartsManager::~QwtChartsManager


void QwtChartsManager::initialize ( )
{
	if (0 != _manager)
		throw Exception(UTF8STR ("QwtChartsManager::initialize : API déjà initialisée."));

	Q_INIT_RESOURCE (QwtCharts);

	_panels.clear ( );

	_manager	= new QwtChartsManager ( );
	CHECK_NULL_PTR_ERROR (_manager)

	// Création des actions :
	if (0 == _printAction)
	{
		_printAction	=
			new QAction (QIcon (":/images/print.png"), "Imprimer ...",_manager);
		_printAction->setShortcut (QKeySequence(Qt::CTRL + Qt::Key_P));
		connect (_printAction, SIGNAL (triggered ( )), _manager,
		         SLOT(printCallback( )));
	}
	// Mode zoom interactif / annulation zoom :
	_zoomAction	= new QAction (QIcon (":/images/rubberband.png"),
						QSTR ("Mode zoom / sélection"), _manager);
	_zoomAction->setShortcut (QKeySequence(Qt::CTRL + Qt::Key_Z));
	_zoomAction->setCheckable (true);
	_zoomAction->setChecked (_zoomMode);
	connect (_zoomAction, SIGNAL (toggled (bool)), _manager,
	         SLOT (zoomModeCallback (bool)));
	_cancelZoomAction	= new QAction ("Annuler tout zoom", _manager);
	connect (_cancelZoomAction, SIGNAL (triggered ( )), _manager,
	         SLOT (cancelZoomCallback ( )));
	_adjustScalesAction	= new QAction (QIcon (":/images/autoscale.png"),
		QSTR ("Ajuster le graphique à ce qui est visible"), _manager);
	connect (_adjustScalesAction, SIGNAL (triggered ( )), _manager,
	         SLOT (adjustScalesCallback ( )));
	_undoZoomAction	= new QAction (QIcon (":/images/undozoom.png"),
					               "Annuler le dernier zoom", _manager);
	_undoZoomAction->setShortcut (Qt::Key_Minus);
	connect (_undoZoomAction, SIGNAL (triggered ( )), _manager,
	         SLOT (undoZoomCallback ( )));
	_redoZoomAction	= new QAction (QIcon (":/images/redozoom.png"),
					               "Refaire le dernier zoom", _manager);
	_redoZoomAction->setShortcut (Qt::Key_Plus);
	connect (_redoZoomAction, SIGNAL (triggered ( )), _manager,
	         SLOT (redoZoomCallback ( )));

	// Mode de sélection :
	_selectionModeAction	= new QtExclusiveActionGroup (_manager);
	_unselectAction	= new QAction (
						QSTR ("Annuler la sélection"), _manager);
	CHECK_NULL_PTR_ERROR (_selectionModeAction->menu ( ))
	connect (_unselectAction, SIGNAL (triggered ( )), _manager,
	         SLOT (unselectCallback ( )));
	_selectionModeAction->menu ( )->addAction (_unselectAction);
	_selectionModeAction->menu ( )->addSeparator ( );
	_selectionModeAction->addAction (QIcon (":/images/sel_mode_curve.png"), QSTR ("Mode de sélection courbe"));
	_selectionModeAction->addAction (QIcon (":/images/sel_mode_point.png"), QSTR ("Mode de sélection point"));
	_selectionModeAction->addAction (QIcon (":/images/sel_mode_interval.png"), QSTR ("Mode de sélection rectangle élastique"));
	_selectionModeAction->addAction (QIcon (":/images/sel_mode_linear_interpolation.png"), QSTR ("Mode de sélection/modification par interpolation linéaire"));
	_selectionModeAction->setCurrentIndex (0);
	connect (_selectionModeAction, SIGNAL (activated (int)), _manager,
	         SLOT (selectionModeCallback (int)));
	_selectionModeAction->menu ( )->addSeparator ( );
	_undoLastModifiedPointAction	=	
					new QAction ("Annuler le dernier point saisi", _manager);
	connect (_undoLastModifiedPointAction, SIGNAL (triggered ( )), _manager,
	         SLOT (undoLastModifiedPointCallback ( )));
	_selectionModeAction->menu ( )->addAction (_undoLastModifiedPointAction);

	// Graphique :
	_editChartAction	= new QAction (QIcon (":/images/palette.png"),
						               "Editer le graphique ...", _manager);
	connect (_editChartAction, SIGNAL (triggered ( )), _manager,\
	         SLOT (editChartCallback ( )));
	_backgroundAction	= new QAction ("Couleur du fond ...", _manager);
	connect (_backgroundAction, SIGNAL (triggered ( )), _manager,
	         SLOT (backgroundCallback ( )));
	_marginAction	= new QAction ("Marges ...", _manager);
	connect (_marginAction, SIGNAL (triggered ( )), _manager,
	         SLOT (marginCallback ( )));

	// Légende :
	_showLegendAction	= new QAction ("Afficher", _manager);
	_showLegendAction->setCheckable (true);
	connect (_showLegendAction, SIGNAL (toggled (bool)), _manager,
	         SLOT (showLegendCallback (bool)));
	QActionGroup*	actionGroup	= new QActionGroup (_manager);
	actionGroup->setExclusive (true);
	_topLegendAction	= new QAction ("Haut", _manager);
	_topLegendAction->setCheckable (true);
	connect (_topLegendAction, SIGNAL (toggled (bool)), _manager,
	         SLOT (topLegendCallback (bool)));
	actionGroup->addAction (_topLegendAction);
	_bottomLegendAction	= new QAction ("Bas", _manager);
	_bottomLegendAction->setCheckable (true);
	connect (_bottomLegendAction, SIGNAL (toggled (bool)), _manager,
	         SLOT (bottomLegendCallback (bool)));
	actionGroup->addAction (_bottomLegendAction);
	_rightLegendAction	= new QAction ("Droite", _manager);
	_rightLegendAction->setCheckable (true);
	connect (_rightLegendAction, SIGNAL (toggled (bool)), _manager,
	         SLOT (rightLegendCallback (bool)));
	actionGroup->addAction (_rightLegendAction);
	_leftLegendAction	= new QAction ("Gauche", _manager);
	_leftLegendAction->setCheckable (true);
	connect (_leftLegendAction, SIGNAL (toggled (bool)), _manager,
	         SLOT (leftLegendCallback (bool)));
	actionGroup->addAction (_leftLegendAction);
	_legendMarginAction	= new QAction ("Marges ...", _manager);
	connect (_legendMarginAction, SIGNAL (triggered ( )), _manager,
	         SLOT (legendMarginCallback ( )));
	_legendFrameWidthAction	= new QAction ("Epaisseur du cadre ...", _manager);
	connect (_legendFrameWidthAction, SIGNAL (triggered ( )), _manager,
	         SLOT (legendFrameWidthCallback ( )));

	// Grille :
	_verMajorLinesAction	=
		new QAction ("Afficher les lignes verticales principales", _manager);
	_verMajorLinesAction->setCheckable (true);
	connect (_verMajorLinesAction, SIGNAL (toggled (bool)), _manager,
	         SLOT (displayVerMajorLinesCallback (bool)));
	_verMinorLinesAction	=
		new QAction ("Afficher les lignes verticales secondaires", _manager);
	_verMinorLinesAction->setCheckable (true);
	connect (_verMinorLinesAction, SIGNAL (toggled (bool)), _manager,
	         SLOT (displayVerMinorLinesCallback (bool)));
	_horMajorLinesAction	=
		new QAction ("Afficher les lignes horizontales principales", _manager);
	_horMajorLinesAction->setCheckable (true);
	connect (_horMajorLinesAction, SIGNAL (toggled (bool)), _manager,
	         SLOT (displayHorMajorLinesCallback (bool)));
	_horMinorLinesAction	=
		new QAction ("Afficher les lignes horizontales secondaires", _manager);
	_horMinorLinesAction->setCheckable (true);
	connect (_horMinorLinesAction, SIGNAL (toggled (bool)), _manager,
	         SLOT (displayHorMinorLinesCallback (bool)));
						
	_manager->createToolBar ( );
}	// QwtChartsManager::initialize


void QwtChartsManager::finalize ( )
{
	if (0 == _manager)
		throw Exception(UTF8STR ("QwtChartsManager::finalize : API non initialisée."));

	if (0 != _toolBar)
	{
		delete _toolBar;
		_toolBar	= 0;
	}	// if (0 != _toolBar)
	delete _manager;
	_manager	= 0;
	_panels.clear ( );
}	// QwtChartsManager::finalize


bool QwtChartsManager::isInitialized ( )
{
	return 0 == _manager ? false : true;
}	// QwtChartsManager::isInitialized


void QwtChartsManager::setPrintAction (QAction* printAction)
{
	_printAction	= printAction;
}	// QwtChartsManager::setPrintAction


QwtChartsManager& QwtChartsManager::getManager ( )
{
	if (0 == _manager)
		throw Exception (UTF8STR ("QwtChartsManager::getManager : API non initialisée."));

	return *_manager;
}	// QwtChartsManager::getManager


void QwtChartsManager::registerChartPanel (QwtChartPanel& panel)
{
	if (false == isInitialized  ( ))
		throw Exception (UTF8STR ("Impossibilité d'enregistrer la fenêtre graphique : API non initialisée."));

	bool	registered	= false;
	for (vector<QwtChartPanel*>::const_iterator it = _panels.begin ( );
	     _panels.end ( ) != it; it++)
		if (*it == &panel)
		{
			registered	= true;
			break;
		}
	if (false == registered)
		_panels.push_back (&panel);
	panel.enableZoom (_zoomMode);
	try
	{
		panel.setSelectionMode (_selectionMode);
	}
	catch (...)
	{	// v >= 0.47.0 : risque d'échouer car pas encore de coordinates picker.
	}
}	// QwtChartsManager::registerChartPanel


void QwtChartsManager::unregisterChartPanel (QwtChartPanel& panel)
{
	if (false == isInitialized  ( ))
		throw Exception (UTF8STR ("Impossibilité de déréférencer la fenêtre graphique : API non initialisée."));

	for (vector<QwtChartPanel*>::iterator it = _panels.begin ( );
	     _panels.end ( ) != it; it++)
	{
		if (&panel == *it)
		{
			_panels.erase (it);
			// Par prudence on s'assure qu'il est totalement enlevé :
			unregisterChartPanel (panel);
			break;
		}	// if (&panel == *it)
	}	// for (vector<QwtChartPanel*>::iterator it = ...
}	// QwtChartsManager::unregisterChartPanel


QToolBar& QwtChartsManager::getToolBar ( )
{
	if (0 == _toolBar)
		throw Exception (UTF8STR ("QwtChartsManager::getToolBar : API non initialisée."));

	return *_toolBar;
}	// QwtChartsManager::getToolBar


QMenu* QwtChartsManager::createMenu (QMenu* menu, const QString& title)
{	
	QMenu*	newMenu	= 0 == menu ? new QMenu (title) : menu;
	assert (0 != newMenu);

	assert (0 != _zoomAction);
	assert (0 != _cancelZoomAction);
	assert (0 != _undoZoomAction);
	assert (0 != _redoZoomAction);
	assert (0 != _adjustScalesAction);
	assert (0 != _selectionModeAction);
	assert (0 != _printAction);
	assert (0 != _backgroundAction);
	assert (0 != _marginAction);
	newMenu->addAction (_zoomAction);
	newMenu->addAction (_cancelZoomAction);
	newMenu->addAction (_undoZoomAction);
	newMenu->addAction (_redoZoomAction);
	newMenu->addAction (_adjustScalesAction);
	newMenu->addSeparator ( );
	newMenu->addAction (_selectionModeAction);
	newMenu->addSeparator ( );
	newMenu->addAction (_printAction);
	newMenu->addSeparator ( );
	newMenu->addAction (_editChartAction);
	newMenu->addAction (_backgroundAction);
	newMenu->addAction (_marginAction);

	// Légende :
	assert (0 != _showLegendAction);
	assert (0 != _topLegendAction);
	assert (0 != _bottomLegendAction);
	assert (0 != _rightLegendAction);
	assert (0 != _leftLegendAction);
	assert (0 != _legendMarginAction);
	assert (0 != _legendFrameWidthAction);
	QMenu*	legendMenu	= newMenu->addMenu (QSTR ("Légende ..."));
	legendMenu->addAction (_showLegendAction);
	QMenu*	legendPosMenu	= legendMenu->addMenu ("Position ...");
	legendPosMenu->addAction (_topLegendAction);
	legendPosMenu->addAction (_bottomLegendAction);
	legendPosMenu->addAction (_rightLegendAction);
	legendPosMenu->addAction (_leftLegendAction);
	legendMenu->addAction (_legendMarginAction);
	legendMenu->addAction (_legendFrameWidthAction);

	// Grille :
	assert (0 != _verMajorLinesAction);
	assert (0 != _verMinorLinesAction);
	assert (0 != _horMajorLinesAction);
	assert (0 != _horMinorLinesAction);
	QMenu*	gridMenu	= newMenu->addMenu ("Grille ...");
	gridMenu->addAction (_verMajorLinesAction);
	gridMenu->addAction (_verMinorLinesAction);
	gridMenu->addAction (_horMajorLinesAction);
	gridMenu->addAction (_horMinorLinesAction);

	return newMenu;
}	// QwtChartsManager::createMenu


void QwtChartsManager::setSelectionMode (
								QwtPlotCoordinatesPicker::SELECTION_MODE mode)
{
	if ((0 != _zoomAction) && (true == _zoomAction->isChecked ( )))
		_zoomAction->setChecked (false);

	if (mode != _selectionMode)
	{
		_selectionMode	= mode;
		if (0 != _selectionModeAction)
			_selectionModeAction->setCurrentIndex ((int)mode);

		for (vector<QwtChartPanel*>::iterator it = _panels.begin ( );
		     _panels.end ( ) != it; it++)
		{
			if (0 != *it)
				(*it)->setSelectionMode (_selectionMode);
		}	// for (vector<QwtChartPanel*>::iterator it = _panels.begin ( );
	}	// if (mode != _selectionMode)
}	// QwtChartsManager::setSelectionMode


void QwtChartsManager::displayInformationMessage (QWidget* parent, const UTF8String& message)
{
	 QtMessageBox::displayInformationMessage (parent, "Gestionnaire de graphiques", message);
}	// QwtChartsManager::displayInformationMessage


void QwtChartsManager::displayWarningMessage (QWidget* parent, const UTF8String& message)
{
	 QtMessageBox::displayWarningMessage (
								parent, "Gestionnaire de graphiques", message);
}	// QwtChartsManager::displayWarningMessage


void QwtChartsManager::displayErrorMessage (QWidget* parent, const UTF8String& message)
{
	 QtMessageBox::displayErrorMessage (
								parent, "Gestionnaire de graphiques", message);
}	// QwtChartsManager::displayErrorMessage


void QwtChartsManager::objectDestroyed (QObject* object)
{
	if (object == _toolBar)
	{
		_toolBar	= 0;
		return;
	}	// if (object == _toolBar)
}	// QwtChartsManager::objectDestroyed


void QwtChartsManager::printCallback ( )
{
	QwtChartPanel*	window	= getActiveChartPanel ( );
	if (0 != window)
	{
		BEGIN_TRY_CATCH_BLOCK
		QWidget*		canvas	= window->getPlot ( ).canvas ( );	
		CHECK_NULL_PTR_ERROR (canvas)

		static	QString	lastFileName;
		static	QString	lastPrinterName;
		static  bool	toPrinter	= true;
		QPrinter	printer (QPrinter::HighResolution);
		// On propose par défaut les mêmes paramètres que la dernière
		// impression :
		if (true == toPrinter)
		{
			if (0 != lastFileName.size ( ))
				printer.setOutputFileName (lastFileName);
		}	// if (true == toPrinter)
		else
			if (0 != lastFileName.size ( ))
				printer.setOutputFileName (lastFileName);
		printer.setOrientation (canvas->width ( ) > canvas->height ( ) ?
		                             QPrinter::Landscape : QPrinter::Portrait);
		unique_ptr<QwtChartPrintDialog>	printDialog(new QwtChartPrintDialog(&printer, window));
		printDialog->setWindowTitle ("Impression");
		if (QDialog::Accepted == printDialog->exec ( ))
		{
			QtAutoWaitingCursor	cursor (true);
	
			if (false == window->print (printer))
				return;

			QPrintEngine*	engine	= printer.printEngine ( );
			QVariant	name	= 0 == engine ? QVariant ("inconnue") :
					engine->property (QPrintEngine::PPK_PrinterName);
			UTF8String		msg (charset), dest (charset);
			if (0 != printer.outputFileName ( ).size ( ))
			{
				toPrinter		= false;
				lastFileName	= printer.outputFileName ( );
				dest << "dans le fichier "
				     << printer.outputFileName ( ).toStdString ( );
			}
			else
			{
				toPrinter		= true;
				lastPrinterName	= printer.printerName ( );
				dest << "vers l'imprimante "
				     << name.toString ( ).toStdString ( );
			}	// else if (0 != printer.outputFileName ( ).size ( ))
			switch (printer.printerState ( ))
			{
				case QPrinter::Error	: 
					msg << "Echec de l'impression du document "
					    << window->getTitle ( ).text ( ).toStdString  ( ) << " "
					    << dest << ".";
					displayErrorMessage (window, msg);
					break;
				case QPrinter::Idle		:
					msg << "Impression du document "
					    << window->getTitle ( ).text ( ).toStdString  ( )
					    << " " << dest << " effectué.";
					displayInformationMessage (window, msg);
					break;
				case QPrinter::Active		:
					msg << "Impression du document "
					    << window->getTitle ( ).text ( ).toStdString  ( )
					    << " " << dest << " en cours.";
					displayInformationMessage (window, msg);
					break;
				default		:
					msg << "Status inconnu ("
					    << (unsigned long)printer.printerState ( )
					    << ") de l'impression du document "
					    << window->getTitle ( ).text ( ).toStdString  ( )
					    << " " << dest << ".";
					displayWarningMessage (window, msg);
			}	// switch (printer.printerState ( ))
		}	// if (QDialog::Accepted == printDialog->exec ( ))

		COMPLETE_TRY_CATCH_BLOCK
		}	// if (0 != window)
		else
		{
			displayWarningMessage (0, "Impression impossible : absence de fenêtre graphique active.");
		}	// else if (0 != window)
}	// QwtChartsManager::printCallback 


void QwtChartsManager::zoomModeCallback (bool toggled)
{
	QwtChartPanel*	window	= 0;
	BEGIN_TRY_CATCH_BLOCK
	_zoomMode	= toggled;

	if (0 != _selectionModeAction)
		_selectionModeAction->setEnabled (!_zoomMode);

	for (vector<QwtChartPanel*>::iterator it = _panels.begin ( );
	     _panels.end ( ) != it; it++)
	{
		window	= *it;
		(*it)->enableZoom (_zoomMode);
	}
	COMPLETE_TRY_CATCH_BLOCK
}	// QwtChartsManager::zoomModeCallback


void QwtChartsManager::cancelZoomCallback ( )
{
	QwtChartPanel*	window	= 0;
	BEGIN_TRY_CATCH_BLOCK
	for (vector<QwtChartPanel*>::iterator it = _panels.begin ( );
	     _panels.end ( ) != it; it++)
	{
		window	= *it;
		if (true == (*it)->isActiveWindow ( ))
			(*it)->cancelZoom ( );
	}	// for (vector<QwtChartPanel*>::iterator it = _panels.begin ( ); ...
	COMPLETE_TRY_CATCH_BLOCK
}	// QwtChartsManager::cancelZoomCallback


void QwtChartsManager::undoZoomCallback ( )
{
	QwtChartPanel*	window	= 0;

	BEGIN_TRY_CATCH_BLOCK

	window	= getActiveChartPanel ( );
	if (0 != window)
		window->undoZoom ( );

	COMPLETE_TRY_CATCH_BLOCK
}	// QwtChartsManager::undoZoomCallback


void QwtChartsManager::redoZoomCallback ( )
{
	QwtChartPanel*	window	= 0;

	BEGIN_TRY_CATCH_BLOCK

	window	= getActiveChartPanel ( );
	if (0 != window)
		window->redoZoom ( );

	COMPLETE_TRY_CATCH_BLOCK
}	// QwtChartsManager::redoZoomCallback


void QwtChartsManager::adjustScalesCallback ( )
{
	QwtChartPanel*	window	= 0;

	BEGIN_TRY_CATCH_BLOCK

	window	= getActiveChartPanel ( );
	if (0 != window)
		window->adjustScales ( );

	COMPLETE_TRY_CATCH_BLOCK
}	// QwtChartsManager::adjustScalesCallback


void QwtChartsManager::unselectCallback ( )
{
	QwtChartPanel*	window	= 0;
	BEGIN_TRY_CATCH_BLOCK
	window	= getActiveChartPanel ( );

	if (0 != window)
		window->clearSelection ( );

	COMPLETE_TRY_CATCH_BLOCK
}	// QwtChartsManager::unselectCallback


void QwtChartsManager::undoLastModifiedPointCallback ( )
{
	QwtCurveChartPanel*	window	= 0;
	BEGIN_TRY_CATCH_BLOCK
	window	= dynamic_cast<QwtCurveChartPanel*>(getActiveChartPanel ( ));

	if (0 != window)
		window->undoLastModifiedPoint ( );
	COMPLETE_TRY_CATCH_BLOCK
}	// QwtChartsManager::undoLastModifiedPointCallback


void QwtChartsManager::selectionModeCallback (int index)
{
	if ((0 != _zoomAction) && (true == _zoomAction->isChecked ( )))
		_zoomAction->setChecked (false);

	QwtCurveChartPanel*	window	= 0;
	BEGIN_TRY_CATCH_BLOCK
	_selectionMode	= (QwtPlotCoordinatesPicker::SELECTION_MODE)index;

	for (vector<QwtChartPanel*>::iterator it = _panels.begin ( );
	     _panels.end ( ) != it; it++)
	{
		if (0 != *it)
		{
			try
			{
				(*it)->setSelectionMode (_selectionMode);
			}
			catch (...)
			{
				// v 3.6.1 : on ne lève l'exception que si c'est le panneau
				// courrant qui refuse le changement de mode. En effet, dans
				// certaines applications on peut avoir plusieurs panneaux d'
				// affichés et il serait regretable d'arrêter la sélection
				// parce-que l'un d'entre-eux, non en cours d'édition, ne s'y
				// prête pas.
				if (*it == currentPanel)
					throw;	// v 3.6.1
			}
		}
	}

	COMPLETE_TRY_CATCH_BLOCK
}	// QwtChartsManager::selectionModeCallback


void QwtChartsManager::editChartCallback ( )
{
	QwtChartPanel*	window	= 0;
	BEGIN_TRY_CATCH_BLOCK
	window	= getActiveChartPanel ( );

	if (0 != window)
	{
		QwtChartOptionsDialog	dialog (
			window, window, true, editionDialogHelpURL, editionDialogHelpTag);
		dialog.exec ( );
	}
	COMPLETE_TRY_CATCH_BLOCK
}	// QwtChartsManager::editChartCallback


void QwtChartsManager::backgroundCallback ( )
{
	QwtChartPanel*	window	= 0;
	BEGIN_TRY_CATCH_BLOCK
	window	= getActiveChartPanel ( );

	if (0 != window)
	{
		const QColor&	initial	= window->getBackgroundColor ( );
		QColor			color	= QColorDialog::getColor (initial, window, QSTR ("Couleur du fond"), QColorDialog::DontUseNativeDialog);
		if (true == color.isValid ( ))
			window->setBackgroundColor (color);
	}
	COMPLETE_TRY_CATCH_BLOCK
}	// QwtChartsManager::backgroundCallback


void QwtChartsManager::marginCallback ( )
{
	QwtChartPanel*	window	= 0;
	BEGIN_TRY_CATCH_BLOCK
	window	= getActiveChartPanel ( );

	if (0 != window)
	{	
		bool	ok		= true;
#ifdef QT_4
		int		margin	= QInputDialog::getInteger (
#else	// QT_4
		int		margin	= QInputDialog::getInt (
#endif	// QT_4
						window, "Saisie des marges d'un graphique", "Marges",
						window->getMargin ( ), 0, 10000, 1, &ok);
		if (true == ok)
			window->setMargin (margin);
	}
	COMPLETE_TRY_CATCH_BLOCK
}	// QwtChartsManager::marginCallback


void QwtChartsManager::showLegendCallback (bool show)
{
	QwtChartPanel*	window	= 0;
	BEGIN_TRY_CATCH_BLOCK
	window	= getActiveChartPanel ( );

	if (0 != window)
		window->showLegend (show, window->getLegendPosition  ( ));
	COMPLETE_TRY_CATCH_BLOCK
}	// QwtChartsManager::showLegendCallback


void QwtChartsManager::topLegendCallback (bool checked)
{
	if (false == checked)
		return;

	QwtChartPanel*	window	= 0;
	BEGIN_TRY_CATCH_BLOCK
	window	= getActiveChartPanel ( );

	if (0 != window)
		window->showLegend (window->hasLegend ( ), QwtPlot::TopLegend);
	COMPLETE_TRY_CATCH_BLOCK
}	// QwtChartsManager::topLegendCallback


void QwtChartsManager::bottomLegendCallback (bool checked)
{
	if (false == checked)
		return;

	QwtChartPanel*	window	= 0;
	BEGIN_TRY_CATCH_BLOCK
	window	= getActiveChartPanel ( );

	if (0 != window)
		window->showLegend (window->hasLegend ( ), QwtPlot::BottomLegend);
	COMPLETE_TRY_CATCH_BLOCK
}	// QwtChartsManager::bottomLegendCallback


void QwtChartsManager::rightLegendCallback (bool checked)
{
	if (false == checked)
		return;

	QwtChartPanel*	window	= 0;
	BEGIN_TRY_CATCH_BLOCK
	window	= getActiveChartPanel ( );

	if (0 != window)
		window->showLegend (window->hasLegend ( ), QwtPlot::RightLegend);
	COMPLETE_TRY_CATCH_BLOCK
}	// QwtChartsManager::rightLegendCallback


void QwtChartsManager::leftLegendCallback (bool checked)
{
	if (false == checked)
		return;

	QwtChartPanel*	window	= 0;
	BEGIN_TRY_CATCH_BLOCK
	window	= getActiveChartPanel ( );

	if (0 != window)
		window->showLegend (window->hasLegend ( ), QwtPlot::LeftLegend);
	COMPLETE_TRY_CATCH_BLOCK
}	// QwtChartsManager::leftLegendCallback


void QwtChartsManager::legendMarginCallback ( )
{
	QwtChartPanel*	window	= 0;
	BEGIN_TRY_CATCH_BLOCK
	window	= getActiveChartPanel ( );

	if (0 != window)
	{	
		bool	ok		= true;
#ifdef QT_4
		int		margin	= QInputDialog::getInteger (
#else	// QT_4
		int		margin	= QInputDialog::getInt (
#endif	// QT_4
						window, QSTR ("Saisie de des marges de la légende"),
						"Marges", window->getLegendMargin ( ), 0, 20,
						1, &ok);
		if (true == ok)
			window->setLegendMargin (margin);
	}
	COMPLETE_TRY_CATCH_BLOCK
}	// QwtChartsManager::legendMarginCallback


void QwtChartsManager::legendFrameWidthCallback ( )
{
	QwtChartPanel*	window	= 0;
	BEGIN_TRY_CATCH_BLOCK
	window	= getActiveChartPanel ( );

	if (0 != window)
	{	
		bool	ok		= true;
#ifdef QT_4
		int		width	= QInputDialog::getInteger (
#else	// QT_4
		int		width	= QInputDialog::getInt (
#endif	// QT_4
						window, QSTR ("Saisie de l'épaisseur du cadre de la légende"),
						"Epaisseur", window->getLegendFrameWidth ( ), 0, 20,
						1, &ok);
		if (true == ok)
			window->setLegendFrameWidth (width);
	}
	COMPLETE_TRY_CATCH_BLOCK
}	// QwtChartsManager::legendFrameWidthCallback


void QwtChartsManager::displayVerMajorLinesCallback (bool checked)
{
	QwtChartPanel*	window	= 0;
	BEGIN_TRY_CATCH_BLOCK
	window	= getActiveChartPanel ( );

	if (0 != window)
		window->displayVerMajorLines (checked);
	COMPLETE_TRY_CATCH_BLOCK
}	// QwtChartsManager::displayVerMajorLinesCallback


void QwtChartsManager::displayVerMinorLinesCallback (bool checked)
{
	QwtChartPanel*	window	= 0;
	BEGIN_TRY_CATCH_BLOCK
	window	= getActiveChartPanel ( );

	if (0 != window)
		window->displayVerMinorLines (checked);
	COMPLETE_TRY_CATCH_BLOCK
}	// QwtChartsManager::displayVerMinorLinesCallback


void QwtChartsManager::displayHorMajorLinesCallback (bool checked)
{
	QwtChartPanel*	window	= 0;
	BEGIN_TRY_CATCH_BLOCK
	window	= getActiveChartPanel ( );

	if (0 != window)
		window->displayHorMajorLines (checked);
	COMPLETE_TRY_CATCH_BLOCK
}	// QwtChartsManager::displayHorMajorLinesCallback


void QwtChartsManager::displayHorMinorLinesCallback (bool checked)
{
	QwtChartPanel*	window	= 0;
	BEGIN_TRY_CATCH_BLOCK
	window	= getActiveChartPanel ( );

	if (0 != window)
		window->displayHorMinorLines (checked);
	COMPLETE_TRY_CATCH_BLOCK
}	// QwtChartsManager::displayHorMinorLinesCallback



void QwtChartsManager::focusChanged (const QwtChartPanel* panel)
{
	assert (0 != _showLegendAction);
	assert (0 != _topLegendAction);
	assert (0 != _bottomLegendAction);
	assert (0 != _rightLegendAction);
	assert (0 != _leftLegendAction);
	currentPanel	= (QwtChartPanel*)panel;
	QtActionAutoLock	showLegendLock (_showLegendAction);
	QtActionAutoLock	topLegendLock (_topLegendAction);
	QtActionAutoLock	bottomLegendLock (_bottomLegendAction);
	QtActionAutoLock	rightLegendLock (_rightLegendAction);
	QtActionAutoLock	leftLegendLock (_leftLegendAction);
	if (0 != currentPanel)
	{
		_showLegendAction->setChecked (currentPanel->hasLegend ( ));
		switch (currentPanel->getLegendPosition ( ))
		{
			case QwtPlot::TopLegend		:
							_topLegendAction->setChecked (true);	break;
			case QwtPlot::BottomLegend	:
							_bottomLegendAction->setChecked (true);	break;
			case QwtPlot::RightLegend		:
							_rightLegendAction->setChecked (true);	break;
			case QwtPlot::LeftLegend		:
							_leftLegendAction->setChecked (true);	break;
		}	// switch (currentPanel->getLegendPosition ( ))

		assert (0 != _verMajorLinesAction);
		assert (0 != _verMinorLinesAction);
		assert (0 != _horMajorLinesAction);
		assert (0 != _horMinorLinesAction);
		QtActionAutoLock	horMajorLock (_horMajorLinesAction);
		QtActionAutoLock	horMinorLock (_horMinorLinesAction);
		QtActionAutoLock	verMajorLock (_verMajorLinesAction);
		QtActionAutoLock	verMinorLock (_verMinorLinesAction);
		_verMajorLinesAction->setChecked (
								currentPanel->areVerMajorLinesDisplayed ( ));
		_verMinorLinesAction->setChecked (
								currentPanel->areVerMinorLinesDisplayed ( ));
		_horMajorLinesAction->setChecked (
								currentPanel->areHorMajorLinesDisplayed ( ));
		_horMinorLinesAction->setChecked (
								currentPanel->areHorMinorLinesDisplayed ( ));
	}	// if (0 != currentPanel)
}	// QwtChartsManager::focusChanged


QwtChartPanel* QwtChartsManager::getActiveChartPanel ( )
{
	// PB : QApplication::focusWidget et isActiveWindow ne fonctionnent pas bien
	// (Qt 4.3.2) => gestion "manuelle" du focus.
	return currentPanel;
}	// QwtChartsManager::getActiveChartPanel


void QwtChartsManager::createToolBar ( )
{
	QAction*	action	= 0;

	assert (0 == _toolBar);
	_toolBar	= new QToolBar ( );
	CHECK_NULL_PTR_ERROR (_toolBar)
	connect (_toolBar, SIGNAL (destroyed (QObject*)), this,
	         SLOT (objectDestroyed (QObject*)));

	assert (0 != _editChartAction);
	assert (0 != _printAction);
	assert (0 != _zoomAction);
	assert (0 != _adjustScalesAction);
	assert (0 != _undoZoomAction);
	assert (0 != _redoZoomAction);
	assert (0 != _selectionModeAction);
	_toolBar->addAction (_printAction);
	_toolBar->addSeparator ( );
	_toolBar->addAction (_zoomAction);
	_toolBar->addAction (_adjustScalesAction);
	_toolBar->addAction (_undoZoomAction);
	_toolBar->addAction (_redoZoomAction);
	_toolBar->addSeparator ( );
	_toolBar->addAction (_selectionModeAction);
	_toolBar->addSeparator ( );
	_toolBar->addAction (_editChartAction);
}	// QwtChartsManager::createToolBar
