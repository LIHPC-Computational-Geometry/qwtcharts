#ifndef QWT_UTILITIES_H
#define QWT_UTILITIES_H

#include <qwt_global.h>		// => QWT_VERSION
#include <qwt_plot_curve.h>
#include <qwt_symbol.h>

#include <TkUtil/util_config.h>


#include <qwt_column_symbol.h>


/**
 * <P>Classe offrant des services <I>Qwt</I> complémentaires.
 * </P>
 */
class QwtUtilities
{
	public :

	/**
	 * Copie le premier symbol reçu en argument dans le second.
	 */
	static void copySymbol (const QwtSymbol& copied, QwtSymbol& copy);

	/**
	 * Affecte au symbole transmis en premier argument les propriétés
	 * transmises en arguments suivants.
	 */
	static void setSymbolAttributes (
						QwtSymbol& symbol, QwtSymbol::Style style, QSize& size,
						QColor& penColor, QColor& brushColor);

	/**
	 * Effectue une copie de la courbe reçue en argument et la retourne,
	 */
	static QwtPlotCurve* cloneCurve (
						const QwtPlotCurve& curve, const IN_STD string& name);

	/**
	 * Copie la série de données transmise en premier argument dans celle
	 * transmise en second argument.
	 */
	static void copySeriesData (
            const QwtSeriesData<QPointF>& copied, QwtSeriesData<QPointF>& copy);

	/**
	 * Effectue une copie du symbol reçu en argument et la retourne.
	 */
	static QwtColumnSymbol* cloneSymbol (const QwtColumnSymbol& symbol);

	/**
	 * Couleurs utilisables pour marquer des sélections.
	 */
	static const QColor		lightOrange, darkOrange;


	private :

	/**
	 * Constructeur. Interdit.
	 */
	QwtUtilities ( );

	/**
	 * Constructeur de copie et opérateur =. RAS.
	 */
	QwtUtilities (const QwtUtilities&);
	QwtUtilities& operator = (const QwtUtilities&);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QwtUtilities ( );
};	// class QwtUtilities

#endif	// QWT_UTILITIES_H
