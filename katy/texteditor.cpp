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
    : QScrollView(parent, name, WNorthWestGravity | WRepaintNoErase | WResizeNoErase | WPaintClever)
{
    m_document = NULL;
    m_cursorLine = 0;
    m_cursorColumn = 0;
    m_cursorOn = FALSE;
    m_selectionAnchorLine = -1;
    m_selectionAnchorColumn = -1;
    m_selectionEndLine = -1;
    m_selectionEndColumn = -1;

    setMinimumWidth(500);
    setMinimumHeight(300);

    // Setup the viewport widget
    viewport()->setBackgroundColor(KGlobalSettings::baseColor()); // TODO: Make configurable
    viewport()->setFont(KGlobalSettings::fixedFont()); // TODO: Make configurable
    viewport()->setFocusPolicy(WheelFocus);
    viewport()->setCursor(ibeamCursor);

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

SelectionRange TextEditor::selectionRange()
{
    SelectionRange range;

    if (m_selectionAnchorLine == -1 || m_selectionAnchorColumn == -1 || m_selectionEndLine == -1 || m_selectionEndColumn == -1)
    {
        range.hasSelection = FALSE;
    }
    else if (m_selectionAnchorLine < m_selectionEndLine)
    {
        range.startLine = m_selectionAnchorLine;
        range.startColumn = m_selectionAnchorColumn;
        range.endLine = m_selectionEndLine;
        range.endColumn = m_selectionEndColumn;
        range.hasSelection = TRUE;
    }
    else if (m_selectionAnchorLine > m_selectionEndLine)
    {
        range.startLine = m_selectionEndLine;
        range.startColumn = m_selectionEndColumn;
        range.endLine = m_selectionAnchorLine;
        range.endColumn = m_selectionAnchorColumn;
        range.hasSelection = TRUE;
    }
    else
    {
        range.startLine = range.endLine = m_selectionAnchorLine;

        if (m_selectionAnchorColumn < m_selectionEndColumn)
        {
            range.startColumn = m_selectionAnchorColumn;
            range.endColumn = m_selectionEndColumn;
            range.hasSelection = TRUE;
        }
        else if (m_selectionAnchorColumn > m_selectionEndColumn)
        {
            range.startColumn = m_selectionEndColumn;
            range.endColumn = m_selectionAnchorColumn;
            range.hasSelection = TRUE;
        }
        else
        {
            range.startColumn = range.endColumn = m_selectionAnchorColumn;
            range.hasSelection = FALSE;
        }
    }

    return range;
}

void TextEditor::moveCursorLeft(bool extendSelection)
{
    int cursorColumn = m_cursorColumn,
          cursorLine = m_cursorLine;

    if (cursorColumn > 0)
    {
        cursorColumn--;
    }
    else
    {
        if (cursorLine > 0)
        {
            cursorLine--;
            cursorColumn = m_document->line(cursorLine).text.length();
        }
    }

    if (extendSelection)
        extendSelectionTo(cursorLine, cursorColumn);
    else
        deselect();

    eraseCursor();

    m_cursorColumn = cursorColumn;
    m_cursorLine = cursorLine;

    ensureCursorVisible();
}

void TextEditor::moveCursorRight(bool extendSelection)
{
    int cursorColumn = m_cursorColumn,
          cursorLine = m_cursorLine,
          lineLength = m_document->line(cursorLine).text.length();

    if (cursorColumn < lineLength)
    {
        cursorColumn++;
    }
    else
    {
        if (cursorLine < m_document->lineCount() - 1)
        {
            cursorLine++;
            cursorColumn = 0;
        }
    }

    if (extendSelection)
        extendSelectionTo(cursorLine, cursorColumn);
    else
        deselect();

    eraseCursor();

    m_cursorColumn = cursorColumn;
    m_cursorLine = cursorLine;

    ensureCursorVisible();
}

void TextEditor::moveCursorUp(bool extendSelection)
{
    int cursorColumn = m_cursorColumn,
          cursorLine = m_cursorLine;

    if (cursorLine > 0)
    {
        cursorLine--;
        int lineLength = m_document->line(cursorLine).text.length();
        if (cursorColumn > lineLength)
            cursorColumn = lineLength;
    }

    if (extendSelection)
        extendSelectionTo(cursorLine, cursorColumn);
    else
        deselect();

    eraseCursor();

    m_cursorColumn = cursorColumn;
    m_cursorLine = cursorLine;

    ensureCursorVisible();
}

void TextEditor::moveCursorDown(bool extendSelection)
{
    int cursorColumn = m_cursorColumn,
          cursorLine = m_cursorLine;

    if (cursorLine < m_document->lineCount() - 1)
    {
        cursorLine++;
        int lineLength = m_document->line(cursorLine).text.length();
        if (cursorColumn > lineLength)
            cursorColumn = lineLength;
    }

    if (extendSelection)
        extendSelectionTo(cursorLine, cursorColumn);
    else
        deselect();

    eraseCursor();

    m_cursorColumn = cursorColumn;
    m_cursorLine = cursorLine;

    ensureCursorVisible();
}

void TextEditor::deselect()
{
    int topLine = m_selectionAnchorLine < m_selectionEndLine ? m_selectionAnchorLine : m_selectionEndLine;
    int bottomLine = m_selectionAnchorLine > m_selectionEndLine ? m_selectionAnchorLine : m_selectionEndLine;

    topLine = topLine < m_selectionAnchorLine ? topLine : m_selectionAnchorLine;
    topLine = topLine < m_selectionEndLine ? topLine : m_selectionEndLine;
    bottomLine = bottomLine > m_selectionAnchorLine ? bottomLine : m_selectionAnchorLine;
    bottomLine = bottomLine > m_selectionEndLine ? bottomLine : m_selectionEndLine;

    m_selectionAnchorLine = -1;
    m_selectionAnchorColumn = -1;
    m_selectionEndLine = -1;
    m_selectionEndColumn = -1;

    updateLines(topLine, bottomLine);
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

QRect TextEditor::calculateCursorRect(int cursorLine, int cursorColumn)
{
    QFontMetrics fontMetrics = viewport()->fontMetrics();
    QString text = m_document->line(cursorLine).text;
    int cursorX1, cursorX2, cursorY1, cursorY2;

    cursorX1 = fontMetrics.size(SingleLine | ExpandTabs, text, cursorColumn, fontMetrics.width(' ') * 8).width() - 2;
    cursorX2 = cursorX1 + 4;
    if (cursorX1 < 0)
        cursorX1 = 0;
    cursorY1 = cursorLine * fontMetrics.lineSpacing() + 1;
    cursorY2 = cursorLine * fontMetrics.lineSpacing() + fontMetrics.height() + 1;

    return QRect(cursorX1, cursorY1, cursorX2 - cursorX1 + 1, cursorY2 - cursorY1 + 1);
}

void TextEditor::eraseCursor()
{
    m_cursorOn = FALSE;
    repaintContents(calculateCursorRect(m_cursorLine, m_cursorColumn), FALSE);
}

void TextEditor::drawCursor()
{
    m_cursorOn = TRUE;
    repaintContents(calculateCursorRect(m_cursorLine, m_cursorColumn), FALSE);
}

void TextEditor::ensureCursorVisible()
{
    QRect cursorRect = calculateCursorRect(m_cursorLine, m_cursorColumn);
    m_cursorOn = TRUE;

    int scrollX = 0, scrollY = 0;

    if (cursorRect.left() < contentsX())
    {
        while (cursorRect.left() < contentsX() + scrollX)
            scrollX += -100;
    }

    if (cursorRect.right() > contentsX() + visibleWidth() - 1)
    {
        while (cursorRect.right() > contentsX() + visibleWidth() - 1 + scrollX)
            scrollX += 100;
    }

    if (cursorRect.top() < contentsY())
    {
        while (cursorRect.top() < contentsY() + scrollY)
            scrollY += -viewport()->fontMetrics().lineSpacing();
    }

    if (cursorRect.bottom() > contentsY() + visibleHeight() - 1)
    {
        while (cursorRect.bottom() > contentsY() + visibleHeight() - 1 + scrollY)
            scrollY += viewport()->fontMetrics().lineSpacing();
    }

    if (scrollX != 0 || scrollY != 0)
        scrollBy(scrollX, scrollY);
    else
        repaintContents(cursorRect, FALSE);
}

void TextEditor::extendSelectionTo(int line, int column)
{
    if (m_selectionAnchorLine < 0 || m_selectionAnchorColumn < 0)
    {
        m_selectionAnchorLine = m_cursorLine;
        m_selectionAnchorColumn = m_cursorColumn;
        m_selectionEndLine = m_cursorLine;
        m_selectionEndColumn = m_cursorColumn;
    }

    int topLine = m_selectionAnchorLine < m_selectionEndLine ? m_selectionAnchorLine : m_selectionEndLine;
    int bottomLine = m_selectionAnchorLine > m_selectionEndLine ? m_selectionAnchorLine : m_selectionEndLine;

    m_selectionEndLine = line;
    m_selectionEndColumn = column;

    topLine = topLine < m_selectionAnchorLine ? topLine : m_selectionAnchorLine;
    topLine = topLine < m_selectionEndLine ? topLine : m_selectionEndLine;
    bottomLine = bottomLine > m_selectionAnchorLine ? bottomLine : m_selectionAnchorLine;
    bottomLine = bottomLine > m_selectionEndLine ? bottomLine : m_selectionEndLine;

    updateLines(topLine, bottomLine);
}

void TextEditor::updateLines(int start, int end)
{
    int lineHeight = viewport()->fontMetrics().lineSpacing();
    int updateTop = 1 + lineHeight * start;
    int updateHeight = (end - start + 1) * lineHeight;
    QRect updateRect(0, updateTop, contentsWidth(), updateHeight);
    updateContents(updateRect);
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
        SelectionRange selRange = selectionRange();

        if (startLine < 0)
            startLine = 0;

        p->setTabStops(fontMetrics.width(' ') * 8); // TODO: Make configurable

        int i;

        for (i = startLine; i < endLine && i < m_document->lineCount(); ++i)
        {
            QString text = m_document->line(i).text;

            if (selRange.hasSelection)
            {
                if (i == selRange.startLine && i == selRange.endLine)
                {
                    p->setPen(KGlobalSettings::textColor()); // TODO: Make configurable
                    p->setBackgroundColor(KGlobalSettings::baseColor()); // TODO: Make configurable
                    int firstWidth = fontMetrics.size(SingleLine | ExpandTabs, text, selRange.startColumn, fontMetrics.width(' ') * 8).width();
                    p->drawText(1, 1 + lineHeight * i, firstWidth, lineHeight, ExpandTabs, text, selRange.startColumn);

                    p->setPen(KGlobalSettings::highlightedTextColor()); // TODO: Make configurable
                    p->setBackgroundColor(KGlobalSettings::highlightColor()); // TODO: Make configurable
                    int secondWidth = fontMetrics.size(SingleLine | ExpandTabs, text, selRange.endColumn, fontMetrics.width(' ') * 8).width() - firstWidth;
                    p->drawText(1 + firstWidth, 1 + lineHeight * i, secondWidth, lineHeight, ExpandTabs, text.mid(selRange.startColumn, selRange.endColumn - selRange.startColumn));

                    p->setPen(KGlobalSettings::textColor()); // TODO: Make configurable
                    p->setBackgroundColor(KGlobalSettings::baseColor()); // TODO: Make configurable
                    int thirdWidth = fontMetrics.size(SingleLine | ExpandTabs, text, -1, fontMetrics.width(' ') * 8).width() - secondWidth - firstWidth;
                    p->drawText(1 + firstWidth + secondWidth, 1 + lineHeight * i, thirdWidth, lineHeight, ExpandTabs, text.mid(selRange.endColumn));
                }
                else if (i == selRange.startLine)
                {
                    p->setPen(KGlobalSettings::textColor()); // TODO: Make configurable
                    p->setBackgroundColor(KGlobalSettings::baseColor()); // TODO: Make configurable
                    int firstWidth = fontMetrics.size(SingleLine | ExpandTabs, text, selRange.startColumn, fontMetrics.width(' ') * 8).width();
                    p->drawText(1, 1 + lineHeight * i, firstWidth, lineHeight, ExpandTabs, text, selRange.startColumn);

                    p->setPen(KGlobalSettings::highlightedTextColor()); // TODO: Make configurable
                    p->setBackgroundColor(KGlobalSettings::highlightColor()); // TODO: Make configurable
                    int secondWidth = fontMetrics.size(SingleLine | ExpandTabs, text, -1, fontMetrics.width(' ') * 8).width() - firstWidth;
                    p->drawText(1 + firstWidth, 1 + lineHeight * i, secondWidth, lineHeight, ExpandTabs, text.mid(selRange.startColumn));
                }
                else if (i > selRange.startLine && i < selRange.endLine)
                {
                    p->setPen(KGlobalSettings::highlightedTextColor()); // TODO: Make configurable
                    p->setBackgroundColor(KGlobalSettings::highlightColor()); // TODO: Make configurable
                    int textWidth = fontMetrics.size(SingleLine | ExpandTabs, text, -1, fontMetrics.width(' ') * 8).width();
                    p->drawText(1, 1 + lineHeight * i, textWidth, lineHeight, ExpandTabs, text);
                }
                else if (i == selRange.endLine)
                {
                    p->setPen(KGlobalSettings::highlightedTextColor()); // TODO: Make configurable
                    p->setBackgroundColor(KGlobalSettings::highlightColor()); // TODO: Make configurable
                    int firstWidth = fontMetrics.size(SingleLine | ExpandTabs, text, selRange.endColumn, fontMetrics.width(' ') * 8).width();
                    p->drawText(1, 1 + lineHeight * i, firstWidth, lineHeight, ExpandTabs, text, selRange.endColumn);

                    p->setPen(KGlobalSettings::textColor()); // TODO: Make configurable
                    p->setBackgroundColor(KGlobalSettings::baseColor()); // TODO: Make configurable
                    int secondWidth = fontMetrics.size(SingleLine | ExpandTabs, text, -1, fontMetrics.width(' ') * 8).width() - firstWidth;
                    p->drawText(1 + firstWidth, 1 + lineHeight * i, secondWidth, lineHeight, ExpandTabs, text.mid(selRange.endColumn));
                }
                else
                {
                    p->setPen(KGlobalSettings::textColor()); // TODO: Make configurable
                    p->setBackgroundColor(KGlobalSettings::baseColor()); // TODO: Make configurable
                    // TODO: Replace this with our own pixel size calculation function, maybe?
                    // TODO: Make tab stop ("fontMetrics.width(' ') * 8") configurable
                    int textWidth = fontMetrics.size(SingleLine | ExpandTabs, text, -1, fontMetrics.width(' ') * 8).width();
                    // TODO: Replace this with our own text drawing function, maybe?
                    p->drawText(1, 1 + lineHeight * i, textWidth, lineHeight, ExpandTabs, text);
                    // p->drawText(1, lineHeight + i * lineHeight, text); // TODO: Replace this with our own text drawing function
                }
            }
            else
            {
                p->setPen(KGlobalSettings::textColor()); // TODO: Make configurable
                p->setBackgroundColor(KGlobalSettings::baseColor()); // TODO: Make configurable
                // TODO: Replace this with our own pixel size calculation function, maybe?
                // TODO: Make tab stop ("fontMetrics.width(' ') * 8") configurable
                int textWidth = fontMetrics.size(SingleLine | ExpandTabs, text, -1, fontMetrics.width(' ') * 8).width();
                // TODO: Replace this with our own text drawing function, maybe?
                p->drawText(1, 1 + lineHeight * i, textWidth, lineHeight, ExpandTabs, text);
                // p->drawText(1, lineHeight + i * lineHeight, text); // TODO: Replace this with our own text drawing function
            }
        }

        if (m_cursorOn)
        {
            QString text = m_document->line(m_cursorLine).text;
            int cursorX, cursorY1, cursorY2;

            //cursorX = stringColumnX(fontMetrics, lineText, m_cursorColumn);
            // TODO: Replace this with our own pixel position calculation function, maybe?
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
        updateContents(calculateCursorRect(m_cursorLine, m_cursorColumn));
    }
}

void TextEditor::keyPressEvent(QKeyEvent *event)
{
    bool shiftPressed = ((event->state() & ShiftButton) == ShiftButton);

    switch (event->key())
    {
        case Key_Left:
            moveCursorLeft(shiftPressed);
            break;

        case Key_Right:
            moveCursorRight(shiftPressed);
            break;

        case Key_Up:
            moveCursorUp(shiftPressed);
            break;

        case Key_Down:
            moveCursorDown(shiftPressed);
            break;

        default:
            event->ignore();
    }
}

