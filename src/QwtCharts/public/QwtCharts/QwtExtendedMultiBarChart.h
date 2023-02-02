#ifndef QWT_EXTENDED_MULTI_BAR_CHART_H
#define QWT_EXTENDED_MULTI_BAR_CHART_H

#include <qwt_global.h>     // => QWT_VERSION


#include <TkUtil/util_config.h>

#include <qwt_plot_multi_barchart.h>

#include <vector>


/**
 * <P>Classe d'histogramme Qwt offrant des services permettant des actions
 * interactives type sélection de classes.
 * </P>
 */
class QwtExtendedMultiBarChart : public QwtPlotMultiBarChart
{
	public :

	/**
	 * Surcharge de l'énuméré QwtPlotMultiBarChart::ChartStyle dans l'attente
	 * que OVERLAPPED y figure.
	 * STYLE_GROUPED et STYLE_STACKED sont les reprises de cet énuméré,
	 * STYLE_OVERLAPPED est un nouveau style où les barres des différentes
	 * séries se chevauchent.
	 */
	enum HISTOGRAM_STYLE {STYLE_GROUPED, STYLE_STACKED, STYLE_OVERLAPPED};

	/**
	 * Constructeur.
	 * @param		Nom de l'histogramme.
	 */
	QwtExtendedMultiBarChart (const QString& title);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QwtExtendedMultiBarChart ( );

	/**
	 * @return		Le nombre de séries.
	 * @see			getClassesNum
	 */
	virtual size_t getSeriesNum ( ) const;

	/**
	 * @return		Le nombre de classes.
	 * @see			getSeriesNum
	 */
	virtual size_t getClassesNum ( ) const;


	/**
	 * @return		Le style de l'histogramme.
	 */
	virtual HISTOGRAM_STYLE getHistogramStyle ( ) const
	{ return _style; }

	/**
	 * @param		Le nouveau style de l'histogramme.
	 */
	virtual void setHistogramStyle (HISTOGRAM_STYLE style);

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

	enum POSITION { TOP, CENTER, BOTTOM };

	/**
	 * @return	La position d'affichage du nombre d'éléments par classe.
	 * @see		setElementNumPos
	 * @see		getElementNumPos
	 */
	virtual POSITION getElementNumPos ( ) const;

	/**
	 * @return	La position d'affichage du nombre d'éléments par classe.
	 * @see		setElementNumPos
	 * @see		getElementNumPos
	 */
	virtual void setElementNumPos (POSITION pos);

	/**
	 * @return	<I>true</I> si la série dont l'index est transmis argument est
	 *			affichée, <I>false</I> dans le cas contraire.
	 * @see		setVisible
	 */
	virtual bool isVisible (size_t index) const;

	/**
	 * Affiche ou masque la série dont l'index est transmis en argument.
	 * @see		isVisible
	 */
	virtual void setVisible (size_t index, bool visible);

	/**
	 * @return	<I>true</I> si la classe <I>c</I> de la série <I>s</I> est
	 * sélectionnée, <I>false</I> dans le cas contraire.
	 * @see		clearSelection
	 * @see		addToSelection
	 * @see		removeFromSelection
	 * @see		getSelection
	 */
	virtual bool isSelected (size_t s, size_t c) const;

	/**
	 * Annule toutes les sélections en cours.
	 */
	virtual void clearSelection ( );

	/**
	 * Ajoute les classes transmises en argument
	 * (numéro de série, numéro de classe) à la liste des classes sélectionnées.
	 * @see		isSelected
	 * @see		clearSelection
	 * @see		removeFromSelection
	 * @see		getSelection
	 */
	virtual void addToSelection (
					const IN_STD vector<IN_STD pair<size_t, size_t> >& classes);

	/**
	 * Enlève les classes transmises en argument
	 * (numéro de série, numéro de classe) de la liste des classes
	 * sélectionnées.
	 * @see		isSelected
	 * @see		addToSelection
	 * @see		clearSelection
	 * @see		getSelection
	 */
	virtual void removeFromSelection (
					const IN_STD vector<IN_STD pair<size_t, size_t> >& classes);

	/**
	 * @return	La liste des classes (numéro de série, numéro de classe)
	 *			sélectionnées.
	 * @see		clearSelection
	 * @see		addToSelection
	 */
	virtual IN_STD vector<IN_STD pair<size_t, size_t> > getSelection ( ) const;

	/**
	 * @return	La liste des classes (numéro de série, numéro de classe)
	 *			pointées par le point dont les coordonnées (en pixels) sont
	 *			transmises en arguments.
	 */
	virtual IN_STD vector<IN_STD pair<size_t, size_t> > intersectedClasses (
															int x, int y) const;

	/**
	 * @param	Coordonnées du point dont on cherche à savoir s'il appartient
	 * 			à au moins une classe.
	 * @param	La valeur médiane correspondant au point transmis en argument
	 * 			en cas de classe pointée.
	 * @param	Nombre d'éléments de l'éventuelle classe pointée.
	 * @return	<I>true</I> si une classe est pointée, sinon <I>false</I>.
	 */
	virtual bool intersects (int x, int y, double& abs, size_t& ord) const;


	protected :

	/**
	 * Redimensionne les informations en adéquation avec le nouveau jeu de
	 * données.
	 */
	virtual void dataChanged ( );

	/**
	 * Dessine la série d'index transmis en argument conformément aux arguments
	 * reçus et au style de l'histogramme.
	 * Invoque drawSample de la classe parente si le style n'est pas
	 * <I>STYLE_OVERLAPPED<I>, sinon invoque <I>drawOverlappedBars</I>.
	 */
	virtual void drawSample (
			QPainter* painter, const QwtScaleMap& xMap, const QwtScaleMap& yMap,
			const QRectF& canvasRect, const QwtInterval& boundingInterval,
			int index, const QwtSetSample& sample) const;

	/**
	 * Dessine la série d'index transmis en argument conformément aux arguments
	 * et pour un histogramme de style <I>chevauchant</I>.
	 */
	virtual void drawOverlappedBars (
			QPainter* painter, const QwtScaleMap& xMap, const QwtScaleMap& yMap,
			const QRectF& canvasRect, int index, double sampleWidth,
			const QwtSetSample &sample) const;

	/**
	 * Représente sur le graphique la <I>barIndex-ème</I> classe de la 
	 * <I>sampleIndex-ème</I> série. Mémorise les coordonnées du rectangle
	 * à des fins d'interrogations pour des opérations interactives.
	 * @see			intersectedClasses
 	 */
	virtual void  drawBar (QPainter*, int sampleIndex, int barIndex,
	                       const QwtColumnRect &) const;


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QwtExtendedMultiBarChart (const QwtExtendedMultiBarChart&);
	QwtExtendedMultiBarChart& operator = (const QwtExtendedMultiBarChart&);

	/** Les rectangles contenant les barres des histogrammes. */
	mutable IN_STD vector < IN_STD vector <QRectF> >		_rectangles;

	/** Les classes sont elles sélectionnées. */
	IN_STD vector < IN_STD vector <bool> >					_selectedClasses;

	HISTOGRAM_STYLE											_style;

	/** Faut-il afficher le nombre d'éléments par classe ? */
	bool													_displayElementsNum;

	 /** La position d'affichage du nombre d'éléments par classe. */
	POSITION												_elementsNumPos;

	/** Les séries représentées. */
	IN_STD vector <bool>									_visibleSeries;
};	// class QwtExtendedMultiBarChart


#endif	// QWT_EXTENDED_MULTI_BAR_CHART_H
