/****************************************************************************
** Form interface generated from reading ui file 'katyreplace.ui'
**
** Created: Fri Feb 23 14:20:01 2001
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef KATYREPLACE_H
#define KATYREPLACE_H

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

class KatyReplace : public QWidget
{ 
    Q_OBJECT

public:
    KatyReplace( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~KatyReplace();

    QLabel* findTextLabel;
    QLineEdit* findText;
    QLabel* replaceTextLabel;
    QLineEdit* replaceText;
    QGroupBox* optionsGroup;
    QCheckBox* caseSensitive;
    QCheckBox* regularExpression;
    QButtonGroup* directionGroup;
    QRadioButton* forward;
    QRadioButton* backward;

protected:
    QHBoxLayout* hbox;
    QHBoxLayout* hbox_2;
    QHBoxLayout* hbox_3;
    QVBoxLayout* vbox;
    QVBoxLayout* vbox_2;
    QVBoxLayout* vbox_3;
};

#endif // KATYREPLACE_H
