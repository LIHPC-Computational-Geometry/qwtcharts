#include "QwtCharts/QwtChartsManager.h"
#include <QApplication>


int main (int argc, char* argv[])
{
	QApplication	application (argc, argv);
	QwtChartsManager::editionDialogHelpURL	= 
		"classQwtCurveChartPanel.html";
	QwtChartsManager::initialize ( );
    QwtChartPanel::legendCheckable	= QwtLegendData::Checkable;
    
    // No start of graphical application during GitHub CI (just link test)
	// return application.exec ( );
	return 0;
}