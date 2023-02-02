#ifndef QT_EXT_STRING_NUMBER_FORMAT_COMBOBOX_H
#define QT_EXT_STRING_NUMBER_FORMAT_COMBOBOX_H

#include <QComboBox>


/**
 * <P>Classe de boite à défilement permettant de sélectionner un format pour
 * les conversions nombre -> QString.
 * </P>
 * <P>Le format 'c' signifie le plus concis possible et est à implémenter par
 * l'application.
 * @see		QString::arg
 * @see		QString::number
 */
class QtExtStringNumberFormatComboBox : public QComboBox
{
	public :

	/**
	 * Constructeur par défaut.
	 * @param		Widget parent.
	 * @param		Format proposé par défaut
	 */
	QtExtStringNumberFormatComboBox (QWidget* parent, char format = 'g');

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtExtStringNumberFormatComboBox ( );

	/**
	 * @return		Le format en cours.
	 * @see			setFormat
	 */
	virtual char getFormat ( ) const;

	/**
	 * @param		Le nouveau format sélectionné.
	 * @see			getFormat
	 * @exception	Une exception est levée si le caractère transmis en argument
	 *				n'est pas valide
	 * @see			QString::arg
	 * @see			QString::number
	 */
	virtual void setFormat (char format);

	private :

	/** Constructeurs de copie interdits. */
	QtExtStringNumberFormatComboBox (const QtExtStringNumberFormatComboBox&);
	QtExtStringNumberFormatComboBox& operator = (
										const QtExtStringNumberFormatComboBox&);
};	// class QtExtStringNumberFormatComboBox

#endif	// QT_EXT_STRING_NUMBER_FORMAT_COMBOBOX_H
