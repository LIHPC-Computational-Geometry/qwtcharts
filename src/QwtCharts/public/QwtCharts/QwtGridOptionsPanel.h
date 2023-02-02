#ifndef QWT_GRID_OPTIONS_PANEL_H
#define QWT_GRID_OPTIONS_PANEL_H

#include <TkUtil/util_config.h>
#include <QwtCharts/QwtChartPanel.h>
#include <QtUtil/QtPenPanel.h>

#include <QCheckBox>
#include <qwt_plot.h>



/**
 * <P>Classe de panneau permettant de paramétrer la grille d'un graphique de
 * type QwtChartPanel.
 * </P>
 * @warning		Les méthodes de cette classe sont susceptibles de lever de
 *				exception de type Exception.
 * @see			QwtChartPanel
 */
class QwtGridOptionsPanel : public QwtChartPanel::QwtChartEditionPanel
{
	public :

	/**
	 * Constructeur par défaut : ajuste ses propriétés à celles de l'éventuel
	 * panneau transmis en argument.
	 * @param		Widget parent.
	 * @param		Eventuel panneau graphique à modifier via ce panneau.
	 */
	QwtGridOptionsPanel (QWidget* parent, QwtChartPanel* chartPanel);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QwtGridOptionsPanel ( );

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
	 * @return		Le stylo pour le tracé des lignes majeures.
	 * @see			setMajorPen
	 */
	virtual QPen getMajorPen ( ) const;

	/**
	 * @param		Le nouveau stylo pour le tracé des lignes majeures.
	 * @see			setMajorPen
	 */
	virtual void setMajorPen (const QPen& pen);

	/**
	 * @return		Le stylo pour le tracé des lignes mineures.
	 * @see			setMinorPen
	 */
	virtual QPen getMinorPen ( ) const;

	/**
	 * @param		Le nouveau stylo pour le tracé des lignes mineures.
	 * @see			setMinorPen
	 */
	virtual void setMinorPen (const QPen& pen);

	/**
	 * Met à jour l'éventuel panneau graphique associé.
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
	QwtGridOptionsPanel (const QwtGridOptionsPanel&);
	QwtGridOptionsPanel& operator = (const QwtGridOptionsPanel&);

	/** Le panneau graphique associé. */
	QwtChartPanel*					_chartPanel;

	/** Les cases à cocher "afficher les lignes hor/ver maj/min". */
	QCheckBox						*_horMajCheckbox, *_horMinCheckbox,
									*_verMajCheckbox, *_verMinCheckbox;

	/** Les stylos utilisés pour les lignes mineures et majeures. */
	QtPenPanel						*_majorPenPanel, *_minorPenPanel;
};	// class QwtGridOptionsPanel

#endif	// QWT_GRID_OPTIONS_PANEL_H
