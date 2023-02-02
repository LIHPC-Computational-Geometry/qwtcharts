#ifndef QWT_EXTENDED_PLOT_CURVE_H
#define QWT_EXTENDED_PLOT_CURVE_H

#include <TkUtil/PaintAttributes.h>
#include <TkUtil/util_config.h>

#include <qwt_plot_curve.h>
#include <string>



/**
 * <P>Classe permettant d'assurer la jonction entre des courbes Qwt et des
 * objets représentables graphiquement d'une application quelconque.
 * </P>
 * <P>La création de cette classe a été rendue nécessaire afin de pouvoir
 * surcharger certains méthodes de la classe <I>QwtPlotCurve</I> telle que
 * <I>setPen</I> qui ne sont pas virtuelles.
 * </P>
 */
class QwtExtendedPlotCurve : public QwtPlotCurve
{
	public :

	/**
	 * Constructeur.
	 * @param		Nom de la courbe.
	 */
	QwtExtendedPlotCurve (const QString& name);

	/**
	 * Constructeur.
	 * @param		Nom de la courbe.
	 * @param		Paramètres de représentation de la courbe.
	 */
	QwtExtendedPlotCurve (
					const QString& name, const IN_UTIL PaintAttributes& pa);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QwtExtendedPlotCurve ( );

	/**
	 * @return		Les paramètres de représentation de la courbe.
	 * @see			setPaintAttributes
	 */
	virtual IN_UTIL PaintAttributes getPaintAttributes ( ) const;

	/**
	 * @param		Nouveaux attributs de représentation de la courbe.
	 * @see			getPaintAttributes
	 * @see			setPen
	 */
	virtual void setPaintAttributes (const IN_UTIL PaintAttributes& pa);

	/**
	 * Notifie les éventuels observateurs d'une éventuelle modification des
	 * paramètres de représentation.
	 * @param		Nouveau stylo pour les tracés.
	 */
	virtual void setPenAttributes (const QPen& pen);


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QwtExtendedPlotCurve (const QwtExtendedPlotCurve&);
	QwtExtendedPlotCurve& operator = (const QwtExtendedPlotCurve&);
};	// class QwtExtendedPlotCurve


#endif	// QWT_EXTENDED_PLOT_CURVE_H
