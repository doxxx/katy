#include "katyreplaceimpl.h"
#include "katyapp.h"

#include <klocale.h>

KatyReplaceImpl::KatyReplaceImpl(QWidget *parent)
        : KDialogBase(parent, NULL, TRUE, i18n("Replace"), Help | Ok | Cancel) {
    widget = new KatyReplace(this);
    setMainWidget(widget);

    widget->findText->setFocus();
}

KatyReplaceImpl::~KatyReplaceImpl() {}

int KatyReplaceImpl::show(QWidget *parent, QString &findText, QString &replaceText, bool &backward, bool &caseSensitive, bool &regularExpression) {
    KatyReplaceImpl dlg(parent);

    dlg.setFindText(findText);
    dlg.setReplaceText(replaceText);
    dlg.setBackward(backward);
    dlg.setCaseSensitive(caseSensitive);
    dlg.setRegularExpression(regularExpression);

    int result = dlg.exec();
    if (result == QDialog::Accepted) {
        findText = dlg.findText();
        replaceText = dlg.replaceText();
        backward = dlg.backward();
        caseSensitive = dlg.caseSensitive();
        regularExpression = dlg.regularExpression();
    }

    return result;
}
#include "katyreplaceimpl.moc"
