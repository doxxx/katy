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

    connect(m_document, SIGNAL(documentModified()), this, SLOT(slotDocumentModified()));
    connect(m_document, SIGNAL(documentNotModified()), this, SLOT(slotDocumentNotModified()));
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

    m_document = new TextDocument(url);
    m_editor->setDocument(m_document);

    connect(m_document, SIGNAL(documentModified()), this, SLOT(slotDocumentModified()));
    connect(m_document, SIGNAL(documentNotModified()), this, SLOT(slotDocumentNotModified()));

    emit signalChangeCaption(url.url(), false);
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
    emit signalChangeCaption(m_document->url().url(), true);
}

void KatyView::slotDocumentNotModified()
{
    emit signalChangeCaption(m_document->url().url(), false);
}

