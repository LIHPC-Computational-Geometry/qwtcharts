#ifndef QWT_EXTENDED_PLOT_ZOOMER_H
#define QWT_EXTENDED_PLOT_ZOOMER_H


#include <qwt_global.h> // => QWT_VERSION


#include <TkUtil/util_config.h>
class QwtChartPanel;

#include <qwt_plot_canvas.h>
#include <qwt_plot.h>
#include <qwt_plot_zoomer.h>



/**
 * <P>Classe créé pour contourner un bogue rencontré avec Qwt 5.2.0 lorsque
 * l'on utilise les <I>QwtPlotZoomer</I> couples aux <I>QwtPlotRescaler</I>.
 * </P>
 * <P>Dans de telles circonstances, le comportement du zoom avec contrainte de
 * rapport d'aspect n'est pas toujours celui attendu (il peut ne pas fonctionner
 * ou donner des résultats autres que ceux définis).
 * </P>
 * <P>Depuis interagit également avec une instance de la classe
 * <I>QwtChartPanel</I> lors d'évènements souris type "molette" pour jouer sur 
 * le facteur de zoom.
 * </P>
 */
class QwtExtendedPlotZoomer : public QwtPlotZoomer
{
	public :

	/**
	 * Constructeurs.
	 */
	QwtExtendedPlotZoomer (QwtPlotCanvas* canvas, bool doReplot = true);
	QwtExtendedPlotZoomer (
			int xAxis, int yAxis, QwtPlotCanvas* canvas, bool doReplot = true);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QwtExtendedPlotZoomer ( );

	/**
	 * @return		Eventuelle instance de <I>QwtPlotRescaler</I> à reparamétrer
	 *				lors des opérations de zoom.
	 */
	virtual QwtChartPanel* getChartPanel ( )
	{ return _chartPanel; }
	virtual const QwtChartPanel* getChartPanel ( ) const
	{ return _chartPanel; }

	/**
	 * @param		Eventuelle instance de <I>QwtPlotRescaler</I> à reparamétrer
	 *				lors des opérations de zoom.
	 */
	virtual void setChartPanel (QwtChartPanel* chartPanel)
	{ _chartPanel	= chartPanel; }


	protected :

	/**
	 * Appellé lors d'un évènement de la molette de la souris.
	 */
	virtual void widgetWheelEvent (QWheelEvent* event);

	virtual void rescale ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QwtExtendedPlotZoomer (const QwtExtendedPlotZoomer&);
	QwtExtendedPlotZoomer& operator = (const QwtExtendedPlotZoomer&);

	/** Eventuelle instance de <I>QwtPlotRescaler</I> à reparamétrer
	 *lors des opérations de zoom. */
	QwtChartPanel*		_chartPanel;
};	// class QwtExtendedPlotZoomer


#endif	// QWT_EXTENDED_PLOT_ZOOMER_H
