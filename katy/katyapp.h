/*
 * Class for main Katy application
 * Copyright (c) by Gordon Tyler <gordon@doxxx.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
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
