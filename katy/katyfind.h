/****************************************************************************
** Form interface generated from reading ui file 'katyfind.ui'
**
** Created: Wed May 8 22:20:47 2002
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

class KatyFind : public QWidget {
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
    QVBoxLayout* KatyFindLayout;
    QHBoxLayout* Layout2;
    QHBoxLayout* Layout1;
    QVBoxLayout* optionsGroupLayout;
    QVBoxLayout* directionGroupLayout;
};

#endif // KATYFIND_H
