#include "QwtCharts/QwtExtendedMultiBarChart.h"

/**
 * <P>Remarques CP :</P>
 * <UL>
 * <LI>Une <I>barre Qwt</I> (<I>Bar</I>) est une <I>classe</I> d'histogramme,
 * <LI>Un <I>échantillon Qwt</I> (<I>Sample</I>) est une série de données.
 * <LI>Un <I>Histogramme</I>, au sns CP (!), est un graphique représentant
 * sous formes de barres des séries de données dont les données sont regroupées
 * par classe, chaque classe de chaque série étant matérialisée par une barre.
 * </UL>
 */

#include "QwtCharts/QwtUtilities.h"

#include <TkUtil/Exception.h>
#include <TkUtil/InternalError.h>
#include <TkUtil/MemoryError.h>
#include <QtUtil/QtUnicodeHelper.h>

#include <QPalette>

#include <qwt_column_symbol.h>
#include <qwt_legend.h>
#include <qwt_legend_label.h>
#include <qwt_plot.h>
#include <qwt_plot_layout.h>
#include <qwt_scale_map.h>


#include <iostream>
#include <assert.h>


USING_STD
USING_UTIL

static const Charset	charset ("àéèùô");
USE_ENCODING_AUTODETECTION


QwtExtendedMultiBarChart::QwtExtendedMultiBarChart (const QString& title)
	: QwtPlotMultiBarChart (title),
	  _rectangles ( ), _selectedClasses ( ),
	  _style (QwtExtendedMultiBarChart::STYLE_GROUPED),
	  _displayElementsNum (true),
	  _elementsNumPos(QwtExtendedMultiBarChart::TOP),
	  _visibleSeries ( )
{
	setHistogramStyle (QwtExtendedMultiBarChart::STYLE_GROUPED);
}	// QwtExtendedMultiBarChart::QwtExtendedMultiBarChart


QwtExtendedMultiBarChart::QwtExtendedMultiBarChart (
												const QwtExtendedMultiBarChart&)
	: QwtPlotMultiBarChart (""),
	  _rectangles ( ), _selectedClasses ( ),
	  _style (QwtExtendedMultiBarChart::STYLE_GROUPED),
	  _displayElementsNum (true),
	  _elementsNumPos(QwtExtendedMultiBarChart::TOP),
	  _visibleSeries ( )
{
	assert (0 && "QwtExtendedMultiBarChart copy constructor is not allowed.");
}	// QwtExtendedMultiBarChart::QwtExtendedMultiBarChart


QwtExtendedMultiBarChart& QwtExtendedMultiBarChart::operator = (
												const QwtExtendedMultiBarChart&)
{
	assert (0 && "QwtExtendedMultiBarChart operator = is not allowed.");
	return *this;
}	// QwtExtendedMultiBarChart::QwtExtendedMultiBarChart


QwtExtendedMultiBarChart::~QwtExtendedMultiBarChart ( )
{
}	// QwtExtendedMultiBarChart::~QwtExtendedMultiBarChart


size_t QwtExtendedMultiBarChart::getSeriesNum ( ) const
{
	const QwtSeriesData<QwtSetSample>*	d	= data ( );

	return (0 == d) || (0 == d->size ( )) ? 0 : d->sample (0).set.size ( );
}	// QwtExtendedMultiBarChart::getSeriesNum


size_t QwtExtendedMultiBarChart::getClassesNum ( ) const
{
	const QwtSeriesData<QwtSetSample>*	d	= data ( );

	return 0 == d ? 0 : d->size ( );
}	// QwtExtendedMultiBarChart::getClassesNum


void QwtExtendedMultiBarChart::setHistogramStyle (
								QwtExtendedMultiBarChart::HISTOGRAM_STYLE style)
{
	if (style != _style)
	{
		_style	= style;
		switch (_style)
		{
			case QwtExtendedMultiBarChart::STYLE_GROUPED	:
				QwtPlotMultiBarChart::setStyle (QwtPlotMultiBarChart::Grouped);
				break;
			case QwtExtendedMultiBarChart::STYLE_STACKED	:
				QwtPlotMultiBarChart::setStyle (QwtPlotMultiBarChart::Stacked);
				break;
			case QwtExtendedMultiBarChart::STYLE_OVERLAPPED	:
				// cf. QwtPlotMultiBarChart::setStyle
				legendChanged ( );
				itemChanged ( );
				break;
			default	:
				throw Exception (UTF8STR ("Style non supporté dans QwtExtendedMultiBarChart::setHistogramStyle."));
		}	// switch (_style)
	}	// if (style != _style)
}	// QwtExtendedMultiBarChart::setHistogramStyle


bool QwtExtendedMultiBarChart::displayElementsNum ( ) const
{
	return _displayElementsNum;
}	// QwtExtendedMultiBarChart::displayElementsNum


void QwtExtendedMultiBarChart::displayElementsNum (bool display)
{
	if (display != _displayElementsNum)
	{
		_displayElementsNum	= display;

		// L'idée ici est de provoquer un replot sans tout recalculer ...
		legendChanged ( );
	}	// if (display != _displayElementsNum)
}	// QwtExtendedMultiBarChart::displayElementsNum


QwtExtendedMultiBarChart::POSITION
							QwtExtendedMultiBarChart::getElementNumPos ( ) const
{
	return _elementsNumPos;
}	// QwtExtendedMultiBarChart::getElementNumPos


void QwtExtendedMultiBarChart::setElementNumPos (
										QwtExtendedMultiBarChart::POSITION pos)
{
	if (pos != _elementsNumPos)
	{
		_elementsNumPos	= pos;

		// L'idée ici est de provoquer un replot sans tout recalculer ...
		if (true == displayElementsNum ( ))
			legendChanged ( );
	}	// if (pos != _elementsNumPos)
}	// QwtExtendedMultiBarChart::setElementNumPos


bool QwtExtendedMultiBarChart::isVisible (size_t index) const
{
	if (index > getSeriesNum ( ))
	{
		UTF8String	message (charset);
		message << "QwtExtendedMultiBarChart::isVisible. Index invalide ("
		        << index << "). Nombre de séries : " << getSeriesNum ( )
		        << ".";
		throw Exception (message);
	}	// if (index > getSeriesNum ( ))
	if (index > _visibleSeries.size ( ))
	{
		UTF8String	message (charset);
		message << "Erreur interne. Index invalide ("
		        << index << "). Nombre de séries : " << getSeriesNum ( )
		        << ".";
		INTERNAL_ERROR (exc, message, "QwtExtendedMultiBarChart::isVisible")
		throw exc;
	}	// if (index > _visibleSeries.size ( ))

	return _visibleSeries [index];
}	// QwtExtendedMultiBarChart::isVisible


void QwtExtendedMultiBarChart::setVisible (size_t index, bool visible)
{
	if (index > getSeriesNum ( ))
	{
		UTF8String	message (charset);
		message << "QwtExtendedMultiBarChart::setVisible. Index invalide ("
		        << index << "). Nombre de séries : " << getSeriesNum ( )
		        << ".";
		throw Exception (message);
	}	// if (index > getSeriesNum ( ))
	if (index > _visibleSeries.size ( ))
	{
		UTF8String	message (charset);
		message << "Erreur interne. Index invalide ("
		        << index << "). Nombre de séries : " << getSeriesNum ( )
		        << ".";
		INTERNAL_ERROR (exc, message, "QwtExtendedMultiBarChart::setVisible")
		throw exc;
	}	// if (index > _visibleSeries.size ( ))

	if (_visibleSeries [index] != visible)
		_visibleSeries [index]	= visible;
}	// QwtExtendedMultiBarChart::setVisible


bool QwtExtendedMultiBarChart::isSelected (size_t s, size_t c) const
{
	if ((s >= getSeriesNum ( )) || (c >= getClassesNum ( )))
	{
		UTF8String	message (charset);
		message << "Erreur, classe inexistante (Série : " << s
			    << ", class : " << c
			    << ") dans QwtExtendedMultiBarChart::isSelected. "
			    << "Nombre de séries : " << getSeriesNum ( )
			    << ", nombre de classes : " << getClassesNum ( )
			    << ".";
		throw Exception (message);
	}	// if ((s >= getSeriesNum ( )) || (c >= getClassesNum ( )))

	return _selectedClasses [c][s];
}	// QwtExtendedMultiBarChart::isSelected


void QwtExtendedMultiBarChart::clearSelection ( )
{
	const size_t	barsNum		= getClassesNum ( );
	const size_t	seriesNum	= getSeriesNum ( );

	for (size_t b = 0; b < barsNum; b++)
		for (size_t s = 0; s < seriesNum; s++)
			_selectedClasses [b][s]	= false;

	// L'idée ici est de provoquer un replot sans tout recalculer ...
	legendChanged ( );
}	// QwtExtendedMultiBarChart::clearSelection


void QwtExtendedMultiBarChart::addToSelection (
					const vector<pair<size_t, size_t> >& classes)
{
	for (vector< pair<size_t, size_t> >::const_iterator it = classes.begin ( );
	     classes.end ( ) != it; it++)
		_selectedClasses [(*it).second][(*it).first]	= true;
/*cout << "SELECTION IS : " << endl;
	const size_t	barsNum		= getClassesNum ( );
	const size_t	seriesNum	= getSeriesNum ( );
	for (size_t b = 0; b < barsNum; b++)
		for (size_t s = 0; s < seriesNum; s++)
			if (true == _selectedClasses [b][s])
					cout << "(" << s << ", " << b << ") ";
cout << endl; */
}	// QwtExtendedMultiBarChart::addToSelection


void QwtExtendedMultiBarChart::removeFromSelection (
					const vector<pair<size_t, size_t> >& classes)
{
	for (vector< pair<size_t, size_t> >::const_iterator it = classes.begin ( );
	     classes.end ( ) != it; it++)
		_selectedClasses [(*it).second][(*it).first]	= false;
/*cout << "SELECTION IS : " << endl;
	const size_t	barsNum		= getClassesNum ( );
	const size_t	seriesNum	= getSeriesNum ( );
	for (size_t b = 0; b < barsNum; b++)
		for (size_t s = 0; s < seriesNum; s++)
			if (true == _selectedClasses [b][s])
					cout << "(" << s << ", " << b << ") ";
cout << endl;*/
}	// QwtExtendedMultiBarChart::removeFromSelection


vector<pair<size_t, size_t> > QwtExtendedMultiBarChart::getSelection ( ) const
{
	
	vector<pair<size_t, size_t> >	selectedClasses;

	const size_t	barsNum		= getClassesNum ( );
	const size_t	seriesNum	= getSeriesNum ( );

	for (size_t b = 0; b < barsNum; b++)
		for (size_t s = 0; s < seriesNum; s++)
			if (true == _selectedClasses [b][s])
				selectedClasses.push_back (pair<size_t, size_t>(s, b));

	return selectedClasses;
}	// QwtExtendedMultiBarChart::getSelection


vector <pair<size_t, size_t> > QwtExtendedMultiBarChart::intersectedClasses (
															int x, int y) const
{
	vector <pair<size_t, size_t> >	classes;

	const size_t	barsNum		= getClassesNum ( );
	const size_t	seriesNum	= getSeriesNum ( );

	for (size_t b = 0; b < barsNum; b++)
	{
		for (size_t s = 0; s < seriesNum; s++)
		{
			if (_rectangles [b][s].contains (x, y))
			{
				classes.push_back (pair<size_t, size_t>(s, b));
			}	// if (_rectangles [b][s].contains (x, y))
		}	// for (size_t s = 0; s < seriesNum; s++)
	}	// for (size_t b = 0; b < barsNum; b++)

	return classes;
}	// QwtExtendedMultiBarChart::intersectedClasses


bool QwtExtendedMultiBarChart::intersects (
								int x, int y, double& abs, size_t& ord) const
{
	const size_t	barsNum		= getClassesNum ( );
	const size_t	seriesNum	= getSeriesNum ( );

	for (size_t b = 0; b < barsNum; b++)
	{
		for (size_t s = 0; s < seriesNum; s++)
		{
			if (_rectangles [b][s].contains (x, y))
			{
				QwtSetSample	sam	= sample (b);
				abs	= sam.value;
				ord	= sam.set [s];

				return true;
			}	// if (_rectangles [b][s].contains (x, y))
		}	// for (size_t s = 0; s < seriesNum; s++)
	}	// for (size_t b = 0; b < barsNum; b++)

	return false;
}	// QwtExtendedMultiBarChart::intersects


void QwtExtendedMultiBarChart::drawSample (
			QPainter* painter,
			const QwtScaleMap& xMap, const QwtScaleMap& yMap,
			const QRectF& canvasRect, const QwtInterval& boundingInterval,
			int index, const QwtSetSample& sample) const
{
//	if ((index < _visibleSeries.size ( )) && (false == _visibleSeries [index]))
//		return;

	switch (getHistogramStyle ( ))
	{
		case QwtExtendedMultiBarChart::STYLE_GROUPED	:
		case QwtExtendedMultiBarChart::STYLE_STACKED	:
		QwtPlotMultiBarChart::drawSample (
			painter, xMap, yMap, canvasRect, boundingInterval, index, sample);
		break;
		case QwtExtendedMultiBarChart::STYLE_OVERLAPPED	:
		{
			double	sampleWidth	= Qt::Vertical == orientation ( ) ?
				QwtPlotMultiBarChart::sampleWidth (xMap, canvasRect.width ( ), 
								boundingInterval.width(), sample.value) :
				QwtPlotMultiBarChart::sampleWidth (yMap, canvasRect.height ( ),
								boundingInterval.width(), sample.value);
			drawOverlappedBars (
				painter, xMap, yMap, canvasRect, index, sampleWidth, sample);
		}
		break;
		default	:
		{
			UTF8String	mess (charset);
mess << "QwtExtendedMultiBarChart::drawSample, style non supporté ("
     << (unsigned long)getHistogramStyle ( ) << ").";
		ConsoleOutput::cout ( ) << mess << co_endl;
		}
	}	// switch (getHistogramStyle ( ))
}	// QwtExtendedMultiBarChart::drawSample


void QwtExtendedMultiBarChart::drawOverlappedBars (
			QPainter* painter, const QwtScaleMap& xMap, const QwtScaleMap& yMap,
			const QRectF& canvasRect, int index, double sampleWidth,
			const QwtSetSample &sample) const
{	// CP : Code fortement inspiré de QwtPlotMultiBarChart::drawGroupedBars.
	const int		numBars		= sample.set.size ( );

	if (0 == numBars)
		return;
	if (Qt::Horizontal == orientation ( ))
	{
		UTF8String	mess (charset);
		mess << "QwtExtendedMultiBarChart::drawOverlappedBars. Seule "
		     << "l'orientation verticale est actuellement supportée. Tracé de "
		     << "l'histogramme selon cette orientation.";
		ConsoleOutput::cerr ( ) << mess << co_endl;
	}

	// sampleWidth est la largeur occupée par une classe.
	// La moitié de cette espace est occupé par la barre complétement visible,
	// l'autre moitié est à répartir entre les n-1 barres restantes.
	const double	barWidth	= sampleWidth / 2;
	const double	offset		= 1 == numBars ? 0 : barWidth / (numBars - 1);
	const double	y1			= yMap.transform (baseline ( ));
//	const double	x0			= xMap.transform (sample.value)-0.5*sampleWidth;
	const double	x0			= xMap.transform (sample.value)-0.5*barWidth;

	for (int i = numBars - 1; i >= 0; i--)
	{
		if ((i < _visibleSeries.size ( )) && (false == _visibleSeries [i]))
			continue;
		const double	x1	= x0 + i * offset;
		const double	x2	= x1 + barWidth;
		const double	y2	= yMap.transform (sample.set[i]);

		QwtColumnRect	barRect;
		barRect.direction	= ( y1 < y2 ) ?
						QwtColumnRect::TopToBottom : QwtColumnRect::BottomToTop;
		barRect.hInterval	= QwtInterval (x1, x2).normalized ( );
		if (0 != i)
			barRect.hInterval.setBorderFlags (QwtInterval::ExcludeMinimum);
		barRect.vInterval	= QwtInterval (y1, y2).normalized ( );
		drawBar (painter, index, i, barRect);
	}	// for (int i = numBars - 1; i >= 0; i--)
}	// QwtExtendedMultiBarChart::drawOverlappedBars


void QwtExtendedMultiBarChart::drawBar (
						QPainter* painter, int sampleIndex, int barIndex,
						const QwtColumnRect& rect) const
{
	if ((barIndex < _visibleSeries.size ( )) &&
	    (false == _visibleSeries [barIndex]))
		return;

	assert (0 != painter);
	QPen	oldPen	= painter->pen ( );
	QwtColumnSymbol*	sym	= 0;
	QwtExtendedMultiBarChart*	ch	= (QwtExtendedMultiBarChart*)(this);
	if ((sampleIndex >= 0) && (barIndex >= 0))
		if (true == isSelected (barIndex, sampleIndex))
		{
			sym	= QwtUtilities::cloneSymbol (*symbol (barIndex));
			QwtColumnSymbol*	tmp	=
								QwtUtilities::cloneSymbol (*symbol (barIndex));
			CHECK_NULL_PTR_ERROR (sym)
			CHECK_NULL_PTR_ERROR (tmp)
			QPalette	palette (tmp->palette ( ));
			palette.setColor (QPalette::Dark, QwtUtilities::darkOrange);
			palette.setColor (QPalette::Light, QwtUtilities::lightOrange);
			tmp->setPalette (palette);
			ch->setSymbol (barIndex, tmp);
		}	// if (true == isSelected (sampleIndex, barIndex))

	QwtPlotMultiBarChart::drawBar (painter, sampleIndex, barIndex, rect);
	// Affichage du nombre d'éléments de la classe :
	if ((true == displayElementsNum ( )) && (sampleIndex>=0) && (barIndex>=0))
	{
		painter->save ( );

		QPalette	p (symbol (barIndex)->palette ( ));
		const QwtSeriesData<QwtSetSample>*	data	= this->data ( );
		const QwtSetSample	sample	= data->sample (sampleIndex);
		double	num	= sample.set [barIndex];
		// On affiche que si non nul :
		if (0 != num)
		{
			bool			textColor	= true;
			QString			text		= QString ( ).setNum (num);
			QFontMetrics	metrics		= painter->fontMetrics ( );
			double			x			= rect.toRect ( ).x ( );
			double			y			= rect.toRect ( ).y ( );
#if QT_VERSION < QT_VERSION_CHECK(5, 11, 0)
			x	= x + (rect.toRect ( ).width ( ) - metrics.width (text)) / 2;
#else	// QT_VERSION < QT_VERSION_CHECK(5, 11, 0)
// REM : à partir de la version 5.11 QFontMetrics::width (text) est remplacé par QFontMetrics::horizontalAdvance (text, -1)
			x	= x + (rect.toRect ( ).width ( ) - metrics.horizontalAdvance (text)) / 2;	// v 4.2.1
#endif	// QT_VERSION < QT_VERSION_CHECK(5, 11, 0)
			switch (getElementNumPos ( ))
			{
				case CENTER	:
					y += (rect.toRect( ).height( ) - metrics.height( )) / 2;
					y	+= metrics.ascent ( );
					textColor	= false;	
					break;
				case BOTTOM	:
					y += rect.toRect ( ).height ( ) - metrics.ascent ( );
					textColor	= false;
					break;
				default		:
				{
					static bool	first	= true;
					if (true == first)
					{
						UTF8String	mess (charset);
						mess << "Erreur en " __FILE__ << ' ' << (unsigned long)__LINE__
						     << " Position non supportée ("
						     << (unsigned long)getElementNumPos ( ) << ")";
						ConsoleOutput::cerr ( ) << mess << co_endl;
					}
					first	= false;
				}
				break;
				case TOP	: y -= 5;
			}	// switch (getElementNumPos ( ))
			if (true == textColor)
				painter->setPen (symbol (
							barIndex)->palette ( ).color (QPalette::Window));
			painter->drawText (x, y - 5, text);
//			painter->drawText (x, y - metrics.ascent ( ), QString ( ).setNum (num));
		}	// if (0 != num)

		painter->restore ( );
	}	// if ((true == displayElementsNum ( )) && ...

	// On remet le symbole "non sélectionné" pour les barres suivantes :
	if (0 != sym)
		ch->setSymbol (barIndex, sym);

	if ((sampleIndex < 0) || (barIndex < 0))
		return;

	_rectangles [sampleIndex][barIndex]	= rect.toRect ( );
}	// QwtExtendedMultiBarChart::drawBar


void QwtExtendedMultiBarChart::dataChanged ( )
{
	const size_t	barsNum		= getClassesNum ( );
	const size_t	seriesNum	= getSeriesNum ( );

	_rectangles.resize (barsNum);
	_selectedClasses.resize (barsNum);
	_visibleSeries.resize (seriesNum);
	vector< vector <bool> >::iterator	its	= _selectedClasses.begin ( );
	for (vector< vector <QRectF> >::iterator itr = _rectangles.begin ( );
	     _rectangles.end ( ) != itr; itr++, its++)
	{
		(*itr).resize (seriesNum);
		(*its).resize (seriesNum);
	}	// for (vector<vector <QRectF> >::iterator itr = ...

	for (size_t i = 0; i < seriesNum; i++)
		_visibleSeries [i]	= true;	// CP : before clearSelection ...

	clearSelection ( );

	QwtPlotMultiBarChart::dataChanged ( );

	QwtPlot*		chart	= plot ( );
	QwtLegend*		legend	= 0 == chart ?
								0 : dynamic_cast<QwtLegend*>(chart->legend ( ));
	QList<QWidget*>	widgets;
	if (0 != legend)
		widgets	= legend->legendWidgets (chart->itemToInfo (this));
	for (int i = 0; i < widgets.size ( ); i++)
	{
		QwtLegendLabel*	label	= dynamic_cast<QwtLegendLabel*>(widgets [i]);
		if (0 != label)
			label->setChecked (true);
	}	// for (int i = 0; i < widgets.size ( ); i++)
}	// QwtExtendedMultiBarChart::dataChanged

