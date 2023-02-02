#ifndef QWT_EXTENDED_PLOT_H
#define QWT_EXTENDED_PLOT_H


#include <qwt_global.h> // => QWT_VERSION


#include <TkUtil/util_config.h>


#include <qwt_plot.h>



/**
 * <P>Classe créé pour contourner un bogue rencontré avec Qwt 5.2.0 lorsque
 * l'on utilise les <I>QwtPlot</I> dans des fenêtres MDI avec une instance de
 * <I>QwtPlotRescaler</I>.</P>
 * <P>Dans de telles circonstances, le fait de redimensionner une fenêtre
 * contenant un graphique, en diminuant sa taille, provoque des défauts
 * d'affichage de l'instance de <I>QMdiArea</I> parent et des
 * <I>QMdiSubwindow</I> démasquées.
 * </P>
 */
class QwtExtendedPlot : public QwtPlot
{
	public :

	/**
	 * Constructeurs.
	 */
	QwtExtendedPlot (QWidget* = NULL);
	QwtExtendedPlot (const QwtText& title, QWidget* = NULL);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QwtExtendedPlot ( );

	/**
	 * Réaffiche le contenu du graphique.
	 */
	virtual void replot ( );

	/**
	 * Bloque les appels à <I>replot</I> du canvas.
	 */
	void lockCanvas (bool lock)
	{ _lockCanvas = lock; }


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QwtExtendedPlot (const QwtExtendedPlot&);
	QwtExtendedPlot& operator = (const QwtExtendedPlot&);

	bool	_lockCanvas;
};	// class QwtExtendedPlot


#endif	// QWT_EXTENDED_PLOT_H
