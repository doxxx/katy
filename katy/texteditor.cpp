/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "texteditor.h"

#include <kdebug.h>
#include <kglobalsettings.h>
#include <qpainter.h>

#include "textdocument.h"

TextEditor::TextEditor(QWidget *parent, QString name)
    : QScrollView(parent, name, WNorthWestGravity | WRepaintNoErase | WResizeNoErase)
{
    m_document = NULL;
    m_cursorLine = 0;
    m_cursorColumn = 0;
    m_cursorOn = FALSE;
    m_selectionAnchorLine = -1;
    m_selectionAnchorColumn = -1;

    setMinimumWidth(500);
    setMinimumHeight(300);

    // Setup the viewport widget
    viewport()->setBackgroundColor(KGlobalSettings::baseColor()); // TODO: Make configurable
    viewport()->setFont(KGlobalSettings::fixedFont()); // TODO: Make configurable
    viewport()->setFocusPolicy(WheelFocus);

    viewport()->setFocus();

    // Start the timer which makes the cursor flash
    m_cursorTimerId = startTimer(250); // TODO: Make configurable
}

TextEditor::~TextEditor()
{
}

TextDocument *TextEditor::document()
{
    return m_document;
}

void TextEditor::setDocument(TextDocument *doc)
{
    m_document = doc;
    m_cursorLine = 0;
    m_cursorColumn = 0;
    m_selectionAnchorLine = -1;
    m_selectionAnchorColumn = -1;
    recalculateDocumentSize();
    viewport()->update();
}

void TextEditor::moveCursorLeft(bool extendSelection)
{
    if (m_cursorColumn > 0)
    {
        eraseCursor();

        m_cursorColumn--;

        QRect cursorRect = calculateCursorRect();
        m_cursorOn = TRUE;
        if (cursorRect.left() < contentsX())
            scrollBy(-100, 0);
        else
            repaintContents(cursorRect, FALSE);
    }
}

void TextEditor::moveCursorRight(bool extendSelection)
{
    int lineLength = m_document->line(m_cursorLine).text.length();
    if (m_cursorColumn < lineLength)
    {
        eraseCursor();

        m_cursorColumn++;

        QRect cursorRect = calculateCursorRect();
        m_cursorOn = TRUE;
        if (cursorRect.right() > contentsX() + visibleWidth() - 1)
            scrollBy(100, 0);
        else
            repaintContents(cursorRect, FALSE);
    }
}

void TextEditor::moveCursorUp(bool extendSelection)
{
    if (m_cursorLine > 0)
    {
        eraseCursor();

        m_cursorLine--;

        QRect cursorRect = calculateCursorRect();
        m_cursorOn = TRUE;
        if (cursorRect.top() < contentsY())
            scrollBy(0, -viewport()->fontMetrics().lineSpacing());
        else
            repaintContents(cursorRect, FALSE);
    }
}

void TextEditor::moveCursorDown(bool extendSelection)
{
    if (m_cursorLine < m_document->lineCount() - 1)
    {
        eraseCursor();

        m_cursorLine++;

        QRect cursorRect = calculateCursorRect();
        m_cursorOn = TRUE;
        if (cursorRect.bottom() > contentsY() + visibleHeight() - 1)
            scrollBy(0, viewport()->fontMetrics().lineSpacing());
        else
            repaintContents(cursorRect, FALSE);
    }
}

void TextEditor::recalculateDocumentSize()
{
    if (m_document == NULL)
    {
        resizeContents(0, 0);
        return;
    }

    QFontMetrics fontMetrics = viewport()->fontMetrics();
    int documentWidth = 0;
    int documentHeight = 0;

    // Calculate the document width
    TextDocument::TextLineList::ConstIterator it;
    for (it = m_document->lineIterator(0); it != m_document->endLineIterator(); ++it)
    {
        QString text = (*it).text;
        int width = fontMetrics.size(SingleLine | ExpandTabs, text, -1, fontMetrics.width(' ') * 8).width();
        if (width > documentWidth)
            documentWidth = width;
    }

    // Calculate the document height
    documentHeight = m_document->lineCount() * fontMetrics.lineSpacing();

    // Add a single pixel border on all sides
    documentWidth += 2;
    documentHeight += 2;

    // Tell the scroll widget the new size of our document
    resizeContents(documentWidth, documentHeight);
}

QRect TextEditor::calculateCursorRect()
{
    QFontMetrics fontMetrics = viewport()->fontMetrics();
    QString text = m_document->line(m_cursorLine).text;
    int cursorX1, cursorX2, cursorY1, cursorY2;

    cursorX1 = fontMetrics.size(SingleLine | ExpandTabs, text, m_cursorColumn, fontMetrics.width(' ') * 8).width() - 2;
    cursorX2 = cursorX1 + 4;
    if (cursorX1 < 0)
        cursorX1 = 0;
    cursorY1 = m_cursorLine * fontMetrics.lineSpacing() + 1;
    cursorY2 = m_cursorLine * fontMetrics.lineSpacing() + fontMetrics.height() + 1;

    return QRect(cursorX1, cursorY1, cursorX2 - cursorX1 + 1, cursorY2 - cursorY1 + 1);
}

void TextEditor::eraseCursor()
{
    m_cursorOn = FALSE;
    repaintContents(calculateCursorRect(), FALSE);
}

void TextEditor::drawCursor()
{
    m_cursorOn = TRUE;
    repaintContents(calculateCursorRect(), FALSE);
}

void TextEditor::drawContents(QPainter *p, int cx, int cy, int cw, int ch)
{
//    kdDebug() << "drawContents " << cx << " " << cy << " " << cw << " " << ch << endl;

    p->setBackgroundMode(OpaqueMode);
    p->setBackgroundColor(KGlobalSettings::baseColor()); // TODO: Make configurable
    p->eraseRect(cx, cy, cw, ch);

    if (m_document != NULL)
    {
        QFontMetrics fontMetrics = p->fontMetrics();
        int lineHeight = fontMetrics.lineSpacing();
        int startLine = cy / lineHeight - 1;
        int endLine = (cy + ch) / lineHeight + 1;

        if (startLine < 0)
            startLine = 0;

        p->setPen(KGlobalSettings::textColor()); // TODO: Make configurable
        p->setTabStops(fontMetrics.width(' ') * 8);

        int i;

        for (i = startLine; i < endLine && i < m_document->lineCount(); ++i)
        {
            QString text = m_document->line(i).text;
            int pos;

            do
            {
                pos = text.find('\t');
                if (pos >= 0)
                {
                    QString pad;
                    pad.fill(' ', 8 - (pos % 8));
                    text.replace(pos, 1, pad);
                }
            } while (pos >= 0);

            int textWidth = fontMetrics.size(SingleLine | ExpandTabs, text, -1, fontMetrics.width(' ') * 8).width();
            p->drawText(1, 1 + lineHeight * i, textWidth, lineHeight, ExpandTabs, text);
//            p->drawText(1, lineHeight + i * lineHeight, text); // TODO: Replace this with our own text drawing function
        }

        if (m_cursorOn)
        {
            QString text = m_document->line(m_cursorLine).text;
            int cursorX, cursorY1, cursorY2;

            //cursorX = stringColumnX(fontMetrics, lineText, m_cursorColumn);
            cursorX = fontMetrics.size(SingleLine | ExpandTabs, text, m_cursorColumn, fontMetrics.width(' ') * 8).width();
            cursorY1 = m_cursorLine * lineHeight + 1;
            cursorY2 = m_cursorLine * lineHeight + fontMetrics.height() + 1;

            p->setPen(QColor(0, 0, 0));
            p->setRasterOp(NotXorROP);
            p->drawLine(cursorX - 2, cursorY1, cursorX + 2, cursorY1);
            p->drawLine(cursorX, cursorY1, cursorX, cursorY2);
            p->drawLine(cursorX - 2, cursorY2, cursorX + 2, cursorY2);
            p->setRasterOp(CopyROP);
        }
    }
}

void TextEditor::timerEvent(QTimerEvent *event)
{
    if (viewport()->hasFocus())
    {
        m_cursorOn = !m_cursorOn;
        updateContents(calculateCursorRect());
    }
}

void TextEditor::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
        case Key_Left:
            moveCursorLeft();
            break;

        case Key_Right:
            moveCursorRight();
            break;

        case Key_Up:
            moveCursorUp();
            break;

        case Key_Down:
            moveCursorDown();
            break;

        default:
            event->ignore();
    }
}

