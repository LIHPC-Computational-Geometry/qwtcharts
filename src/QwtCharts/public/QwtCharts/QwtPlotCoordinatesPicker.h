#ifndef QWT_PLOT_COORDINATES_PICKER_H
#define QWT_PLOT_COORDINATES_PICKER_H

#include <TkUtil/util_config.h>

#include <qwt_global.h>	// => QWT_VERSION

#include <qwt_compat.h>

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_picker.h>
#include <vector>

class QwtChartPanel;


/**
 * <P>Classe de picker Qwt de base affichant les coordonnées pointées par la
 * souris.
 * Permet la sélection d'entités du graphique qui peuvent être déplacées
 * interactivement.
 * </P>
 * <P>Interagit avec une instance de la classe <I>QwtChartPanel</I> lors
 * d'évènements souris type "molette" pour jouer sur le facteur de zoom.
 * </P>
 * @warning		Les instances de cette classe ne sont pas informées des
 *				éventuels évènements pouvant survenir au niveau du graphique,
 *				tel qu'un zoom. Il convient d'appeler la méthode updateFromPlot
 *				dans de telles circonstances.
 */
class QwtPlotCoordinatesPicker : public QwtPlotPicker
{
	Q_OBJECT

	public :

	/**
	 * Constructeur par défaut.
	 */
	QwtPlotCoordinatesPicker (
					int xAxis, int yAxis, int selectionFlags,
					QwtPicker::RubberBand rubberBand,
					QwtPicker::DisplayMode trackerMode, QwtPlot* plot);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QwtPlotCoordinatesPicker ( );

	/**
	 * @return		Le graphique pris en charge.
	 */
	virtual QwtPlot* getPlot ( )
	{ return _plot; }
	virtual const QwtPlot* getPlot ( ) const
	{ return _plot; }

	/**
	 * @return      L'éventuel panneau associé contenant le graphique pris en
	 * 				charge.
	 */
	virtual QwtChartPanel* getChartPanel ( )
	{ return _chartPanel; }
	virtual const QwtChartPanel* getChartPanel ( ) const
	{ return _chartPanel; }

	/**
	 * @param       Eventuel  panneau associé contenant le graphique pris en
	 *				charge.
	 */
	virtual void setChartPanel (QwtChartPanel* chartPanel)
	{ _chartPanel   = chartPanel; }

	/**
	 * @return		<I>true</I> si les courbes sont éditables interactivement,
	 *				<I>false</I> dans le cas contraire.
	 */
	virtual bool allowCurveEdition ( ) const
	{ return _allowCurveEdition; }

	/**
	 * @param		<I>true</I> si les courbes sont éditables interactivement,
	 *				<I>false</I> dans le cas contraire.
	 */
	virtual void allowCurveEdition (bool allow);

	/** 
	 * Peut-on modifier interactivement la sélection d'une courbe parallèlement
	 * à l'axe des abscisses ?
	 * @param		<I>true</I> si cette opération est autorisée, <I>false</I>
	 *				dans le cas contraire.
	 * @see			allowAbscissaTranslation
	 */
	virtual void allowAbscissaTranslation (bool allow);

	/**
	 * @return		<I>true</I> si la modification interactive de la sélection
	 *				d'une courbe parallèlement à l'axe des abscisses est	
	 *				autorisée, <I>false</I> dans le cas contraire.
	 * @see			allowAbscissaTranslation
	 */
	virtual bool allowAbscissaTranslation ( ) const
	{ return _allowAbscissaTranslation; }

	/**
	 * Les modes de sélections de données du graphique :
	 * <UL>
	 * <LI><I>CURVE</I> : par clic souris, permet la sélection d'une courbe,
	 * <LI><I>POINT</I> : par clic souris, seule une donnée est alors
	 * sélectionnable,
	 * <LI><I>RUBBER_BAND</I> : par tracé élastique d'un rectangle, permet la
	 * sélection d'un ensemble de données,
	 * <LI><I>LINEAR_INTERPOLATION</I> : par définition de segments successifs
	 * par clics souris, touche <I>CTRL</I> enfoncée, où les ordonnées des
	 * points de chaque segments sont interpolées de manière linéaire. L'arrêt
	 * de la modification est effectué par clic souris touche <I>CTRL</I> 
	 * relâchée.
	 * </UL>
	 */
	enum SELECTION_MODE {CURVE, POINT, RUBBER_BAND, LINEAR_INTERPOLATION};

	/**
	 * Structure décrivant une sélection.
	 */
	struct Selection
	{
		/**
		 * Constructeur par défaut. Absence de sélection.
		 */
		Selection ( );

		/**
		 * Constructeur de copie, opérateur = : RAS.
		 */
		Selection (const Selection&);
		Selection& operator = (const Selection&);

		/**
		 * Annule toute sélection en cours.
		 */
		void clear ( );

		/** Y a-t-il une sélection en cours ? */
		bool					hasSelection;

		/** Le mode de sélection. */
		SELECTION_MODE			mode;

		/** La définition de la zone de sélection. */
		double					x1, y1, x2, y2;

		/** La courbe sélectionnée. */
		size_t					selectedCurveIndex;

		/** Les points sélectionnés. */
		IN_STD vector<size_t>	selectedPointIndexes;
	};	// Selection

	/**
	 * @return		La sélection courante.
	 */
	virtual Selection& getSelection ( );
	virtual const Selection& getSelection ( ) const;

	/**
	 * @return		Le mode de sélection courrant
	 * @see			setSelectionMode
	 */
	virtual SELECTION_MODE getSelectionMode ( ) const
	{ return _selection.mode; }

	/**
	 * @param		Nouveau mode de sélection. Annule la sélection courrante
	 *				si ce nouveau mode est différent de celui en cours.
	 * @warning		Ne fait rien d'autre, méthode à surcharger.
	 * @see			getSelectionMode
	 */
	virtual void setSelectionMode (SELECTION_MODE mode);

	/**
	 * @return		Les extrémités de la zone sélectionnée.
	 * @warning		Méthode à surcharger, retourne (0., 0., 0., 0.) par défaut.
	 */
	virtual void getSelectionArea (
						double& x1, double& y1, double& x2, double& y2) const;

	/**
	 * @return		Les extrema de la zone sélectionnée.
	 * @warning		Méthode à surcharger, retourne (0., 0., 0., 0.) par défaut.
	 */
	virtual void getSelectionExtrema (
				double& xmin, double& ymin, double& xmax, double& ymax) const;

	/**
	 * @return		La courbe sélectionnée, ou 0 s'il n'y en a pas.
	 * @warning		<B>Méthode à surcharger, retourne 0 par défaut.</B>
	 */
	virtual QwtPlotCurve* getSelectedCurve ( );
	virtual const QwtPlotCurve* getSelectedCurve ( ) const;

	/**
	 * Annule la sélection en cours.
	 */
	virtual void clearSelection ( );

	/**
	 * Se met à jour vis à vis du graphique suite à une modification de ce
	 * dernier. Méthode à invoquer suite à une opération de zoom par exemple.
	 * @warning		Ne fait rien, méthode à surcharger.
	 */
	virtual void updateFromPlot ( );


	protected :

	/**
	 * Appellé lors d'un évènement bouton de la souris.
	 */
	virtual void widgetMousePressEvent (QMouseEvent* event);
	virtual void widgetMouseMoveEvent (QMouseEvent* event);

	/**
	 * Appellé lors d'un évènement de la molette de la souris.
	 */
	virtual void widgetWheelEvent (QWheelEvent* event);

	/**
	 * Appelé lors d'évènements clavier.
	 */
	virtual void widgetKeyPressEvent (QKeyEvent* ev);
	virtual void widgetKeyReleaseEvent (QKeyEvent* ev);

	/**
	 * Affecte le bon curseur de la souris en fonction des coordonnées reçues.
	 */
	virtual void selectCursor (const QPoint& point);

	/**
	 * @return		<I>true</I> si le point tranmis en argument est dans la zone
	 *				de sélection, <I>false</I> dans le cas contraire
	 *				(coordonnées en pixels). Tient compte de la sensibilité de
	 *				sélection en mode <I>POINT</I>.
	 * @warning		Méthode à surcharger. Retoune false par défaut.
	 */
	virtual bool inSelectionArea (int x, int y) const;

	/**
	 * @return		La position initiale du curseur lors d'un évènement
	 *				interactif.
	 */
	virtual QPoint& getInitialCursorPos ( )
	{ return _initialCursorPos; }
	virtual const QPoint& getInitialCursorPos ( ) const
	{ return _initialCursorPos; }

	/**
	 * @return		La position courante du curseur lors d'un évènement
	 *				interactif.
	 */
	virtual QPoint& getCursorPos ( )
	{ return _cursorPos; }
	virtual const QPoint& getCursorPos ( ) const
	{ return _cursorPos; }

	/**
	 * @return		Rectangle englobant la zone sélectionnée.
	 */
	virtual QRect& getViewportSelectionArea ( )
	{ return _viewportSelectionArea; }
	virtual const QRect& getViewportSelectionArea ( ) const
	{ return _viewportSelectionArea; }

	/**
	 * @return		<I>true</I> si la touche contrôle est enfoncée, <I>false</I>
	 *				dans le cas contraire.
	 */
	virtual bool controlKeyPressed ( ) const
	{ return _controlPressed; }


	private :

	/** Constructeurs de copie interdits. */
	QwtPlotCoordinatesPicker (const QwtPlotCoordinatesPicker&);
	QwtPlotCoordinatesPicker& operator = (const QwtPlotCoordinatesPicker&);

	/** Le graphique pris en charge. */
	QwtPlot*					_plot;

	/** Eventuel panneau pris en charge. */
	QwtChartPanel*				_chartPanel;

	/** <I>true</I> si les opérations d'édition interactive de courbes sont
	 * autorisées. */
	bool						_allowCurveEdition;

	/** <I>true</I> si les opérations d'édition interactive de courbes 
	 * parallèlement à l'axe des abscisses sont autorisées. */
	bool						_allowAbscissaTranslation;

	/** La sélection courante. */
	Selection					_selection;

	/** Les indices des points saisis par l'utilisateur lors de modifcations
	 * (par exemple ceux donnés par l'utilisateur lors d'une modification par
	 * interpolation, mais ne comprend pas les autres points modifiés, à savoir
	 * ceux compris par les intervalles définis par l'utilisateur). */
	IN_STD vector<size_t>		_userPointIndexes;

	/** Coordonnées du curseur de la souris. */
	QPoint						_cursorPos;

	/** Coordonnées du curseur de la souris lors de l'appel à begin. */
	QPoint						_initialCursorPos;

	/** Le curseur inital de la fenêtre. */
	Qt::CursorShape				_initialCursorShape;

	/** Rectangle englobant la zone sélectionnée (zone sélectionnée majorée de
	 * _selectionDistance de chaque côté). */
	QRect						_viewportSelectionArea;

	/** Vaut <I>true</I> si la touche contrôle est enfoncée, <I>false</I> dans
	 * le cas contraire. */
	bool						_controlPressed;
};	// class QwtPlotCoordinatesPicker

#endif	// QWT_PLOT_COORDINATES_PICKER_H
