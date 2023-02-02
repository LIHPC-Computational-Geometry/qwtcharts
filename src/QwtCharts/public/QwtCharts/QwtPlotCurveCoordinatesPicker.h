#ifndef QWT_PLOT_CURVE_COORDINATES_PICKER_H
#define QWT_PLOT_CURVE_COORDINATES_PICKER_H

#include <TkUtil/util_config.h>

#include <qwt_global.h>	// => QWT_VERSION

#include <qwt_interval.h>
#include <qwt_point_3d.h>
#include <qwt_series_data.h>
#include <qwt_compat.h>

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_picker.h>
#include <vector>

#include "QwtCharts/QwtChartPanel.h"


/**
 * <P>Classe de picker Qwt affichant les coordonnées pointées par la souris
 * spécialisé pour les graphiques de type <I>Courbes</I>.
 * Permet la sélection d'entités du graphique qui peuvent être déplacées
 * interactivement.
 * </P>
 * @warning		Les instances de cette classe ne sont pas informées des
 *				éventuels évènements pouvant survenir au niveau du graphique,
 *				tel qu'un zoom. Il convient d'appeler la méthode updateFromPlot
 *				dans de telles circonstances.
 */
class QwtPlotCurveCoordinatesPicker : public QwtPlotCoordinatesPicker
{
	Q_OBJECT

	public :

	/**
	 * Constructeur par défaut.
	 */
	QwtPlotCurveCoordinatesPicker (
					int xAxis, int yAxis, int selectionFlags,
					QwtPicker::RubberBand rubberBand,
					QwtPicker::DisplayMode trackerMode, QwtPlot* plot);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QwtPlotCurveCoordinatesPicker ( );

	/**
	 * @return		Le mode de représentation du graphique.
	 * @see			setDisplayMode
	 * @see			trackerText
	 */
	virtual QwtChartPanel::DISPLAY_MODE getDisplayMode ( ) const
	{ return _displayMode; }

	/**
	 * @param		Le nouveau mode de représentation du graphique.
	 * @see			getDisplayMode
	 */
	virtual void setDisplayMode (QwtChartPanel::DISPLAY_MODE mode);

	/**
	 * @return		Le mode de sélection courrant
	 * @see			setSelectionMode
	 */
	virtual SELECTION_MODE getSelectionMode ( ) const
	{ return getSelection ( ).mode; }

	/**
	 * @param		Nouveau mode de sélection. Annule la sélection courrante
	 *				si ce nouveau mode est différent de celui en cours.
	 * @see			getSelectionMode
	 */
	virtual void setSelectionMode (SELECTION_MODE mode);

	/**
	 * @return		La courbe sélectionnée, ou 0 s'il n'y en a pas.
	 */
	virtual QwtPlotCurve* getSelectedCurve ( );
	virtual const QwtPlotCurve* getSelectedCurve ( ) const;

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
	 * Annule, si possible, la dernière modification interactive effectuée.
	 */
	virtual void undoLastModifiedPoint ( );

	/**
	 * Se met à jour vis à vis du graphique suite à une modification de ce
	 * dernier. Méthode à invoquer suite à une opération de zoom par exemple.
	 * Invoque <I>replot</I> sur le graphique si nécessaire.
	 */
	virtual void updateFromPlot ( );

	/**
	 * @return	L'index du point le plus proche de la courbe tranmise en premier
	 *			argument dont l'abscisse en transmis en second argument.
	 */
	virtual size_t pointAtX (const QwtPlotCurve& curve, double x) const;

	/**
	 * La distance en pixels à partir de laquelle un point est sélectionnable.
	 */
	static double	_selectionDistance;

	/**
	 * @return		La courbe représentant la représentation de la sélection.
	 * @warning		<B>ATTENTION :</B> accesseurs à des fins internes.
	 */
	virtual QwtPlotCurve* getSelectionRepresentation ( );
	virtual const QwtPlotCurve* getSelectionRepresentation ( ) const;


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
	virtual void widgetMousePressEvent (QMouseEvent* event);
	virtual void widgetMouseReleaseEvent (QMouseEvent* event);

	/**
	 * Appelé lors de déplacement de la souris.
	 */
	virtual void widgetMouseMoveEvent (QMouseEvent* event);


	/**
	 * Appelé lors de l'initialisation d'une sélection.
	 */
	virtual void begin ( );

	/**
	 * Appelé en fin d'action sur une sélection.
	 * @return		true si l'opération de sélection est acceptée.
	 * @see			curveEnd
	 * @see			pointEnd
	 * @see			rectEnd
	 */
	virtual bool end (bool ok);

	/**
	 * Appelé en fin d'action sur une sélection de type courbe.
	 * @return		true si l'opération de sélection est acceptée.
	 */
	virtual bool curveEnd (bool ok);

	/**
	 * Appelé en fin d'action sur une sélection de type point.
	 * @return		true si l'opération de sélection est acceptée.
	 */
	virtual bool pointEnd (bool ok);

	/**
	 * Appelé en fin d'action sur une sélection de type rectangle.
	 * @return		true si l'opération de sélection est acceptée.
	 */
	virtual bool rectEnd (bool ok);

	/**
	 * Appelé en fin d'action sur une sélection de type interpolation
	 * linéaire.
	 * @return		true si l'opération de sélection est acceptée.
	 */
	virtual bool linearInterpolationEnd (bool ok);

	/**
	 * @return		Le texte que l'instance doit afficher à côté du curseur de
	 *				la souris. Prend en compte le type de représentation du
	 *				graphique.
	 * @see			getDisplayMode
	 */
	virtual QwtText trackerText (const QwtDoublePoint& pos) const;

	/**
	 * Met à jour la sélection en fonction de ses paramètres et des courbes
	 * en cours.
	 */
	virtual void updateSelection ( );

	/**
	 * @return		<I>true</I> si le point tranmis en argument est dans la zone
	 *				de sélection, <I>false</I> dans le cas contraire
	 *				(coordonnées en pixels). Tient compte de la sensibilité de
	 *				sélection en mode <I>POINT</I>.
	 *                          */
    virtual bool inSelectionArea (int x, int y) const;

	/**
	 * Déplace la sélection en cours.
	 * @param		<I>dx</I> et <I>dy</I> du déplacement à effectuer.
	 */
	virtual void moveSelection (int dx, int dy);

	/**
	 * Créé la représentation de la sélection si une courbe est sélectionnée.
	 */
	virtual void createSelectionRepresentation ( );


	private :

	/** Constructeurs de copie interdits. */
	QwtPlotCurveCoordinatesPicker (const QwtPlotCurveCoordinatesPicker&);
	QwtPlotCurveCoordinatesPicker& operator = (
										const QwtPlotCurveCoordinatesPicker&);

	/** Le mode d'affichage. */
	QwtChartPanel::DISPLAY_MODE		_displayMode;

    /** Les indices des points saisis par l'utilisateur lors de modifcations
	 * (par exemple ceux donnés par l'utilisateur lors d'une modification par
	 * interpolation, mais ne comprend pas les autres points modifiés, à savoir
	 * ceux compris par les intervalles définis par l'utilisateur). */
	IN_STD vector<size_t>			_userPointIndexes;


	/** La représentation de la sélection. */
	QwtPlotCurve*					_selectionRepresentation;

	/** L'original d'une courbe en cours de modification. */
	QwtPlotCurve*					_originalCurve;

	/** Copie des données originales de la courbe en cours de modification. */
	QwtSeriesData<QPointF>*			_originalData;
};	// class QwtPlotCurveCoordinatesPicker

#endif	// QWT_PLOT_CURVE_COORDINATES_PICKER_H
