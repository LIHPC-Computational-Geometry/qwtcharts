#include "QwtCharts/QwtUtilities.h"


#include <TkUtil/AutoArrayPtr.h>
#include <TkUtil/MemoryError.h>

#include <iostream>
#include <assert.h>


USING_UTIL
USING_STD


const QColor	QwtUtilities::lightOrange (255, 170, 0);
const QColor	QwtUtilities::darkOrange (160, 110, 0);


QwtUtilities::QwtUtilities ( )
{
	assert (0 && "QwtUtilities default constructor is not allowed.");
}	// QwtUtilities::QwtUtilities


QwtUtilities::QwtUtilities (const QwtUtilities& qesd)
{
	assert (0 && "QwtUtilities copy constructor is not allowed.");
}	// QwtUtilities::QwtUtilities


QwtUtilities& QwtUtilities::operator = (const QwtUtilities& qesd)
{
	assert (0 && "QwtUtilities assignment operator is not allowed.");

	return *this;
}	// QwtUtilities::::operator =


QwtUtilities::~QwtUtilities ( )
{
	assert (0 && "QwtUtilities destructor is not allowed.");
}	// QwtUtilities::~QwtUtilities


void QwtUtilities::copySymbol (const QwtSymbol& copied, QwtSymbol& copy)
{
	copy.setStyle (copied.style ( ));
	copy.setPen (copied.pen ( ));
	copy.setBrush (copied.brush ( ));
	copy.setSize (copied.size ( ));
}	// QwtUtilities::copySymbol


void QwtUtilities::setSymbolAttributes (
					QwtSymbol& symbol, QwtSymbol::Style style, QSize& size,
					QColor& penColor, QColor& brushColor)
{
	symbol.setStyle (style);
	symbol.setPen (penColor);
	QBrush  brush (symbol.brush ( ));
	brush.setColor (brushColor);
	symbol.setBrush (brush);
	symbol.setSize (size);
}	// QwtUtilities::setSymbolAttributes


QwtPlotCurve* QwtUtilities::cloneCurve (
								const QwtPlotCurve& curve, const string& name)
{
	QwtPlotCurve*   clone   = new QwtPlotCurve (name.c_str ( ));
	clone->setPen (curve.pen ( ));
	clone->setBrush (curve.brush ( ));
	clone->setStyle (curve.style ( ));
	const QwtSymbol*	curveSymbol = curve.symbol ( );
	QwtSymbol*		  	symbol  = 0 == curveSymbol ?  0 : new QwtSymbol ( );
	if (0 != symbol)
		QwtUtilities::copySymbol (*curveSymbol, *symbol);
	clone->setSymbol (symbol);
	AutoArrayPtr<double>	xData (curve.dataSize ( )),
							yData (curve.dataSize ( ));

	for (size_t i = 0; i < curve.dataSize ( ); i++)
	{
		xData [i]   = curve.sample (i).x ( );
		yData [i]   = curve.sample (i).y ( );
	}	for (size_t i = 0; i < curve.dataSize ( ); i++)
	clone->setSamples (xData.get ( ), yData.get ( ), curve.dataSize ( ));

	return clone;
}	// QwtUtilities::cloneCurve


void QwtUtilities::copySeriesData (
			const QwtSeriesData<QPointF>& copied, QwtSeriesData<QPointF>& copy)
{
	QwtArraySeriesData<QPointF>*	asdCopy =
						dynamic_cast<QwtArraySeriesData<QPointF>*>(&copy);
	CHECK_NULL_PTR_ERROR (asdCopy)
	QVector<QPointF>	samples;
	for (size_t i = 0; i < copied.size ( ); i++)
		samples.push_back (copied.sample (i));
	asdCopy->setSamples (samples);
/* Plante, pourquoi ??? 0 == asdCopied
	C'est du QwtPointSeriesData qui hérite de QwtArraySeriesData<QPointF>.

	const QwtArraySeriesData<QPointF>*  asdCopied   =
				dynamic_cast<const QwtArraySeriesData<QPointF>*>(&copied);
	QwtArraySeriesData<QPointF>*	asdCopy =
				dynamic_cast<QwtArraySeriesData<QPointF>*>(&copy);
	CHECK_NULL_PTR_ERROR (asdCopied)
	CHECK_NULL_PTR_ERROR (asdCopy)
	asdCopy->setSamples (asdCopied->samples ( ));
*/
}	// QwtUtilities::copySeriesData


QwtColumnSymbol* QwtUtilities::cloneSymbol (const QwtColumnSymbol& symbol)
{
	QwtColumnSymbol*	newSymbol	= new QwtColumnSymbol (symbol.style ( ));
	CHECK_NULL_PTR_ERROR (newSymbol)
	newSymbol->setFrameStyle (symbol.frameStyle ( ));
	newSymbol->setLineWidth (symbol.lineWidth ( ));
	newSymbol->setPalette (symbol.palette ( ));

	return newSymbol;
}	// QwtUtilities::cloneSymbol
