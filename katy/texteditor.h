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

    // Operations
    void moveCursorLeft(bool extendSelection = FALSE);
    void moveCursorRight(bool extendSelection = FALSE);
    void moveCursorUp(bool extendSelection = FALSE);
    void moveCursorDown(bool extendSelection = FALSE);

public slots:

signals:

protected:
    void drawContents(QPainter *p, int cx, int cy, int cw, int ch);
    void timerEvent(QTimerEvent *event);
    void keyPressEvent(QKeyEvent *event);

    void recalculateDocumentSize();
    QRect calculateCursorRect();
    void eraseCursor();
    void drawCursor();

private:
    // runtime data
    TextDocument *m_document;
    int m_cursorTimerId;
    int m_cursorLine;
    int m_cursorColumn;
    bool m_cursorOn;
    int m_selectionAnchorLine;
    int m_selectionAnchorColumn;

    // configurable data

};

#endif
