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

#include "textdocument.h"

class QPainter;
class KURL;

class TextEditor;

/**
 * This is the main view class for Katy.
 *
 * @short Main view
 * @author Gordon Tyler <gordon@doxxx.net>
 */
class KatyView : public QWidget
{
    Q_OBJECT
public:
    KatyView(QWidget *parent);
    virtual ~KatyView();

    /**
     * Current document's URL
     */
    KURL currentURL();

    /**
     * Random 'set' function
     */
    void openURL(const KURL& url);

    TextDocument *document();
    TextEditor *editor();

signals:
    /**
     * Change the content of the statusbar
     */
    void signalChangeStatusbar(const QString& text);

    /**
     * Change the content of the caption
     */
    void signalChangeCaption(const QString& filename, bool modified);

public slots:
    void slotDocumentModified();
    void slotDocumentNotModified();
    void slotDocumentExternallyChanged();

private:
    TextDocument *m_document;
    TextEditor *m_editor;
};

#endif // KATYVIEW_H
