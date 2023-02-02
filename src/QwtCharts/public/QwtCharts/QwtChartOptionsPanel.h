#ifndef QWT_CHART_OPTIONS_PANEL_H
#define QWT_CHART_OPTIONS_PANEL_H

#include <QwtCharts/QwtAxisOptionsPanel.h>
#include <QwtCharts/QwtCurvesOptionsPanel.h>
#include <QwtCharts/QwtGridOptionsPanel.h>
#include <QwtCharts/QwtLegendOptionsPanel.h>
#include <QwtCharts/QwtChartGeneralOptionsPanel.h>

#include <QTabWidget>



/**
 * <P>Classe de panneau permettant de paramétrer un graphique de
 * type QwtChartPanel. Ce panneau est composé de panneaux spécialisés organisés
 * sous formes d'onglets.
 * </P>
 * @warning		Les méthodes de cette classe sont susceptibles de lever de
 *				exception de type Exception.
 * @see			QwtChartPanel
 */
class QwtChartOptionsPanel : public QWidget
{
	public :

	/**
	 * Constructeur par défaut : ajuste ses propriétés à celles de l'éventuel
	 * panneau transmis en argument. Ajoute aux panneaux prédéfinis ceux
	 * éventuellement créés par <I>chartPanel</I> en invoquant sa méthode
	 * <I>createEditionPanels</I>.
	 * @param		Widget parent.
	 * @param		Eventuel panneau graphique à modifier via ce panneau.
	 */
	QwtChartOptionsPanel (QWidget* parent, QwtChartPanel* chartPanel);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QwtChartOptionsPanel ( );

	/**
	 * Met à jour l'éventuel panneau graphique associé.
	 */
	virtual void apply ( );

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

	/**
	 * @return		Le panneau courrant
	 */
	virtual QwtChartPanel::QwtChartEditionPanel& getCurrentPanel ( );
	virtual const QwtChartPanel::QwtChartEditionPanel& getCurrentPanel ( )const;

	/**
	 * @return		Le panneau de paramétrage des options "générales" du
	 *				graphique	représenté.
	 */
	QwtChartGeneralOptionsPanel& getGeneralPanel ( );
	const QwtChartGeneralOptionsPanel& getGeneralPanel ( ) const;

	/**
	 * @return		Le panneau d'édition des axes du graphique.
	 */
	QwtAxisOptionsPanel& getAxisPanel ( );
	const QwtAxisOptionsPanel& getAxisPanel ( ) const;

	/**
	 * @return		Le panneau d'édition de la grille du graphique.
	 */
	QwtGridOptionsPanel& getGridPanel ( );
	const QwtGridOptionsPanel& getGridPanel ( ) const;

	/**
	 * @return		Le panneau d'édition de la légende du graphique.
	 */
	QwtLegendOptionsPanel& getLegendPanel ( );
	const QwtLegendOptionsPanel& getLegendPanel ( ) const;

	/**
	 * @return		Le panneau d'édition de des courbes du graphique.
	 */
	QwtCurvesOptionsPanel& getCurvesPanel ( );
	const QwtCurvesOptionsPanel& getCurvesPanel ( ) const;

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


	protected :

	/**
	 * @return		Le gestionnaire d'onglets.
	 */
	QTabWidget& getTabWidget ( );
	const QTabWidget& getTabWidget ( ) const;


	private :

	/** Constructeurs de copie interdits. */
	QwtChartOptionsPanel (const QwtChartOptionsPanel&);
	QwtChartOptionsPanel& operator = (const QwtChartOptionsPanel&);

	/** Le gestionnaire d'onglets. */
	QTabWidget*						_tabWidget;

	/** Le panneau graphique associé. */
	QwtChartPanel*					_chartPanel;

	/** Le panneau de paramétrage des options "générales" du graphique
	 *  associée. */
	QwtChartGeneralOptionsPanel*	_generalOptionsPanel;

	/** Le panneau associé de paramétrage des options des axes. */
	QwtAxisOptionsPanel*			_axisOptionsPanel;

	/** Le panneau de paramétrage de la grille associée. */
	QwtGridOptionsPanel*			_gridOptionsPanel;

	/** Le panneau de paramétrage de la légende associée. */
	QwtLegendOptionsPanel*			_legendOptionsPanel;

	/** Le panneau de paramétrage des courbes associées. */
	QwtCurvesOptionsPanel*			_curvesOptionsPanel;

	/** Les panneaux d'édition du graphique représenté. */
	IN_STD vector<QwtChartPanel::QwtChartEditionPanel*>			_editionPanels;
};	// class QwtChartOptionsPanel

#endif	// QWT_CHART_OPTIONS_PANEL_H
