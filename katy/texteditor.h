/*
 * Class for Text Editor widget
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

#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#include <qscrollview.h>

class QWidget;
class QPainter;

#include "textdocument.h"

/**
 *
 */
class SelectionRange {
public:
    int startLine, startColumn, endLine, endColumn;
    bool hasSelection;
};

/**
 * This is the TextEditor widget which displays and allows editing of
 * a text document.
 *
 * @short Text Editor widget
 * @author Gordon Tyler <gordon@doxxx.net>
 */
class TextEditor : public QScrollView {
    Q_OBJECT
public:
    /**
     * Constructor
     */
    TextEditor(QWidget *parent=0, QString name=0);

    /**
     * Destructor
     */
    ~TextEditor();

    /**
     * Returns the TextDocument used.
     */
    TextDocument *document() {
        return m_document;
    }

    /**
     * Sets the TextDocument to be used.
     */
    void setDocument(TextDocument *doc);

    DocumentPosition documentPosition() {
        return DocumentPosition(m_cursorLine, m_cursorColumn);
    }

    /**
     * Returns the selection range.
     */
    SelectionRange selectionRange();

    /**
     * Returns the selected text.
     */
    QString selectedText();

    void setSelectedText(QString newText);

    // Operations
    void moveCursorTo(int line, int column, bool extendSelection=FALSE);
    void moveCursorLeft(bool extendSelection=FALSE);
    void moveCursorRight(bool extendSelection=FALSE);
    void moveCursorWordLeft(bool extendSelection=FALSE);
    void moveCursorWordRight(bool extendSelection=FALSE);
    void moveCursorUp(bool extendSelection=FALSE);
    void moveCursorDown(bool extendSelection=FALSE);
    void moveCursorPageUp(bool extendSelection=FALSE);
    void moveCursorPageDown(bool extendSelection=FALSE);
    void moveCursorHome(bool extendSelection=FALSE);
    void moveCursorEnd(bool extendSelection=FALSE);
    void moveCursorDocumentStart(bool extendSelection=FALSE);
    void moveCursorDocumentEnd(bool extendSelection=FALSE);
    void deselect();

    void resetView();

signals:
    void cursorMoved(int line, int column);

public slots:
    void cut();
    void copy();
    void paste();
    void selectAll();
    void refresh();

private slots:
    void document_lineChanged(int line, TextLine oldLine, TextLine newLine);
    void document_linesInserted(int line, TextLineList newLine);
    void document_linesRemoved(int line, int count);

protected:
    void drawContents(QPainter *p, int cx, int cy, int cw, int ch);
    void timerEvent(QTimerEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void contentsMousePressEvent(QMouseEvent *event);
    void contentsMouseMoveEvent(QMouseEvent *event);
    void contentsMouseDoubleClickEvent(QMouseEvent *event);

    void recalculateDocumentSize();
    void recalculateDocumentSize(QString newLineText);
    void recalculateDocumentSize(QString oldLineText, QString newLineText);
    QRect calculateCursorRect(int cursorLine, int cursorColumn, int *cursorMiddleX=NULL);
    void eraseCursor();
    void drawCursor();
    void ensureCursorVisible();
    void extendSelectionTo(int line, int column);
    void repaintLines(int start, int end);
    int calculateTextWidth(QFontMetrics fontMetrics, QString text, int length=-1);
    void paintText(QPainter *p, int x, int y, QString text, int start=0, int end=-1);
    void pointToLineColumn(QPoint p, int &line, int &column);
    bool deleteSelection();
    bool textIsPrint(QString text);

private:
    // runtime data
    TextDocument *m_document;
    int m_cursorTimerId;
    int m_cursorLine;
    int m_cursorColumn;
    bool m_cursorOn;
    int m_selectionAnchorLine;
    int m_selectionAnchorColumn;
    int m_selectionEndLine;
    int m_selectionEndColumn;
    QFont m_font;

    // configurable data
};

#endif
