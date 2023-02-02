#ifndef QT_CURVES_APPLICATION_H
#define QT_CURVES_APPLICATION_H

#include <QMainWindow>
#include <QMenu>
#include <QAction>
#include <QMdiArea>
#include <QMdiSubWindow>

#include <QwtCharts/QwtCurveChartPanel.h>
#include <QwtCharts/QwtHistogramChartPanel.h>


/**
 * Classe MDI affichant une courbe.
 */
class QtCurveView : public QMdiSubWindow
{
	Q_OBJECT

	public :

	enum CURVE_TYPE {SIN, COS, TAN, ALL};

	QtCurveView (QMdiArea* parent, CURVE_TYPE type);

	/**
	 * @param		Menu affiché lorsque l'utilisateur clique du bouton droit
	 *				de la souris.
	 */
	virtual void setContextMenu (QMenu* menu);

	/**
	 * @return		Une référence sur le graphique représenté.
	 */
	QwtCurveChartPanel& getChartPanel ( )
	{ return *_plotWidget; }
	const QwtCurveChartPanel& getChartPanel ( ) const
	{ return *_plotWidget; }


	protected :

	/**
	 * Affiche le menu popup associé à l'emplacement du curseur de la souris.
	 */
	virtual void contextMenuEvent (QContextMenuEvent* event);

	/**
	 * Bogue workaround. Dans cette configuration un déplacement de souris
	 * amorcé avec le clic gauche en dehors de la zone de tracé, mais dans la
	 * légende, a tendance à redimensionner la fenêtre MDI.
	 */
	virtual void mousePressEvent (QMouseEvent* mouseEvent);
	virtual void mouseReleaseEvent (QMouseEvent* mouseEvent);
	virtual void mouseMoveEvent (QMouseEvent* mouseEvent);


	protected slots :

	virtual void displayModeCallback (QwtChartPanel*);


	private :

	CURVE_TYPE			_type;
	QwtCurveChartPanel*	_plotWidget;
	QMenu*				_contextMenu;
	bool				_dragging;	// Bogue workaround
};	// class QtCurveView


/**
 * Classe affichant un histogramme.
 */
class QtHistogramView : public QMdiSubWindow
{
	public :

	/**
	 * \param		<I>series</I> est le nombre de séries de l'histogramme.
	 */
	QtHistogramView (QMdiArea* parent, unsigned char series);

	/**
	 * @param		Menu affiché lorsque l'utilisateur clique du bouton droit
	 *				de la souris.
	 */
	virtual void setContextMenu (QMenu* menu);

	/**
	 * @return		Une référence sur le graphique représenté.
	 */
	QwtHistogramChartPanel& getChartPanel ( )
	{ return *_plotWidget; }
	const QwtHistogramChartPanel& getChartPanel ( ) const
	{ return *_plotWidget; }


	protected :

	/**
	 * Affiche le menu popup associé à l'emplacement du curseur de la souris.
	 */
	virtual void contextMenuEvent (QContextMenuEvent* event);


	private :

	QwtHistogramChartPanel*		_plotWidget;
	QMenu*						_contextMenu;
};	// class QtHistogramView


/**
 * Fenêtre principale de l'application permettant d'afficher plusieurs courbes
 * dans un support MDI.
 */
class QtChartsMainWindow : public QMainWindow
{
	Q_OBJECT


	public :

	QtChartsMainWindow (QWidget* parent);


	protected slots :

	virtual void invertZoomRotationCallback ( );
	virtual void exitCallback ( );
	virtual void cascadeCallback ( );
	virtual void tileCallback ( );
	virtual void sinCallback ( );
	virtual void cosCallback ( );
	virtual void tanCallback ( );
	virtual void allCallback ( );
	virtual void histogram1Callback ( );
	virtual void histogram3Callback ( );
	virtual void histogram5Callback ( );
	virtual void subWindowActivated (QMdiSubWindow*);


	protected :

	virtual QtCurveView* createCurveView (QtCurveView::CURVE_TYPE type);
	virtual QtHistogramView* createHistogramView (unsigned char num);


	private :

	QMdiArea			*_mdiArea;
	QMenu				*_windowMenu;
	QtCurveView*		_curveViews [QtCurveView::ALL + 1];
	QtHistogramView*	_histogramViews [3];
};	// class QtChartsMainWindow


#endif	// QT_CURVES_APPLICATION_H
