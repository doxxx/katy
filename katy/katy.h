/*
 * Class for main Katy window
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

#ifndef KATY_H
#define KATY_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <kmainwindow.h>

//#include "katyapp.h"
#include "katyview.h"

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
    bool queryClose();

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
    void editFind();
    void editFindNext();
    void editReplace();
    void editTabsToSpaces();
    void editSpacesToTabs();
    void showToolbar();
    void showStatusbar();
    void configureKeys();
    void configureToolbars();
    void preferences();

    void changeStatusbar(const QString& text);
    void changeEOLType(const TextDocument::EOLType type);

private:
    void setupAccel();
    void setupActions();

private:
    KatyView *m_view;

    QPrinter *m_printer;

    KToggleAction *m_toolbarAction;
    KToggleAction *m_statusbarAction;
    KSelectAction *m_eolTypeAction;
    KRecentFilesAction *m_openRecentAction;
    KAction *m_tabsToSpacesAction;
    KAction *m_spacesToTabsAction;

    QString m_findText;
    QString m_replaceText;
    bool m_backward;
    bool m_caseSensitive;
    bool m_regularExpression;

};

#endif // KATY_H
