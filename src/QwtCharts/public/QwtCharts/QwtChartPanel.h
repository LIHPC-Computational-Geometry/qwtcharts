#ifndef QWT_CHART_PANEL_H
#define QWT_CHART_PANEL_H

#include <qwt_global.h>	// => QWT_VERSION

#include <TkUtil/util_config.h>
#include <TkUtil/LogOutputStream.h>
#include <TkUtil/PaintAttributes.h>
#include <QWidget>
#include <QMenu>
#include <QPrinter>

#include <qwt_legend.h>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_rescaler.h>
#include "QwtCharts/QwtExtendedPlotZoomer.h"
#include "QwtCharts/QwtPlotCoordinatesPicker.h"
#include <qwt_plot_zoomer.h>

#include <string>


class QwtChartOptionsDialog;
class QwtExtendedPlotCurve;


/**
 * <P>Classe "panneau graphique" de base reposant sur Qwt.</P>
 * <P>Les interactions avec cette classe et ses classes dérivées peuvent être
 * gérées automatiquement par la classe QwtChartsManager. Cela permet
 * notamment d'éditer les propriétés du graphique via la boite de dialogue
 * <I>QwtChartOptionsDialog</I>. Les onglets de cette boite de dialogue
 * peuvent être enrichis par de nouveaux onglets fournis par des classes
 * dérivées de celle-ci en surchargeant la méthode <I>createEditionPanels</I>.
 * </P>
 * <P>Le fichier d'entête de la classe QwtCurveChartPanel contient un exemple
 * d'utilisation de cette API. </P>
 * @warning		Les méthodes de cette classe sont susceptibles de lever de
 *				exception de type Exception.
 * @see			QwtChartsManager
 * @see			QwtCurveChartPanel
 */
class QwtChartPanel : public QWidget
{
	Q_OBJECT


	public :

	/**
	 * Constructeur par défaut : graphique vide, blanc, avec légende à droite.
	 * S'enregistre auprès de l'API QwtChartsManager si celle-ci est
	 * initialisée.
	 * @param		Widget parent.
	 */
	QwtChartPanel (QWidget* parent);

	/**
	 * Destructeur.
	 * Se déréférence de l'API QwtChartsManager si celle-ci est
	 * initialisée.
	 */
	virtual ~QwtChartPanel ( );

	/**
	 * @param		flux de logs à utiliser. Peut être nul.
	 * @see			log
	 */
	virtual void setLogOutputStream (IN_UTIL LogOutputStream* stream);

	/**
	 * @return		Le nombre de séries.
	 */
	virtual size_t getItemCount ( ) const;

	/**
	 * @return		La liste des séries du graphique.
	 */
	virtual IN_STD vector<QwtPlotItem*> getPlotItems ( ) const
	{ return _plotItems; }

	/**
	 * Le mode de représentation : cartésienne ou polaire.
	 */
	enum DISPLAY_MODE {CARTESIAN = 1, POLAR = 2};

	/**
	 * @return		Le mode de représentation courant.
	 * @see			setDisplayMode
	 */
	virtual DISPLAY_MODE getDisplayMode ( ) const
	{ return _displayMode; }

	/**
	 * @param		Le nouveau mode de représentation.
	 * @warning		<B>ATTENTION</B> : les coordonnées des points ne sont pas
	 *				modifier, c'est à l'application de le faire et de provoquer
	 *				un réaffichage des courbes.
	 * @see			getDisplayMode
	 */
	virtual void setDisplayMode (DISPLAY_MODE mode);

	/**
	 * @return		<I>true</I> si le mode de représentation transmis en
	 *				argument est autorisé, <I>false</I> dans le cas contraire.
	 * @see			setDisplayMode
	 * @see			enableDisplayModes
	 */
	virtual bool isDisplayModeAllowed (DISPLAY_MODE mode) const;

	/**
	 * @param		Modes de représentation autorisés obtenus par combinaison
	 *				de type <B>|</B> de valeurs de l'énuméré DISPLAY_MODE
	 *				(ex : <I>CARTESIAN | POLAR</I>).
	 * @see			isDisplayModeAllowed
	 */
	virtual void enableDisplayModes (size_t mask);

	/**
	 * @return		La couleur de fond du panneau.
	 * @see			setBackgroundColor
	 */
	virtual const QColor& getBackgroundColor ( ) const;

	/**
	 * @param		Couleur de fond à appliquer au panneau.
	 * @see			getBackgroundColor
	 */
	virtual void setBackgroundColor (const QColor& bg);

	/**
	 * @return		Les marges du graphique.
	 */
	virtual int getMargin ( ) const;

	/**
	 * @param		Nouvelles marges du graphique.
	 */
	virtual void setMargin (int margin);

	/**
	 * @return		Le titre du graphique.
	 */
	virtual QwtText getTitle ( ) const;

	/**
	 * @param		Le titre du graphique.
	 */
	virtual void setTitle (const QwtText& title);

	/**
	 * @param		Titre de l'axe des abscisses.
	 */	
	virtual void setAbscissaTitle (const QwtText& title);

	/**
	 * @param		Titre de l'axe des ordonnées.
	 */	
	virtual void setOrdinateTitle (const QwtText& title);

	/**
	 * @param		En retour, valeur minimale en abscisses.
	 * @param		En retour, valeur maximale en abscisses.
	 * @param		En retour, <I>true</I> si l'échelle est automatiquement
	 *				calculée, <I>false</I> dans le cas contraire.
	 * @return		<I>true</I> si min et max sont déterminés, false dans le cas
	 *				contraire.
	 * @see			setAbscissaScale
	 * @see			setVisibleAbscissaScale
	 */
	virtual bool getAbscissaScale (double& min, double& max, bool& autoScale);

	/**
	 * @param		En retour, valeur minimale en abscisses de ce qui est
	 * 				affiché.
	 * @param		En retour, valeur maximale en abscisses de ce qui est
	 * 				affiché.
	 * @return		<I>true</I> si min et max sont déterminés, false dans le cas
	 *				contraire.
	 * @see			getVisibleOrdinateScale
	 */
	virtual bool getVisibleAbscissaScale (double& min, double& max);

	/**
	 * @return		La distance entre 2 lignes majeures verticales (et
	 *				concernant donc l'axe des abscisses). Si nulle cette
	 *				distance est automatiquement calculée par Qwt.
	 */
	virtual double getAbscissaStep ( ) const
	{ return _abscissaScale._tickStep; }

	/**
	 * @param		Nouvelle valeur minimale en abscisses.
	 * @param		Nouvelle valeur maximale en abscisses.
	 * @param		<I>true</I> si l'échelle doit être automatiquement
	 *				calculée, <I>false</I> dans le cas contraire.
	 * @param		La distance entre 2 lignes majeures verticales (et
	 *				concernant donc l'axe des abscisses). Si nulle cette
	 *				distance est automatiquement calculée par Qwt.
	 * @see			getAbscissaScale
	 * @see			getAbscissaStep
	 */
	virtual void setAbscissaScale (
				double min, double max, bool autoScale, double tickStep = 0.);

	/**
	 * @param		En retour, valeur minimale en ordonnées.
	 * @param		En retour, valeur maximale en ordonnées.
	 * @param		En retour, <I>true</I> si l'échelle est automatiquement
	 *				calculée, <I>false</I> dans le cas contraire.
	 * @return		<I>true</I> si min et max sont déterminés, false dans le cas
	 *				contraire.
	 * @see			setOrdinateScale
	 */
	virtual bool getOrdinateScale (double& min, double& max, bool& autoScale);

	/**
	 * @param		En retour, valeur minimale en ordonnées de ce qui est
	 * 				affiché.
	 * @param		En retour, valeur maximale en ordonnées de ce qui est
	 * 				affiché.
	 * @return		<I>true</I> si min et max sont déterminés, false dans le cas
	 *				contraire.
	 * @see			getVisibleAbscissaScale
	 */
	virtual bool getVisibleOrdinateScale (double& min, double& max);

	/**
	 * @return		La distance entre 2 lignes majeures horizontales (et
	 *				concernant donc l'axe des ordonnées). Si nulle cette
	 *				distance est automatiquement calculée par Qwt.
	 */
	virtual double getOrdinateStep ( ) const
	{ return _ordinateScale._tickStep; }

	/**
	 * @param		Nouvelle valeur minimale en ordonnées.
	 * @param		Nouvelle valeur maximale en ordonnées.
	 * @param		<I>true</I> si l'échelle doit être automatiquement
	 *				calculée, <I>false</I> dans le cas contraire.
	 * @param		La distance entre 2 lignes majeures horizontales (et
	 *				concernant donc l'axe des ordonnées). Si nulle cette
	 *				distance est automatiquement calculée par Qwt.
	 * @see			getOrdinateScale
	 * @see			getOrdinateStep
	 */
	virtual void setOrdinateScale (
				double min, double max, bool autoScale, double tickStep = 0.);

	/**
	 * @return		<I>true</I>) si les lignes verticales majeures sont
	 *				affichées, sinon <I>false</I>.
	 * @see			displayHorMajorLines
	 */
	virtual bool areHorMajorLinesDisplayed ( ) const;

	/**
	 * @param		Affiche (<I>true</I>) ou masque (<I>false</I>) les lignes
	 *				verticales majeures.
	 * @see			areHorMajorLinesDisplayed
	 */
	virtual void displayHorMajorLines (bool display);

	/**
	 * @return		<I>true</I>) si les lignes verticales mineures sont
	 *				affichées, sinon <I>false</I>.
	 * @see			displayHorMinorLines
	 */
	virtual bool areHorMinorLinesDisplayed ( ) const;

	/**
	 * @param		Affiche (<I>true</I>) ou masque (<I>false</I>) les lignes
	 *				verticales mineures.
	 * @see			areHorMinorLinesDisplayed
	 */
	virtual void displayHorMinorLines (bool display);

	/**
	 * @return		<I>true</I>) si les lignes horizontales majeures sont
	 *				affichées, sinon <I>false</I>.
	 * @see			displayVerMajorLines
	 */
	virtual bool areVerMajorLinesDisplayed ( ) const;

	/**
	 * @param		Affiche (<I>true</I>) ou masque (<I>false</I>) les lignes
	 *				horizontales majeures.
	 * @see			areVerMajorLinesDisplayed
	 */
	virtual void displayVerMajorLines (bool enable);

	/**
	 * @return		<I>true</I>) si les lignes horizontales mineures sont
	 *				affichées, sinon <I>false</I>.
	 * @see			displayVerMinorLines
	 */
	virtual bool areVerMinorLinesDisplayed ( ) const;

	/**
	 * @param		Affiche (<I>true</I>) ou masque (<I>false</I>) les lignes
	 *				horizontales mineures.
	 * @see			areVerMinorLinesDisplayed
	 */
	virtual void displayVerMinorLines (bool enable);

	/**
	 * @param		Affiche (true) ou masque (false) la légende.
	 * @param		Emplacement de la légende en cas d'affichage.
	 */
	virtual void showLegend (
			bool show, QwtPlot::LegendPosition position = QwtPlot::RightLegend);

	/**
	 * @return		true si la légende est affichée, false dans le cas
	 *				contraire.
	 * @see			getLegend
	 */
	virtual bool hasLegend ( ) const;

	/**
	 * @return		La légende. Là où l'abstraction atteint ses limites ...
	 */
	virtual const QwtLegend* getLegend ( ) const;
	virtual QwtLegend* getLegend ( );
	
	/**
	 * @return		L'épaisseur du cadre de la légende.
	 * @see			setLegendFrameWidth
	 */
	virtual int getLegendFrameWidth ( ) const;

	/**
	 * @param		La nouvelle épaisseur du cadre de la légende.
	 * @exception	Une exception est levée si l'épaisseur est négative.
	 * @see			getLegendFrameWidth
	 */
	virtual void setLegendFrameWidth (int width);

	/**
	 * @return		Les marges du cadre de la légende.
	 * @see			setLegendMargin
	 */
	virtual int getLegendMargin ( ) const;

	/**
	 * @param		Les nouvelles marges de la légende.
	 * @exception	Une exception est levée si la marge est négative.
	 * @see			getLegendMargin
	 */
	virtual void setLegendMargin (int margin);

	/**
	 * @return		la position de la légende.
	 * @see			showLegend
	 */
	virtual QwtPlot::LegendPosition getLegendPosition ( ) const
	{ return _legendPosition; }

	/**
	 * @return		true si les éléments de la légende sont cochables, false
	 *				dans le cas contraire.
	 * @see			setLegendCheckable
	 */
	virtual bool isLegendCheckable ( ) const;

	/**
	 * @param		rend les éléments de légende cochables (<I>true</I>) ou
	 *				non (<I>false</I>). Si oui, les courbes sont affichées
	 *				lorsque les éléments sont cochés et masquées dans le cas
	 *				contraire.
	 * @see			isLegendCheckable
	 */
	virtual void setLegendCheckable (bool checkable);

	/**
	 * @return		<I>true</I> si la légende de la sélection est affichage,
	 * 				<?Ifalse</I> dans le cas contraire.
	 */
	virtual bool isSelectionLegendShown ( ) const;

	/**
	 * @param		Affiche (true) ou masque (false) la légende de la sélection.
	 */
	virtual void showSelectionLegend (bool show);

	/**
	 * @return		La largeur du stylo dans le tracé des courbes dans la 
	 *				légende.
	 * @see			setLegendCurveWidth
	 */
	virtual int getLegendCurveWidth ( ) const;

	/**
	 * @param		La nouvelle largeur du stylo dans le tracé des courbes dans
	 *				la légende.
	 * @see			getLegendCurveWidth
	 */
	virtual void setLegendCurveWidth (int width);

	/**
	 * @return		La précision utilisée lors des conversions valeur -> texte
	 *				des échelles.
	 * @exception	Une exception est levée si le gestionnaire de tracé des
	 *				exes n'est pas de type <I>QwtExtendedScaleDraw</I>.
	 * @see			setScaleLabelPrecision
	 */
	virtual unsigned char getScaleLabelPrecision ( ) const;

	/**
	 * @param		La précision utilisée lors des conversions valeur -> texte
	 *				des échelles.
	 * @exception	Une exception est levée si le gestionnaire de tracé des
	 *				exes n'est pas de type <I>QwtExtendedScaleDraw</I>.
	 * @see			getScaleLabelPrecision
	 * @warning		Affecte l'ensemble des axes pour la version courrante
	 */
	virtual void setScaleLabelPrecision (unsigned char precision);

	/**
	 * @return		Le format utilisé lors des conversions valeur -> texte des
	 *				échelles ('f', 'F', 'e', 'E', 'g' ou 'G'). 
	 * @exception	Une exception est levée si le gestionnaire de tracé des
	 *				exes n'est pas de type <I>QwtExtendedScaleDraw</I>.
	 * @see			setScaleLabelFormat
	 * @see			QString::number
	 */
	virtual char getScaleLabelFormat ( ) const;

	/**
	 * @return		Le format utilisé lors des conversions valeur -> texte des
	 *				échelles ('f', 'F', 'e', 'E', 'g' ou 'G'). 
	 * @exception	Une exception est levée si le gestionnaire de tracé des
	 *				exes n'est pas de type <I>QwtExtendedScaleDraw</I>.
	 * @see			setScaleLabelFormat
	 * @see			QString::number
	 * @warning		Affecte l'ensemble des axes pour la version courrante
	 */
	virtual void setScaleLabelFormat (char format);

	/**
	 * Imprime le document dans l'imprimante transmise en argument.
	 * Dans le cas d'une impression dans un fichier, cette fonction veille à ce
	 * l'image fasse au moins imageSizeMin * imageSizeMin pixels (par défaut,
	 * 1 million de pixels).
	 * @return		<I>true</I> si l'impression a été effectuée, <I>false</I>
	 *				dans le cas contraire.
	 * @warning		<B>Le rapport d'aspect hauteur/largeur n'est pas garanti
	 *				dans le cas d'impressions type postscript (imprimante,
	 *				formats PS et PDF) qui directement traités par Qt et ne
	 *				passent pas par l'API QImage</B>. Un contournement aurait pû
	 *				être de générer une image postscript puis de l'imprimer,
	 *				mais Qt (4.3.2) ne semble pas fournir le service nécessaire.
	 */
	virtual bool print (QPrinter& printer);

	/**
	 * @param		Active (true) ou inactive (false) la possibilité de
	 *				zoomer à la souris.
	 */
	virtual void enableZoom (bool enable);

	/**
	 * @return		true s'il est possible de zoomer à la souris, false dans
	 *				le cas contraire.
	 */
	virtual bool zoomEnabled ( ) const;

	/**
	 * Effectue un zoom centré du graphique selon les informations transmises
	 * en argument. Appelle à cet effet <I>zoom (double)</I>. Accepte
	 * l'évènement transmis en argument.
	 * @param		Evènement à l'origine de l'action. L'algorithme de zoom
	 *				repose sur la valeur de <I>event.delta ( )</I>.
	 */
	virtual void zoom (QWheelEvent& event);

	/**
	 * Effectue un zoom centré du graphique du facteur tranmis en argument.
	 * @param		Facteur de zoom (1 : identité, < 1 : augmentation, > 1 :
	 *				diminution).
	 */
	virtual void zoom (double factor);

	/**
	 * @param		Active (true) ou inactive (false) la possibilité de
	 *				déplacer à la souris la zone graphique dans la fenêtre.
	 */
	virtual void enablePanning (bool enable);

	/**
	 * @return		true s'il est possible de déplacer à la souris la zone
	 *				graphique dans la fenêtre, false dans le cas contraire.
	 */
	virtual bool panningEnabled ( ) const;

	/**
	 * @return		true si  opérations de zoom sont annulables, false dans le
	 *				cas contraire (retour à la situation initiale).
	 * @see			cancelZoom
	 */
	virtual bool zoomCancelable ( ) const;

	/**
	 * Annule tout zoom effectué.
	 * Emet le signal <I>zoomModified</I>.
	 * @see			redoZoom
	 * @see			undoZoom
	 * @see			zoomCancelable
	 */
	virtual void cancelZoom ( );

	/**
	 * Ajuste les échelles du graphique à ce qui y est visible.
	 */
	virtual void adjustScales ( );

	/**
	 * @return		true si une opération d'annulation de zoom est possible,
	 *				false dans le cas contraire.
	 * @see			undoZoom
	 */
	virtual bool zoomUndoable ( ) const;

	/**
	 * Annule, s'il y en a une, la précedante opération de zoom.
	 * Emet le signal <I>zoomModified</I>.
	 * @see			redoZoom
	 * @see			cancelZoom
	 * @see			zoomUndoable
	 */
	virtual void undoZoom ( );

	/**
	 * @return		true si une opération d'annulation d'annulation de zoom est
	 *				possible, false dans le cas contraire.
	 * @see			redoZoom
	 */
	virtual bool zoomRedoable ( ) const;

	/**
	 * Refait, s'il y en a une, la précedante opération de zoom ayant été
	 * annulée.
	 * Emet le signal <I>zoomModified</I>.
	 * @see			undoZoom
	 * @see			cancelZoom
	 * @see			zoomRedoable
	 */
	virtual void redoZoom ( );

	/**
	 * @return		true si au moins une série est modifiée (interactivement
	 *				par l'utilisateur), false dans le cas contraire.
	 * @see			setModified
	 */
	virtual bool isModified ( ) const;

	/**
	 * @return		true si la série d'indice <I>item</I> est modifiée
	 *				(interactivement par l'utilisateur), false dans le cas
	 *				contraire.
	 * @see			setModified
	 */
	virtual bool isModified (size_t item) const;

	/**
	 * Marque la série d'indice <I>item</I> comme modifiée si modified vaut
	 * <I>true</I>, et comme non modifiée dans le cas contraire.
	 */
	virtual void setModified (size_t item, bool modified);

	/**
	 * A invoquer lorsque les données ont été modifiées par un autre biais
	 * que par cette bibliothèque.
	 * Met à jour la gestion du zoom et l'éventuelle sélection.
	 */
	virtual void dataModified ( );

	/**
	 * Ajuste le graphique à ses courbes. Cette opération est automatiquement
	 * faite par Qwt par défaut, mais pas toujours comme attendu.
	 */
	virtual void adjustChart ( );

	/**
	 * @return		Une référence sur le panneau graphique Qwt.
	 */
	QwtPlot& getPlot ( );
	const QwtPlot& getPlot ( ) const;

	/**
	 * @return		L'éventuelle grille du graphique.
	 */
	QwtPlotGrid* getGrid ( )
	{ return _grid; }
	const QwtPlotGrid* getGrid ( ) const
	{ return _grid; }

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
	 * @return	La courbe sélectionnée, ou 0 s'il n'y en a pas.
	 */
	virtual QwtPlotCurve* getSelectedCurve ( );
	virtual const QwtPlotCurve* getSelectedCurve ( ) const;

	/**
	 * Annule l'éventuelle sélection actuelle.
	 */
	virtual void clearSelection ( );

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
	 * @return		<I>true</I> si le rapport d'aspect doit est forcé,
	 *				<I>false</I> dans le cas contraire.
	 * @warning		Versions de Qwt supérieures ou égales à 5.2.0 uniquement.
	 * @see			getAspectRatio
	 */
	virtual bool useAspectRatio ( ) const;

	/**
	 * <P>Force l'utilisation d'un rapport d'aspect (vertical/horizontal).</P>
	 * @param		<I>true</I> si le rapport d'aspect doit être forcé,
	 *				<I>false</I> dans le cas contraire.
	 * @warning		Versions de Qwt supérieures ou égales à 5.2.0 uniquement.
	 * @see			setAspectRatio
	 */
	virtual void useAspectRatio (bool use);

	/**
	 * @return		Le rapport d'aspect hauteur/largeur appliqué.
	 * @warning		Versions de Qwt supérieures ou égales à 5.2.0 uniquement.
	 * @see			useAspectRatio
	 * @see			setAspectRatio
	 */
	virtual double getAspectRatio ( ) const;

	/**
	 * @param		Le rapport d'aspect hauteur/largeur à appliquer.
	 * @warning		Versions de Qwt supérieures ou égales à 5.2.0 uniquement.
	 * @see			useAspectRatio
	 * @see			getAspectRatio
	 */
	virtual void setAspectRatio (double ar);

	/**
	 * Classe abstraite de panneau permettant l'édition d'une instance
	 * spécialisée de la classe QwtChartPanel.
	 */
	class QwtChartEditionPanel : public QWidget
	{
		public :

		/**
		 * Constructeur. RAS.
		 * @param		parent
		 * @param		graphique représenté. Ne doit pas être nul.
		 */
		QwtChartEditionPanel (QWidget* parent, QwtChartPanel* chartPanel);

		/**
		 * Destructeur. RAS.
		 */
		virtual ~QwtChartEditionPanel ( )
		{ }

		/**
		 * Appelé lorsqu'une courbe est ajoutée au graphique.
		 * Ne fait rien par défaut.
		 * @param		Courbe ajoutée.
		 */
		virtual void curveAdded (QwtExtendedPlotCurve* curve)
		{ }

		/**
		 * Appelé lorsqu'une courbe est enlevée du graphique.
		 * Ne fait rien par défaut.
		 * @param		Courbe enlevée.
		 */
		virtual void curveRemoved (QwtExtendedPlotCurve* curve)
		{ }

		/**
		 * Met à jour le graphique représenté en fonction du paramétrage de
		 * l'instance. Ne fait rien par défaut, méthode à surcharger.
		 */
		virtual void apply ( );


		protected :

		/**
		 * @return		Le graphique représenté.
		 */
		const QwtChartPanel& getChartPanel ( ) const;
		QwtChartPanel& getChartPanel ( );


		private :

		/** Constructeur de copie et opérateur = : interdits. */
		QwtChartEditionPanel (const QwtChartEditionPanel&);
		QwtChartEditionPanel& operator = (const QwtChartEditionPanel&);

		/** Le graphique représenté. */
		QwtChartPanel*			_chartPanel;
	};	// class QwtChartEditionPanel


	/**
	 * Cette méthode permet de créer des panneaux spécifiques d'édition du
	 * graphique. Cette méthode est notamment invoquée par le constructeur de
	 * la classe QwtChartOptionsDialog. Par défaut cette méthode utilise
	 * l'instance de la classe <I>QwtChartSpecificPanelCreator</I>
	 * transmise en argument à  la méthode <I>setSpecificsPanelsCreator</I>.
	 * @param		Parent des panneaux créés
	 * @return		liste des panneaux créés
	 * @see			setSpecificsPanelsCreator
	 * @see			QwtChartOptionsDialog
	 * @see			QwtChartEditionPanel
	 */
	virtual IN_STD vector<QwtChartEditionPanel*> createEditionPanels (
															QWidget* parent);

	/**
	 * Classe de création de panneaux d'édition spécifique à un graphique.
	 */
	class QwtChartSpecificPanelCreator
	{
		public :

		/**
		 * Constructeur.
		 * @param		Graphique représenté. Ne doit pas être nul.
		 */
		QwtChartSpecificPanelCreator (QwtChartPanel* chartPanel);

		/**
		 * Créé et retourne les panneaux d'édition spécifiques à un graphique.
		 * Méthode à surcharger, ne fait rien par défaut.
		 */
		virtual IN_STD vector<QwtChartEditionPanel*> createEditionPanels (
															QWidget* parent);


		protected :

		/**
		 * @return		Len graphique représenté.
		 */
		virtual const QwtChartPanel& getChartPanel ( ) const;
		virtual QwtChartPanel& getChartPanel ( );


		private :

		/** Constructeur de copie et opérateur = : interdits. */
		QwtChartSpecificPanelCreator (const QwtChartSpecificPanelCreator&);
		QwtChartSpecificPanelCreator& operator = (
										const QwtChartSpecificPanelCreator&);

		/** Le graphique à représenter. */
		QwtChartPanel*			_chartPanel;
	};	// class QwtChartSpecificPanelCreator


	/**
	 * Structure de donnée contenant les attributs de base d'affichage de
	 * données (courbes, séries d'histogrammes, ...).
	 */
	struct DataPaintAttributes
	{
		DataPaintAttributes (
				QwtPlotItem* plotItem, const IN_UTIL PaintAttributes& pen);
		DataPaintAttributes (
				const IN_STD string& name, const IN_UTIL PaintAttributes& pen);
		DataPaintAttributes (const DataPaintAttributes&);
		DataPaintAttributes& operator = (const DataPaintAttributes&);

		/** L'élément représenté. */
		QwtPlotItem*				_plotItem;

		/** Le nom de l'élément. */
		IN_STD string				_name;

		/** Le stylo. */
		IN_UTIL PaintAttributes		_pen;

		/** Les couleurs sont elles valides ? */
		bool						_fgValid, _bgValid;
	};	// struct DataPaintAttributes

	/**
	 * @return		La liste des propriétés d'affichage des courbes.
	 * @see			setDataPaintAttributes
	 */
	virtual IN_STD vector<DataPaintAttributes> getDataPaintAttributes ( ) const;

	/**
	 * @param		Indice de la courbe dont on modifie les propriétés
	 * 				d'affichage.
	 * @param		Propriétés d'affichage à appliquer.
	 * @see			getDataPaintAttributes
	 */
	virtual void setDataPaintAttributes (
					size_t index, const DataPaintAttributes& paintAttributes);

	/**
	 * Affecte l'instance de création des panneaux d'édition spécifiques
	 * du graphique. Cette instance est détruite par cette classe.
	 */
	virtual void setSpecificsPanelsCreator (
									QwtChartSpecificPanelCreator* creator);

	/**
	 * @return		L'éventuelle instance de boite de dialogue utilisée pour
	 *				éditer le graphique.
	 */
	virtual QwtChartOptionsDialog* getEditionDialog ( )
	{ return _editionDialog; }

	/**
	 * Affecte à l'instance la boite de dialogue utilisée pour éditer le
	 * graphique.
	 */
	virtual void setEditionDialog (QwtChartOptionsDialog*);

	/** Les lignes verticales majeures sont elles affichées par défaut ?
	 * Vaut <I>true</I> par défaut. */
	static bool							drawHorMajorLines;

	/** Les lignes verticales mineures sont elles affichées par défaut ?
	 * Vaut <I>false</I> par défaut. */
	static bool							drawHorMinorLines;

	/** Les lignes horizontales majeures sont elles affichées par défaut ?
	 * Vaut <I>true</I> par défaut. */
	static bool							drawVerMajorLines;

	/** Les lignes horizontales mineures sont elles affichées par défaut ?
	 * Vaut <I>false</I> par défaut. */
	static bool							drawVerMinorLines;

	/** La position de la légende par défaut. Vaut <I>QwtPlot::RightLegend</I>
	 * par défaut. */
	static QwtPlot::LegendPosition		legendPosition;

	/** La légende est-elle, par défaut, cochable ? Vaut <I>false</I> par
	 * défaut. */
	static QwtLegendData::Mode			legendCheckable;

	/** L'épaisseur des courbes dans la légende. Vaut 3 par défaut. */
	static int							legendCurveWidth;

	/** La précision par défaut lors des conversions valeur -> texte des
	 * échelles. Vaut 10 par défaut. */
	static unsigned char				scaleLabelPrecision;

	/** Le format par défaut lors des conversions valeur -> texte des
	 * échelles ('f', 'F', 'e', 'E', 'g' ou 'G'). Vaut 'g' par défaut.
	 */
	static char							scaleLabelFormat;

	/** La taille du plus petit côté d'une image lors d'une impression dans un
	 * fichier. Vaut 1000 pixels par défaut.
	 */
	static int							imageSizeMin;

	/** Le sens d'utilisation de la molette de la souris pour zoomer en avant
	 * (grossissement). Vaut <I>true</I> par défaut (c.a.d. que si on pousse
	 * en éloignement la molette, on zoom en avant).
	 */
	static bool							mouseZoomUp;

	/** Faut-il afficher la légende de la sélection ? <I>False</I> par défaut.*/
	static bool							displaySelectionLegend;


	signals :

	/**
	 * Signal envoyé lors d'une opération quelconque de zoom.
	 * @param		Instance dont le zoom a été modifié
	 */	
	void zoomModified (QwtChartPanel*);

	/**
	 * Signal envoyé lorsque le type de représentation (cartésienne/polaire)
	 * du graphique change. L'application doit adapter les données représentées
	 * en fonction du nouveau type de représentation.
	 * @param		Instance dont le type de représentation vient de changer.
	 */
	void displayModeModified (QwtChartPanel*);


	protected slots :

	/**
	 * Appelé lorsqu'un item de la légende est (dé)coché. Affiche ou masque la
	 * courbe associée.
	 * Signature avec QwtPlotItem pour les versions inférieurs à 6.x de Qwt,
	 * avec QVariant à partir de la version 6.0.
	 */
	virtual void legendCheckedCallback (QwtPlotItem*, bool);
	virtual void legendCheckedCallback (
							const QVariant& itemInfo, bool on, int index);

	/**
	 * Appelé lorsqu'une opération de zoom vient d'être effectuée.
	 * Emet le signal <I>zoomModified</I>.
	 */
	// Rem CP : le compilo moc ne tient pas compte des -D...
	// => on a ici 2 fonctions callbacks analogues de même signature car
	// QwtDoubleRect est un typedef sur QRectF ... Mais ça ne marche pas au
	// niveau de connect qui repose sur des strings.
	virtual void zoomCallback_Qwt5 (const QwtDoubleRect&);
	virtual void zoomCallback_Qwt6 (const QRectF&);


	protected :

	/**
	 * @param		Paramètres d'impression.
	 * @return		<I>true</I> s'il faut utiliser l'API <I>QImage</I> pour
	 *				réaliser l'impression dont les paramètres sont transmis
	 *				en argument, <I>false</I> dans le cas contraire.
	 *				L'API <I>QImage</I> est utilisée si l'impression doit être
	 *				faite dans un fichier qui n'est pas aux formats suivants :
	 *				<I>ps</I>, <I>pdf</I>.
	 */
	static bool useQImage (const QPrinter& printer);

	/**
	 * @param		Paramètres d'impression.
	 * @param		En entrée, taille du graphique à l'écran. En retour, largeur
	 *				et hauteur de l'image à utiliser pour avoir une image dont
	 *				la dimension la plus petite fait au moins
	 *				<I>imageSizeMin</I> pixels.
	 * @see			useQImage
	 * @see			imageSizeMin
	 */
	static void getImageSize (const QPrinter& printer, int& width, int& height);

	/**
	 * Informe la classe QwtChartsManager que le panneau a reçu le focus pour
	 * mise à jour éventuelle des actions.
	 */
	virtual void focusInEvent (QFocusEvent*) ;

	/**
	 * Structure décrivant les échelles des axes.
	 */
	struct AxisScale
	{
		/**
		 * Constructeurs. RAS.
		 */
		AxisScale (double min = 0., double max = 0., bool automatic = true,
		           double tickStep = 0., bool updated = false);
		AxisScale (const AxisScale&);
		AxisScale& operator = (const AxisScale& axis);

		/** Valeurs min et max sur l'axe. */
		double			_min, _max;

		/** L'échelle est elle automatiquement calculée ? */
		bool			_auto;

		/** L'échelle est-elle à jour ? */
		bool			_updated;

		/** Les distances entre 2 lignes majeures. */
		double			_tickStep;
	};	// struct AxisScale


	/**
	 * Envoie le message transmis en argument dans l'éventuel flux de logs
	 * associé.
	 * @see			setLogOutputStream
	 */	
	virtual void log (const IN_UTIL Log& log);

	/**
	 * Ajoute la série transmise en argument au graphique.
	 * Si la série dérive de la classe <I>QwtExtendedPlotCurve</I>, informe
	 * l'éventuelle boite de dialogue associée de cet ajout.
	 * @see		setEditionDialog
	 */
	virtual void addPlotItem (QwtPlotItem&);

	/**
	 * Enlève la série transmise en argument du graphique.
	 * Si la série dérive de la classe <I>QwtExtendedPlotCurve</I>, informe
	 * l'éventuelle boite de dialogue associée de cette suppression.
	 * @see		setEditionDialog
	 */
	virtual void removePlotItem (QwtPlotItem&);


	protected :


	/**
	 * @return		une référence sur l'instance chargée d'afficher les
	 *				coordonnées des points survolés et de gérer la sélection
	 *				d'entités du graphique.
	 */
	QwtPlotCoordinatesPicker& getCoordinatesPicker ( );
	const QwtPlotCoordinatesPicker& getCoordinatesPicker ( ) const;

	/**
	 * @param		Référence sur l'instance chargée d'afficher les
	 * 				coordonnées des points survolés et de gérer la sélection
	 * 				d'entités du graphique.
	 * @warning		S'enregistre auprès de QwtChartsManager si ce n'est déjà
	 *				fait.
	 * @warning		<B>Cette instance est adoptée, c.a.d. qu'elle sera détruite
	 * 				par ce panneau.</B>
	 */
	virtual void setCoordinatesPicker (QwtPlotCoordinatesPicker& picker);


	private :

	/** Constructeurs de copie interdits. */
	QwtChartPanel (const QwtChartPanel&);
	QwtChartPanel& operator = (const QwtChartPanel&);

	/** Le mode de représentation. */
	DISPLAY_MODE					_displayMode;

	/** Les modes de représentation autorisés. */
	size_t							_allowedDisplayModes;

	/** Les échelles des axes. */
	AxisScale						_abscissaScale, _ordinateScale;

	/** Le panneau graphique Qwt. */
	QwtPlot*						_plotWidget;

	/** La liste des séries du graphique. */
	IN_STD vector<QwtPlotItem*>		_plotItems;

	/** Les séries sont elles modifiées ou non. */
	IN_STD vector<bool>				_modificationList;

	/** La grille d'affichage des lignes horizontales et verticales. */
	QwtPlotGrid*					_grid;

	/** Les éléments de la légende sont ils cochables ou non ? */
	QwtLegendData::Mode				_legendCheckable;

	/** La position de la légende. */
	QwtPlot::LegendPosition			_legendPosition;

	/** Faut-il afficher la légende de la sélection ? */
	bool							_showSelectionLegend;

	/** L'épaisseur des courbes dans la légende. */
	int								_legendCurveWidth;

	/** L'affichage des coordonnées pointées par la souris. */
	QwtPlotCoordinatesPicker*		_coordsPicker;

	/** La gestion du zoom. */
	QwtExtendedPlotZoomer*			_zoomer;

	/** La gestion du rapport d'aspect. */
	QwtPlotRescaler*				_rescaler;

	/** La gestion du déplacement du contenu du graphe. */
	QwtPlotPanner*					_panner;

	/** L'éventuel gestionnaire de logs associés. */
	IN_UTIL LogOutputStream*		_logStream;

	/** L'éventuelle instance chargée de créer des panneaux d'édition
	 * spécialisés du graphique. */
	QwtChartSpecificPanelCreator*	_specificPanelCreator;

	/** L'éventuelle boite de dialogue utilisée pour éditer le graphique. */
	QwtChartOptionsDialog*			_editionDialog;
};	// class QwtChartPanel

#endif	// QWT_CHART_PANEL_H
