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

#ifndef KATYVIEW_H
#define KATYVIEW_H

#include <qwidget.h>

class KURL;

class TextEditor;
class TextDocument;

/**
 * KatyView is the container view for a document.
 *
 * @short Main view container
 * @author Gordon Tyler <gordon@doxxx.net>
 */
class KatyView : public QWidget {
    Q_OBJECT
public:
    KatyView(QWidget *parent);
    virtual ~KatyView();

    /**
     * Show a TextDocument in this view.
     */
    virtual void showDocument(TextDocument *document);

    /**
     * Return the document currently being shown.
     */
    virtual TextDocument *document();

    virtual TextEditor *editor();

signals:
    /**
     * Indicates that the document filename has changed or that the document
     * has been modified.
     */
    void documentStatusChanged(const KURL& url, bool modified);

    /**
     * Indicates that the status bar line/column should be updated
     */
    void updateLineColumn(int line, int column);

public slots:
    virtual void slotDocumentModified();
    virtual void slotDocumentNotModified();
    virtual void slotDocumentExternallyChanged();

private:
    TextDocument *m_document;
    TextEditor *m_editor;
};

#endif // KATYVIEW_H
