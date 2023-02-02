#ifndef QWT_CHART_PRINT_DIALOG_H
#define QWT_CHART_PRINT_DIALOG_H

#include <TkUtil/util_config.h>

#include <QCheckBox>
#include <QPrintDialog>

#include <qwt_global.h>


/**
 * <P>Boite de dialogue personnalisée d'impression de graphiques Qwt.</P>
 */
class QwtChartPrintDialog : public QPrintDialog
{
	public :

	/**
	 * Constructeur versions supérieures ou égales à la version 5.2.0 de Qwt.
	 * @param		Outil d'impression utilisé
	 * @param		Parent de la boite de dialogue.
	 */
	QwtChartPrintDialog (QPrinter* printer, QWidget* parent = 0);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QwtChartPrintDialog ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QwtChartPrintDialog (const QwtChartPrintDialog&);
	QwtChartPrintDialog& operator = (const QwtChartPrintDialog&);

	/** Case à cocher "maintient du rapport d'aspect". */
//	QCheckBox*		_aspectRatioCheckBox;
};	// class QwtChartPrintDialog


#endif	// QWT_CHART_PRINT_DIALOG_H
