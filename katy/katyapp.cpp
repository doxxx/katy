/*
 * Class for main Katy application
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

#include "katyapp.h"
#include "katy.h"

#include <kcmdlineargs.h>
#include <kconfig.h>
#include <kglobalsettings.h>
#include <kglobal.h>
#include <kdebug.h>

KatyApp::KatyApp()
        : alreadyRestored(FALSE) {}

KatyApp::~KatyApp() {}

int KatyApp::newInstance() {
    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

    if (isRestored() && !alreadyRestored) {
        alreadyRestored = true;
        RESTORE(Katy)
    } else {
        if (args->count() > 0) {
            for (int i = 0; i < args->count(); i++) // Counting start at 0!
            {
                Katy *window = newWindow();
                window->load(args->url(i));
                window->show();
            }
        } else {
            Katy *window = newWindow();
            window->show();
        }
    }

    return 0;
}

Katy *KatyApp::newWindow() {
    Katy *window = new Katy();
    windowList.append(window);
    updateWindowsMenu();
    return window;
}

void KatyApp::removeWindow(Katy *window) {
    windowList.remove(window);
    updateWindowsMenu();
}

KatyListIterator KatyApp::windowsIterator() {
    return KatyListIterator(windowList);
}

QFont KatyApp::readConfig_Font() {
    KConfigGroupSaver configGroupSaver(KGlobal::config(), "Appearance");
    QFont *defaultFont = new QFont(KGlobalSettings::fixedFont());
    return KGlobal::config()->readFontEntry("Font", defaultFont);
}

QColor KatyApp::readConfig_NormalForeground() {
    KConfigGroupSaver configGroupSaver(KGlobal::config(), "Appearance");
    QColor *defaultColour = new QColor(KGlobalSettings::textColor());
    return KGlobal::config()->readColorEntry("NormalForeground", defaultColour);
}

QColor KatyApp::readConfig_NormalBackground() {
    KConfigGroupSaver configGroupSaver(KGlobal::config(), "Appearance");
    QColor *defaultColour = new QColor(KGlobalSettings::baseColor());
    return KGlobal::config()->readColorEntry("NormalBackground", defaultColour);
}

QColor KatyApp::readConfig_SelectedForeground() {
    KConfigGroupSaver configGroupSaver(KGlobal::config(), "Appearance");
    QColor *defaultColour = new QColor(KGlobalSettings::highlightedTextColor());
    return KGlobal::config()->readColorEntry("SelectedForeground", defaultColour);
}

QColor KatyApp::readConfig_SelectedBackground() {
    KConfigGroupSaver configGroupSaver(KGlobal::config(), "Appearance");
    QColor *defaultColour = new QColor(KGlobalSettings::highlightColor());
    return KGlobal::config()->readColorEntry("SelectedBackground", defaultColour);
}

int KatyApp::readConfig_TabSize() {
    KConfigGroupSaver configGroupSaver(KGlobal::config(), "Indenting");
    return KGlobal::config()->readNumEntry("TabSize", 8);
}

bool KatyApp::readConfig_UseSpaces() {
    KConfigGroupSaver configGroupSaver(KGlobal::config(), "Indenting");
    return KGlobal::config()->readBoolEntry("UseSpaces", FALSE);
}

int KatyApp::readConfig_IndentSize() {
    KConfigGroupSaver configGroupSaver(KGlobal::config(), "Indenting");
    return KGlobal::config()->readNumEntry("IndentSize", 8);
}

void KatyApp::writeConfig_Font(QFont font) {
    KConfigGroupSaver configGroupSaver(KGlobal::config(), "Appearance");
    KGlobal::config()->writeEntry("Font", font);
    emit configChanged();
}

void KatyApp::writeConfig_NormalForeground(QColor color) {
    KConfigGroupSaver configGroupSaver(KGlobal::config(), "Appearance");
    KGlobal::config()->writeEntry("NormalForeground", color);
    emit configChanged();
}

void KatyApp::writeConfig_NormalBackground(QColor color) {
    KConfigGroupSaver configGroupSaver(KGlobal::config(), "Appearance");
    KGlobal::config()->writeEntry("NormalBackground", color);
    emit configChanged();
}

void KatyApp::writeConfig_SelectedForeground(QColor color) {
    KConfigGroupSaver configGroupSaver(KGlobal::config(), "Appearance");
    KGlobal::config()->writeEntry("SelectedForeground", color);
    emit configChanged();
}

void KatyApp::writeConfig_SelectedBackground(QColor color) {
    KConfigGroupSaver configGroupSaver(KGlobal::config(), "Appearance");
    KGlobal::config()->writeEntry("SelectedBackground", color);
    emit configChanged();
}

void KatyApp::writeConfig_TabSize(int tabSize) {
    KConfigGroupSaver configGroupSaver(KGlobal::config(), "Indenting");
    KGlobal::config()->writeEntry("TabSize", tabSize);
    emit configChanged();
}

void KatyApp::writeConfig_UseSpaces(bool useSpaces) {
    KConfigGroupSaver configGroupSaver(KGlobal::config(), "Indenting");
    KGlobal::config()->writeEntry("UseSpaces", useSpaces);
    emit configChanged();
}

void KatyApp::writeConfig_IndentSize(int indentSize) {
    KConfigGroupSaver configGroupSaver(KGlobal::config(), "Indenting");
    KGlobal::config()->writeEntry("IndentSize", indentSize);
    emit configChanged();
}

void KatyApp::updateWindowsMenu() {
    QPtrList<KAction> oldActions;
    oldActions.setAutoDelete(true);

    Katy *window;
    for (window = windowList.first(); window; window = windowList.next()) {
        oldActions.append(window->windowsMenuItemAction());
        window->createWindowsMenuItemAction();
    }

    for (window = windowList.first(); window; window = windowList.next()) {
        window->updateWindowsMenu();
    }

    oldActions.clear();
}
