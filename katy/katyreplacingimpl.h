#ifndef KATYREPLACINGIMPL_H
#define KATYREPLACINGIMPL_H
#include "katyreplacing.h"

class KatyReplacingImpl : public KatyReplacing {
    Q_OBJECT

public:
    KatyReplacingImpl( QWidget* parent = 0, const char* name = 0, bool modal = TRUE, WFlags fl = 0 );
    ~KatyReplacingImpl();

    enum DialogCode2 {
        Replace = Accepted,
        ReplaceAll,
        Skip
    };

    static int ask(QWidget *parent);

protected slots:
    void slotCancel();
    void slotReplace();
    void slotReplaceAll();
    void slotSkip();

};

#endif // KATYREPLACINGIMPL_H
