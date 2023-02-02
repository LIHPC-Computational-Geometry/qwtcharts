#ifndef QWT_CURVE_CHART_PANEL_H
#define QWT_CURVE_CHART_PANEL_H

#include <QwtCharts/QwtChartPanel.h>
#include <QwtCharts/QwtExtendedPlotCurve.h>
#include <QwtCharts/QwtPlotCoordinatesPicker.h>

#include <qwt_plot_curve.h>

#include <string>


/**
 * <P>
 * Classe "panneau graphique" contenant des courbes et reposant sur Qwt.
 * </P>
 * <P><B>Utilisation de la sélection avec édition interactive :</B><BR>
 * La sélection se fait à la souris à la condition préalable que 
 * allowCurveEdition (true) ait été invoqué. Trois modes de sélection sont
 * possibles (cf. setSelectionMode) : sélection d'une courbe par clic de la 
 * souris, d'un point par clic de la souris dessus, ou sélection de points dans
 * une zone rectangulaire par délimitation du rectangle à la souris avec le
 * bouton gauche enfoncé. La <B>désélection</B> s'effectue par clic du bouton
 * gauche dans une zone éloignée de la sélection courante. Le <B>déplacement</B>
 * des points sélectionnés s'effectue par déplacement de la souris avec le
 * bouton gauche enfoncé.
 * </P>
 * <P>Les classes dérivées peuvent spécifier le type de courbes utilisées en
 * surchargeant la méthode <I>instantiateCurve</I>.
 * </P>
 * <P>
 * Exemple d'utilisation :<BR>
 * <PRE>
 *
 * // Classe MDI affichant une courbe.
 *
 * class QtCurveView : public QMdiSubWindow
 * {
 * 	public :
 * 
 * 	enum CURVE_TYPE {SIN, COS, TAN};
 * 
 * 	QtCurveView (QMdiArea* parent, CURVE_TYPE type);
 * 
 *
 * // Menu affiché lorsque l'utilisateur clique du bouton droit de la souris.
 *
 * 	virtual void setContextMenu (QMenu* menu);
 *
 * // @return	Une référence sur le graphique représenté.
 *	QwtCurveChartPanel& getChartPanel ( )
 *	{ return *_plotWidget; }
 *	const QwtCurveChartPanel& getChartPanel ( ) const
 *	{ return *_plotWidget; }
 * 
 * 
 * 	protected :
 * 
 * 	//
 * 	//Affiche le menu popup associé à l'emplacement du curseur de la souris.
 * 	//
 * 	virtual void contextMenuEvent (QContextMenuEvent* event);
 *
 *	// Bogue workaround. Dans cette configuration un déplacement de souris
 *	// amorcé avec le clic gauche en dehors de la zone de tracé, mais dans la
 *	// légende, a tendance à redimensionner la fenêtre MDI.
 *
 *  virtual void mousePressEvent (QMouseEvent* mouseEvent);
 *  virtual void mouseReleaseEvent (QMouseEvent* mouseEvent);
 *  virtual void mouseMoveEvent (QMouseEvent* mouseEvent);
 *
 * 
 * 
 * 	private :
 * 
 * 	CURVE_TYPE			_type;
 * 	QwtCurveChartPanel*	_plotWidget;
 * 	QMenu*				_contextMenu;
 * 	bool				_dragging;
 * };	// class QtCurveView
 * 
 * 
 * //
 * //Fenêtre principale de l'application permettant d'afficher plusieurs courbes
 * //dans un support MDI.
 * //
 * class QtCurvesMainWindow : public QMainWindow
 * {
 * 	Q_OBJECT
 * 
 * 
 * 	public :
 * 
 * 	QtCurvesMainWindow (QWidget* parent);
 * 
 * 
 * 	protected slots :
 * 
 * 	virtual void exitCallback ( );
 * 	virtual void cascadeCallback ( );
 * 	virtual void tileCallback ( );
 * 	virtual void sinCallback ( );
 * 	virtual void cosCallback ( );
 * 	virtual void tanCallback ( );
 *	virtual void subWindowActivated (QMdiSubWindow*);
 * 
 * 
 * 	protected :
 * 
 * 	virtual QtCurveView* createCurveView (QtCurveView::CURVE_TYPE type);
 * 
 * 
 * 	private :
 * 
 * 	QMdiArea		*_mdiArea;
 * 	QMenu			*_windowMenu;
 * 	QtCurveView*	_curveViews [QtCurveView::TAN + 1];
 * };	// class QtCurvesMainWindow
 *
 * QtCurveView::QtCurveView (QMdiArea* parent, CURVE_TYPE type)
 * 	: QMdiSubWindow (parent), _type (type), _plotWidget (0), _contextMenu (0),
 * 	  _dragging (false)
 * {
 * 	_plotWidget	= new QwtCurveChartPanel (this);
 * 	_plotWidget->resize (1200, 800);
 * 	layout ( )->addWidget (_plotWidget);
 * 	_plotWidget->setAbscissaTitle (QwtText ("X"));
 * 	_plotWidget->setOrdinateTitle (QwtText ("Y"));
 * 	QString	name;
 * 	switch (_type)
 * 	{
 * 		case SIN	: name	= "y = sin (x)";	break;
 * 		case COS	: name	= "y = cos (x)";	break;
 * 		case TAN	: name	= "y = tan (x)";	break;
 * 	};
 * 	QwtPlotCurve&	curve	= _plotWidget->addCurve (name);
 * 	curve.setRenderHint (QwtPlotItem::RenderAntialiased);
 * 
 * 	AutoArrayPtr<double>	xData (720);
 * 	AutoArrayPtr<double>	yData (720);
 * 	for (int i = 0; i < 720; i++)
 * 	{
 * 		double	rad	= 2 * M_PI * (i - 360) / 360;
 * 		xData [i]	= rad;
 * 		switch (_type)
 * 		{
 * 			case SIN	:
 * 				yData [i]	= sin (rad);
 * 				break;
 * 			case COS	:
 * 				yData [i]	= cos (rad);
 * 				break;
 * 			case TAN	:
 * 				yData [i]	= tan (rad);
 * 				break;
 * 		}	// switch (_type)
 * 	}	// for (int i = 0; i < 720; i++)
 * 	curve.setData (xData.get ( ), yData.get ( ), 720);
 * 	// Mise à jour du graphique, notamment pour que la fonction d'annulation du
 * 	// zoom fonctionne.
 * 	_plotWidget->dataModified ( );
 *	// Rendre la courbe y = sin (x) éditable :
 *	if (SIN == _type)
 *		_plotWidget->allowCurveEdition (true);
 * }	// QtCurveView::QtCurveView
 * 
 * 
 * void QtCurveView::setContextMenu (QMenu* menu)
 * {
 * 	_contextMenu	= menu;
 * }	// QtCurveView::setContextMenu
 * 
 * 
 * void QtCurveView::contextMenuEvent (QContextMenuEvent* event)
 * {
 * 	if (0 != _contextMenu)
 * 		_contextMenu->popup (event->globalPos ( ));
 * }	// QtCurveView::contextMenuEvent
 *
 * void QtCurveView::mousePressEvent (QMouseEvent* mouseEvent)
 * {
 *		QPoint  p           = mouseEvent->pos ( );
 *		QRegion childRegion = childrenRegion ( );
 *		if ((false == childRegion.contains (p)) || (true == _dragging))
 *		{
 *			_dragging   = true;
 *			QMdiSubWindow::mousePressEvent (mouseEvent);
 *		}
 *		else
 *		{
 *			_dragging   = false;
 *			mouseEvent->ignore ( );
 *		}
 * }   // QtCurveView::mousePressEvent
 *
 * void QtCurveView::mouseReleaseEvent (QMouseEvent* mouseEvent)
 * {
 *	if (true == _dragging)
 *	{
 *		_dragging   = false;
 *		QMdiSubWindow::mouseReleaseEvent (mouseEvent);
 *	}
 * }   // QtCurveView::mouseReleaseEvent
 *
 * void QtCurveView::mouseMoveEvent (QMouseEvent* mouseEvent)
 * {
 *  QPoint  p           = mouseEvent->pos ( );
 *  QRegion childRegion = childrenRegion ( );
 *  if ((true == _dragging) || (false == childRegion.contains (p)))
 *  	QMdiSubWindow::mouseMoveEvent (mouseEvent);
 *  else
 *  	_dragging   = false;
 * }   // QtCurveView::mouseMoveEvent
 *
 * 
 * QtCurvesMainWindow::QtCurvesMainWindow (QWidget* parent)
 * 	: QMainWindow (parent), _mdiArea (0), _windowMenu (0)
 * {
 * 	setWindowTitle ("Courbes");
 * 	_mdiArea	= new QMdiArea (this);
 * 	setCentralWidget (_mdiArea);
 * // Les panneaux graphiques ne reçoivent pas l'évènement focusInEvent
 * // car c'est un ancêtre (QtCurveView) qui le recevra => s'informer du
 * // changement de fenêtre active afin d'en faire part au gestionnaire
 * // de graphiques :
 * connect (_mdiArea, SIGNAL (subWindowActivated (QMdiSubWindow*)), this,
 * SLOT (subWindowActivated (QMdiSubWindow*)));
 * 
 * 	for (size_t i = 0; i < (size_t)QtCurveView::TAN + 1; i++)
 * 		_curveViews [i]	= 0;
 * 
 * 	// Création du menu :
 * 	QAction*	action	= 0;
 * 	QMenuBar*	bar	= menuBar ( );
 * 	QMenu*		menu	= bar->addMenu ("Application");
 * 	menu->addAction ("Quitter", this, SLOT (exitCallback ( )));
 * 	_windowMenu	= bar->addMenu ("Fenêtre");
 * 	_windowMenu->addAction ("Cascade", this, SLOT (cascadeCallback ( )));
 * 	_windowMenu->addAction ("Tuiles", this, SLOT (tileCallback ( )));
 * 	_windowMenu->addSeparator ( );
 * 	_windowMenu->addAction (QwtChartsManager::getPrintAction ( ));
 * 	_windowMenu->addSeparator ( );
 * 	_windowMenu->addAction ("sin (x)", this, SLOT (sinCallback ( )));
 * 	_windowMenu->addAction ("cos (x)", this, SLOT (cosCallback ( )));
 * 	_windowMenu->addAction ("tan (x)", this, SLOT (tanCallback ( )));
 * 
 * 	// Utilisation de la barre d'outils par défaut de QwtChartsManager :
 * 	addToolBar (Qt::LeftToolBarArea, &QwtChartsManager::getToolBar ( ));
 * }	// QtCurvesMainWindow::QtCurvesMainWindow
 * 
 * 
 * void QtCurvesMainWindow::exitCallback ( )
 * {
 * 	QApplication::exit ( );
 * }	// QtCurvesMainWindow::exitCallback
 * 
 * 
 * void QtCurvesMainWindow::tileCallback ( )
 * {
 * 	_mdiArea->tileSubWindows ( );
 * }	// QtCurvesMainWindow::tileCallback
 * 
 * 
 * void QtCurvesMainWindow::cascadeCallback ( )
 * {
 * 	_mdiArea->cascadeSubWindows ( );
 * }	// QtCurvesMainWindow::cascadeCallback
 * 
 * 
 * void QtCurvesMainWindow::sinCallback ( )
 * {
 * 	QtCurveView*	view	= _curveViews [QtCurveView::SIN];
 * 	if (0 != view)
 * 	{
 * 		view->showNormal ( );
 * 		_mdiArea->setActiveSubWindow (view);
 * 	}
 * 	else
 * 		createCurveView (QtCurveView::SIN);
 * }	// QtCurvesMainWindow::sinCallback
 * 
 * 
 * void QtCurvesMainWindow::cosCallback ( )
 * {
 * 	QtCurveView*	view	= _curveViews [QtCurveView::COS];
 * 	if (0 != view)
 * 	{
 * 		view->showNormal ( );
 * 		_mdiArea->setActiveSubWindow (view);
 * 	}
 * 	else
 * 		createCurveView (QtCurveView::COS);
 * }	// QtCurvesMainWindow::cosCallback
 * 
 * 
 * void QtCurvesMainWindow::tanCallback ( )
 * {
 * 	QtCurveView*	view	= _curveViews [QtCurveView::TAN];
 * 	if (0 != view)
 * 	{
 * 		view->showNormal ( );
 * 		_mdiArea->setActiveSubWindow (view);
 * 	}
 * 	else
 * 		createCurveView (QtCurveView::TAN);
 * }	// QtCurvesMainWindow::tanCallback
 * 
 * 
 * QtCurveView* QtCurvesMainWindow::createCurveView (QtCurveView::CURVE_TYPE type)
 * {
 * 	QtCurveView*	view	= new QtCurveView (_mdiArea, type);
 * 	_mdiArea->addSubWindow (view);
 * 	view->show ( );
 * 	QMenu*	menu	= new QMenu (view);
 * 	menu->addMenu (_windowMenu);
 * 	QMenu*	graphicMenu	= QwtChartsManager::createMenu (0, "Graphique ...");
 * 	menu->addMenu (graphicMenu);	
 * 	view->setContextMenu (menu);
 * 	_curveViews [type]	= view;
 * 
 * 	return view;
 * }	// QtCurvesMainWindow::createCurveView
 *
 * void QtCurvesMainWindow::subWindowActivated (QMdiSubWindow* window)
 * {
 *     QtCurveView*    curveView   = dynamic_cast<QtCurveView*>(window);
 *     QwtChartPanel*  panel       =
 *                          0 == curveView ? 0 : &curveView->getChartPanel ( );
 *     QwtChartsManager::getManager ( ).focusChanged (panel);
 * }   // QtCurvesMainWindow::subWindowActivated
 *
 * 
 * int main (int argc, char* argv[])
 * {
 * 	QApplication	application (argc, argv);
 * 	QwtChartsManager::initialize ( );
 * 	QtCurvesMainWindow*	mainWindow	= new QtCurvesMainWindow (0);
 * 	mainWindow->show ( );
 * 	return application.exec ( );
 * }	// main
 * </PRE>
 * </P>
 * @warning		Les méthodes de cette classe sont susceptibles de lever de
 *				exception de type Exception.
 */
class QwtCurveChartPanel : public QwtChartPanel
{
	Q_OBJECT


	public :

	/**
	 * Constructeur par défaut.
	 * @param		Widget parent.
	 */
	QwtCurveChartPanel (QWidget* parent);

	/**
	 * Destructeur.
	 */
	virtual ~QwtCurveChartPanel ( );

	/**
	 * A invoquer lorsque les données ont été modifiées par un autre biais
	 * que par cette bibliothèque.
	 * Met à jour la gestion du zoom et de la sélection en cours.
	 */
	virtual void dataModified ( );

	/**
	 * Créé une courbe et l'ajoute au graphique. Les classes dérivées peuvent
	 * choisir le type de courbe à ajouter en surchargeant la méthode 
	 * <I>instantiateCurve</I>.
	 * @param		Nom de la courbe créée.
	 * @return		Référence sur la courbe créée.
	 * @see			instantiateCurve
	 */
	virtual QwtExtendedPlotCurve& addCurve (const QString& name);

	/**
	 * Enlève et détruit la courbe transmise en argument du graphique.
	 * @exception	Une exception est levée si la courbe n'appartient pas au
	 *				graphique.
	 * @warning		La courbe n'est plus utilisable après cet appel.
	 */
	virtual void removeCurve (QwtExtendedPlotCurve* curve);

	/**
	 * @param		Indice de la courbe demandée.
	 * @return		Une référence sur la i-ème courbe gérée.
	 * @exception	Une exception est levée en cas d'absence de telle courbe.
	 */
	virtual QwtExtendedPlotCurve& getCurve (size_t i);
	virtual const QwtExtendedPlotCurve& getCurve (size_t i) const;

	/**
	 * @param		Nom de la courbe demandée.
	 * @return		Une référence sur première courbe de nom <I>name</I>
	 *				gérée.
	 * @exception	Une exception est levée en cas d'absence de telle courbe.
	 */
	virtual QwtExtendedPlotCurve& getCurve (const QString& name);
	virtual const QwtExtendedPlotCurve& getCurve (const QString& name) const;

	/**
	 * Ajuste le graphique à ses courbes. Cette opération est automatiquement
	 * faite par Qwt par défaut, mais pas toujours comme attendu. Par exemple,
	 * une courbe allant de x = 0 à x = 180 est dans un graphique allant de 0 
	 * à 200.
	 * @param		Faut-il ajuster le graphique en abscisses
	 * @param		Faut-il ajuster le graphique en ordonnées
	 */
	virtual void adjustChart (
					bool adjustAbscissa = true, bool adjustOrdinates = true);

	/**
	 * Passe (<I>true</I>) en possibilité de mode édition interactive de courbe
	 * ou quitte ce mode (<I>false</I>).
	 * @param		true si les courbes peuvent être modifiées de manière
	 *				interactive à la souris, false dans le cas contraire.
	 * @warning		Le mode d'édition interactive est suspendu si le mode
	 *				zoom est activé.
	 * @see			pointModified
	 * @see			allowAbscissaTranslation
	 */
	virtual void allowCurveEdition (bool allow);

	/**
	 * @return		true si les courbes sont éditables de manière interactive,
	 *				false dans le cas contraire.
	 * @warning		susceptible de retourner true alors que l'édition 
	 *				interactive de courbe est (temporairement) inactivée pour
	 *				cause de mode zoom interactif activé.
	 * @see			pointModified
	 * @see			allowAbscissaTranslation
	 */
	virtual bool allowCurveEdition ( ) const;

	/** 
	 * Peut-on modifier interactivement la sélection d'une courbe parallèlement
	 * à l'axe des abscisses ?
	 * @param		<I>true</I> si cette opération est autorisée, <I>false</I>
	 *				dans le cas contraire.
	 * @see			abscissaTranslationAllowed
	 */
	virtual void allowAbscissaTranslation (bool allow);

	/**
	 * @return		<I>true</I> si la modification interactive de la sélection
	 *				d'une courbe parallèlement à l'axe des abscisses est	
	 *				autorisée, <I>false</I> dans le cas contraire.
	 * @see			allowAbscissaTranslation
	 */
	virtual bool allowAbscissaTranslation ( ) const;

	/**
	 * @return		Le mode de sélection courrant
	 * @see			setSelectionMode
	 */
	virtual QwtPlotCoordinatesPicker::SELECTION_MODE getSelectionMode ( ) const;

	/**
	 * @param		Nouveau mode de sélection. Annule la sélection courrante
	 *				si ce nouveau mode est différent de celui en cours.
	 * @see			getSelectionMode
	 */
	virtual void setSelectionMode (
								QwtPlotCoordinatesPicker::SELECTION_MODE mode);

	/**
	 * Annule, si possible, la dernière modification interactive effectuée.
	 */
	virtual void undoLastModifiedPoint ( );

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


	signals :

	/**
	 * Signal émit lorsqu'une courbe est modifiée de manière interactive.
	 * @param		indice de la courbe modifiée
	 * @param		indice du point modifié
	 */
	void pointModified (int curve, int point);


	protected :

	/**
	 * Créé une courbe sans l'ajouter au graphique. Les classes dérivées peuvent
	 * choisir le type de courbe à ajouter en surchargeant cette méthode.
	 * @param		Nom de la courbe créée.
	 * @return		Pointeur sur la courbe créée.
	 * @see			addCurve
	 */
	virtual QwtExtendedPlotCurve* instantiateCurve (const QString& name);


	protected slots :

	/**
	 * Appelé lorsqu'un point d'une courbe est modifié interactivement à la
	 * souris. Invoque setModified (curve, true).
	 * Emet le signal pointModified avec pour arguments ceux reçus.
	 * @param		indice de la courbe modifiée
	 * @param		indice du point modifié
	 * @see			pointModified
	 * @see			allowCurveEdition
	 */
	void pointModifiedCallback (int curve, int point);


	private :

	/** Constructeurs de copie interdits. */
	QwtCurveChartPanel (const QwtCurveChartPanel&);
	QwtCurveChartPanel& operator = (const QwtCurveChartPanel&);

	/** Les courbes gérées. */
	IN_STD vector<QwtExtendedPlotCurve*>		_curves;
};	// class QwtCurveChartPanel

#endif	// QWT_CURVE_CHART_PANEL_H
