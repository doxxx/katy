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

#include "katy.h"
#include "katyapp.h"
#include "katyview.h"
#include "textdocument.h"
#include "texteditor.h"
#include "katypref.h"
#include "katytabstospacesimpl.h"
#include "katyfindimpl.h"
#include "katyreplaceimpl.h"
#include "katyreplacingimpl.h"

#include <unistd.h>

#include <qdragobject.h>
#include <qlineedit.h>
#include <qpainter.h>
#include <qpaintdevicemetrics.h>

#include <kglobal.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kmenubar.h>
#include <kkeydialog.h>
#include <kaccel.h>
#include <kio/netaccess.h>
#include <kfiledialog.h>
#include <kconfig.h>
#include <kurl.h>
#include <kfiledialog.h>
#include <kedittoolbar.h>
#include <kstdaccel.h>
#include <kaction.h>
#include <kstdaction.h>
#include <krecentdocument.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <kstatusbar.h>

enum StatusBarItems
{
    StatusBar_Line = 1,
    StatusBar_Column
};

Katy::Katy()
    : KMainWindow()
{
    // accept dnd
    setAcceptDrops(true);

    m_windowsMenuActions.setAutoDelete(false);
    m_windowsMenuItemAction = NULL;

    // Initialise the main view
    m_view = new KatyView(this);
    setCentralWidget(m_view);

    // allow the view to change the statusbar and caption
    connect(m_view, SIGNAL(documentStatusChanged(const KURL&, bool)), SLOT(updateDocumentStatus(const KURL&, bool)));
    connect(m_view, SIGNAL(updateLineColumn(int, int)), SLOT(updateLineColumn(int, int)));

    // Setup actions
    setupActions();

    // Setup the status bar
    statusBar()->insertItem(i18n("Line %1").arg("999999"), StatusBar_Line, 0, TRUE);
    statusBar()->insertItem(i18n("Column %1").arg("9999"), StatusBar_Column, 0, TRUE);
    statusBar()->show();
    
    // Read config options
    readOptions(katyapp->config());

    // Init window with a new document
    m_view->showDocument(new TextDocument());
    updateEOLType(m_view->document()->eolType());
}

Katy::~Katy()
{
    saveOptions(katyapp->config());
    katyapp->removeWindow(this);
}

void Katy::setupActions()
{
    KStdAction::openNew(this, SLOT(fileNew()), actionCollection());
    KStdAction::open(this, SLOT(fileOpen()), actionCollection());
    m_openRecentAction = KStdAction::openRecent(this, SLOT(fileOpenRecent(const KURL&)), actionCollection());
    m_openRecentAction->loadEntries(KGlobal::config());
    KStdAction::save(this, SLOT(fileSave()), actionCollection());
    KStdAction::saveAs(this, SLOT(fileSaveAs()), actionCollection());
    new KAction(i18n("Save All..."), 0, this, SLOT(fileSaveAll()), actionCollection(), "file_save_all");
    KStdAction::close(this, SLOT(fileClose()), actionCollection());

    m_eolTypeAction = new KSelectAction(i18n("EOL Type"), 0, this, SLOT(fileChangeEOLType()), actionCollection(), "file_eol_type");
    QStringList eolTypesList;
    eolTypesList << i18n("DOS") << i18n("Unix") << i18n("Mac");
    m_eolTypeAction->setItems(eolTypesList);
    m_eolTypeAction->setCurrentItem(1);

    KStdAction::quit(kapp, SLOT(closeAllWindows()), actionCollection());

    KStdAction::cut(this, SLOT(editCut()), actionCollection());
    KStdAction::copy(this, SLOT(editCopy()), actionCollection());
    KStdAction::paste(this, SLOT(editPaste()), actionCollection());
    KStdAction::selectAll(this, SLOT(editSelectAll()), actionCollection());

    KStdAction::find(this, SLOT(editFind()), actionCollection());
    KStdAction::findNext(this, SLOT(editFindNext()), actionCollection());
    KStdAction::replace(this, SLOT(editReplace()), actionCollection());

    m_tabsToSpacesAction = new KAction(i18n("Tabs to Spaces..."), 0, this, SLOT(editTabsToSpaces()), actionCollection(), "edit_tabs_to_spaces");
    m_spacesToTabsAction = new KAction(i18n("Spaces to Tabs..."), 0, this, SLOT(editSpacesToTabs()), actionCollection(), "edit_spaces_to_tabs");

    m_toolbarAction = KStdAction::showToolbar(this, SLOT(showToolbar()), actionCollection());
    m_statusbarAction = KStdAction::showStatusbar(this, SLOT(showStatusbar()), actionCollection());

    KStdAction::keyBindings(this, SLOT(configureKeys()), actionCollection());
    KStdAction::configureToolbars(this, SLOT(configureToolbars()), actionCollection());
    KStdAction::preferences(this, SLOT(preferences()), actionCollection());
    
    createGUI("katyui.rc", false);
}

void Katy::load(const QString& url)
{
    KURL kurl(url);
    load(kurl);
}

void Katy::load(const KURL& url)
{
    TextDocument *document = new TextDocument();
    document->openURL(url);

    m_view->showDocument(document);
    updateEOLType(m_view->document()->eolType());

    KRecentDocument::add(url.url(), TRUE);
    m_openRecentAction->addURL(url);
}

KAction *Katy::windowsMenuItemAction()
{
    return m_windowsMenuItemAction;
}

void Katy::createWindowsMenuItemAction()
{
    QString text;
    if (m_view->document()->url().isEmpty())
    {
        text = i18n("Untitled");
    }
    else
    {
        text = m_view->document()->url().fileName();
    }
    text += (m_view->document()->modified() ? "*" : "");
    
    m_windowsMenuItemAction = new KRadioAction(text);
    connect(m_windowsMenuItemAction, SIGNAL(activated()), this, SLOT(activateWindow()));
    m_windowsMenuItemAction->setExclusiveGroup("window_list_actions");
}

void Katy::updateDocumentStatus(const KURL &url, bool modified)
{
    QString caption;
    
    if (url.isEmpty()) 
    {
        caption = i18n("Untitled");
    } 
    else 
    {
        caption = url.fileName();
    }
    
    if (m_windowsMenuItemAction)
    {
        m_windowsMenuItemAction->setText(caption + (modified ? "*" : ""));
    }
    
    setCaption(caption, modified);
}

bool Katy::queryExit()
{
    m_openRecentAction->saveEntries(KGlobal::config());
    return true;
}

bool Katy::queryClose()
{
    if (m_view->document()->modified())
    {
        switch (KMessageBox::warningYesNoCancel(this, i18n("Save changes to document?")))
        {
            case KMessageBox::Yes:
                fileSave();
                return true;
            case KMessageBox::No:
                return true;
            default: // cancel
                return false;
        }
    }

    return true;
}

void Katy::readOptions(KConfig *config)
{
    KConfigGroupSaver configGroupSaver(config, "Window");
    m_toolbarAction->setChecked(config->readBoolEntry("ShowToolbar", true));
    showToolbar();
    m_statusbarAction->setChecked(config->readBoolEntry("ShowStatusbar", true));
    showStatusbar();
    QSize size = config->readSizeEntry("DefaultSize");
    if (!size.isEmpty())
    {
        resize(size);
    }
}

void Katy::saveOptions(KConfig *config)
{
    KConfigGroupSaver configGroupSaver(config, "Window");
    config->writeEntry("ShowToolbar", m_toolbarAction->isChecked());
    config->writeEntry("ShowStatusbar", m_statusbarAction->isChecked());
    config->writeEntry("DefaultSize", size());
}

void Katy::saveProperties(KConfig *config)
{
    // the 'config' object points to the session managed
    // config file.  anything you write here will be available
    // later when this app is restored

    if (!m_view->document()->url().isEmpty())
        config->writeEntry("lastURL", m_view->document()->url().url());
}

void Katy::readProperties(KConfig *config)
{
    // the 'config' object points to the session managed
    // config file.  this function is automatically called whenever
    // the app is being restored.  read in here whatever you wrote
    // in 'saveProperties'

    KURL url = KURL(config->readEntry("lastURL"));

    if (!url.isEmpty())
        load(url);
}

void Katy::dragEnterEvent(QDragEnterEvent *event)
{
    // accept uri drops only
    event->accept(QUriDrag::canDecode(event));
}

void Katy::dropEvent(QDropEvent *event)
{
    // this is a very simplistic implementation of a drop event.  we
    // will only accept a dropped URL.  the Qt dnd code can do *much*
    // much more, so please read the docs there
    QStrList uri;

    // see if we can decode a URI.. if not, just ignore it
    if (QUriDrag::decode(event, uri))
    {
        // okay, we have a URI.. process it
        QString url, target;
        url = uri.first();

        // load in the file
        load(url);
    }
}

void Katy::windowActivationChange(bool oldActive)
{
    KMainWindow::windowActivationChange(oldActive);
    m_windowsMenuItemAction->setChecked(oldActive);
}

void Katy::fileNew()
{
    // this slot is called whenever the File->New menu is selected,
    // the New shortcut is pressed (usually CTRL+N) or the New toolbar
    // button is clicked

    // create a new window
    katyapp->newWindow()->show();
}

void Katy::fileOpen()
{
    // this slot is called whenever the File->Open menu is selected,
    // the Open shortcut is pressed (usually CTRL+O) or the Open toolbar
    // button is clicked
    KURL url = KFileDialog::getOpenURL(QString::null, QString::null, this);
    if (!url.isEmpty())
    {
        if (m_view->document()->url().isEmpty() && !m_view->document()->modified())
        {
            load(url);
        }
        else
        {
            Katy *newWindow = katyapp->newWindow();
            newWindow->load(url);
            newWindow->show();
        }
    }
}

void Katy::fileOpenRecent(const KURL& url)
{
    if (!url.isEmpty() && !url.isMalformed())
    {
        if (m_view->document()->url().isEmpty() && !m_view->document()->modified())
        {
            load(url);
        }
        else
        {
            Katy *newWindow = katyapp->newWindow();
            newWindow->load(url);
            newWindow->show();
        }
    }
}

void Katy::fileSave()
{
    // this slot is called whenever the File->Save menu is selected,
    // the Save shortcut is pressed (usually CTRL+S) or the Save toolbar
    // button is clicked

    if (m_view->document()->url().isEmpty())
    {
        fileSaveAs();
    }
    else
    {
        m_view->document()->save();
    }
}

void Katy::fileSaveAs()
{
    KURL file_url = KFileDialog::getSaveURL();
    if (!file_url.isEmpty() && !file_url.isMalformed())
    {
        m_view->document()->saveURL(file_url);
    }
}

void Katy::fileSaveAll()
{
    KatyListIterator it = katyapp->windowsIterator();
    for (Katy *window = it.toFirst(); window; ++it, window = it.current())
    {
        window->fileSave();
    }
}

void Katy::fileClose()
{
    close();
}

void Katy::fileChangeEOLType()
{
    switch (m_eolTypeAction->currentItem())
    {
        case 0:
            m_view->document()->setEOLType(TextDocument::EOL_CRLF);
            break;

        case 1:
            m_view->document()->setEOLType(TextDocument::EOL_LF);
            break;

        case 2:
            m_view->document()->setEOLType(TextDocument::EOL_CR);
            break;
    }
}

void Katy::editCut()
{
    m_view->editor()->cut();
}

void Katy::editCopy()
{
    m_view->editor()->copy();
}

void Katy::editPaste()
{
    m_view->editor()->paste();
}

void Katy::editSelectAll()
{
    m_view->editor()->selectAll();
}

void Katy::editFind()
{
    int result = KatyFindImpl::show(this, m_findText, m_backward, m_caseSensitive, m_regularExpression);

    if (result == QDialog::Accepted)
    {
        int flags = 0;

        if (m_backward)
            flags |= TextDocument::Backward;
        if (m_caseSensitive)
            flags |= TextDocument::CaseSensitive;
        if (m_regularExpression)
            flags |= TextDocument::RegularExpression;

        DocumentRange range = m_view->document()->findText(m_findText, m_view->editor()->documentPosition(), flags);
        if (range.valid)
        {
            m_view->editor()->moveCursorTo(range.startLine, range.startColumn);
            m_view->editor()->moveCursorTo(range.endLine, range.endColumn, TRUE);
        }
    }
}

void Katy::editFindNext()
{
    int flags = 0;

    if (m_backward)
        flags |= TextDocument::Backward;
    if (m_caseSensitive)
        flags |= TextDocument::CaseSensitive;
    if (m_regularExpression)
        flags |= TextDocument::RegularExpression;

    DocumentRange range = m_view->document()->findText(m_findText, m_view->editor()->documentPosition(), flags);
    if (range.valid)
    {
        m_view->editor()->moveCursorTo(range.startLine, range.startColumn);
        m_view->editor()->moveCursorTo(range.endLine, range.endColumn, TRUE);
    }
}

void Katy::editReplace()
{
    int result = KatyReplaceImpl::show(this, m_findText, m_replaceText, m_backward, m_caseSensitive, m_regularExpression);

    if (result == QDialog::Accepted)
    {
        int flags = 0;

        if (m_backward)
            flags |= TextDocument::Backward;
        if (m_caseSensitive)
            flags |= TextDocument::CaseSensitive;
        if (m_regularExpression)
            flags |= TextDocument::RegularExpression;

        DocumentRange range = m_view->document()->findText(m_findText, m_view->editor()->documentPosition(), flags);
        bool cancel = FALSE;
        bool ask = TRUE;

        while (range.valid && !cancel)
        {
            m_view->editor()->moveCursorTo(range.startLine, range.startColumn);
            m_view->editor()->moveCursorTo(range.endLine, range.endColumn, TRUE);

            bool replace = FALSE;

            if (ask)
            {
                result = KatyReplacingImpl::ask(this);
                switch (result)
                {
                    case KatyReplacingImpl::Replace:
                        replace = TRUE;
                        break;

                    case KatyReplacingImpl::ReplaceAll:
                        replace = TRUE;
                        ask = FALSE;
                        break;

                    case KatyReplacingImpl::Skip:
                        break;

                    case QDialog::Rejected:
                        cancel = TRUE;
                        break;
                }
            }
            else
            {
                replace = TRUE;
            }

            if (replace)
                m_view->editor()->setSelectedText(m_replaceText);

            katyapp->processEvents();

            if (!cancel)
                range = m_view->document()->findText(m_findText, m_view->editor()->documentPosition(), flags);
        }
    }
}

void Katy::editTabsToSpaces()
{
    int spaces;
    bool leadingOnly;
    if (KatyTabsToSpacesImpl::tabsToSpaces(this, spaces, leadingOnly) == QDialog::Accepted)
    {
        m_view->document()->tabsToSpaces(spaces, leadingOnly);
    }
}

void Katy::editSpacesToTabs()
{
    int spaces;
    bool leadingOnly;
    if (KatyTabsToSpacesImpl::spacesToTabs(this, spaces, leadingOnly) == QDialog::Accepted)
    {
        m_view->document()->spacesToTabs(spaces, leadingOnly);
    }
}

void Katy::showToolbar()
{
    // this is all very cut and paste code for showing/hiding the
    // toolbar
    if (m_toolbarAction->isChecked())
        toolBar()->show();
    else
        toolBar()->hide();
}

void Katy::showStatusbar()
{
    // this is all very cut and paste code for showing/hiding the
    // statusbar
    if (m_statusbarAction->isChecked())
        statusBar()->show();
    else
        statusBar()->hide();
}

void Katy::configureKeys()
{
    KKeyDialog::configureKeys(actionCollection(), "katyui.rc");
}

void Katy::configureToolbars()
{
    // use the standard toolbar editor
    KEditToolbar dlg(actionCollection());
    if (dlg.exec())
    {
        // recreate our GUI
        createGUI();
    }
}

void Katy::preferences()
{
    // popup some sort of preference dialog, here
    KatyPreferences dlg;
    if (dlg.exec())
    {
        // redo your settings
    }
}

void Katy::updateEOLType(const TextDocument::EOLType type)
{
    switch (type)
    {
        case TextDocument::EOL_CRLF:
            m_eolTypeAction->setCurrentItem(0);
            break;

        case TextDocument::EOL_LF:
            m_eolTypeAction->setCurrentItem(1);
            break;

        case TextDocument::EOL_CR:
            m_eolTypeAction->setCurrentItem(2);
            break;

        default:
            break;
    }
}

void Katy::updateLineColumn(int line, int column)
{
    statusBar()->changeItem(i18n("Line %1").arg(QString::number(line + 1)), StatusBar_Line);
    statusBar()->changeItem(i18n("Column %1").arg(QString::number(column + 1)), StatusBar_Column);
}

void Katy::updateWindowsMenu()
{
    unplugActionList("window_list");
    
    m_windowsMenuActions.clear();
    
    KatyListIterator it = katyapp->windowsIterator();
    for (Katy *window = it.toFirst(); window; ++it, window = it.current())
    {
        m_windowsMenuActions.append(window->windowsMenuItemAction());
    }
    
    plugActionList("window_list", m_windowsMenuActions);
}

void Katy::activateWindow()
{
    show();
    
    // Nasty hack to to wait for a minimized window to be restored
    // before it can be raised and activated
    while (isMinimized())
    {
        katyapp->processEvents(100);
    }
    
    raise();
    setActiveWindow();
}
