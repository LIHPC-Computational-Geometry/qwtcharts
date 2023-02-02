#ifndef QWT_CHART_GENERAL_OPTIONS_PANEL_H
#define QWT_CHART_GENERAL_OPTIONS_PANEL_H

#include <QwtCharts/QwtChartPanel.h>
#include <QtUtil/QtFontPanel.h>
#include <QtUtil/QtTextField.h>
#include <QtUtil/QtIntTextField.h>
#include <QtUtil/QtDoubleTextField.h>

#include <qwt_plot.h>
#include <QCheckBox>
#include <QRadioButton>



/**
 * <P>Classe de panneau permettant de paramétrer des options "générales" d'un
 * graphique de type QwtChartPanel.
 * </P>
 * @warning		Les méthodes de cette classe sont susceptibles de lever de
 *				exception de type Exception.
 * @see			QwtChartPanel
 */
class QwtChartGeneralOptionsPanel : public QwtChartPanel::QwtChartEditionPanel
{
	Q_OBJECT


	public :

	/**
	 * Constructeur par défaut : ajuste ses propriétés à celles de l'éventuel
	 * panneau transmis en argument.
	 * @param		Widget parent.
	 * @param		Eventuel panneau graphique à modifier via ce panneau.
	 */
	QwtChartGeneralOptionsPanel (QWidget* parent, QwtChartPanel* chartPanel);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QwtChartGeneralOptionsPanel ( );

	/**
	 * @return		le titre du graphique.
	 * @see			setTitle
	 */
	virtual QwtText getTitle ( ) const;

	/**
	 * @param		Nouveau titre du graphique.
	 * @see			getTitle
	 */
	virtual void setTitle (const QwtText& title);

	/**
	 * @return		Le mode de représentation du graphique.
	 * @see			setDisplayMode
	 */
	virtual QwtChartPanel::DISPLAY_MODE getDisplayMode ( ) const;

	/**
	 * @param		Le nouveau mode de représentation du graphique.
	 * @see			getDisplayMode
	 */
	virtual void setDisplayMode (QwtChartPanel::DISPLAY_MODE mode);

	/**
	 * @return		La couleur de fond du graphique.
	 * @see			setBackgroundColor
	 */
	virtual QColor getBackgroundColor ( ) const;

	/**
	 * @param		La nouvelle couleur du graphique.
	 * @see			getBackgroundColor
	 */
	virtual void setBackgroundColor (const QColor& color);

	/**
	 * @return		La marge appliquée autour du graphique.
	 * @see			setMargins
	 */
	virtual unsigned int getMargins ( ) const;

	/**
	 * @param		Nouvelle valeurs des marges.
	 * @see			getMargins
	 */
	virtual void setMargins (unsigned int margins);

	/**
	 * @return		<I>true</I> si le rapport d'aspect hauteur/largeur doit
	 *				être forcé, <I>false</I> dans le cas contraire.
	 * @see			getAspectRatio
	 */
	virtual bool useAspectRatio ( ) const;

	/**
	 * @param		<I>true</I> si le rapport d'aspect hauteur/largeur doit
	 *				être forcé, <I>false</I> dans le cas contraire.
	 * @see			setAspectRatio
	 */
	virtual void useAspectRatio (bool use);

	/**
	 * @return		Le rapport d'aspect hauteur/largeur.
	 * @see			setAspectRatio
	 * @see			useAspectRatio
	 */
	virtual double getAspectRatio ( ) const;

	/**
	 * @param		Le rapport d'aspect hauteur/largeur.
	 * @see			setAspectRatio
	 * @see			useAspectRatio
	 */
	virtual void setAspectRatio (double ar);

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
	virtual void editTitleCallback ( );

	/**
	 * Callback appellé lorsque l'utilisateur clique sur le bouton "Couleur de
	 * fond". Affiche une boite de dialogue d'édition des couleurs et suit les
	 * instructions de l'utilisateur.
	 */
	virtual void backgroundColorCallback ( );


	private :

	/** Constructeurs de copie interdits. */
	QwtChartGeneralOptionsPanel (const QwtChartGeneralOptionsPanel&);
	QwtChartGeneralOptionsPanel& operator = (const QwtChartGeneralOptionsPanel&);

	/** Le panneau graphique associé. */
	QwtChartPanel*					_chartPanel;

	/** La couleur de fond. */
	QColor							_backgroundColor;

	/** La zone de saisie du texte. */
	QtTextField*					_titleTextField;

	/** La police de caractères du titre. */
	QtFontPanel*					_titleFontPanel;

	/** Le mode de représentation du graphique. */
	QRadioButton					*_cartesianButton, *_polarButton;

	/** Les marges appliquées. */
	QtIntTextField*					_marginsTextField;

	/** Le rapport d'aspect hauteur/largeur doit il être forcé ? */
	QCheckBox*						_aspectRatioCheckbox;

	/** Le rapport d'aspect hauteur/largeur. */
	QtDoubleTextField*				_aspectRatioTextField;
};	// class QwtChartGeneralOptionsPanel

#endif	// QWT_CHART_GENERAL_OPTIONS_PANEL_H
