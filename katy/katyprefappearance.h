/****************************************************************************
** Form interface generated from reading ui file 'katyprefappearance.ui'
**
** Created: Wed May 8 22:20:48 2002
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

class KatyPrefAppearancePage : public QFrame {
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
    QVBoxLayout* KatyPrefAppearancePageLayout;
    QHBoxLayout* Layout17;
    QHBoxLayout* Layout14;
    QVBoxLayout* normalTextBoxLayout;
    QHBoxLayout* Layout14_2;
    QHBoxLayout* Layout11;
    QVBoxLayout* selectedTextBoxLayout;
    QHBoxLayout* Layout12;
    QHBoxLayout* Layout13;
};

#endif // KATYPREFAPPEARANCEPAGE_H
