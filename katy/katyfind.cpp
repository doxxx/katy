/****************************************************************************
** Form implementation generated from reading ui file 'katyfind.ui'
**
** Created: Wed May 8 22:20:47 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "katyfind.h"

#include <qvariant.h>
#include <klocale.h>
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a KatyFind which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 */
KatyFind::KatyFind( QWidget* parent, const char* name, WFlags fl )
        : QWidget( parent, name, fl ) {
    if ( !name )
        setName( "KatyFind" );
    resize( 278, 129 );
    setCaption( i18n( "Find" ) );
    KatyFindLayout = new QVBoxLayout( this, 11, 6, "KatyFindLayout");

    Layout2 = new QHBoxLayout( 0, 0, 6, "Layout2");

    findTextLabel = new QLabel( this, "findTextLabel" );
    findTextLabel->setText( i18n( "Find:" ) );
    Layout2->addWidget( findTextLabel );

    findText = new QLineEdit( this, "findText" );
    Layout2->addWidget( findText );
    KatyFindLayout->addLayout( Layout2 );

    Layout1 = new QHBoxLayout( 0, 0, 6, "Layout1");

    optionsGroup = new QGroupBox( this, "optionsGroup" );
    optionsGroup->setTitle( i18n( "Options" ) );
    optionsGroup->setColumnLayout(0, Qt::Vertical );
    optionsGroup->layout()->setSpacing( 6 );
    optionsGroup->layout()->setMargin( 11 );
    optionsGroupLayout = new QVBoxLayout( optionsGroup->layout() );
    optionsGroupLayout->setAlignment( Qt::AlignTop );

    caseSensitive = new QCheckBox( optionsGroup, "caseSensitive" );
    caseSensitive->setText( i18n( "Case sensitive" ) );
    optionsGroupLayout->addWidget( caseSensitive );

    regularExpression = new QCheckBox( optionsGroup, "regularExpression" );
    regularExpression->setText( i18n( "Regular expression" ) );
    optionsGroupLayout->addWidget( regularExpression );
    Layout1->addWidget( optionsGroup );

    directionGroup = new QButtonGroup( this, "directionGroup" );
    directionGroup->setTitle( i18n( "Direction" ) );
    directionGroup->setColumnLayout(0, Qt::Vertical );
    directionGroup->layout()->setSpacing( 6 );
    directionGroup->layout()->setMargin( 11 );
    directionGroupLayout = new QVBoxLayout( directionGroup->layout() );
    directionGroupLayout->setAlignment( Qt::AlignTop );

    forward = new QRadioButton( directionGroup, "forward" );
    forward->setText( i18n( "Forward" ) );
    directionGroupLayout->addWidget( forward );

    backward = new QRadioButton( directionGroup, "backward" );
    backward->setText( i18n( "Backward" ) );
    directionGroupLayout->addWidget( backward );
    Layout1->addWidget( directionGroup );
    KatyFindLayout->addLayout( Layout1 );
}

/*
 *  Destroys the object and frees any allocated resources
 */
KatyFind::~KatyFind() {
    // no need to delete child widgets, Qt does it all for us
}

