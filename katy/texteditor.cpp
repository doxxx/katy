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
    m_selectionEndLine = -1;
    m_selectionEndColumn = -1;

    setMinimumWidth(500);
    setMinimumHeight(300);

    // Setup the viewport widget
    viewport()->setBackgroundMode(NoBackground); // NoBackground because drawContents paints every pixel
    viewport()->setFocusPolicy(WheelFocus);
    viewport()->setCursor(ibeamCursor);

    // TODO: Make configurable
    viewport()->setFont(KGlobalSettings::fixedFont());

    viewport()->setFocus();

    // Start the timer which makes the cursor flash
    // TODO: Make configurable (flash enabled && flash speed)
    m_cursorTimerId = startTimer(250);
    if (m_cursorTimerId == 0)
    {
        kdWarning() << "Could not start timer for cursor flashing" << endl;
        m_cursorOn = TRUE;
    }
}

TextEditor::~TextEditor()
{
    if (m_cursorTimerId != 0)
        killTimer(m_cursorTimerId);
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

void TextEditor::moveCursorTo(int line, int column, bool extendSelection )
{
    int cursorColumn = column,
          cursorLine = line;

    if (cursorLine < 0)
    {
        cursorLine = 0;
    }
    else if (cursorLine >= m_document->lineCount())
    {
        cursorLine = m_document->lineCount() - 1;
    }

    int lineLength = m_document->line(cursorLine).text.length();

    if (cursorColumn < 0)
    {
        cursorColumn = 0;
    }
    else if (cursorColumn > lineLength)
    {
        cursorColumn = lineLength;
    }

    if (cursorColumn == m_cursorColumn && cursorLine == m_cursorLine)
        return;

    if (extendSelection)
        extendSelectionTo(cursorLine, cursorColumn);
    else
        deselect();

    eraseCursor();

    m_cursorColumn = cursorColumn;
    m_cursorLine = cursorLine;

    ensureCursorVisible();
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

void TextEditor::moveCursorPageUp(bool extendSelection )
{
    int cursorColumn = m_cursorColumn,
          cursorLine = m_cursorLine,
     pageHeightLines = viewport()->height() / viewport()->fontMetrics().lineSpacing();

    if (cursorLine >= pageHeightLines)
    {
        cursorLine -= pageHeightLines;
    }
    else
    {
        cursorLine = 0;
    }

    int lineLength = m_document->line(cursorLine).text.length();
    if (cursorColumn > lineLength)
        cursorColumn = lineLength;

    if (extendSelection)
        extendSelectionTo(cursorLine, cursorColumn);
    else
        deselect();

    eraseCursor();

    m_cursorColumn = cursorColumn;
    m_cursorLine = cursorLine;

    scrollBy(0, -(pageHeightLines * viewport()->fontMetrics().lineSpacing()));
}

void TextEditor::moveCursorPageDown(bool extendSelection )
{
}

void TextEditor::moveCursorHome(bool extendSelection)
{
}

void TextEditor::moveCursorEnd(bool extendSelection)
{
}

void TextEditor::deselect()
{
    int topLine = m_selectionAnchorLine < m_selectionEndLine ? m_selectionAnchorLine : m_selectionEndLine;
    int bottomLine = m_selectionAnchorLine > m_selectionEndLine ? m_selectionAnchorLine : m_selectionEndLine;

    m_selectionAnchorLine = -1;
    m_selectionAnchorColumn = -1;
    m_selectionEndLine = -1;
    m_selectionEndColumn = -1;

    repaintLines(topLine, bottomLine);
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
        int width = calculateTextWidth(fontMetrics, text);
        if (width > documentWidth)
            documentWidth = width;
    }

    // Calculate the document height
    documentHeight = m_document->lineCount() * fontMetrics.lineSpacing();

    // Tell the scroll widget the new size of our document
    resizeContents(documentWidth, documentHeight);
}

void TextEditor::recalculateDocumentSize(QString oldLineText, QString newLineText)
{
    if (m_document == NULL)
    {
        resizeContents(0, 0);
        return;
    }

    QFontMetrics fontMetrics = viewport()->fontMetrics();
    int documentWidth = contentsWidth();
    int oldLineWidth = calculateTextWidth(fontMetrics, oldLineText);
    int newLineWidth = calculateTextWidth(fontMetrics, newLineText);

    if (newLineWidth > oldLineWidth && newLineWidth > documentWidth)
        resizeContents(newLineWidth, contentsHeight());
    else if (newLineWidth < oldLineWidth)
        recalculateDocumentSize();
    else
        return;
}

QRect TextEditor::calculateCursorRect(int cursorLine, int cursorColumn, int *cursorMiddleX)
{
    QFontMetrics fontMetrics = viewport()->fontMetrics();
    QString text = m_document->line(cursorLine).text;
    int cursorX, cursorX1, cursorX2, cursorY1, cursorY2;

    cursorX = calculateTextWidth(fontMetrics, text, cursorColumn);
    cursorX1 = cursorX - 2;
    cursorX2 = cursorX + 2;
    if (cursorX1 < 0)
        cursorX1 = 0;
    cursorY1 = cursorLine * fontMetrics.lineSpacing();
    cursorY2 = cursorLine * fontMetrics.lineSpacing() + fontMetrics.height() - 1;

    if (cursorMiddleX != NULL)
        *cursorMiddleX = cursorX;

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

    repaintContents(cursorRect, FALSE);

    if (scrollX != 0 || scrollY != 0)
        scrollBy(scrollX, scrollY);
}

void TextEditor::extendSelectionTo(int line, int column)
{
    int topLine, bottomLine;
    int leftCol, rightCol;

    if (m_selectionAnchorLine < 0)
    {
        m_selectionAnchorLine = m_selectionEndLine = m_cursorLine;
        m_selectionAnchorColumn = m_selectionEndColumn = m_cursorColumn;
    }

    topLine = bottomLine = m_selectionEndLine;
    leftCol = rightCol = m_selectionEndColumn;

    m_selectionEndLine = line;
    m_selectionEndColumn = column;

    topLine = topLine < m_selectionEndLine ? topLine : m_selectionEndLine;
    bottomLine = bottomLine > m_selectionEndLine ? bottomLine : m_selectionEndLine;
    leftCol = leftCol < m_selectionEndColumn ? leftCol : m_selectionEndColumn;
    rightCol = rightCol > m_selectionEndColumn ? rightCol : m_selectionEndColumn;

    if (topLine != bottomLine)
    {
        repaintLines(topLine, bottomLine);
    }
    else
    {
        QFontMetrics fontMetrics = viewport()->fontMetrics();
        QString text = m_document->line(topLine).text;
        int lineHeight = fontMetrics.lineSpacing();
        int top = lineHeight * topLine;
        int left = calculateTextWidth(fontMetrics, text, leftCol);
        int right = calculateTextWidth(fontMetrics, text, rightCol);
        int width = right - left + 1;
        QRect rect(left, top, width, lineHeight);
        repaintContents(rect, FALSE);
    }
}

void TextEditor::repaintLines(int start, int end)
{
    int lineHeight = viewport()->fontMetrics().lineSpacing();
    int top = lineHeight * start;
    int height = (end - start + 1) * lineHeight;
    QRect rect(0, top, QMAX(contentsWidth(), visibleWidth()), height);
    repaintContents(rect, FALSE);
}

int TextEditor::calculateTextWidth(QFontMetrics fontMetrics, QString text, int length)
{
    int lineHeight = fontMetrics.lineSpacing();
    QChar c;
    int cx = 0;
    QString temp;
    int drawTextLength = (length >= 0) ? length : text.length();

    for (int i = 0; i < drawTextLength; i++)
    {
        c = text[i];
        if (c == QChar(9))
        {
            if (temp.length() > 0)
            {
                int tempWidth = fontMetrics.width(temp);
                cx += tempWidth;
                temp.truncate(0);
            }

            // TODO: Make tab stop size configurable
            int tabWidth = fontMetrics.width(' ') * (8 - (i % 8));
            cx += tabWidth;
        }
        else
        {
            temp += c;
        }
    }

    if (temp.length() > 0)
    {
        int tempWidth = fontMetrics.width(temp);
        cx += tempWidth;
    }

    return cx;
}

void TextEditor::paintText(QPainter *p, int x, int y, QString text, int start, int length)
{
    QFontMetrics fontMetrics = p->fontMetrics();
    int lineHeight = fontMetrics.lineSpacing();
    QChar c;
    int cx = x;
    QString temp;
    int stop = (length >= 0) ? (start + length) : text.length();

    for (int i = start; i < stop; i++)
    {
        c = text[i];
        if (c == QChar(9))
        {
            if (temp.length() > 0)
            {
                int tempWidth = fontMetrics.width(temp);
                p->eraseRect(cx, y, tempWidth, lineHeight);
                p->drawText(cx, y, tempWidth, lineHeight, AlignLeft | AlignTop, temp);
                cx += tempWidth;
                temp.truncate(0);
            }

            // TODO: Make tab stop size configurable
            int tabWidth = fontMetrics.width(' ') * (8 - (i % 8));
            p->eraseRect(cx, y, tabWidth, lineHeight);
            cx += tabWidth;
        }
        else
        {
            temp += c;
        }
    }

    if (temp.length() > 0)
    {
        int tempWidth = fontMetrics.width(temp);
        p->eraseRect(cx, y, tempWidth, lineHeight);
        p->drawText(cx, y, tempWidth, lineHeight, AlignLeft | AlignTop, temp);
    }
}

void TextEditor::pointToLineCol(QPoint p, int &line, int &col)
{
    QFontMetrics fontMetrics = viewport()->fontMetrics();

    line = p.y() / fontMetrics.lineSpacing();

    if (line < 0 || line >= m_document->lineCount())
        return;

    QString text = m_document->line(line).text;

    for (col = 0; col < text.length(); col++)
    {
        int x = calculateTextWidth(fontMetrics, text, col);
        if (p.x() <= x + fontMetrics.width(text[col]) / 2)
            break;
    }
}

void TextEditor::drawContents(QPainter *p, int cx, int cy, int cw, int ch)
{
    p->setBackgroundMode(OpaqueMode);

    if (m_document != NULL)
    {
        QFontMetrics fontMetrics = p->fontMetrics();
        int lineHeight = fontMetrics.lineSpacing();
        int startLine = cy / lineHeight - 1;
        int endLine = (cy + ch) / lineHeight + 1;
        int maxPaintWidth = QMAX(contentsWidth(), viewport()->width());
        SelectionRange selRange = selectionRange();

        if (startLine < 0)
            startLine = 0;

        // TODO: Make tab stop size configurable
        p->setTabStops(fontMetrics.width(' ') * 8);

        int i;

        for (i = startLine; i < endLine && i < m_document->lineCount(); ++i)
        {
            QString text = m_document->line(i).text;

            if (selRange.hasSelection)
            {
                if (text.length() > 0)
                {
                    if (i == selRange.startLine && i == selRange.endLine)
                    {
                        p->setPen(KGlobalSettings::textColor()); // TODO: Make configurable
                        p->setBackgroundColor(KGlobalSettings::baseColor()); // TODO: Make configurable
                        int firstWidth = calculateTextWidth(fontMetrics, text, selRange.startColumn);
                        paintText(p, 0, lineHeight * i, text, 0, selRange.startColumn);

                        p->setPen(KGlobalSettings::highlightedTextColor()); // TODO: Make configurable
                        p->setBackgroundColor(KGlobalSettings::highlightColor()); // TODO: Make configurable
                        int secondWidth = calculateTextWidth(fontMetrics, text, selRange.endColumn) - firstWidth;
                        paintText(p, firstWidth, lineHeight * i, text, selRange.startColumn, selRange.endColumn);

                        p->setPen(KGlobalSettings::textColor()); // TODO: Make configurable
                        p->setBackgroundColor(KGlobalSettings::baseColor()); // TODO: Make configurable
                        int thirdWidth = calculateTextWidth(fontMetrics, text) - secondWidth - firstWidth;
                        paintText(p, firstWidth + secondWidth, lineHeight * i, text, selRange.endColumn);

                        p->eraseRect(firstWidth + secondWidth + thirdWidth, lineHeight * i, maxPaintWidth - firstWidth + secondWidth + thirdWidth, lineHeight);
                    }
                    else if (i == selRange.startLine)
                    {
                        p->setPen(KGlobalSettings::textColor()); // TODO: Make configurable
                        p->setBackgroundColor(KGlobalSettings::baseColor()); // TODO: Make configurable
                        int firstWidth = calculateTextWidth(fontMetrics, text, selRange.startColumn);
                        paintText(p, 0, lineHeight * i, text, 0, selRange.startColumn);

                        p->setPen(KGlobalSettings::highlightedTextColor()); // TODO: Make configurable
                        p->setBackgroundColor(KGlobalSettings::highlightColor()); // TODO: Make configurable
                        int secondWidth = calculateTextWidth(fontMetrics, text) - firstWidth;
                        paintText(p, firstWidth, lineHeight * i, text, selRange.startColumn);

                        p->eraseRect(firstWidth + secondWidth, lineHeight * i, maxPaintWidth - firstWidth + secondWidth, lineHeight);
                    }
                    else if (i > selRange.startLine && i < selRange.endLine)
                    {
                        p->setPen(KGlobalSettings::highlightedTextColor()); // TODO: Make configurable
                        p->setBackgroundColor(KGlobalSettings::highlightColor()); // TODO: Make configurable
                        int textWidth = calculateTextWidth(fontMetrics, text);
                        paintText(p, 0, lineHeight * i, text);

                        p->eraseRect(textWidth, lineHeight * i, maxPaintWidth - textWidth, lineHeight);
                    }
                    else if (i == selRange.endLine)
                    {
                        p->setPen(KGlobalSettings::highlightedTextColor()); // TODO: Make configurable
                        p->setBackgroundColor(KGlobalSettings::highlightColor()); // TODO: Make configurable
                        int firstWidth = calculateTextWidth(fontMetrics, text, selRange.endColumn);
                        paintText(p, 0, lineHeight * i, text, 0, selRange.endColumn);

                        p->setPen(KGlobalSettings::textColor()); // TODO: Make configurable
                        p->setBackgroundColor(KGlobalSettings::baseColor()); // TODO: Make configurable
                        int secondWidth = calculateTextWidth(fontMetrics, text) - firstWidth;
                        paintText(p, firstWidth, lineHeight * i, text, selRange.endColumn);

                        p->eraseRect(firstWidth + secondWidth, lineHeight * i, maxPaintWidth - firstWidth + secondWidth, lineHeight);
                    }
                    else
                    {
                        p->setPen(KGlobalSettings::textColor()); // TODO: Make configurable
                        p->setBackgroundColor(KGlobalSettings::baseColor()); // TODO: Make configurable
                        int textWidth = calculateTextWidth(fontMetrics, text);
                        paintText(p, 0, lineHeight * i, text);

                        p->eraseRect(textWidth, lineHeight * i, maxPaintWidth - textWidth, lineHeight);
                    }
                }
                else
                {
                    if (i >= selRange.startLine && i < selRange.endLine)
                        p->setBackgroundColor(KGlobalSettings::highlightColor()); // TODO: Make configurable
                    else
                        p->setBackgroundColor(KGlobalSettings::baseColor()); // TODO: Make configurable

                    p->eraseRect(0, lineHeight * i, maxPaintWidth, lineHeight);
                }
            }
            else
            {
                if (text.length() > 0)
                {
                    p->setPen(KGlobalSettings::textColor()); // TODO: Make configurable
                    p->setBackgroundColor(KGlobalSettings::baseColor()); // TODO: Make configurable
                    int textWidth = calculateTextWidth(fontMetrics, text);
                    paintText(p, 0, lineHeight * i, text);

                    p->eraseRect(textWidth, lineHeight * i, maxPaintWidth - textWidth, lineHeight);
                }
                else
                {
                    p->setBackgroundColor(KGlobalSettings::baseColor()); // TODO: Make configurable
                    p->eraseRect(0, lineHeight * i, maxPaintWidth, lineHeight);
                }
            }
        }

        if (contentsHeight() < viewport()->height())
        {
            p->setBackgroundColor(KGlobalSettings::baseColor()); // TODO: Make configurable
            p->eraseRect(0, contentsHeight(), maxPaintWidth, viewport()->height() - contentsHeight());
        }

        if (m_cursorOn)
        {
            int cursorMiddleX;
            QRect cursorRect = calculateCursorRect(m_cursorLine, m_cursorColumn, &cursorMiddleX);

            p->setPen(QColor(0, 0, 0));
            p->setRasterOp(NotXorROP);
            p->drawLine(cursorRect.left(), cursorRect.top(), cursorRect.right(), cursorRect.top());
            p->drawLine(cursorMiddleX, cursorRect.top(), cursorMiddleX, cursorRect.bottom());
            p->drawLine(cursorRect.left(), cursorRect.bottom(), cursorRect.right(), cursorRect.bottom());
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

        case Key_PageUp:
            moveCursorPageUp(shiftPressed);
            break;

        case Key_PageDown:
            moveCursorPageDown(shiftPressed);
            break;

        case Key_Enter:
        case Key_Return:
        {
            kdDebug() << "Enter" << endl;
            TextLine line = m_document->line(m_cursorLine);
            TextLine newLine(line.text.mid(m_cursorColumn));
            line.text.truncate(m_cursorColumn);
            m_document->setLine(m_cursorLine, line);
            m_document->insertLine(m_cursorLine, newLine, TRUE);
            m_cursorColumn = 0;
            m_cursorLine++;
            recalculateDocumentSize();
            repaintLines(m_cursorLine - 1, m_document->lineCount() - 1);
            ensureCursorVisible();
        }
        break;

        default:
            if (event->text().length() > 0)
            {
                TextLine line = m_document->line(m_cursorLine);
                QString oldText = line.text;
                line.text.insert(m_cursorColumn, event->text());
                m_document->setLine(m_cursorLine, line);
                m_cursorColumn += event->text().length();
                recalculateDocumentSize(oldText, line.text);
                repaintLines(m_cursorLine, m_cursorLine);
                ensureCursorVisible();
            }
            else
            {
                event->ignore();
            }
    }
}

void TextEditor::contentsMousePressEvent(QMouseEvent *event)
{
    int line, col;

    pointToLineCol(event->pos(), line, col);

    bool leftButton = (event->button() & LeftButton == LeftButton);
    bool shiftPressed = (event->state() & ShiftButton == ShiftButton);

    if (leftButton)
    {
        moveCursorTo(line, col, shiftPressed);
    }
}

void TextEditor::contentsMouseMoveEvent(QMouseEvent *event)
{
    int line, col;

    pointToLineCol(event->pos(), line, col);

    bool leftButton = (event->state() & LeftButton == LeftButton);

    if (leftButton)
    {
        moveCursorTo(line, col, TRUE);
    }
}
