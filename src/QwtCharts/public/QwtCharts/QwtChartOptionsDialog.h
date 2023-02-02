#ifndef QWT_CHART_OPTIONS_DIALOG_H
#define QWT_CHART_OPTIONS_DIALOG_H

#include <QwtCharts/QwtChartOptionsPanel.h>
#include <QtUtil/QtDlgClosurePanel.h>

#include <QDialog>


/**
 * <P>Classe de boite de dialogue permettant de paramétrer un graphique de
 * type QwtChartDialog. Cette boite de dialogue est composé de panneaux
 * spécialisés organisés sous formes d'onglets.
 * </P>
 * <P>Cette classe fournit un service d'aide contextuelle reposant sur l'API
 * <I>QtHelpWindow</I>.
 * </P>
 * @warning		Les méthodes de cette classe sont susceptibles de lever de
 *				exception de type Exception.
 * @see			QwtChartOptionsPanel
 */
class QwtChartOptionsDialog : public QDialog
{
	Q_OBJECT


	public :

	/**
	 * Constructeur par défaut : ajuste ses propriétés à celles de l'éventuel
	 * panneau transmis en argument.
	 * @param		Widget parent.
	 * @param		Eventuel panneau graphique à modifier via ce panneau.
	 * @param		Caractère modal de la boite de dialogue.
	 * @param		URL de l'aide.
	 * @param		Balise de l'aide.
	 * @see			QtHelpWindow
	 */
	QwtChartOptionsDialog (
			QWidget* parent, QwtChartPanel* chartDialog, bool modal = true,
			const IN_STD string& helpURL = "",
			const IN_STD string& helpTag = "");

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QwtChartOptionsDialog ( );

	/**
	 * @return		Le panneau de paramétrage de la grille du graphique	
	 *				représenté.
	 */
	QwtChartOptionsPanel& operator -> ( );
	const QwtChartOptionsPanel& operator -> ( ) const;

	/**
	 * Méthode à invoquer lorsqu'une courbe est ajouté au graphique. Met à jour
	 * les panneaux d'édition.
	 * @param		Courbe ajoutée
	 */
	virtual void curveAdded (QwtExtendedPlotCurve* curve);

	/**
	 * Méthode à invoquer lorsqu'une courbe est enlevée du graphique. Met à jour
	 * les panneaux d'édition.
	 * @param		Courbe enlevée
	 */
	virtual void curveRemoved (QwtExtendedPlotCurve* curve);


	protected slots :

	/**
	 * Appeler lorsque l'utilisateur clique sur le bouton "Appliquer". Met à
	 * jour l'éventuel panneau graphique associé.
	 */
	virtual void applyCallback ( );


	private :

	/** Constructeurs de copie interdits. */
	QwtChartOptionsDialog (const QwtChartOptionsDialog&);
	QwtChartOptionsDialog& operator = (const QwtChartOptionsDialog&);

	/** Le panneau gérant les propriétés du graphique. */
	QwtChartOptionsPanel*			_chartOptionsPanel;

	/** Le panneau contenant les boutons "Appliquer" et "Fermer". */
	QtDlgClosurePanel*				_dlgClosurePanel;
};	// class QwtChartOptionsDialog

#endif	// QWT_CHART_OPTIONS_DIALOG_H
