/*
 * katy.h
 *
 * Copyright (C) 2000 Gordon Tyler <gtyler@iafrica.com>
 */

#ifndef KATY_H
#define KATY_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <kapp.h>
#include <kmainwindow.h>

#include "katyview.h"
#include "katyiface.h"

class QPrinter;
class KToggleAction;
class KSelectAction;
class KRecentFilesAction;
class KURL;

/**
 * This class serves as the main window for Katy.  It handles the
 * menus, toolbars, and status bars.
 *
 * @short Main window class
 * @author Gordon Tyler <gtyler@iafrica.com>
 * @version 0.3
 */
class Katy : public KMainWindow
{
    Q_OBJECT
public:
    /**
     * Default Constructor
     */
    Katy();

    /**
     * Default Destructor
     */
    virtual ~Katy();

    /**
     * Use this method to load whatever file/URL you have
     */
    void load(const QString& url);
    void load(const KURL& url);

protected:
    /**
     * Overridden virtuals for Qt drag 'n drop (XDND)
     */
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dropEvent(QDropEvent *event);

protected:
    bool queryExit();

    /**
     * This function is called when it is time for the app to save its
     * properties for session management purposes.
     */
    void saveProperties(KConfig *);

    /**
     * This function is called when this app is restored.  The KConfig
     * object points to the session management config file that was saved
     * with @ref saveProperties
     */
    void readProperties(KConfig *);


private slots:
    void fileNew();
    void fileOpen();
    void fileOpenRecent(const KURL& url);
    void fileSave();
    void fileSaveAs();
    void filePrint();
    void fileChangeEOLType();
    void showToolbar();
    void showStatusbar();
    void configureKeys();
    void configureToolbars();
    void preferences();

    void changeStatusbar(const QString& text);
    void changeCaption(const QString& text);
    void changeEOLType(const TextDocument::EOLType type);

private:
    void setupAccel();
    void setupActions();

private:
    KatyView *m_view;

    QPrinter   *m_printer;
    KToggleAction *m_toolbarAction;
    KToggleAction *m_statusbarAction;
    KSelectAction *m_eolTypeAction;

    KRecentFilesAction *m_openRecentAction;
};

#endif // KATY_H
