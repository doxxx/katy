/****************************************************************************
** Form implementation generated from reading ui file 'katyreplacing.ui'
**
** Created: Wed May 8 22:20:48 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "katyreplacing.h"

#include <qvariant.h>
#include <klocale.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a KatyReplacing which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
KatyReplacing::KatyReplacing( QWidget* parent,  const char* name, bool modal, WFlags fl )
        : QDialog( parent, name, modal, fl ) {
    if ( !name )
        setName( "KatyReplacing" );
    resize( 379, 52 );
    setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, sizePolicy().hasHeightForWidth() ) );
    setCaption( i18n( "Replacing" ) );
    KatyReplacingLayout = new QVBoxLayout( this, 11, 6, "KatyReplacingLayout");

    Layout4 = new QHBoxLayout( 0, 0, 6, "Layout4");

    replace = new QPushButton( this, "replace" );
    replace->setText( i18n( "&Replace" ) );
    Layout4->addWidget( replace );

    replaceAll = new QPushButton( this, "replaceAll" );
    replaceAll->setText( i18n( "Replace &All" ) );
    Layout4->addWidget( replaceAll );

    skip = new QPushButton( this, "skip" );
    skip->setText( i18n( "&Skip" ) );
    Layout4->addWidget( skip );

    cancel = new QPushButton( this, "cancel" );
    cancel->setText( i18n( "&Cancel" ) );
    Layout4->addWidget( cancel );
    KatyReplacingLayout->addLayout( Layout4 );

    // signals and slots connections
    connect( replace, SIGNAL( clicked() ), this, SLOT( slotReplace() ) );
    connect( replaceAll, SIGNAL( clicked() ), this, SLOT( slotReplaceAll() ) );
    connect( skip, SIGNAL( clicked() ), this, SLOT( slotSkip() ) );
    connect( cancel, SIGNAL( clicked() ), this, SLOT( slotCancel() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
KatyReplacing::~KatyReplacing() {
    // no need to delete child widgets, Qt does it all for us
}

void KatyReplacing::slotCancel() {
    qWarning( "KatyReplacing::slotCancel(): Not implemented yet!" );
}

void KatyReplacing::slotReplace() {
    qWarning( "KatyReplacing::slotReplace(): Not implemented yet!" );
}

void KatyReplacing::slotReplaceAll() {
    qWarning( "KatyReplacing::slotReplaceAll(): Not implemented yet!" );
}

void KatyReplacing::slotSkip() {
    qWarning( "KatyReplacing::slotSkip(): Not implemented yet!" );
}

