/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#include <qscrollview.h>

class QWidget;
class QPainter;

class TextDocument;

/**
 *
 */
class SelectionRange
{
public:
    int startLine, startColumn, endLine, endColumn;
    bool hasSelection;
};

/**
 * This is the TextEditor widget which displays and allows editing of
 * a text document.
 *
 * @short Text Editor widget
 * @author Gordon Tyler <gtyler@iafrica.com>
 * @version 0.3
 */
class TextEditor : public QScrollView
{
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
    TextDocument *document();

    /**
     * Sets the TextDocument to be used.
     */
    void setDocument(TextDocument *doc);

    /**
     * Returns the selection range.
     */
    SelectionRange selectionRange();

    // Operations
    void moveCursorTo(int line, int column, bool extendSelection = FALSE);
    void moveCursorLeft(bool extendSelection = FALSE);
    void moveCursorRight(bool extendSelection = FALSE);
    void moveCursorUp(bool extendSelection = FALSE);
    void moveCursorDown(bool extendSelection = FALSE);
    void moveCursorPageUp(bool extendSelection = FALSE);
    void moveCursorPageDown(bool extendSelection = FALSE);
    void moveCursorHome(bool extendSelection = FALSE);
    void moveCursorEnd(bool extendSelection = FALSE);
    void deselect();

public slots:

signals:

protected:
    void drawContents(QPainter *p, int cx, int cy, int cw, int ch);
    void timerEvent(QTimerEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void contentsMousePressEvent(QMouseEvent *event);
    void contentsMouseMoveEvent(QMouseEvent *event);

    void recalculateDocumentSize();
    void recalculateDocumentSize(QString oldLineText, QString newLineText);
    QRect calculateCursorRect(int cursorLine, int cursorColumn, int *cursorMiddleX = NULL);
    void eraseCursor();
    void drawCursor();
    void ensureCursorVisible();
    void extendSelectionTo(int line, int column);
    void repaintLines(int start, int end);
    int calculateTextWidth(QFontMetrics fontMetrics, QString text, int length = -1);
    void paintText(QPainter *p, int x, int y, QString text, int start = 0, int length = -1);
    void pointToLineCol(QPoint p, int &line, int &col);

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

    // configurable data
};

#endif
