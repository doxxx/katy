/*
 * Class for main view widget
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

#include "katyview.h"
#include "textdocument.h"
#include "texteditor.h"

#include <qlayout.h>

#include <kurl.h>
#include <kdebug.h>
#include <klocale.h>
#include <kmessagebox.h>

KatyView::KatyView(QWidget *parent)
    : QWidget(parent)
{
    // Setup our layout manager to automatically add our widgets
    QHBoxLayout *top_layout = new QHBoxLayout(this);
    top_layout->setAutoAdd(true);

    // Create our TextEditor widget
    m_editor = new TextEditor(this);
    m_document = new TextDocument();
    m_editor->setDocument(m_document);

    connect(m_document, SIGNAL(documentModified()), SLOT(slotDocumentModified()));
    connect(m_document, SIGNAL(documentNotModified()), SLOT(slotDocumentNotModified()));
}

KatyView::~KatyView()
{
}

KURL KatyView::currentURL()
{
    return m_document->url();
}

void KatyView::openURL(const KURL& url)
{
    if (m_document != NULL)
        delete m_document;

    m_document = new TextDocument();
    m_document->openURL(url);
    m_editor->setDocument(m_document);

    connect(m_document, SIGNAL(documentModified()), SLOT(slotDocumentModified()));
    connect(m_document, SIGNAL(documentNotModified()), SLOT(slotDocumentNotModified()));
    connect(m_document, SIGNAL(documentExternallyChanged()), SLOT(slotDocumentExternallyChanged()));

    emit signalChangeCaption(url.url(), FALSE);
}

TextDocument *KatyView::document()
{
    return m_document;
}

TextEditor *KatyView::editor()
{
    return m_editor;
}

void KatyView::slotDocumentModified()
{
    if (m_document->url().isEmpty())
        emit signalChangeCaption("Untitled", TRUE);
    else
        emit signalChangeCaption(m_document->url().url(), TRUE);
}

void KatyView::slotDocumentNotModified()
{
    if (m_document->url().isEmpty())
        emit signalChangeCaption("Untitled", FALSE);
    else
        emit signalChangeCaption(m_document->url().url(), FALSE);
}

void KatyView::slotDocumentExternallyChanged()
{
    QString msg;

    if (m_document->modified())
        msg = i18n("Document has been externally modified. Reload and lose your changes?");
    else
        msg = i18n("Document has been externally modified. Reload?");

    if (KMessageBox::questionYesNo(this, msg) == KMessageBox::Yes)
    {
        m_document->openURL(m_document->url());
        m_editor->resetView();
    }
}
