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

