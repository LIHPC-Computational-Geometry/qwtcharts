#ifndef QWT_CHART_CURVES_OPTIONS_PANEL_H
#define QWT_CHART_CURVES_OPTIONS_PANEL_H

#include <QwtCharts/QwtChartPanel.h>
#include <QwtCharts/QwtExtendedPlotCurve.h>
#include <QtUtil/QtPenPanel.h>
#include <QtUtil/QtTextField.h>
#include <QtUtil/QtIntTextField.h>

#include <qwt_plot.h>
#include <QListWidget>

#include <vector>



/**
 * <P>Classe de panneau permettant de paramétrer les options des courbes d'un
 * graphique de type QwtChartPanel.
 * </P>
 * @warning		Les méthodes de cette classe sont susceptibles de lever de
 *				exception de type Exception.
 * @see			QwtChartPanel
 */
class QwtCurvesOptionsPanel : public QwtChartPanel::QwtChartEditionPanel
{
	public :

	/**
	 * Constructeur par défaut : ajuste ses propriétés à celles de l'éventuel
	 * panneau transmis en argument.
	 * @param		Widget parent.
	 * @param		Eventuel panneau graphique à modifier via ce panneau.
	 */
	QwtCurvesOptionsPanel (QWidget* parent, QwtChartPanel* chartPanel);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QwtCurvesOptionsPanel ( );

	/**
	 * Met à jour l'éventuel panneau graphique associé.
	 */
	virtual void apply ( );

	/**
	 * Appelé lorsqu'une courbe est ajoutée au graphique. Met à jour le panneau.
	 * @param		Courbe ajoutée.
	 */
	virtual void curveAdded (QwtExtendedPlotCurve* curve);

	/**
	 * Appelé lorsqu'une courbe est enlevée du graphique. Met à jour le panneau.
	 * @param		Courbe enlevée.
	 */
	virtual void curveRemoved (QwtExtendedPlotCurve* curve);

	/**
	 * @return		<I>true</I> si l'instance a un panneau graphique associé,
	 *				sinon <I>false</I>.
	 * @see			getChartPanel
	 */
	virtual bool hasChartPanel ( ) const
	{ return 0 == _chartPanel ? false : true; }

	/*
	 * @return		Une référence sur le panneau graphique associé.
	 * @exception	Lève une exception si aucun panneau graphique n'est associé.
	 * @see			hasChartPanel
	 */
	QwtChartPanel& getChartPanel ( );
	const QwtChartPanel& getChartPanel ( ) const;


	private :

	/** Constructeurs de copie interdits. */
	QwtCurvesOptionsPanel (const QwtCurvesOptionsPanel&);
	QwtCurvesOptionsPanel& operator = (const QwtCurvesOptionsPanel&);

	/** Le panneau graphique associé. */
	QwtChartPanel*							_chartPanel;

	/** La liste des courbes du panneau graphique. */
	IN_STD vector<QwtPlotItem*>				_curves;
	QListWidget*							_curvesList;

	/** Le stylo de tracé des courbes.  */
	QtPenPanel*								_penPanel;
};	// class QwtCurvesOptionsPanel

#endif	// QWT_CHART_CURVES_OPTIONS_PANEL_H
