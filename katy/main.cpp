#include "katyapp.h"
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>

static const char *description = I18N_NOOP("A KDE Application");

static const char *version = "v0.3";

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
