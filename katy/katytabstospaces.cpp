/****************************************************************************
** Form implementation generated from reading ui file 'katytabstospaces.ui'
**
** Created: Thu Feb 8 00:51:52 2001
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "katytabstospaces.h"

#include <klocale.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a KatyTabsToSpaces which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 */
KatyTabsToSpaces::KatyTabsToSpaces( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "KatyTabsToSpaces" );
    resize( 188, 73 ); 
    setCaption( i18n( "KatyTabsToSpaces"  ) );
    vbox = new QVBoxLayout( this ); 
    vbox->setSpacing( 6 );
    vbox->setMargin( 11 );

    leadingOnly = new QCheckBox( this, "leadingOnly" );
    leadingOnly->setText( i18n( "Leading characters only"  ) );
    leadingOnly->setChecked( TRUE );
    QToolTip::add(  leadingOnly, i18n( "Check this to convert only tabs or spaces at the beginning of lines." ) );
    vbox->addWidget( leadingOnly );

    hbox = new QHBoxLayout; 
    hbox->setSpacing( 6 );
    hbox->setMargin( 0 );

    numberOfSpacesLabel = new QLabel( this, "numberOfSpacesLabel" );
    numberOfSpacesLabel->setText( i18n( "Number of spaces:"  ) );
    hbox->addWidget( numberOfSpacesLabel );

    numberOfSpaces = new QSpinBox( this, "numberOfSpaces" );
    hbox->addWidget( numberOfSpaces );
    vbox->addLayout( hbox );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
KatyTabsToSpaces::~KatyTabsToSpaces()
{
    // no need to delete child widgets, Qt does it all for us
}

