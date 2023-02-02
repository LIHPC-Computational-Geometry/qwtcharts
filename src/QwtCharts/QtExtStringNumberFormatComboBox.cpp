#include "QwtCharts/QtExtStringNumberFormatComboBox.h"
#include <TkUtil/Exception.h>
#include <TkUtil/InternalError.h>
#include <QtUtil/QtUnicodeHelper.h>

#include <assert.h>


USING_UTIL

static const Charset	charset ("àéèùô");


struct StringNumberFormatMap
{
	const char	format;
	const char*	label;
};


#define FORMAT_NUM	(6)

static const	size_t				formatsNum	= FORMAT_NUM;
static const StringNumberFormatMap	formats [FORMAT_NUM]	=
{
	{ 'e', "[-]9.9e[+|-]999"},
	{ 'E', "[-]9.9E[+|-]999"},
	{ 'f', "[-]9.9"},
	{ 'g', "Le plus concis de 'e' et 'f'"},
	{ 'G', "Le plus concis de 'E' et 'f'"},
	{ 'c', "Le plus concis possible"}
};	// formats


QtExtStringNumberFormatComboBox::QtExtStringNumberFormatComboBox (
												QWidget* parent, char format)
	: QComboBox (parent)
{
	for (size_t i = 0; i < formatsNum; i++)
	{
		UTF8String	label (charset);
		label << formats [i].format << " - " << formats [i].label;
		addItem (UTF8TOQSTRING (label), formats [i].format);
	}	// for (size_t i = 0; i < formatsNum; i++)
}	// QtExtStringNumberFormatComboBox::QtExtStringNumberFormatComboBox


QtExtStringNumberFormatComboBox::QtExtStringNumberFormatComboBox (
										const QtExtStringNumberFormatComboBox&)
	: QComboBox ((QWidget*)0)
{
	assert (0 && "QtExtStringNumberFormatComboBox copy constructor is not allowed.");
}	// QtExtStringNumberFormatComboBox::QtExtStringNumberFormatComboBox (const QtExtStringNumberFormatComboBox& view)


QtExtStringNumberFormatComboBox& QtExtStringNumberFormatComboBox::operator = (
										const QtExtStringNumberFormatComboBox&)
{
	assert (0 && "QtExtStringNumberFormatComboBox::operator = is not allowed.");
	return *this;
}	// QtExtStringNumberFormatComboBox::operator =


QtExtStringNumberFormatComboBox::~QtExtStringNumberFormatComboBox ( )
{
}	// QtExtStringNumberFormatComboBox::~QtExtStringNumberFormatComboBox


char QtExtStringNumberFormatComboBox::getFormat ( ) const
{
	int	index	= currentIndex ( );
	if (index >= formatsNum)
	{
		INTERNAL_ERROR (exc, "Erreur de gestion de format.",
		                     "QtExtStringNumberFormatComboBox::getFormat")
		throw exc;
	}	// if (index >= formatsNum)

	return formats [index].format;
}	// QtExtStringNumberFormatComboBox::getFormat


void QtExtStringNumberFormatComboBox::setFormat (char format)
{
	for (size_t i = 0; i < formatsNum; i++)
	{
		if (formats [i].format == format)
		{
			setCurrentIndex (i);
			return;
		}	// if (formats [i].format == format)
	}	// for (size_t i = 0; i < formatsNum; i++)

	UTF8String	msg (charset);
	msg << "Format invalide : " << format << ".";
	throw Exception (msg);
}	// QtExtStringNumberFormatComboBox::setFormat


