/****************************************************************************
** Form implementation generated from reading ui file 'katyprefindenting.ui'
**
** Created: Thu Feb 8 00:36:01 2001
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "katyprefindenting.h"

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
 *  Constructs a KatyPrefIndentingPage which is a child of 'parent', with the 
 *  name 'name'.' 
 */
KatyPrefIndentingPage::KatyPrefIndentingPage( QWidget* parent,  const char* name )
    : QFrame( parent, name )
{
    if ( !name )
	setName( "KatyPrefIndentingPage" );
    resize( 332, 104 ); 
    setFrameShadow( QFrame::Plain );
    setFrameShape( QFrame::NoFrame );
    setCaption( i18n( "Tabs and Indenting"  ) );
    vbox = new QVBoxLayout( this ); 
    vbox->setSpacing( 6 );
    vbox->setMargin( 11 );

    hbox = new QHBoxLayout; 
    hbox->setSpacing( 6 );
    hbox->setMargin( 0 );

    tabSizeLabel = new QLabel( this, "tabSizeLabel" );
    tabSizeLabel->setText( i18n( "Tab Character Size:"  ) );
    hbox->addWidget( tabSizeLabel );

    tabSize = new QSpinBox( this, "tabSize" );
    hbox->addWidget( tabSize );
    vbox->addLayout( hbox );

    useSpaces = new QCheckBox( this, "useSpaces" );
    useSpaces->setText( i18n( "Tab key inserts spaces instead of tab characters"  ) );
    vbox->addWidget( useSpaces );

    hbox_2 = new QHBoxLayout; 
    hbox_2->setSpacing( 6 );
    hbox_2->setMargin( 0 );

    indentSizeLabel = new QLabel( this, "indentSizeLabel" );
    indentSizeLabel->setText( i18n( "Number Of Spaces:"  ) );
    indentSizeLabel->setEnabled( FALSE );
    hbox_2->addWidget( indentSizeLabel );

    indentSize = new QSpinBox( this, "indentSize" );
    indentSize->setEnabled( FALSE );
    hbox_2->addWidget( indentSize );
    vbox->addLayout( hbox_2 );

    // signals and slots connections
    connect( useSpaces, SIGNAL( toggled(bool) ), indentSize, SLOT( setEnabled(bool) ) );
    connect( useSpaces, SIGNAL( toggled(bool) ), indentSizeLabel, SLOT( setEnabled(bool) ) );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
KatyPrefIndentingPage::~KatyPrefIndentingPage()
{
    // no need to delete child widgets, Qt does it all for us
}

