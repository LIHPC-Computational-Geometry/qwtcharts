#ifndef QWT_LEGEND_OPTIONS_PANEL_H
#define QWT_LEGEND_OPTIONS_PANEL_H

#include <QwtCharts/QwtChartPanel.h>
#include <QtUtil/QtIntTextField.h>

#include <QCheckBox>
#include <QRadioButton>
#include <qwt_plot.h>



/**
 * <P>Classe de panneau permettant de paramétrer la légende d'un graphique de
 * type QwtChartPanel.
 * </P>
 * @warning		Les méthodes de cette classe sont susceptibles de lever de
 *				exception de type Exception.
 * @see			QwtChartPanel
 */
class QwtLegendOptionsPanel : public QwtChartPanel::QwtChartEditionPanel
{
	public :

	/**
	 * Constructeur par défaut : ajuste ses propriétés à celles de l'éventuel
	 * panneau transmis en argument.
	 * @param		Widget parent.
	 * @param		Eventuel panneau graphique à modifier via ce panneau.
	 */
	QwtLegendOptionsPanel (QWidget* parent, QwtChartPanel* chartPanel);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QwtLegendOptionsPanel ( );

	/**
	 * @return		<I>true</I> si la légende est affichée, <I>false</I> dans le
	 *				cas contraire.
	 */
	virtual bool isLegendShown ( ) const;

	/**
	 * Affiche ou masque la légende.
	 * @param		<I>true</I> si la légende doit être affichée, <I>false</I>
	 *				dans le cas contraire.
	 * @param		La position de la légende si elle doit être affichée.
	 * @see			isLegendShown
	 */
	virtual void showLegend (bool show, QwtPlot::LegendPosition position);

	/**
	 * @return		La position de la légende.
	 * @see			setLegendPosition
	 */
	virtual QwtPlot::LegendPosition getLegendPosition ( ) const;

	/**
	 * @param		La nouvelle position de la légende.
	 */
	virtual void setLegendPosition (QwtPlot::LegendPosition position);


	/**
	 * @return		<I>true</I>) si les items sont cochables, <I>false</I> dans
	 *				le cas contraire.
	 * @see			setItemsCheckable ( )
	 */
	virtual bool areItemsCheckable ( ) const;

	/**
	 * @param		<I>true</I>) si les items doivent être cochables, 
	 *				<I>false</I> dans le cas contraire.
	 * @see			areItemsCheckable ( )
	 */
	virtual void setItemsCheckable (bool checkable);

	/**
	 * @return		<I>true</I> si la légende de la sélection est affichée,
	 *              <I>false</I> dans le cas contraire.
	 */
	virtual bool isSelectionLegendShown ( ) const;

	/**
	 * Affiche ou masque la légende de la sélection.
	 * @param		<I>true</I> si la légende de la sélection doit être
	 * 				affichée, <I>false</I> dans le cas contraire.
	 * @see			isSelectionLegendShown
	 */
	virtual void showSelectionLegend (bool show);

	/**
	 * @return		L'épaisseur de la bordure.
	 * @see			setFrameWidth
	 */
	virtual int getFrameWidth ( ) const;

	/**
	 * @param		La nouvelle épaisseur de la bordure.
	 * @see			getFrameWidth
	 */
	virtual void setFrameWidth (int width);

	/**
	 * @return		La marge de la légende.
	 * @see			setMargin
	 */
	virtual int getMargin ( ) const;

	/**
	 * @param		La nouvelle marge de la légende.
	 * @see			getMargin
	 */
	virtual void setMargin (int margin);

	/**
	 * @return		L'épaisseur des courbes dans la légende.
	 * @see			setCurveWidth
	 */
	virtual int getCurveWidth ( ) const;

	/**
	 * @param		L'épaisseur des courbes dans la légende.
	 * @see			getCurveWidth
	 */
	virtual void setCurveWidth (int width);

	/**
	 * Met à jour l'éventuel panneau graphique associé.
	 * @exception	Méthode susceptible de lever une exception.
	 */
	virtual void apply ( );

	/**
	 * @return		<I>true</I> si l'instance a un panneau graphique associé,
	 *				sinon <I>false</I>.
	 * @see			getChartPanel
	 */
	virtual bool hasChartPanel ( ) const
	{ return 0 == _chartPanel ? false : true; }

	/*
	 * @return		Une référence sur le panneau graphique associé.
	 * @exception	Lève une exception si aucun panneau graphique n'est associé.
	 * @see			hasChartPanel
	 */
	QwtChartPanel& getChartPanel ( );
	const QwtChartPanel& getChartPanel ( ) const;


	private :

	/** Constructeurs de copie interdits. */
	QwtLegendOptionsPanel (const QwtLegendOptionsPanel&);
	QwtLegendOptionsPanel& operator = (const QwtLegendOptionsPanel&);

	/** Le panneau graphique associé. */
	QwtChartPanel*					_chartPanel;

	/** La case à cocher "Afficher". */
	QCheckBox*						_showLegendCheckbox;

	/** La case à cocher "Afficher la légende de la sélection". */
	QCheckBox*						_showSelectionLegendCheckbox;

	/** La case à cocher "items cochables". */
	QCheckBox*						_itemsCheckableCheckbox;

	/** L'épaisseur de la bordure. */
	QtIntTextField*					_frameWidthTextField;

	/** La marge de la légende. */
	QtIntTextField*					_marginTextField;

	/** L'épaisseur des courbes dans la légende. */
	QtIntTextField*					_curveWidthTextField;

	/** La position de la légende. */
	QRadioButton					*_leftButton, *_rightButton, *_topButton,
									*_bottomButton;
};	// class QwtLegendOptionsPanel

#endif	// QWT_LEGEND_OPTIONS_PANEL_H
