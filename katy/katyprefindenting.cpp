/****************************************************************************
** Form implementation generated from reading ui file 'katyprefindenting.ui'
**
** Created: Wed May 8 22:20:48 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "katyprefindenting.h"

#include <qvariant.h>
#include <klocale.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a KatyPrefIndentingPage which is a child of 'parent', with the 
 *  name 'name'.' 
 */
KatyPrefIndentingPage::KatyPrefIndentingPage( QWidget* parent,  const char* name )
        : QFrame( parent, name ) {
    if ( !name )
        setName( "KatyPrefIndentingPage" );
    resize( 332, 104 );
    setFrameShadow( QFrame::Plain );
    setFrameShape( QFrame::NoFrame );
    setCaption( i18n( "Tabs and Indenting" ) );
    KatyPrefIndentingPageLayout = new QVBoxLayout( this, 11, 6, "KatyPrefIndentingPageLayout");

    Layout5 = new QHBoxLayout( 0, 0, 6, "Layout5");

    tabSizeLabel = new QLabel( this, "tabSizeLabel" );
    tabSizeLabel->setText( i18n( "Tab Character Size:" ) );
    Layout5->addWidget( tabSizeLabel );

    tabSize = new QSpinBox( this, "tabSize" );
    Layout5->addWidget( tabSize );
    KatyPrefIndentingPageLayout->addLayout( Layout5 );

    useSpaces = new QCheckBox( this, "useSpaces" );
    useSpaces->setText( i18n( "Tab key inserts spaces instead of tab characters" ) );
    KatyPrefIndentingPageLayout->addWidget( useSpaces );

    Layout6 = new QHBoxLayout( 0, 0, 6, "Layout6");

    indentSizeLabel = new QLabel( this, "indentSizeLabel" );
    indentSizeLabel->setText( i18n( "Number Of Spaces:" ) );
    indentSizeLabel->setEnabled( FALSE );
    Layout6->addWidget( indentSizeLabel );

    indentSize = new QSpinBox( this, "indentSize" );
    indentSize->setEnabled( FALSE );
    Layout6->addWidget( indentSize );
    KatyPrefIndentingPageLayout->addLayout( Layout6 );

    // signals and slots connections
    connect( useSpaces, SIGNAL( toggled(bool) ), indentSize, SLOT( setEnabled(bool) ) );
    connect( useSpaces, SIGNAL( toggled(bool) ), indentSizeLabel, SLOT( setEnabled(bool) ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
KatyPrefIndentingPage::~KatyPrefIndentingPage() {
    // no need to delete child widgets, Qt does it all for us
}

