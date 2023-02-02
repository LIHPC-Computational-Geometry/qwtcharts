#ifndef QWT_HISTOGRAM_CHART_PANEL_H
#define QWT_HISTOGRAM_CHART_PANEL_H

#include <qwt_global.h>		// => QWT_VERSION


#include <QwtCharts/QwtChartPanel.h>
#include <QwtCharts/QwtExtendedMultiBarChart.h>


#include <string>


/**
 * <P>
 * Classe "panneau graphique" contenant des histogrammes et reposant sur Qwt.
 * </P>
 *
 * <P>Cette classe repose sur la classe <I>QwtExtendedMultiBarChart</I> et ne 
 * permet de ce fait pas de modifier dynamiquement une partie de l'histogramme, 
 * c'est du tout ou rien.
 * </P>
 *
 * @warning 	<B>Requiert Qwt 6 ou plus récent. </B>
 * @warning		Les méthodes de cette classe sont susceptibles de lever de
 *				exception de type Exception.
 */
class QwtHistogramChartPanel : public QwtChartPanel
{
	Q_OBJECT


	public :

	/**
	 * Constructeur par défaut.
	 * @param		Widget parent.
	 */
	QwtHistogramChartPanel (QWidget* parent);

	/**
	 * Destructeur.
	 */
	virtual ~QwtHistogramChartPanel ( );

	/**
	 * A invoquer lorsque les données ont été modifiées par un autre biais
	 * que par cette bibliothèque.
	 * Met à jour la gestion du zoom et de la sélection en cours.
	 */
	virtual void dataModified ( );

	/**
	 * Affecte l'ensemble des séries de l'histogramme.
	 * <I>series</I> est un vecteur contenant des vecteurs de double où
	 * chacun de ces vecteurs de double correspond à une classe de
	 * l'historgamme (et non pas à une série qui serait représentée par un
	 * vecteur ou chaque valeur serait son nombre dans la i-ème classe).
	 * @param		Noms des séries.
	 * @param		Valeurs des classes.
	 * @param		Séries (vecteur de nombre d'éléments pour chaque série dans
	 * 				chaque classes.
	 * @warning		<I>classValues</I> et <I>series</I> doivent avoir la même
	 * 				taille. Les vecteurs de <I>series</I> doibent avoir la même
	 * 				taille que <I>seriesNames</I>.
	 */
	virtual void setSeries (
					const QList<QwtText>& seriesNames,
					const QVector<double>& classValues,
					const QVector< QVector <double> >& series);

	/**
	 * @return		Le style d'histogramme.
	 * @see			setHistogramStyle
	 */
	virtual QwtExtendedMultiBarChart::HISTOGRAM_STYLE
												getHistogramStyle ( ) const;

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

	/**
	 * Créé les panneaux spécifiques d'édition de l'histogramme.
	 * @param		Parent des panneaux créés
	 * @return		Liste des panneaux créés
	 * @see			setSpecificsPanelsCreator
	 * @see			QwtChartOptionsDialog
	 * @see			QwtChartEditionPanel
	 */
	virtual IN_STD vector<QwtChartEditionPanel*> createEditionPanels (
															QWidget* parent);

	/**
	 * @return		La liste des propriétés d'affichage des courbes.
	 * @see			setDataPaintAttributes
	 **/
	virtual IN_STD vector<QwtChartPanel::DataPaintAttributes>
											getDataPaintAttributes ( ) const;

	/**
	 * @param		Indice de la courbe dont on modifie les propriétés
	 *				d'affichage.
	 * @param		Propriétés d'affichage à appliquer.
	 * @see			getDataPaintAttributes
	 **/
	virtual void setDataPaintAttributes (size_t index,
					const QwtChartPanel::DataPaintAttributes& paintAttributes);

	/**
	 * @return		L'histogramme associé.
	 */
	virtual QwtExtendedMultiBarChart& getHistogram ( );
	virtual const QwtExtendedMultiBarChart& getHistogram ( ) const;


	protected :


	protected slots :

	/**
	 * Appelé lorsqu'un item de la légende est (dé)coché. Affiche ou masque la
	 * série associée.
	 */
	virtual void legendCheckedCallback (
								const QVariant& itemInfo, bool on, int index);


	private :

	/** Constructeurs de copie interdits. */
	QwtHistogramChartPanel (const QwtHistogramChartPanel&);
	QwtHistogramChartPanel& operator = (const QwtHistogramChartPanel&);

	/** Les courbes gérées. */
	QwtExtendedMultiBarChart*						_barChart;
};	// class QwtHistogramChartPanel


#endif	// QWT_HISTOGRAM_CHART_PANEL_H
