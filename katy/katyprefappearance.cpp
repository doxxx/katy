/****************************************************************************
** Form implementation generated from reading ui file 'katyprefappearance.ui'
**
** Created: Wed May 8 22:20:48 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "katyprefappearance.h"

#include <qvariant.h>
#include <kcolorbtn.h>
#include <klocale.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

static const char* const image0_data[] = { 
"22 22 2 1",
". c None",
"# c #a4c610",
"........######........",
".....###########......",
"....##############....",
"...################...",
"..######......######..",
"..#####........#####..",
".#####.......#..#####.",
".####.......###..####.",
"####.......#####..####",
"####......#####...####",
"####....#######...####",
"####....######....####",
"####...########...####",
".####.##########..####",
".####..####.#########.",
".#####..##...########.",
"..#####.......#######.",
"..######......######..",
"...###################",
"....##################",
"......###########.###.",
"........######.....#.."};


/* 
 *  Constructs a KatyPrefAppearancePage which is a child of 'parent', with the 
 *  name 'name'.' 
 */
KatyPrefAppearancePage::KatyPrefAppearancePage( QWidget* parent,  const char* name )
    : QFrame( parent, name )
{
    QPixmap image0( ( const char** ) image0_data );
    if ( !name )
	setName( "KatyPrefAppearancePage" );
    resize( 388, 125 ); 
    setFrameShadow( QFrame::Plain );
    setFrameShape( QFrame::NoFrame );
    setCaption( i18n( "Appearance" ) );
    setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, sizePolicy().hasHeightForWidth() ) );
    KatyPrefAppearancePageLayout = new QVBoxLayout( this, 11, 6, "KatyPrefAppearancePageLayout"); 

    Layout17 = new QHBoxLayout( 0, 0, 6, "Layout17"); 

    fontLabel = new QLabel( this, "fontLabel" );
    fontLabel->setText( i18n( "&Font:" ) );
    Layout17->addWidget( fontLabel );

    fontDescriptionLabel = new QLabel( this, "fontDescriptionLabel" );
    fontDescriptionLabel->setText( i18n( "" ) );
    fontDescriptionLabel->setMargin( 0 );
    fontDescriptionLabel->setMidLineWidth( -1 );
    fontDescriptionLabel->setLineWidth( -1 );
    fontDescriptionLabel->setFrameShape( QLabel::WinPanel );
    fontDescriptionLabel->setFrameShadow( QLabel::Sunken );
    fontDescriptionLabel->setMinimumSize( QSize( 0, 22 ) );
    fontDescriptionLabel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)1, 0, 0, fontDescriptionLabel->sizePolicy().hasHeightForWidth() ) );
    Layout17->addWidget( fontDescriptionLabel );

    fontDialogButton = new QPushButton( this, "fontDialogButton" );
    fontDialogButton->setText( i18n( "..." ) );
    fontDialogButton->setMinimumSize( QSize( 22, 22 ) );
    fontDialogButton->setMaximumSize( QSize( 22, 22 ) );
    fontDialogButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, fontDialogButton->sizePolicy().hasHeightForWidth() ) );
    Layout17->addWidget( fontDialogButton );
    KatyPrefAppearancePageLayout->addLayout( Layout17 );

    Layout14 = new QHBoxLayout( 0, 0, 6, "Layout14"); 

    normalTextBox = new QGroupBox( this, "normalTextBox" );
    normalTextBox->setTitle( i18n( "Normal Text Colour" ) );
    normalTextBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)1, 0, 0, normalTextBox->sizePolicy().hasHeightForWidth() ) );
    normalTextBox->setColumnLayout(0, Qt::Vertical );
    normalTextBox->layout()->setSpacing( 6 );
    normalTextBox->layout()->setMargin( 11 );
    normalTextBoxLayout = new QVBoxLayout( normalTextBox->layout() );
    normalTextBoxLayout->setAlignment( Qt::AlignTop );

    Layout14_2 = new QHBoxLayout( 0, 0, 6, "Layout14_2"); 

    normalFgLabel = new QLabel( normalTextBox, "normalFgLabel" );
    normalFgLabel->setText( i18n( "F&oreground:" ) );
    Layout14_2->addWidget( normalFgLabel );

    normalFgButton = new KColorButton( normalTextBox, "normalFgButton" );
    Layout14_2->addWidget( normalFgButton );
    normalTextBoxLayout->addLayout( Layout14_2 );

    Layout11 = new QHBoxLayout( 0, 0, 6, "Layout11"); 

    normalBgLabel = new QLabel( normalTextBox, "normalBgLabel" );
    normalBgLabel->setText( i18n( "Bac&kground:" ) );
    Layout11->addWidget( normalBgLabel );

    normalBgButton = new KColorButton( normalTextBox, "normalBgButton" );
    Layout11->addWidget( normalBgButton );
    normalTextBoxLayout->addLayout( Layout11 );
    Layout14->addWidget( normalTextBox );

    selectedTextBox = new QGroupBox( this, "selectedTextBox" );
    selectedTextBox->setTitle( i18n( "Selected Text Colour" ) );
    selectedTextBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)1, 0, 0, selectedTextBox->sizePolicy().hasHeightForWidth() ) );
    selectedTextBox->setColumnLayout(0, Qt::Vertical );
    selectedTextBox->layout()->setSpacing( 6 );
    selectedTextBox->layout()->setMargin( 11 );
    selectedTextBoxLayout = new QVBoxLayout( selectedTextBox->layout() );
    selectedTextBoxLayout->setAlignment( Qt::AlignTop );

    Layout12 = new QHBoxLayout( 0, 0, 6, "Layout12"); 

    selectedFgLabel = new QLabel( selectedTextBox, "selectedFgLabel" );
    selectedFgLabel->setText( i18n( "Fo&reground:" ) );
    Layout12->addWidget( selectedFgLabel );

    selectedFgButton = new KColorButton( selectedTextBox, "selectedFgButton" );
    Layout12->addWidget( selectedFgButton );
    selectedTextBoxLayout->addLayout( Layout12 );

    Layout13 = new QHBoxLayout( 0, 0, 6, "Layout13"); 

    selectedBgLabel = new QLabel( selectedTextBox, "selectedBgLabel" );
    selectedBgLabel->setText( i18n( "Back&ground:" ) );
    Layout13->addWidget( selectedBgLabel );

    selectedBgButton = new KColorButton( selectedTextBox, "selectedBgButton" );
    Layout13->addWidget( selectedBgButton );
    selectedTextBoxLayout->addLayout( Layout13 );
    Layout14->addWidget( selectedTextBox );
    KatyPrefAppearancePageLayout->addLayout( Layout14 );

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

