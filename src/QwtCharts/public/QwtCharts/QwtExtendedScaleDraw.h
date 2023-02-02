#ifndef QWT_EXTENDED_SCALE_DRAW_H
#define QWT_EXTENDED_SCALE_DRAW_H

#include <TkUtil/util_config.h>

#include <qwt_scale_draw.h>



/**
 * <P>Classe offrant des services complémentaires pour le tracé d'échelles
 * linéaires.
 * </P>
 */
class QwtExtendedScaleDraw : public QwtScaleDraw
{
	public :

	/**
	 * Constructeur par défaut. La précision par défaut de conversion double ->
	 * texte est égale à 6 et le format est 'g'.
	 */
	QwtExtendedScaleDraw (unsigned char precision = 6, char format = 'g');

	/**
	 * Constructeur de copie et opérateur =. RAS.
	 */
	QwtExtendedScaleDraw (const QwtExtendedScaleDraw&);
	QwtExtendedScaleDraw& operator = (const QwtExtendedScaleDraw&);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QwtExtendedScaleDraw ( );

	/**
	 * En mode automatique détermine le nombre de chiffres significatifs à
	 * afficher.
	 * Redessine l'échelle.
	 */
	virtual void draw (QPainter*, const QPalette &) const;

	/**
	 * @param		Réel à convertir en chaîne de caractères.
	 * @return		Chaîne de caractères représentant <I>value</I>.
	 * @see			setPrecision
	 * @see			setFormat
	 * @see			setNumberOfDecimals
	 */
	virtual QwtText label (double value) const;

	/**
	 * @param		précision (nombre maximum de chiffres significatifs)
	 *				utilisée lors des conversions double -> texte. Les zéros à
	 *				la fin ne sont pas considérés et il n'y a pas de remplissage
	 *				par des zéros (utiliser <I>setNumberOfDecimals</I> pour le
	 *				remplissage).
	 * @see			setPrecision
	 * @see			setNumberOfDecimals
	 * @see			label
	 * @see			QString::number
	 */
	virtual unsigned char getPrecision ( ) const
	{ return _precision; }

	/**
	 * @param		précision (nombre de chiffres significatifs) à utiliser lors
	 *				des conversions double -> texte.
	 * @see			getPrecision
	 * @see			label
	 * @see			QString::number
	 */
	virtual void setPrecision (unsigned char precision);

	/**
	 * @param		format utilisée lors des conversions double -> texte
	 *				('f', 'F', 'e', 'E', 'g' ou 'G').
	 * @see			setFormat
	 * @see			label
	 * @see			QString::number
	 */
	virtual char getFormat ( ) const
	{ return _format; }

	/**
	 * @param		format à utiliser lors des conversions double -> texte
	 *				('f', 'F', 'e', 'E', 'g' ou 'G').
	 * @see			getFormat
	 * @see			label
	 * @see			QString::number
	 */
	virtual void setFormat (char format);

	/**
	 * Permet de forcer le nombre de décimales affichées.
	 * @param		<I>true</I> si il faut activer cette fonctionnalité,
	 *				</I>false</I> dans le cas contraire.
	 * @param		Le nombre de décimales à afficher, dans le cas où la
	 * 				fonctionnalité est activée.
	 * @see			isNumberOfDecimalsEnabled
	 * @see			getNumberOfDecimals
	 */
	virtual void setNumberOfDecimals (bool enable, unsigned char num);

	/**
	 * @return		<I>true</I> si il faut activer cette fonctionnalité de
	 *				gestion du nombre de décimales, </I>false</I> dans le cas
	 *				contraire.
	 * @see			setNumberOfDecimals
	 * @see			getNumberOfDecimals
	 */
	virtual bool isNumberOfDecimalsEnabled ( ) const
	{ return _isNumberOfDecimalsEnabled; }

	/**
	 * @return		Le nombre de décimales à afficher.
	 * @see			setNumberOfDecimals
	 * @see			isNumberOfDecimalsEnabled
	 */
	virtual unsigned char getNumberOfDecimals ( ) const
	{ return _numberOfDecimals; }


	private :

	 /** Précision (nombre de chiffres significatifs)) utilisée lors des
	 * conversions double -> texte. */
	unsigned char			_precision;

	/** Format utilisé lors des conversions double -> texte. */
	char					_format;

	 /** <I>true</I> si il faut activer la fonctionnalité de gestion du nombre
	  * de décimales, </I>false</I> dans le cas contraire. */
	bool					_isNumberOfDecimalsEnabled;
	
	/** Le nombre de décimales à afficher. */
	unsigned char			_numberOfDecimals;

	/** Le nombre de décimales à afficher en mode automatique. */
	mutable unsigned char	_autoNumberOfDecimals;
};	// class QwtExtendedScaleDraw

#endif	// QWT_EXTENDED_SCALE_DRAW_H
