/****************************************************************************
** Form implementation generated from reading ui file 'katyreplacing.ui'
**
** Created: Fri Feb 23 18:42:40 2001
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "katyreplacing.h"

#include <klocale.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a KatyReplacing which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
KatyReplacing::KatyReplacing( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "KatyReplacing" );
    resize( 379, 52 ); 
    setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, sizePolicy().hasHeightForWidth() ) );
    setCaption( i18n( "Replacing"  ) );
    vbox = new QVBoxLayout( this ); 
    vbox->setSpacing( 6 );
    vbox->setMargin( 11 );

    hbox = new QHBoxLayout; 
    hbox->setSpacing( 6 );
    hbox->setMargin( 0 );

    replace = new QPushButton( this, "replace" );
    replace->setText( i18n( "&Replace"  ) );
    hbox->addWidget( replace );

    replaceAll = new QPushButton( this, "replaceAll" );
    replaceAll->setText( i18n( "Replace &All"  ) );
    hbox->addWidget( replaceAll );

    skip = new QPushButton( this, "skip" );
    skip->setText( i18n( "&Skip"  ) );
    hbox->addWidget( skip );

    cancel = new QPushButton( this, "cancel" );
    cancel->setText( i18n( "&Cancel"  ) );
    hbox->addWidget( cancel );
    vbox->addLayout( hbox );

    // signals and slots connections
    connect( replace, SIGNAL( clicked() ), this, SLOT( slotReplace() ) );
    connect( replaceAll, SIGNAL( clicked() ), this, SLOT( slotReplaceAll() ) );
    connect( skip, SIGNAL( clicked() ), this, SLOT( slotSkip() ) );
    connect( cancel, SIGNAL( clicked() ), this, SLOT( slotCancel() ) );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
KatyReplacing::~KatyReplacing()
{
    // no need to delete child widgets, Qt does it all for us
}

void KatyReplacing::slotCancel()
{
    qWarning( "KatyReplacing::slotCancel(): Not implemented yet!" );
}

void KatyReplacing::slotReplace()
{
    qWarning( "KatyReplacing::slotReplace(): Not implemented yet!" );
}

void KatyReplacing::slotReplaceAll()
{
    qWarning( "KatyReplacing::slotReplaceAll(): Not implemented yet!" );
}

void KatyReplacing::slotSkip()
{
    qWarning( "KatyReplacing::slotSkip(): Not implemented yet!" );
}

