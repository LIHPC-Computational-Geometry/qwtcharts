#ifndef QWT_CHART_HISTOGRAM_OPTIONS_PANEL_H
#define QWT_CHART_HISTOGRAM_OPTIONS_PANEL_H

#include <QwtCharts/QwtHistogramChartPanel.h>


#include <qwt_plot.h>
#include <QListWidget>
#include <QCheckBox>
#include <QComboBox>


/**
 * <P>Classe de panneau permettant de paramétrer les options d'un
 * graphique de type QwtHistogramChartPanel.
 * </P>
 * @warning		Les méthodes de cette classe sont susceptibles de lever de
 *				exception de type Exception.
 * @see			QwtHistogramChartPanel
 */
class QwtHistogramOptionsPanel : public QwtChartPanel::QwtChartEditionPanel
{
	public :

	/**
	 * Constructeur par défaut : ajuste ses propriétés à celles de l'éventuel
	 * panneau transmis en argument.
	 * @param		Widget parent.
	 * @param		Eventuel panneau graphique à modifier via ce panneau.
	 */
	QwtHistogramOptionsPanel (
					QWidget* parent, QwtHistogramChartPanel* histogramPanel);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QwtHistogramOptionsPanel ( );

	/**
	 * Met à jour l'éventuel panneau graphique associé.
	 */
	virtual void apply ( );

	/**
	 * @return		<I>true</I> si l'instance a un panneau graphique associé,
	 *				sinon <I>false</I>.
	 * @see			getHistogramPanel
	 */
	virtual bool hasHistogramPanel ( ) const
	{ return 0 == _histogramPanel ? false : true; }

	/*
	 * @return		Une référence sur le panneau graphique associé.
	 * @exception	Lève une exception si aucun panneau graphique n'est associé.
	 * @see			hasHistogramPanel
	 */
	QwtHistogramChartPanel& getHistogramPanel ( );
	const QwtHistogramChartPanel& getHistogramPanel ( ) const;

	/**
	 * @return		Le style d'histogramme.
	 * @see			setHistogramStyle
	 */
	virtual QwtExtendedMultiBarChart::HISTOGRAM_STYLE getHistogramStyle ( ) const;

	/**
 	 * @param		Le nouveau style d'histogramme.
 	 * @see			getHistogramStyle
	 */
	virtual void setHistogramStyle (
							QwtExtendedMultiBarChart::HISTOGRAM_STYLE style);

	/**
	 * @return	<I>true</I> si le nombre d'éléments par classe sont écrits,
	 * 			<I>false</I> dans le cas contraire.
	 * @see		getElementNumPos
	 * @see		displayElementsNum
	 */
	virtual bool displayElementsNum ( ) const;

	/**
	 * @param	<I>true</I> si le nombre d'éléments par classe sont écrits,
	 * 			<I>false</I> dans le cas contraire.
	 * @see		displayElementsNum
	 * @see		setElementNumPos
	 */
	virtual void displayElementsNum (bool display);

	/**
	 * @return	La position d'affichage du nombre d'éléments par classe.
	 * @see		setElementNumPos
	 * @see		getElementNumPos
	 */
	virtual QwtExtendedMultiBarChart::POSITION getElementNumPos ( ) const;

	/**
	 * @return	La position d'affichage du nombre d'éléments par classe.
	 * @see		setElementNumPos
	 * @see		getElementNumPos
	 */
	virtual void setElementNumPos (QwtExtendedMultiBarChart::POSITION pos);


	private :

	/** Constructeurs de copie interdits. */
	QwtHistogramOptionsPanel (const QwtHistogramOptionsPanel&);
	QwtHistogramOptionsPanel& operator = (const QwtHistogramOptionsPanel&);

	/** Le panneau graphique associé. */
	QwtHistogramChartPanel*					_histogramPanel;

	/** Le style d'histogramme. */
	QComboBox*								_styleComboBox;

	/** Afficher le nombre d'éléments par classe ? */
	QCheckBox*								_elementsNumCheckBox;

	/** Le cas échéant, où écrire le nombre d'éléments de chaque classe ? */
	QComboBox*								_elementsNumPosComboBox;
};	// class QwtHistogramOptionsPanel

#endif	// QWT_CHART_HISTOGRAM_OPTIONS_PANEL_H
