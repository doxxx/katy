#ifndef KATYIFACE_H
#define KATYIFACE_H

#include <dcopobject.h>

class KatyIface : virtual public DCOPObject {
    K_DCOP
public:

k_dcop:
    virtual void openURL(QString url) = 0;
};

#endif // KATYIFACE_H
