#ifndef QWT_AXIS_OPTIONS_PANEL_H
#define QWT_AXIS_OPTIONS_PANEL_H

#include <QwtCharts/QwtChartPanel.h>
#include <QtUtil/QtDoubleTextField.h>
#include <QtUtil/QtFontPanel.h>
#include <QtUtil/QtIntTextField.h>
#include "QwtCharts/QtExtStringNumberFormatComboBox.h"

#include <QCheckBox>
#include <qwt_plot.h>



/**
 * <P>Classe de panneau permettant de paramétrer les axes d'un graphique de
 * type QwtChartPanel.
 * </P>
 * @warning		Les méthodes de cette classe sont susceptibles de lever de
 *				exception de type Exception.
 * @see			QwtChartPanel
 */
class QwtAxisOptionsPanel : public QwtChartPanel::QwtChartEditionPanel
{
	Q_OBJECT


	public :

	/**
	 * Constructeur par défaut : ajuste ses propriétés à celles de l'éventuel
	 * panneau transmis en argument.
	 * @param		Widget parent.
	 * @param		Eventuel panneau graphique à modifier via ce panneau.
	 */
	QwtAxisOptionsPanel (QWidget* parent, QwtChartPanel* chartPanel);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QwtAxisOptionsPanel ( );

	/**
	 * @return		Le titre des abscisses.
	 * @see			setHorTitle
	 * @see			getVerTitle
	 */
	virtual QwtText getHorTitle ( ) const;

	/**
	 * @param		Le nouveau titre des abscisses.
	 * @see			getHorTitle
	 * @see			setVerTitle
	 */
	virtual void setHorTitle (const QwtText& title);

	/**
	 * @return		Le titre des ordonnées.
	 * @see			setVerTitle
	 * @see			getHorTitle
	 */
	virtual QwtText getVerTitle ( ) const;

	/**
	 * @param		Le nouveau titre des ordonnées.
	 * @see			getVerTitle
	 * @see			setHorTitle
	 */
	virtual void setVerTitle (const QwtText& title);

	/**
	 * @return		<I>true</I>) si les extrema en abscisses sont calculés
	 *				automatiquement, <I>false</I> dans le cas contraire.
	 * @see			enableHorAutoScale ( )
	 */
	virtual bool isHorAutoScale ( ) const;

	/**
	 * @param		<I>true</I>) si les extrema en abscisses sont calculés
	 *				automatiquement, <I>false</I> dans le cas contraire.
	 * @see			isHorAutoScale ( )
	 */
	virtual void enableHorAutoScale (bool enable);


	/**
	 * @return		<I>true</I>) si les extrema en ordonnées sont calculés
	 *				automatiquement, <I>false</I> dans le cas contraire.
	 * @see			enableVerAutoScale ( )
	 */
	virtual bool isVerAutoScale ( ) const;

	/**
	 * @param		<I>true</I>) si les extrema en ordonnées sont calculés
	 *				automatiquement, <I>false</I> dans le cas contraire.
	 * @see			isVerAutoScale ( )
	 */
	virtual void enableVerAutoScale (bool enable);

	/**
	 * @return		La valeur minimale en abscisses.
	 * @see			setHorMin
	 */
	virtual double getHorMin ( ) const;

	/**
	 * @param		La valeur minimale en abscisses.
	 * @see			getHorMin
	 */
	virtual void setHorMin (double min);

	/**
	 * @return		La valeur maximale en abscisses.
	 * @see			setHorMax
	 */
	virtual double getHorMax ( ) const;

	/**
	 * @param		La valeur maximale en abscisses.
	 * @see			getHorMax
	 */
	virtual void setHorMax (double max);

	/**
	 * @return		Le format d'affichage des labels en abscisses.
	 * @see			QString::arg
	 * @see			QString::number
	 */
	virtual char getHorFormat ( ) const;

	/**
	 * @param		Le format d'affichage des labels en abscisses.
	 * @see			QString::arg
	 * @see			QString::number
	 */
	virtual void setHorFormat (char format);

	/**
	 * @return		<I>true</I> si le nombre de décimales en abscisses est
	 *				imposé, </I>false</I> dans le cas contraire.
	 * @see			enableHorNumberOfDecimals
	 */
	virtual bool isHorNumberOfDecimalsEnabled ( ) const;

	/**
	 * @param		<I>true</I> si le nombre de décimales en abscisses est
	 *				imposé, </I>false</I> dans le cas contraire.
	 * @see			isHorNumberOfDecimalsEnabled
	 */
	virtual void enableHorNumberOfDecimals (bool enabled);

	/**
	 * @return		Le nombre de décimales en abscisses.
	 * @see			isHorNumberOfDecimalsEnabled
	 */
	virtual unsigned char getHorNumberOfDecimals ( ) const;

	/**
	 * @param		Le nombre de décimales en abscisses.
	 * @see			enableHorNumberOfDecimals
	 */
	virtual void setHorNumberOfDecimals (unsigned char num);

	/**
	 * @return		La distance entre 2 lignes majeures verticales (et
	 *				concernant donc l'axe des abscisses). Si nulle cette
	 *				distance est automatiquement calculée par Qwt.
	 * @see			setHorMajorTickStep
	 */
	virtual double getHorMajorTickStep ( ) const;

	/**
	 * @param		La distance entre 2 lignes majeures verticales (et
	 *				concernant donc l'axe des abscisses). Si nulle cette
	 *				distance est automatiquement calculée par Qwt.
	 * @exception	Une exception est levée si step est négatif.
	 * @see			getHorMajorTickStep
	 */
	virtual void setHorMajorTickStep (double step);

	/**
	 * @return		La valeur minimale en ordonnées.
	 * @see			setVerMin
	 */
	virtual double getVerMin ( ) const;

	/**
	 * @param		La valeur minimale en ordonnées.
	 * @see			getVerMin
	 */
	virtual void setVerMin (double min);

	/**
	 * @return		La valeur maximale en ordonnées.
	 * @see			setVerMax
	 */
	virtual double getVerMax ( ) const;

	/**
	 * @param		La valeur maximale en ordonnées.
	 * @see			getVerMax
	 */
	virtual void setVerMax (double max);

	/**
	 * @return		Le format d'affichage des labels en ordonnées.
	 * @see			QString::arg
	 * @see			QString::number
	 */
	virtual char getVerFormat ( ) const;

	/**
	 * @param		Le format d'affichage des labels en ordonnées.
	 * @see			QString::arg
	 * @see			QString::number
	 */
	virtual void setVerFormat (char format);

	/**
	 * @return		<I>true</I> si le nombre de décimales en ordonnées est
	 *				imposé, </I>false</I> dans le cas contraire.
	 * @see			enableVerNumberOfDecimals
	 */
	virtual bool isVerNumberOfDecimalsEnabled ( ) const;

	/**
	 * @param		<I>true</I> si le nombre de décimales en ordonnées est
	 *				imposé, </I>false</I> dans le cas contraire.
	 * @see			isVerNumberOfDecimalsEnabled
	 */
	virtual void enableVerNumberOfDecimals (bool enabled);

	/**
	 * @return		Le nombre de décimales en ordonnées.
	 * @see			isHorNumberOfDecimalsEnabled
	 */
	virtual unsigned char getVerNumberOfDecimals ( ) const;

	/**
	 * @param		Le nombre de décimales en ordonnées.
	 * @see			enableHorNumberOfDecimals
	 */
	void setVerNumberOfDecimals (unsigned char num);

	/**
	 * @return		La distance entre 2 lignes majeures horizontales (et
	 *				concernant donc l'axe des ordonnées). Si nulle cette
	 *				distance est automatiquement calculée par Qwt.
	 * @see			setVerMajorTickStep
	 */
	virtual double getVerMajorTickStep ( ) const;

	/**
	 * @param		La distance entre 2 lignes majeures horizontales (et
	 *				concernant donc l'axe des ordonnées). Si nulle cette
	 *				distance est automatiquement calculée par Qwt.
	 * @exception	Une exception est levée si step est négatif.
	 * @see			getVerMajorTickStep
	 */
	virtual void setVerMajorTickStep (double step);


	/**
	 * @return		La fonte appliquée pour les graduations des axes.
	 * @see			setAxisFont
	 */	
	virtual QFont getAxisFont ( ) const;

	/**
	 * @param		La nouvelle fonte à appliquer pour les graduations des axes.
	 * @see			getAxisFont
	 */
	virtual void setAxisFont (const QFont& font);

	/**
	 * @return		La couleur utilisée pour les graduations des axes.
	 * @see			setAxisColor
	 */
	virtual QColor getAxisColor ( ) const;

	/**
	 * @param		La nouvelle couleur utilisée pour les graduations des axes.
	 * @see			getAxisColor
	 */
	virtual void setAxisColor (const QColor& color);

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


	protected slots :


	/**
	 * Callback appellé lorsque l'utilisateur clique sur le bouton "Editer le
	 * titre". Affiche une boite de dialogue d'édition du titre et suit les
	 * instructions de l'utilisateur.
	 */
	virtual void editHorTitleCallback ( );
	virtual void editVerTitleCallback ( );

	/** Appelé lorsqu'une case à cocher "Echelle automatique" change d'état.
	 * Met à jour le caractère fonctionnel des champs de saisie des min/max
	 * des axes.
	 * @param		Non utilisé
	 */
	virtual void autoScaleCallback (int unused);

	/** Appelé lorsqu'une case à cocher "Fixer le nombre de décimales" change
	 * d'état. Met à jour les champs de saisie des nombres de décimales.
	 */
	virtual void enableNumberOfDecimalsCallback (int unused);
	


	private :

	/** Constructeurs de copie interdits. */
	QwtAxisOptionsPanel (const QwtAxisOptionsPanel&);
	QwtAxisOptionsPanel& operator = (const QwtAxisOptionsPanel&);

	/** Le panneau graphique associé. */
	QwtChartPanel*					_chartPanel;

	/** Les titres des axes. */	
	QtTextField						*_horTitleTextField, *_verTitleTextField;

	/** Les polices de caractères des titres. */
	QtFontPanel						*_horTitleFontPanel, *_verTitleFontPanel;

	/** Les cases à cocher "calcul des extrema automatique". */
	QCheckBox						*_horAutoScaleCheckbox,
									*_verAutoScaleCheckbox;

	/** Les champs de texte extrema. */
	QtDoubleTextField				*_horMinTextField, *_horMaxTextField,
									*_verMinTextField, *_verMaxTextField;

	/** Le format d'affichage des labels. */
	QtExtStringNumberFormatComboBox	*_horLabelFormatComboBox,
									*_verLabelFormatComboBox;

	/** Les cases à cocher "nombre de décimales imposées". */
	QCheckBox						*_horNumberOfDecimalsCheckBox,
									*_verNumberOfDecimalsCheckBox;

	/** Les champs de texte "nombre de décimales imposées". */
	QtIntTextField					*_horNumberOfDecimalsTextField,
									*_verNumberOfDecimalsTextField;

	/** Les champs "distance entre 2 lignes majeures de la grille". */
	QtDoubleTextField				*_verMajorTickStepTextField,
									*_horMajorTickStepTextField;

	/** Le panneau de sélection de la police de caractères utilisée pour les
	 * graduations des axes. */
	QtFontPanel*					_axisFontPanel;
};	// class QwtAxisOptionsPanel

#endif	// QWT_AXIS_OPTIONS_PANEL_H
