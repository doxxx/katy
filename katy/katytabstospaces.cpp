/****************************************************************************
** Form implementation generated from reading ui file 'katytabstospaces.ui'
**
** Created: Wed May 8 22:20:49 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "katytabstospaces.h"

#include <qvariant.h>
#include <klocale.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a KatyTabsToSpaces which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 */
KatyTabsToSpaces::KatyTabsToSpaces( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "KatyTabsToSpaces" );
    resize( 188, 73 ); 
    setCaption( i18n( "KatyTabsToSpaces" ) );
    KatyTabsToSpacesLayout = new QVBoxLayout( this, 11, 6, "KatyTabsToSpacesLayout"); 

    leadingOnly = new QCheckBox( this, "leadingOnly" );
    leadingOnly->setText( i18n( "Leading characters only" ) );
    leadingOnly->setChecked( TRUE );
    QToolTip::add( leadingOnly, i18n( "Check this to convert only tabs or spaces at the beginning of lines." ) );
    KatyTabsToSpacesLayout->addWidget( leadingOnly );

    Layout1 = new QHBoxLayout( 0, 0, 6, "Layout1"); 

    numberOfSpacesLabel = new QLabel( this, "numberOfSpacesLabel" );
    numberOfSpacesLabel->setText( i18n( "Number of spaces:" ) );
    Layout1->addWidget( numberOfSpacesLabel );

    numberOfSpaces = new QSpinBox( this, "numberOfSpaces" );
    Layout1->addWidget( numberOfSpaces );
    KatyTabsToSpacesLayout->addLayout( Layout1 );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
KatyTabsToSpaces::~KatyTabsToSpaces()
{
    // no need to delete child widgets, Qt does it all for us
}

