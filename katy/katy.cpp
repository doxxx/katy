/*
 * katy.cpp
 *
 * Copyright (C) 2000 Gordon Tyler <gtyler@iafrica.com>
 */

#include "katy.h"
#include "katyapp.h"
#include "textdocument.h"
#include "texteditor.h"
#include "katypref.h"

#include <qdragobject.h>
#include <qlineedit.h>
#include <qprinter.h>
#include <qprintdialog.h>
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
#include <kurlrequesterdlg.h>

#include <kedittoolbar.h>

#include <kstdaccel.h>
#include <kaction.h>
#include <kstdaction.h>

#include <krecentdocument.h>

Katy::Katy()
    : KMainWindow( 0, "Katy" ),
      m_view(new KatyView(this)),
      m_printer(0)
{
    // accept dnd
    setAcceptDrops(true);

    // tell the KMainWindow that this is indeed the main widget
    setCentralWidget(m_view);

    // then, setup our actions
    setupActions();

    // and a status bar
    statusBar()->show();

    changeEOLType(m_view->eolType());

    // allow the view to change the statusbar and caption
    connect(m_view, SIGNAL(signalChangeStatusbar(const QString&)),
            this,   SLOT(changeStatusbar(const QString&)));
    connect(m_view, SIGNAL(signalChangeCaption(const QString&)),
            this,   SLOT(changeCaption(const QString&)));

    changeCaption("Untitled");
}

Katy::~Katy()
{
    ((KatyApp*)kapp)->removeWindow(this);
}

void Katy::load(const QString& url)
{
    KURL kurl(url);
    load(kurl);
}

void Katy::load(const KURL& url)
{
    KRecentDocument::add(url.url(), TRUE);
    m_openRecentAction->addURL(url);
    m_view->openURL(url);
    changeEOLType(m_view->eolType());
}

void Katy::setupActions()
{
    KStdAction::openNew(this, SLOT(fileNew()), actionCollection());
    KStdAction::open(this, SLOT(fileOpen()), actionCollection());
    KStdAction::save(this, SLOT(fileSave()), actionCollection());
    KStdAction::saveAs(this, SLOT(fileSaveAs()), actionCollection());
    KStdAction::print(this, SLOT(filePrint()), actionCollection());
    KStdAction::quit(kapp, SLOT(quit()), actionCollection());

    KStdAction::cut(m_view->editor(), SLOT(cut()), actionCollection());
    KStdAction::copy(m_view->editor(), SLOT(copy()), actionCollection());
    KStdAction::paste(m_view->editor(), SLOT(paste()), actionCollection());
    KStdAction::selectAll(m_view->editor(), SLOT(selectAll()), actionCollection());

    m_toolbarAction = KStdAction::showToolbar(this, SLOT(showToolbar()), actionCollection());
    m_statusbarAction = KStdAction::showStatusbar(this, SLOT(showStatusbar()), actionCollection());

    KStdAction::keyBindings(this, SLOT(configureKeys()), actionCollection());
    KStdAction::configureToolbars(this, SLOT(configureToolbars()), actionCollection());
    KStdAction::preferences(this, SLOT(preferences()), actionCollection());

    m_eolTypeAction = new KSelectAction(i18n("EOL Type"), 0, this, SLOT(fileChangeEOLType()), actionCollection(), "file_eol_type");
    QStringList eolTypesList;
    eolTypesList << i18n("DOS") << i18n("Unix") << i18n("Mac");
    m_eolTypeAction->setItems(eolTypesList);
    m_eolTypeAction->setCurrentItem(1);

    m_openRecentAction = KStdAction::openRecent(this, SLOT(fileOpenRecent(const KURL&)), actionCollection());
    m_openRecentAction->loadEntries(KGlobal::config());

    createGUI();
}

bool Katy::queryExit()
{
    m_openRecentAction->saveEntries(KGlobal::config());
    return true;
}

void Katy::saveProperties(KConfig *config)
{
    // the 'config' object points to the session managed
    // config file.  anything you write here will be available
    // later when this app is restored

    if (m_view->currentURL().url() != QString::null)
        config->writeEntry("lastURL", m_view->currentURL().url());
}

void Katy::readProperties(KConfig *config)
{
    // the 'config' object points to the session managed
    // config file.  this function is automatically called whenever
    // the app is being restored.  read in here whatever you wrote
    // in 'saveProperties'

    QString url = config->readEntry("lastURL");

    if (url != QString::null)
        m_view->openURL(KURL(url));
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

void Katy::fileNew()
{
    // this slot is called whenever the File->New menu is selected,
    // the New shortcut is pressed (usually CTRL+N) or the New toolbar
    // button is clicked

    // create a new window
    (new Katy)->show();
}

void Katy::fileOpen()
{
    // this slot is called whenever the File->Open menu is selected,
    // the Open shortcut is pressed (usually CTRL+O) or the Open toolbar
    // button is clicked
    KURL url = KURLRequesterDlg::getURL(QString::null, this, i18n("Open Location") );
    if (!url.isEmpty())
    {
        load(url);
    }
}

void Katy::fileOpenRecent(const KURL& url)
{
    if (!url.isEmpty() && !url.isMalformed())
    {
        load(url);
    }
}

void Katy::fileSave()
{
    // this slot is called whenever the File->Save menu is selected,
    // the Save shortcut is pressed (usually CTRL+S) or the Save toolbar
    // button is clicked

    // save the current file
}

void Katy::fileSaveAs()
{
    // this slot is called whenever the File->Save As menu is selected,
    KURL file_url = KFileDialog::getSaveURL();
    if (!file_url.isEmpty() && !file_url.isMalformed())
    {
        // save your info, here
        // add new url to recent documents list
        m_openRecentAction->addURL(file_url);
    }
}

void Katy::filePrint()
{
    // this slot is called whenever the File->Print menu is selected,
    // the Print shortcut is pressed (usually CTRL+P) or the Print toolbar
    // button is clicked

    if (!m_printer) m_printer = new QPrinter;

    if (QPrintDialog::getPrinterSetup(m_printer))
    {
        // setup the printer.  with Qt, you always "print" to a
        // QPainter.. whether the output medium is a pixmap, a screen,
        // or paper
        QPainter p;
        p.begin(m_printer);

        // we let our view do the actual printing
        //QPaintDeviceMetrics metrics(m_printer);
        //m_view->print(&p, metrics.height(), metrics.width());

        // and send the result to the printer
        p.end();
    }
}

void Katy::fileChangeEOLType()
{
    switch (m_eolTypeAction->currentItem())
    {
        case 0:
            m_view->setEOLType(TextDocument::EOL_CRLF);
            break;

        case 1:
            m_view->setEOLType(TextDocument::EOL_LF);
            break;

        case 2:
            m_view->setEOLType(TextDocument::EOL_CR);
            break;
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

void Katy::changeStatusbar(const QString& text)
{
    // display the text on the statusbar
    statusBar()->message(text);
}

void Katy::changeCaption(const QString& text)
{
    // display the text on the caption
    setCaption(text);
}

void Katy::changeEOLType(const TextDocument::EOLType type)
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

