/*
 * Startup routines
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
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>

static const char *description = I18N_NOOP("A KDE Text Editor that provides similar features to UltraEdit(tm).");

static const char *version = "v0.3.1";

static KCmdLineOptions options[] =
{
    { "+[file]", I18N_NOOP("A filename or URL."), 0 },
    KCmdLineLastOption
};

int main(int argc, char **argv)
{
    KAboutData about("katy", I18N_NOOP("Katy"), version, description, KAboutData::License_GPL, "(C) 2000 Gordon Tyler");
    about.addAuthor( "Gordon Tyler", 0, "gordon@doxxx.net" );
    KCmdLineArgs::init(argc, argv, &about);
    KCmdLineArgs::addCmdLineOptions( options );
    KUniqueApplication::addCmdLineOptions();

    if (!KUniqueApplication::start())
        return 0;

    KatyApp app;
    return app.exec();
}
