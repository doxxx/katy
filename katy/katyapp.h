/*
 * katyapp.h
 *
 * Copyright (C) 2000 Gordon Tyler <gtyler@iafrica.com>
 */

#ifndef KATYAPP_H
#define KATYAPP_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <kuniqueapp.h>

#include <qlist.h>

#include "katy.h"

/**
 * This class serves as the main application class for Katy.
 *
 * @short Main application class
 * @author Gordon Tyler <gtyler@iafrica.com>
 * @version 0.3
 */
class KatyApp : public KUniqueApplication
{
public:
    KatyApp();
    ~KatyApp();

    int newInstance();

    Katy *newWindow();
    void removeWindow(Katy *window);

private:
    bool alreadyRestored;
    QList<Katy> windowList;
};

#endif
