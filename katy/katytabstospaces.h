/****************************************************************************
** Form interface generated from reading ui file 'katytabstospaces.ui'
**
** Created: Thu Feb 8 00:51:52 2001
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef KATYTABSTOSPACES_H
#define KATYTABSTOSPACES_H

#include <qvariant.h>
#include <qwidget.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QCheckBox;
class QLabel;
class QSpinBox;

class KatyTabsToSpaces : public QWidget
{ 
    Q_OBJECT

public:
    KatyTabsToSpaces( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~KatyTabsToSpaces();

    QCheckBox* leadingOnly;
    QLabel* numberOfSpacesLabel;
    QSpinBox* numberOfSpaces;

protected:
    QHBoxLayout* hbox;
    QVBoxLayout* vbox;
};

#endif // KATYTABSTOSPACES_H
