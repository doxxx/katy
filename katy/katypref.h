#ifndef KATYPREF_H
#define KATYPREF_H

#include <kdialogbase.h>
#include <qframe.h>

class KatyPrefPageOne;
class KatyPrefPageTwo;

class KatyPreferences : public KDialogBase
{
    Q_OBJECT
public:
    KatyPreferences();

private:
    KatyPrefPageOne *m_pageOne;
    KatyPrefPageTwo *m_pageTwo;
};

class KatyPrefPageOne : public QFrame
{
    Q_OBJECT
public:
    KatyPrefPageOne(QWidget *parent = 0);
};

class KatyPrefPageTwo : public QFrame
{
    Q_OBJECT
public:
    KatyPrefPageTwo(QWidget *parent = 0);
};

#endif // KATYPREF_H
