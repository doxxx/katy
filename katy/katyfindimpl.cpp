#include "katyfindimpl.h"
#include "katyapp.h"

#include <klocale.h>

KatyFindImpl::KatyFindImpl(QWidget *parent)
        : KDialogBase(parent, NULL, TRUE, i18n("Find"), Help | Ok | Cancel) {
    widget = new KatyFind(this);
    setMainWidget(widget);

    widget->findText->setFocus();
}

KatyFindImpl::~KatyFindImpl() {}

int KatyFindImpl::show(QWidget *parent, QString &findText, bool &backward, bool &caseSensitive, bool &regularExpression) {
    KatyFindImpl dlg(parent);

    dlg.setFindText(findText);
    dlg.setBackward(backward);
    dlg.setCaseSensitive(caseSensitive);
    dlg.setRegularExpression(regularExpression);

    int result = dlg.exec();
    if (result == QDialog::Accepted) {
        findText = dlg.findText();
        backward = dlg.backward();
        caseSensitive = dlg.caseSensitive();
        regularExpression = dlg.regularExpression();
    }

    return result;
}
