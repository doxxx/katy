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
     * Construtor
     */
    TextEditor(QWidget *parent=0, QString name=0);
    /**
     * Destructor
     */
    ~TextEditor();

    // Attributes
    TextDocument *document();
    void setDocument(TextDocument *doc);
    SelectionRange selectionRange();

    // Operations
    void moveCursorLeft(bool extendSelection = FALSE);
    void moveCursorRight(bool extendSelection = FALSE);
    void moveCursorUp(bool extendSelection = FALSE);
    void moveCursorDown(bool extendSelection = FALSE);
    void moveCursorHome(bool extendSelection = FALSE);
    void moveCurosrEnd(bool extendSelection = FALSE);
    void deselect();

public slots:

signals:

protected:
    void drawContents(QPainter *p, int cx, int cy, int cw, int ch);
    void timerEvent(QTimerEvent *event);
    void keyPressEvent(QKeyEvent *event);

    void recalculateDocumentSize();
    QRect calculateCursorRect(int cursorLine, int cursorColumn);
    void eraseCursor();
    void drawCursor();
    void ensureCursorVisible();
    void extendSelectionTo(int line, int column);
    void repaintLines(int start, int end);
    int calculateTextWidth(QFontMetrics fontMetrics, QString text, int length = -1);
    void paintText(QPainter *p, int x, int y, int w, int h, QString text, int length = -1);

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
