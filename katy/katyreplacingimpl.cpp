#include "katyreplacing.h"

#include "katyreplacingimpl.h"

/*
 *  Constructs a KatyReplacingImpl which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
KatyReplacingImpl::KatyReplacingImpl( QWidget* parent,  const char* name, bool modal, WFlags fl )
        : KatyReplacing( parent, name, modal, fl ) {}

/*
 *  Destroys the object and frees any allocated resources
 */
KatyReplacingImpl::~KatyReplacingImpl() {
    // no need to delete child widgets, Qt does it all for us
}

/*
 * protected slot
 */
void KatyReplacingImpl::slotCancel() {
    reject();
}
/*
 * protected slot
 */
void KatyReplacingImpl::slotReplace() {
    done(Replace);
}
/*
 * protected slot
 */
void KatyReplacingImpl::slotReplaceAll() {
    done(ReplaceAll);
}
/*
 * protected slot
 */
void KatyReplacingImpl::slotSkip() {
    done(Skip);
}

int KatyReplacingImpl::ask(QWidget *parent) {
    KatyReplacingImpl dlg(parent);

    return dlg.exec();
}

