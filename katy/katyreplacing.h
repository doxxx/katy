/****************************************************************************
** Form interface generated from reading ui file 'katyreplacing.ui'
**
** Created: Wed May 8 22:20:48 2002
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
    QVBoxLayout* KatyReplacingLayout;
    QHBoxLayout* Layout4;
};

#endif // KATYREPLACING_H
