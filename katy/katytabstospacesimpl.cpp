#include "katytabstospacesimpl.h"
#include "katyapp.h"

#include <klocale.h>

KatyTabsToSpacesImpl::KatyTabsToSpacesImpl(bool spacesToTabs, QWidget *parent)
        : KDialogBase(parent, NULL, TRUE, spacesToTabs ? i18n("Spaces to Tabs") : i18n("Tabs to Spaces"), Help | Ok | Cancel) {
    widget = new KatyTabsToSpaces(this);
    widget->numberOfSpaces->setValue(katyapp->readConfig_TabSize());

    setMainWidget(widget);
}

KatyTabsToSpacesImpl::~KatyTabsToSpacesImpl() {}

int KatyTabsToSpacesImpl::tabsToSpaces(QWidget *parent, int &numberOfSpaces, bool &leadingOnly) {
    KatyTabsToSpacesImpl dlg(FALSE, parent);
    int result = dlg.exec();
    if (result == QDialog::Accepted) {
        numberOfSpaces = dlg.numberOfSpaces();
        leadingOnly = dlg.isLeadingOnly();
    }

    return result;
}

int KatyTabsToSpacesImpl::spacesToTabs(QWidget *parent, int &numberOfSpaces, bool &leadingOnly) {
    KatyTabsToSpacesImpl dlg(TRUE, parent);
    int result = dlg.exec();
    if (result == QDialog::Accepted) {
        numberOfSpaces = dlg.numberOfSpaces();
        leadingOnly = dlg.isLeadingOnly();
    }

    return result;
}

#include "katytabstospacesimpl.moc"
