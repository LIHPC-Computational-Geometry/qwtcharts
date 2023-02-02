#ifndef QWT_CHARTS_MANAGER_H
#define QWT_CHARTS_MANAGER_H

#include <TkUtil/util_config.h>
#include <TkUtil/Version.h>
#include <QwtCharts/QwtChartPanel.h>
#include <QtUtil/QtExclusiveActionGroup.h>

#include <QToolBar>
#include <QMenu>

#include <vector>


/*!
 * \mainpage	Page principale de la bibliothèque QwtCharts.
 *
 * @section		presentation	Présentation
 * <P>Cette bibliothèque propose des services de gestion de graphiques reposant
 * sur la bibliothèque <I>Qwt</I> avec automatisation possible de certaines
 * actions (gestion de menu de modification des graphiques, ...).</P>
 * <P>Le fichier d'entête de la classe QwtCurveChartPanel contient un exemple
 * d'utilisation de cette API. </P>
 *
 * \section		depend			Dépendances
 * Cette bibliothèque utilise les bibliothèques TkUtil et QtUtil. Leurs
 * directives de compilation et d'édition des liens s'appliquent également à
 * celle-ci.
 *
 */


/**
 * <P>Gestionnaire de "panneaux graphiques" de type QwtChartPanel. 
 * </P>
 * <P>Cette classe propose des services de bases, tels que la gestion d'actions
 * associées à des menus et/ou barres d'outils, associées à un ensemble
 * d'instances de la classe QwtChartsManager et des classes qui en dérive. Pour
 * profiter des services de cette classe il suffit de l'initialiser à l'aide de
 * sa méthode initialize. Dès lors, toute instance créée de la classe
 * QwtChartPanel, ou d'une de ses classes dérivées, s'enregistre automatiquement
 * auprès de cette classe et bénéficie de ses services.
 * <P>
 * <P>Le fichier d'entête de la classe QwtCurveChartPanel contient un exemple
 * d'utilisation de cette API. </P>
 */
class QwtChartsManager : public QObject
{
	friend class QwtChartPanel;

	Q_OBJECT


	public :

	/**
	 * @return		La version de cette bibliothèque
	 */
	static const IN_UTIL Version& getVersion ( )
	{ return _version; }

	/**
	 * Initalisation de cette API.
	 * @see		finalize
	 * @see		isInitialized
	 * @see		setPrintAction
	 * @see		registerChartPanel
	 */
	static void initialize ( );

	/**
	 * Finalisation de cette API.
	 * @see		initialize
	 */
	static void finalize ( );

	/**
	 * @return		<I>true</I> si cette API est initialisée, <I>false</I> dans
	 *				le cas contraire.
	 */
	static bool isInitialized ( );

	/**
	 * @return		Action utilisée pour les impressions.	
	 * @warning		Doit être préalablement initialisée par <I>initialize</I>
	 *				ou <I>setPrintAction</I>.
	 * @see			initialize
	 * @see			setPrintAction
	 */
	static QAction* getPrintAction ( )
	{ return _printAction; }

	/**
	 * @param		Action à utiliser pour les impressions.	
	 * @warning		A invoquer avant initialize
	 * @see			initialize
	 */
	static void setPrintAction (QAction* printAction);

	/**
	 * @return		L'action utilisée pour passer du mode zoom au mode sélection
	 *				et réciproquement.
	 * @warning		Doit être préalablement initialisée par <I>initialize</I>.
	 */	
	static QAction* getZoomAction ( )
	{ return _zoomAction; }

	/**
	 * @return		L'action utilisée pour annuler le dernier zoom.
	 * @warning		Doit être préalablement initialisée par <I>initialize</I>.
	 */	
	static QAction* getUndoZoomAction ( )
	{ return _undoZoomAction; }

	/**
	 * @return		L'action utilisée pour refaire le dernier zoom.
	 * @warning		Doit être préalablement initialisée par <I>initialize</I>.
	 */	
	static QAction* getRedoZoomAction ( )
	{ return _redoZoomAction; }

	/**
	 * @return		L'action utilisée pour annuler tout zoom.
	 * @warning		Doit être préalablement initialisée par <I>initialize</I>.
	 */	
	static QAction* getCancelZoomAction ( )
	{ return _cancelZoomAction; }

	/**
	 * @return		L'action utilisée pour ajuster le zoom à ce qui est visible.
	 * @warning		Doit être préalablement initialisée par <I>initialize</I>.
	 */	
	static QAction* getAdjustScalesAction ( )
	{ return _adjustScalesAction; }

	/**
	 * @return		L'action utilisée pour changer de mode de sélection
	 *				(rectangle élastique / point).
	 * @warning		Doit être préalablement initialisée par <I>initialize</I>.
	 */	
	static QAction* getSelectionModeAction ( )
	{ return _selectionModeAction; }

	/**
	 * @return		L'action utilisée pour l'édition des graphiques.
	 * @warning		Doit être préalablement initialisée par <I>initialize</I>.
	 */
	static QAction* getEditChartAction ( )	
	{ return _editChartAction; }

	/**
	 * @return		Une référence sur l'unique instance de la classe.
	 * @warning		Lève une exception si l'API n'a pas été initialisée.
	 * @see			initialize
	 */
	static QwtChartsManager& getManager ( );

	/**
	 * Fonction a invoquer lorsque l'instance de la classe QwtChartPanel qui a
	 * le focus change.
	 * Met à jour les actions.
	 * @param		Eventuel panneau actif.
	 */
	void focusChanged (const QwtChartPanel* panel);

	/**
	 * @param		Enregistre le panneau graphique transmis en argument
	 *				dans la liste des panneaux à gérer.
	 * @warning		Une exception est levée si l'API n'est pas initialisée.
	 * @warning		Action effectuée automatiquement par le constructeur de la
	 *				classe QwtChartPanel.
	 * @see			initialize
	 */
	static void registerChartPanel (QwtChartPanel& panel);

	/**
	 * @param		Déréférence le panneau graphique transmis en argument
	 *				de la liste des panneaux à gérer.
	 * @warning		Une exception est levée si l'API n'est pas initialisée.
	 * @warning		Action effectuée automatiquement par le destructeur de la
	 *				classe QwtChartPanel.
	 * @see			initialize
	 */
	static void unregisterChartPanel (QwtChartPanel& panel);

	/**
	 * @return		Une référence sur l'unique barre d'outils (de l'API) gérant
	 *				les graphiques.
	 * @warning		Lève une exception si l'API n'a pas été initialisée.
	 * @see			initialize
	 */
	static QToolBar& getToolBar ( );

	/**
	 * @param		Menu à utiliser pour y insérer les items. Peut être nul.
	 * @param		Intitulé du menu pour le cas où il est créé.
	 * @return		Un menu permettant de gérer les graphiques. Retourne
	 *				<I>menu</I> si non nul, ou un menu créé dans le cas
	 *				contraire.
	 * @warning		Lève une exception si l'API n'a pas été initialisée.
	 * @see			initialize
	 */
	static QMenu* createMenu (QMenu* menu, const QString& title);

	/**
	 * @return		<I>true</I> si on est en mode "zoom interactif",
	 *				<I>false</I> dans le cas contraire.
	 */
	static bool zoomModeEnabled ( )
	{ return _zoomMode; }

	static QwtPlotCoordinatesPicker::SELECTION_MODE getSelectionMode ( )
	{ return _selectionMode; }
	static void setSelectionMode(QwtPlotCoordinatesPicker::SELECTION_MODE mode);

	/**
	 * L'URL contenant l'aide en ligne de la boite de dialogue d'édition des
	 * graphiques. Vide par défaut, information destinée à être lue en
	 * configuration.
	 */
	static IN_STD string			editionDialogHelpURL;

	/**
	 * La balise de l'aide en ligne de la boite de dialogue d'édition des
	 * graphiques. Vide par défaut, information destinée à être lue en
	 * configuration.
	 */
	static IN_STD string			editionDialogHelpTag;

	/**
	 * Affiche le message d'information tranmis en argument.
	 * @param		Fenêtre parente de la boite de dialogue.
	 * @param		Message d'information.
	 */
	static void displayInformationMessage (
						QWidget* parent, const IN_UTIL UTF8String& message);

	/**
	 * Affiche le message d'avertissement tranmis en argument.
	 * @param		Fenêtre parente de la boite de dialogue.
	 * @param		Message d'avertissement.
	 */
	static void displayWarningMessage (
							QWidget* parent, const IN_UTIL UTF8String& message);

	/**
	 * Affiche le message d'erreur tranmis en argument.
	 * @param		Fenêtre parente de la boite de dialogue.
	 * @param		Message d'erreur.
	 */
	static void displayErrorMessage (
							QWidget* parent, const IN_UTIL UTF8String& message);


	protected slots :

	/**
	 * Appelé lorsq'un objet Qt est détruit. Met à jour les variables de classe.
	 */
	void objectDestroyed (QObject* object);

	/**
	 * Imprime la fenêtre active si c'est une instance de la classe
	 * QwtChartPanel.
	 */
	void printCallback ( );

	/**
	 * Passe du mode zoom interactif au mode normal et réciproquement.	
	 */
	void zoomModeCallback  (bool);

	/**
	 * Annule tout zoom de la vue active.
	 */
	void cancelZoomCallback  ( );

	/**
	 * Ajuste les échelles du graphique à ce qui y est visible.
	 */
	void adjustScalesCallback  ( );

	/**
	 * Annule la dernière opération de zoom de la vue active.
	 */
	void undoZoomCallback  ( );

	/**
	 * Refait la dernière opération de zoom de la vue active.
	 */
	void redoZoomCallback  ( );

	/**
	 * Annulation de la sélection courante. */
	void unselectCallback ( );

	/**
	 * Changement du mode de sélection (point, courbe, rectangle élastique,
	 * ...).
	 */
	void selectionModeCallback (int);

	/**
	 * Affiche une boîte de dialogue d'édition des propriétés du panneau ayant
	 * le focus.
	 */
	void editChartCallback ( );


	/**
	 * En mode de sélection/modification par interpolation linéaire, annule la
	 * saisi du dernier point.
	 */
	void undoLastModifiedPointCallback ( );

	/**
	 * Affiche une boîte de dialogue de saisie de la couleur du fond et suit
	 * les instructions de l'utilisateur du panneau ayant le focus.
	 */
	void backgroundCallback ( );

	/**
	 * Affiche une boîte de dialogue de saisie des marges et suit
	 * les instructions de l'utilisateur du panneau ayant le focus.
	 */
	void marginCallback ( );

	/**
	 * Affiche/masque la légende selon la valeur tranmise en argument du panneau
	 * ayant le focus.
	 */
	void showLegendCallback (bool show);

	/**
	 * Positionne la légende dans la partie haute du panneau ayant le focus.
	 */
	void topLegendCallback (bool checked);

	/**
	 * Positionne la légende dans la partie basse du panneau ayant le focus.
	 */
	void bottomLegendCallback (bool checked);

	/**
	 * Positionne la légende dans la partie droite du panneau ayant le focus.
	 */
	void rightLegendCallback (bool checked);

	/**
	 * Positionne la légende dans la partie gauche du panneau ayant le focus.
	 */
	void leftLegendCallback (bool checked);

	/**
	 * Affiche une boite de dialogue de modification des marges
	 * de la légende du panneau ayant le focus.
	 */
	void legendMarginCallback ( );

	/**
	 * Affiche une boite de dialogue de modification de l'épaisseur du cadre
	 * de la légende du panneau ayant le focus.
	 */
	void legendFrameWidthCallback ( );

	/**
	 * Affiche ou masque les lignes verticales principales du panneau ayant le
	 * focus.
	 */
	void displayVerMajorLinesCallback (bool);

	/**
	 * Affiche ou masque les lignes verticales secondaires du panneau ayant le
	 * focus.
	 */
	void displayVerMinorLinesCallback (bool);

	/**
	 * Affiche ou masque les lignes horizontales principales du panneau ayant le
	 * focus.
	 */
	void displayHorMajorLinesCallback (bool);

	/**
	 * Affiche ou masque les lignes horizontales secondaires du panneau ayant le
	 * focus.
	 */
	void displayHorMinorLinesCallback (bool);

	/**
	 * @return		La première instance de <I>QwtChartPanel</I> retrouvée dans
	 *				la descendance de la fenêtre de l'application ayant le focus
	 */
	static QwtChartPanel* getActiveChartPanel ( );


	private :

	/** Constructeurs et destructeur interdits. */
	QwtChartsManager ( );
	QwtChartsManager (const QwtChartsManager&);
	QwtChartsManager& operator = (const QwtChartsManager&);
	~QwtChartsManager ( );

	/** Créé la barre d'outils. */
	void createToolBar ( );

	/** L'éventuelle instance unique de cette classe. */
	static QwtChartsManager*							_manager;

	/** La barre d'outils gérant les graphiques. */
	static QToolBar*									_toolBar;

	/** La liste des panneaux graphiques gérés. */
	static IN_STD vector<QwtChartPanel*>				_panels;

	/** Est-on en mode zoom ? */
	static bool											_zoomMode;

	/** Est-on en mode de sélection "rectangle élastique" ? */
	static QwtPlotCoordinatesPicker::SELECTION_MODE		_selectionMode;

	/** L'action à utiliser pour le mode zoom interactif. */
	static QAction*										_zoomAction;

	/** Les actions à utiliser pour gérer le zoom. */
	static QAction										*_cancelZoomAction,
														*_undoZoomAction,
														*_redoZoomAction,
														*_adjustScalesAction;

	/** L'action à utiliser pour changer de mode de sélection (rectangle
	 * élastique / point). */
	static QtExclusiveActionGroup*						_selectionModeAction;

	/** L'action "Annuler la sélection". */
	static QAction*										_unselectAction;

	/** L'action "Annuler le dernier point saisi". */
	static QAction*								_undoLastModifiedPointAction;

	/** L'action à utiliser pour les impressions. */
	static QAction*										_printAction;

	/** L'action à utiliser pour les éditer le panneau courrant. */
	static QAction*										_editChartAction;

	/** L'action modifier la couleur de fond. */
	static QAction*										_backgroundAction;

	/** L'action à utiliser pour modifier les marges. */
	static QAction*										_marginAction;

	/** L'action à utiliser pour afficher/masquer la légende. */
	static QAction*										_showLegendAction;

	/** L'action à utiliser pour positionner la légende en haut. */
	static QAction*										_topLegendAction;

	/** L'action à utiliser pour positionner la légende en bas. */
	static QAction*										_bottomLegendAction;

	/** L'action à utiliser pour positionner la légende à droite. */
	static QAction*										_rightLegendAction;

	/** L'action à utiliser pour positionner la légende à gauche. */
	static QAction*										_leftLegendAction;

	/** L'action à utiliser pour modifier les marges de la légende. */
	static QAction*										_legendMarginAction;

	/** L'action à utiliser pour modifier l'épaisseur du cadre de la légende. */
	static QAction*										_legendFrameWidthAction;

	/** L'action à utiliser pour afficher/masquer les lignes verticales
	 * majeures. */
	static QAction*										_verMajorLinesAction;

	/** L'action à utiliser pour afficher/masquer les lignes verticales
	 * mineures. */
	static QAction*										_verMinorLinesAction;

	/** L'action à utiliser pour afficher/masquer les lignes horizontales
	 * majeures. */
	static QAction*										_horMajorLinesAction;

	/** L'action à utiliser pour afficher/masquer les lignes horizontales
	 * mineures. */
	static QAction*										_horMinorLinesAction;

	/** La version de cette bibliothèque. */
	static const IN_UTIL Version						_version;
};	// class QwtChartsManager

#endif	// QWT_CHARTS_MANAGER_H
