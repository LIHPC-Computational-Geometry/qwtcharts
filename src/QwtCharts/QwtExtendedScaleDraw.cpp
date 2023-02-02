#include "QwtCharts/QwtExtendedScaleDraw.h"
#include <TkUtil/NumericConversions.h>
#include <QtUtil/QtUnicodeHelper.h>

#include <iostream>
#include <ctype.h>


USING_UTIL
USING_STD

static const Charset	charset ("àéèùô");
USE_ENCODING_AUTODETECTION

QwtExtendedScaleDraw::QwtExtendedScaleDraw (
										unsigned char precision, char format)
	: QwtScaleDraw ( ), _precision (precision), _format (format),
	  _isNumberOfDecimalsEnabled (false), _numberOfDecimals (3),
	  _autoNumberOfDecimals (3)
{
}	// QwtExtendedScaleDraw::QwtExtendedScaleDraw


QwtExtendedScaleDraw::QwtExtendedScaleDraw (const QwtExtendedScaleDraw& qesd)
	: QwtScaleDraw ( ), _precision (qesd._precision), _format (qesd._format),
	  _isNumberOfDecimalsEnabled (qesd._isNumberOfDecimalsEnabled),
	  _numberOfDecimals (qesd._numberOfDecimals),
	  _autoNumberOfDecimals (qesd._autoNumberOfDecimals)
{
}	// QwtExtendedScaleDraw::QwtExtendedScaleDraw


QwtExtendedScaleDraw& QwtExtendedScaleDraw::operator = (
											const QwtExtendedScaleDraw& qesd)
{
cerr << __FILE__ << ' ' << __LINE__ << " QwtExtendedScaleDraw::operator = is not yet implemented for QWT 6.x." << endl;

	if (&qesd != this)
	{
		_precision					= qesd._precision;
		_format						= qesd._format;
		_isNumberOfDecimalsEnabled	= qesd._isNumberOfDecimalsEnabled;
		_numberOfDecimals			= qesd._numberOfDecimals;
		_autoNumberOfDecimals		= qesd._autoNumberOfDecimals;
	}	// if (&qesd != this)

	return *this;
}	// QwtExtendedScaleDraw::::operator =


QwtExtendedScaleDraw::~QwtExtendedScaleDraw ( )
{
}	// QwtExtendedScaleDraw::~QwtExtendedScaleDraw


// Retourne le nombre de chiffres à afficher au minimum pour que chaque
// graduation soit différente de celles qui les encadrent.
// C'est un nombre minimal qu'on retourne, pas un index.
static size_t discriminantChars (const QwtScaleDiv& div)
{
	size_t	disc	= 1;

	QList<double>	ticks	= div.ticks (QwtScaleDiv::MajorTick);
	for (size_t i = 1; i < ticks.count ( ); i++)
	{
		const string	prev	= NumericConversions::toStr (ticks [i - 1]);
		const string	cur	= NumericConversions::toStr (ticks [i]);
		const size_t	stop	= prev.length ( ) < cur.length ( ) ?
								  prev.length ( ) : cur.length ( );
		for (size_t j = 0; j < stop; j++)
		{
			const char	p	= prev [j];
			const char	c	= cur [j];
			if ((p != c) && (0 != isdigit (p)))
				break;

			if ((p == c) && (0 != isdigit (p)))
				disc	= disc > (j + 2) ? disc : (j + 2);
		}	// for (size_t j = 0; j < stop; j++)
	}	// for (size_t i = 1; i < ticks.count ( ); i++)

	return disc;
}	// discriminantChars


static string shortestExpRepresentation (const string& exp)
{
	if (1 >= exp.length ( ))
		return exp;

	UTF8String		rep;
	size_t			added	= 0;
	if (('e' == exp [0]) || ('E' == exp [0]))
	{
		rep << exp [0];
		size_t	i	= 1;
		switch (exp [i])
		{
			case '-'	: rep << exp [i++];	break;
			case '+'	: i++; break;
		}	// switch (exp [i])

		// Elimination des 0 inutiles :
		for ( ; i < exp.length ( ); i++)
			if ('0' != exp [i])
				break;

		for ( ; i < exp.length ( ); i++)
		{
			rep << exp [i];
			added++;
		}	// for ( ; i < exp.length ( ); i++)
	}	// if (('e' == exp [0]) || ('E' == exp [0]))

	if (0 == added)
		rep.clear ( );	// Cas particulier : e+00

	return rep.iso ( );
}	// shortestExpRepresentation


void QwtExtendedScaleDraw::draw (QPainter* painter, const QPalette& palette) const
{
	if ((false == isNumberOfDecimalsEnabled ( )) &&
	    (('c' == getFormat ( )) ||
	     ('f' == getFormat ( )) || ('F' == getFormat ( )) ||
	     ('g' == getFormat ( )) || ('G' == getFormat ( ))))
	{
		const QwtScaleDiv&	div	= scaleDiv ( );
		_autoNumberOfDecimals	= discriminantChars (div);
	}	// if ((false == isNumberOfDecimalsEnabled ( )) && ...
	
	QwtScaleDraw::draw (painter, palette);
}	// QwtExtendedScaleDraw::draw


QwtText QwtExtendedScaleDraw::label (double value) const
{
	bool	automatic	= false;	// Déterminer le nombre de décimales ?
   if (('c' == getFormat ( )) ||
	   ('f' == getFormat ( )) || ('F' == getFormat ( )) ||
	   ('g' == getFormat ( )) || ('G' == getFormat ( )))
		automatic	= true;

	if (false == isNumberOfDecimalsEnabled ( ))
		if (false == automatic)
			return QString::number (value, getFormat( ), (int)getPrecision ( ));

	// Le label retourné par Qt peut avoir perdu en précision.
	// Ex : value = 49.999999995, précision = 10 => 49.99999999
	// => On se fait un label très précis à partir duquel on fera les
	// ajustements.
	const char	fmt		= 'c' == getFormat( ) ? 'e' : getFormat( );
	QString		label	= QString::number (value, fmt, 15);

	UTF8String		strValue (QtUnicodeHelper::qstringToUTF8String (label));
	const size_t	usedDecNum	= true == automatic ?
							_autoNumberOfDecimals : _numberOfDecimals;
	const size_t	dotPos	= strValue.find ('.');
	size_t	expPos	= -1;
	switch (getFormat ( ))
	{
		case 'E'	:
		case 'G'	: expPos	= strValue.find ('E');	break;
		default		: expPos	= strValue.find ('e');
	}	// switch (getFormat ( ))
	size_t			decNum	= (size_t)-1 == dotPos ?
							  0 : strValue.length ( ) - dotPos - 1;
	if ((decNum > 0) && ((size_t)-1 != expPos))	// v 3.7.1
		decNum	-= strValue.length ( ) - expPos - 1;
	if (decNum == usedDecNum)
		return label;

	UTF8String	exponent;
	if (-1 != expPos)
	{
		if ('c' != getFormat ( ))
			exponent	= strValue.substring (expPos);
		else
			exponent	= shortestExpRepresentation(strValue.substring(expPos));
		if (1 <= expPos)
			strValue	= strValue.substring (0, expPos - 1);	// v 3.7.1
	}	// if (-1 != expPos)
	if (decNum > usedDecNum)
	{
		strValue	= strValue.substring (0, dotPos + usedDecNum);
		strValue << exponent;
	}
	else
	{
		size_t	count	= usedDecNum - decNum;
		if ((size_t)-1 == dotPos)
			strValue << ".";
		for (size_t i = 0; i < count; i++)
			strValue << '0';
		strValue << exponent;
	}	// else if (decNum > getNumberOfDecimals ( ))

	return QwtText (strValue.iso ( ).c_str ( ));
}	// QwtExtendedScaleDraw::label


void QwtExtendedScaleDraw::setPrecision (unsigned char precision)
{
	_precision	= precision;
	// Le label créé par label (double) est mis en cache par
	// QwtAbstractScaleDraw pour des raisons de perfs -> invalidateCache
	invalidateCache ( );
}	// QwtExtendedScaleDraw::setPrecision


void QwtExtendedScaleDraw::setFormat (char format)
{
	_format	= format;
	// Le label créé par label (double) est mis en cache par
	// QwtAbstractScaleDraw pour des raisons de perfs -> invalidateCache
	invalidateCache ( );
}	// QwtExtendedScaleDraw::setFormat


void QwtExtendedScaleDraw::setNumberOfDecimals (bool enable, unsigned char num)
{
	if ((enable != _isNumberOfDecimalsEnabled) ||
	    (num != _numberOfDecimals))
	{
		_isNumberOfDecimalsEnabled	= enable;
		_numberOfDecimals			= num;
		invalidateCache ( );
	}	// if ((enable != _isNumberOfDecimalsEnabled) || ...
}	// QwtExtendedScaleDraw::setNumberOfDecimals
