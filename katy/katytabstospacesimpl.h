#ifndef KATYTABSTOSPACESIMPL_H
#define KATYTABSTOSPACESIMPL_H
#include "katytabstospaces.h"

#include <kdialogbase.h>
#include <qspinbox.h>
#include <qcheckbox.h>

class KatyTabsToSpacesImpl : public KDialogBase {
    Q_OBJECT

public:
    KatyTabsToSpacesImpl(bool spacesToTabs = FALSE, QWidget *parent = 0);
    ~KatyTabsToSpacesImpl();

    int numberOfSpaces() {
        return widget->numberOfSpaces->value();
    }

    bool isLeadingOnly() {
        return widget->leadingOnly->isChecked();
    }

    static int tabsToSpaces(QWidget *parent, int &numberOfSpaces, bool &leadingOnly);
    static int spacesToTabs(QWidget *parent, int &numberOfSpaces, bool &leadingOnly);

private:
    KatyTabsToSpaces *widget;
};

#endif // KATYTABSTOSPACESIMPL_H
