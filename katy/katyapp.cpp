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

#include "katyapp.h"

#include <kcmdlineargs.h>

KatyApp::KatyApp()
    : alreadyRestored(false)
{
}

KatyApp::~KatyApp()
{
}

int KatyApp::newInstance()
{
    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

    if (isRestored() && !alreadyRestored)
    {
        alreadyRestored = true;
        RESTORE(Katy)
    }
    else
    {
        if (args->count() > 0)
        {
            for (int i = 0; i < args->count(); i++) // Counting start at 0!
            {
                Katy *window = newWindow();
                window->load(args->url(i));
                window->show();
            }
        }
        else
        {
            Katy *window = newWindow();
            window->show();
        }
    }

    return 0;
}

Katy *KatyApp::newWindow()
{
    Katy *window = new Katy();
    windowList.append(window);
    return window;
}

void KatyApp::removeWindow(Katy *window)
{
    windowList.removeRef(window);
}

