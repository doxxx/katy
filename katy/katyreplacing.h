/****************************************************************************
** Form interface generated from reading ui file 'katyreplacing.ui'
**
** Created: Fri Feb 23 18:42:40 2001
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef KATYREPLACING_H
#define KATYREPLACING_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QPushButton;

class KatyReplacing : public QDialog
{ 
    Q_OBJECT

public:
    KatyReplacing( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~KatyReplacing();

    QPushButton* replace;
    QPushButton* replaceAll;
    QPushButton* skip;
    QPushButton* cancel;

protected slots:
    virtual void slotCancel();
    virtual void slotReplace();
    virtual void slotReplaceAll();
    virtual void slotSkip();

protected:
    QHBoxLayout* hbox;
    QVBoxLayout* vbox;
};

#endif // KATYREPLACING_H
