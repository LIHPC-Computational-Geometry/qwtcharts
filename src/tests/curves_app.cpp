#include "QtCurvesMainWindow.h"
#include "QwtCharts/QwtChartsManager.h"
#include <QApplication>


int main (int argc, char* argv[])
{
	QApplication	application (argc, argv);
	QwtChartsManager::editionDialogHelpURL	= 
		"classQwtCurveChartPanel.html";
	QwtChartsManager::initialize ( );
QwtChartPanel::legendCheckable	= QwtLegendData::Checkable;
	QtChartsMainWindow*	mainWindow	= new QtChartsMainWindow (0);
	mainWindow->show ( );
	return application.exec ( );
}	// main
