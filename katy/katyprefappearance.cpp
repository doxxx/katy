/****************************************************************************
** Form implementation generated from reading ui file 'katyprefappearance.ui'
**
** Created: Thu Feb 8 00:36:01 2001
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "katyprefappearance.h"

#include <kcolorbtn.h>
#include <klocale.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a KatyPrefAppearancePage which is a child of 'parent', with the 
 *  name 'name'.' 
 */
KatyPrefAppearancePage::KatyPrefAppearancePage( QWidget* parent,  const char* name )
    : QFrame( parent, name )
{
    if ( !name )
	setName( "KatyPrefAppearancePage" );
    resize( 388, 125 ); 
    setFrameShadow( QFrame::Plain );
    setFrameShape( QFrame::NoFrame );
    setCaption( i18n( "Appearance"  ) );
    setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, sizePolicy().hasHeightForWidth() ) );
    vbox = new QVBoxLayout( this ); 
    vbox->setSpacing( 6 );
    vbox->setMargin( 11 );

    hbox = new QHBoxLayout; 
    hbox->setSpacing( 6 );
    hbox->setMargin( 0 );

    fontLabel = new QLabel( this, "fontLabel" );
    fontLabel->setText( i18n( "&Font:"  ) );
    hbox->addWidget( fontLabel );

    fontDescriptionLabel = new QLabel( this, "fontDescriptionLabel" );
    fontDescriptionLabel->setText( i18n( ""  ) );
    fontDescriptionLabel->setMargin( 0 );
    fontDescriptionLabel->setMidLineWidth( -1 );
    fontDescriptionLabel->setLineWidth( -1 );
    fontDescriptionLabel->setFrameShape( QLabel::WinPanel );
    fontDescriptionLabel->setFrameShadow( QLabel::Sunken );
    fontDescriptionLabel->setMinimumSize( QSize( 0, 22 ) );
    fontDescriptionLabel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)1, fontDescriptionLabel->sizePolicy().hasHeightForWidth() ) );
    hbox->addWidget( fontDescriptionLabel );

    fontDialogButton = new QPushButton( this, "fontDialogButton" );
    fontDialogButton->setText( i18n( "..."  ) );
    fontDialogButton->setMinimumSize( QSize( 22, 22 ) );
    fontDialogButton->setMaximumSize( QSize( 22, 22 ) );
    fontDialogButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, fontDialogButton->sizePolicy().hasHeightForWidth() ) );
    hbox->addWidget( fontDialogButton );
    vbox->addLayout( hbox );

    hbox_2 = new QHBoxLayout; 
    hbox_2->setSpacing( 6 );
    hbox_2->setMargin( 0 );

    normalTextBox = new QGroupBox( this, "normalTextBox" );
    normalTextBox->setTitle( i18n( "Normal Text Colour"  ) );
    normalTextBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)1, normalTextBox->sizePolicy().hasHeightForWidth() ) );
    normalTextBox->setColumnLayout(0, Qt::Vertical );
    normalTextBox->layout()->setSpacing( 0 );
    normalTextBox->layout()->setMargin( 0 );
    vbox_2 = new QVBoxLayout( normalTextBox->layout() );
    vbox_2->setAlignment( Qt::AlignTop );
    vbox_2->setSpacing( 6 );
    vbox_2->setMargin( 11 );

    hbox_3 = new QHBoxLayout; 
    hbox_3->setSpacing( 6 );
    hbox_3->setMargin( 0 );

    normalFgLabel = new QLabel( normalTextBox, "normalFgLabel" );
    normalFgLabel->setText( i18n( "F&oreground:"  ) );
    hbox_3->addWidget( normalFgLabel );

    normalFgButton = new KColorButton( normalTextBox, "normalFgButton" );
    hbox_3->addWidget( normalFgButton );
    vbox_2->addLayout( hbox_3 );

    hbox_4 = new QHBoxLayout; 
    hbox_4->setSpacing( 6 );
    hbox_4->setMargin( 0 );

    normalBgLabel = new QLabel( normalTextBox, "normalBgLabel" );
    normalBgLabel->setText( i18n( "Bac&kground:"  ) );
    hbox_4->addWidget( normalBgLabel );

    normalBgButton = new KColorButton( normalTextBox, "normalBgButton" );
    hbox_4->addWidget( normalBgButton );
    vbox_2->addLayout( hbox_4 );
    hbox_2->addWidget( normalTextBox );

    selectedTextBox = new QGroupBox( this, "selectedTextBox" );
    selectedTextBox->setTitle( i18n( "Selected Text Colour"  ) );
    selectedTextBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)1, selectedTextBox->sizePolicy().hasHeightForWidth() ) );
    selectedTextBox->setColumnLayout(0, Qt::Vertical );
    selectedTextBox->layout()->setSpacing( 0 );
    selectedTextBox->layout()->setMargin( 0 );
    vbox_3 = new QVBoxLayout( selectedTextBox->layout() );
    vbox_3->setAlignment( Qt::AlignTop );
    vbox_3->setSpacing( 6 );
    vbox_3->setMargin( 11 );

    hbox_5 = new QHBoxLayout; 
    hbox_5->setSpacing( 6 );
    hbox_5->setMargin( 0 );

    selectedFgLabel = new QLabel( selectedTextBox, "selectedFgLabel" );
    selectedFgLabel->setText( i18n( "Fo&reground:"  ) );
    hbox_5->addWidget( selectedFgLabel );

    selectedFgButton = new KColorButton( selectedTextBox, "selectedFgButton" );
    hbox_5->addWidget( selectedFgButton );
    vbox_3->addLayout( hbox_5 );

    hbox_6 = new QHBoxLayout; 
    hbox_6->setSpacing( 6 );
    hbox_6->setMargin( 0 );

    selectedBgLabel = new QLabel( selectedTextBox, "selectedBgLabel" );
    selectedBgLabel->setText( i18n( "Back&ground:"  ) );
    hbox_6->addWidget( selectedBgLabel );

    selectedBgButton = new KColorButton( selectedTextBox, "selectedBgButton" );
    hbox_6->addWidget( selectedBgButton );
    vbox_3->addLayout( hbox_6 );
    hbox_2->addWidget( selectedTextBox );
    vbox->addLayout( hbox_2 );

    // buddies
    fontLabel->setBuddy( fontDialogButton );
    normalFgLabel->setBuddy( normalFgButton );
    normalBgLabel->setBuddy( normalBgButton );
    selectedFgLabel->setBuddy( selectedFgButton );
    selectedBgLabel->setBuddy( selectedBgButton );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
KatyPrefAppearancePage::~KatyPrefAppearancePage()
{
    // no need to delete child widgets, Qt does it all for us
}

