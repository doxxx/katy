/****************************************************************************
** Form interface generated from reading ui file 'katyprefappearance.ui'
**
** Created: Thu Feb 8 00:36:01 2001
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef KATYPREFAPPEARANCEPAGE_H
#define KATYPREFAPPEARANCEPAGE_H

#include <qvariant.h>
#include <qframe.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class KColorButton;
class QGroupBox;
class QLabel;
class QPushButton;

class KatyPrefAppearancePage : public QFrame
{ 
    Q_OBJECT

public:
    KatyPrefAppearancePage( QWidget* parent = 0, const char* name = 0 );
    ~KatyPrefAppearancePage();

    QLabel* fontLabel;
    QLabel* fontDescriptionLabel;
    QPushButton* fontDialogButton;
    QGroupBox* normalTextBox;
    QLabel* normalFgLabel;
    KColorButton* normalFgButton;
    QLabel* normalBgLabel;
    KColorButton* normalBgButton;
    QGroupBox* selectedTextBox;
    QLabel* selectedFgLabel;
    KColorButton* selectedFgButton;
    QLabel* selectedBgLabel;
    KColorButton* selectedBgButton;

protected:
    QHBoxLayout* hbox;
    QHBoxLayout* hbox_2;
    QHBoxLayout* hbox_3;
    QHBoxLayout* hbox_4;
    QHBoxLayout* hbox_5;
    QHBoxLayout* hbox_6;
    QVBoxLayout* vbox;
    QVBoxLayout* vbox_2;
    QVBoxLayout* vbox_3;
};

#endif // KATYPREFAPPEARANCEPAGE_H
