/****************************************************************************
** Form interface generated from reading ui file 'katyprefindenting.ui'
**
** Created: Thu Feb 8 00:36:01 2001
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef KATYPREFINDENTINGPAGE_H
#define KATYPREFINDENTINGPAGE_H

#include <qvariant.h>
#include <qframe.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QCheckBox;
class QLabel;
class QSpinBox;

class KatyPrefIndentingPage : public QFrame
{ 
    Q_OBJECT

public:
    KatyPrefIndentingPage( QWidget* parent = 0, const char* name = 0 );
    ~KatyPrefIndentingPage();

    QLabel* tabSizeLabel;
    QSpinBox* tabSize;
    QCheckBox* useSpaces;
    QLabel* indentSizeLabel;
    QSpinBox* indentSize;

protected:
    QHBoxLayout* hbox;
    QHBoxLayout* hbox_2;
    QVBoxLayout* vbox;
};

#endif // KATYPREFINDENTINGPAGE_H
