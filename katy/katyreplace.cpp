/****************************************************************************
** Form implementation generated from reading ui file 'katyreplace.ui'
**
** Created: Fri Feb 23 14:20:01 2001
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "katyreplace.h"

#include <klocale.h>
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a KatyReplace which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 */
KatyReplace::KatyReplace( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "KatyReplace" );
    resize( 278, 159 ); 
    setCaption( i18n( "Replace"  ) );
    vbox = new QVBoxLayout( this ); 
    vbox->setSpacing( 6 );
    vbox->setMargin( 11 );

    hbox = new QHBoxLayout; 
    hbox->setSpacing( 6 );
    hbox->setMargin( 0 );

    findTextLabel = new QLabel( this, "findTextLabel" );
    findTextLabel->setText( i18n( "Find:"  ) );
    hbox->addWidget( findTextLabel );

    findText = new QLineEdit( this, "findText" );
    hbox->addWidget( findText );
    vbox->addLayout( hbox );

    hbox_2 = new QHBoxLayout; 
    hbox_2->setSpacing( 6 );
    hbox_2->setMargin( 0 );

    replaceTextLabel = new QLabel( this, "replaceTextLabel" );
    replaceTextLabel->setText( i18n( "Replace:"  ) );
    hbox_2->addWidget( replaceTextLabel );

    replaceText = new QLineEdit( this, "replaceText" );
    hbox_2->addWidget( replaceText );
    vbox->addLayout( hbox_2 );

    hbox_3 = new QHBoxLayout; 
    hbox_3->setSpacing( 6 );
    hbox_3->setMargin( 0 );

    optionsGroup = new QGroupBox( this, "optionsGroup" );
    optionsGroup->setTitle( i18n( "Options"  ) );
    optionsGroup->setColumnLayout(0, Qt::Vertical );
    optionsGroup->layout()->setSpacing( 0 );
    optionsGroup->layout()->setMargin( 0 );
    vbox_2 = new QVBoxLayout( optionsGroup->layout() );
    vbox_2->setAlignment( Qt::AlignTop );
    vbox_2->setSpacing( 6 );
    vbox_2->setMargin( 11 );

    caseSensitive = new QCheckBox( optionsGroup, "caseSensitive" );
    caseSensitive->setText( i18n( "Case sensitive"  ) );
    vbox_2->addWidget( caseSensitive );

    regularExpression = new QCheckBox( optionsGroup, "regularExpression" );
    regularExpression->setText( i18n( "Regular expression"  ) );
    vbox_2->addWidget( regularExpression );
    hbox_3->addWidget( optionsGroup );

    directionGroup = new QButtonGroup( this, "directionGroup" );
    directionGroup->setTitle( i18n( "Direction"  ) );
    directionGroup->setColumnLayout(0, Qt::Vertical );
    directionGroup->layout()->setSpacing( 0 );
    directionGroup->layout()->setMargin( 0 );
    vbox_3 = new QVBoxLayout( directionGroup->layout() );
    vbox_3->setAlignment( Qt::AlignTop );
    vbox_3->setSpacing( 6 );
    vbox_3->setMargin( 11 );

    forward = new QRadioButton( directionGroup, "forward" );
    forward->setText( i18n( "Forward"  ) );
    vbox_3->addWidget( forward );

    backward = new QRadioButton( directionGroup, "backward" );
    backward->setText( i18n( "Backward"  ) );
    vbox_3->addWidget( backward );
    hbox_3->addWidget( directionGroup );
    vbox->addLayout( hbox_3 );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
KatyReplace::~KatyReplace()
{
    // no need to delete child widgets, Qt does it all for us
}

