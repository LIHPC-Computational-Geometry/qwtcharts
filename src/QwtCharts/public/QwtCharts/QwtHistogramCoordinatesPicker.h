#ifndef QWT_HISTOGRAM_COORDINATES_PICKER_H
#define QWT_HISTOGRAM_COORDINATES_PICKER_H

#include <TkUtil/util_config.h>

#include <qwt_global.h>	// => QWT_VERSION

#include <qwt_compat.h>
#include "QwtCharts/QwtPlotCoordinatesPicker.h"


class QwtHistogramChartPanel;
class QwtExtendedMultiBarChart;

/**
 * <P>Classe de picker Qwt affichant les coordonnées pointées par la souris
 * spécialisé pour des graphiques type Histogrammes..
 * Permet la sélection d'entités de l'histogramme.
 * </P>
 * @warning		Les instances de cette classe ne sont pas informées des
 *				éventuels évènements pouvant survenir au niveau du graphique,
 *				tel qu'un zoom. Il convient d'appeler la méthode updateFromPlot
 *				dans de telles circonstances.
 */
class QwtHistogramCoordinatesPicker : public QwtPlotCoordinatesPicker
{
	Q_OBJECT

	public :

	/**
	 * Constructeur par défaut.
	 */
	QwtHistogramCoordinatesPicker (
					int xAxis, int yAxis, int selectionFlags,
					QwtPicker::RubberBand rubberBand,
					QwtPicker::DisplayMode trackerMode, QwtPlot* plot);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QwtHistogramCoordinatesPicker ( );

	/**
	 * @return		L'éventuel panneau contenant l'histogramme.
	 */
	virtual QwtHistogramChartPanel* getHistogramPanel ( );
	virtual const QwtHistogramChartPanel* getHistogramPanel ( ) const;

	/**
	 * \@return		L'éventuel histogramme associé.
	 */
	virtual QwtExtendedMultiBarChart* getHistogram ( );
	virtual const QwtExtendedMultiBarChart* getHistogram ( ) const;

	/**
	 * @return		<I>false</I>.
	 */
	virtual bool allowCurveEdition ( ) const
	{ return false; }

	/**
	 * @return		<I>false</I>.
	 */
	virtual bool allowAbscissaTranslation ( ) const
	{ return false; }

	/**
	 * @param		Nouveau mode de sélection. Annule la sélection courrante
	 *				si ce nouveau mode est différent de celui en cours.
	 * @see			getSelectionMode
	 */
	virtual void setSelectionMode (SELECTION_MODE mode);

	/**
	 * @return		La courbe sélectionnée, ou 0 s'il n'y en a pas.
	 */
//	virtual QwtPlotCurve* getSelectedCurve ( );
//	virtual const QwtPlotCurve* getSelectedCurve ( ) const;

	/**
	 * @return		Les extrémités de la zone sélectionnée.
	 */
	virtual void getSelectionArea (
						double& x1, double& y1, double& x2, double& y2) const;

	/**
	 * @return		Les extrema de la zone sélectionnée.
	 */
	virtual void getSelectionExtrema (
				double& xmin, double& ymin, double& xmax, double& ymax) const;

	/**
	 * Annule la sélection en cours.
	 */
	virtual void clearSelection ( );

	/**
	 * Se met à jour vis à vis du graphique suite à une modification de ce
	 * dernier. Méthode à invoquer suite à une opération de zoom par exemple.
	 * Invoque <I>replot</I> sur le graphique si nécessaire.
	 */
	virtual void updateFromPlot ( );

	/**
	 * La distance en pixels à partir de laquelle un point est sélectionnable.
	 */
	static double	_selectionDistance;


	signals :

	/**
	 * Signal émit lorsqu'une courbe est modifiée de manière interactive.	
	 * @param		indice de la courbe modifiée
	 * @param		indice du point modifié
	 */
	void pointModified (int curve, int point);



	protected :

	/**
	 * Appelé lors d'un clic souris.
	 */
	virtual void widgetMouseReleaseEvent (QMouseEvent* event);

	/**
	 * Appelé lors de l'initialisation d'une sélection.
	 */
	virtual void begin ( );

	/**
	 * Appelé en fin d'action sur une sélection.
	 * @return		true si l'opération de sélection est acceptée.
	 * @see			pointEnd
	 * @see			rectEnd
	 */
	virtual bool end (bool ok);

	/**
	 * @return		Le texte que l'instance doit afficher à côté du curseur de
	 *				la souris. Prend en compte le type de représentation du
	 *				graphique.
	 * @see			getDisplayMode
	 */
	virtual QwtText trackerTextF (const QwtDoublePoint& pos) const;	// v 1.12.0

	/**
	 * @return		<I>true</I> si le point tranmis en argument est dans la zone
	 *				de sélection, <I>false</I> dans le cas contraire
	 *				(coordonnées en pixels). Tient compte de la sensibilité de
	 *				sélection en mode <I>POINT</I>.
	 */
	virtual bool inSelectionArea (int x, int y) const;


	private :

	/** Constructeurs de copie interdits. */
	QwtHistogramCoordinatesPicker (const QwtHistogramCoordinatesPicker&);
	QwtHistogramCoordinatesPicker& operator = (
										const QwtHistogramCoordinatesPicker&);
};	// class QwtHistogramCoordinatesPicker

#endif	// QWT_HISTOGRAM_COORDINATES_PICKER_H
