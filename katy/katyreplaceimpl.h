#ifndef KATYREPLACEIMPL_H
#define KATYREPLACEIMPL_H
#include "katyreplace.h"

#include <kdialogbase.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qradiobutton.h>

class KatyReplaceImpl : public KDialogBase
{
    Q_OBJECT

public:
    KatyReplaceImpl(QWidget *parent = 0);
    ~KatyReplaceImpl();

    void setFindText(QString findText)
    {
        widget->findText->setText(findText);
        widget->findText->setSelection(0, findText.length());
    }

    QString findText()
    {
        return widget->findText->text();
    }

    void setReplaceText(QString replaceText)
    {
        widget->replaceText->setText(replaceText);
        widget->replaceText->setSelection(0, replaceText.length());
    }

    QString replaceText()
    {
        return widget->replaceText->text();
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

    static int show(QWidget *parent, QString &findText, QString &replaceText, bool &backward, bool &caseSensitive, bool &regularExpression);

private:
    KatyReplace *widget;
};

#endif // KATYREPLACEIMPL_H
