#ifndef KATYFINDIMPL_H
#define KATYFINDIMPL_H
#include "katyfind.h"

#include <kdialogbase.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qradiobutton.h>

class KatyFindImpl : public KDialogBase
{
    Q_OBJECT

public:
    KatyFindImpl(QWidget *parent = 0);
    ~KatyFindImpl();

    void setFindText(QString findText)
    {
        widget->findText->setText(findText);
        widget->findText->setSelection(0, findText.length());
    }

    QString findText()
    {
        return widget->findText->text();
    }

    void setBackward(bool backward)
    {
        if (backward)
            widget->backward->setChecked(TRUE);
        else
            widget->forward->setChecked(TRUE);
    }

    bool backward()
    {
        return widget->backward->isChecked();
    }

    void setCaseSensitive(bool caseSensitive)
    {
        widget->caseSensitive->setChecked(caseSensitive);
    }

    bool caseSensitive()
    {
        return widget->caseSensitive->isChecked();
    }

    bool regularExpression()
    {
        return widget->regularExpression->isChecked();
    }

    void setRegularExpression(bool regularExpression)
    {
        widget->regularExpression->setChecked(regularExpression);
    }

    static int show(QWidget *parent, QString &findText, bool &backward, bool &caseSensitive, bool &regularExpression);

private:
    KatyFind *widget;
};

#endif // KATYFINDIMPL_H
