/****************************************************************************
** Form interface generated from reading ui file 'katyfind.ui'
**
** Created: Fri Feb 23 14:20:00 2001
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef KATYFIND_H
#define KATYFIND_H

#include <qvariant.h>
#include <qwidget.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QButtonGroup;
class QCheckBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QRadioButton;

class KatyFind : public QWidget
{ 
    Q_OBJECT

public:
    KatyFind( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~KatyFind();

    QLabel* findTextLabel;
    QLineEdit* findText;
    QGroupBox* optionsGroup;
    QCheckBox* caseSensitive;
    QCheckBox* regularExpression;
    QButtonGroup* directionGroup;
    QRadioButton* forward;
    QRadioButton* backward;

protected:
    QHBoxLayout* hbox;
    QHBoxLayout* hbox_2;
    QVBoxLayout* vbox;
    QVBoxLayout* vbox_2;
    QVBoxLayout* vbox_3;
};

#endif // KATYFIND_H