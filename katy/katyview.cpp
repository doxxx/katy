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
        : QWidget(parent) {
    // Setup our layout manager to automatically add our widgets
    QHBoxLayout *top_layout = new QHBoxLayout(this);
    top_layout->setAutoAdd(true);

    // Create our TextEditor widget
    m_editor = new TextEditor(this);
    connect(m_editor, SIGNAL(cursorMoved(int, int)), SIGNAL(updateLineColumn(int, int)));

    m_document = NULL;
}

KatyView::~KatyView() {
    if (m_document != NULL) {
        delete m_document;
    }
}

void KatyView::showDocument(TextDocument *document) {
    if (m_document != NULL) {
        delete m_document;
    }

    m_document = document;

    m_editor->setDocument(m_document);

    connect(m_document, SIGNAL(documentModified()), SLOT(slotDocumentModified()));
    connect(m_document, SIGNAL(documentNotModified()), SLOT(slotDocumentNotModified()));
    connect(m_document, SIGNAL(documentExternallyChanged()), SLOT(slotDocumentExternallyChanged()));

    slotDocumentNotModified();
}

TextDocument *KatyView::document() {
    return m_document;
}

TextEditor *KatyView::editor() {
    return m_editor;
}

void KatyView::slotDocumentModified() {
    emit documentStatusChanged(m_document->url(), TRUE);
}

void KatyView::slotDocumentNotModified() {
    emit documentStatusChanged(m_document->url(), FALSE);
}

void KatyView::slotDocumentExternallyChanged() {
    QString msg;

    if (m_document->modified()) {
        msg = i18n("Document has been externally modified. Reload and lose your changes?");
    } else {
        msg = i18n("Document has been externally modified. Reload?");
    }

    if (KMessageBox::questionYesNo(this, msg) == KMessageBox::Yes) {
        m_document->openURL(m_document->url());
        m_editor->resetView();
    }
}
