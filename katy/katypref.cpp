/*
 * Class for Katy Preferences dialog
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
#include "katypref.h"
#include "katyprefappearance.h"
#include "katyprefindenting.h"

#include <klocale.h>
#include <kfontdialog.h>
#include <kglobal.h>
#include <kconfig.h>
#include <kglobalsettings.h>
#include <kcolorbtn.h>

#include <qlayout.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qfontdatabase.h>
#include <qspinbox.h>
#include <qcheckbox.h>

KatyPreferences::KatyPreferences()
    : KDialogBase(TreeList, "Preferences",
                  Help|Ok|Apply|Cancel, Ok)
{
    enableButtonApply(FALSE);

    initAppearancePage();
    initIndentingPage();

    m_changed = FALSE;
}

void KatyPreferences::slotChooseFont()
{
    QFont font(m_appearancePage->fontDescriptionLabel->font());
    if (KFontDialog::getFont(font, TRUE, this) == KFontDialog::Accepted)
    {
        m_appearancePage->fontDescriptionLabel->clear();
        m_appearancePage->fontDescriptionLabel->setFont(font);
        m_appearancePage->fontDescriptionLabel->setText(describeFont(font));
        slotChanged();
    }
}

void KatyPreferences::initAppearancePage()
{
    QFrame *page;
    QBoxLayout *layout;

    page = addPage(i18n("Appearance"), i18n("Appearance"));
    layout = new QVBoxLayout(page);
    m_appearancePage = new KatyPrefAppearancePage(page);

    QFont font = katyapp->readConfig_Font();
    m_appearancePage->fontDescriptionLabel->clear();
    m_appearancePage->fontDescriptionLabel->setFont(font);
    m_appearancePage->fontDescriptionLabel->setText(describeFont(font));
    connect(m_appearancePage->fontDialogButton, SIGNAL(clicked()), SLOT(slotChooseFont()));
    m_appearancePage->normalFgButton->setColor(katyapp->readConfig_NormalForeground());
    m_appearancePage->normalBgButton->setColor(katyapp->readConfig_NormalBackground());
    m_appearancePage->selectedFgButton->setColor(katyapp->readConfig_SelectedForeground());
    m_appearancePage->selectedBgButton->setColor(katyapp->readConfig_SelectedBackground());

    layout->addWidget(m_appearancePage);
    layout->addStretch();

    connect(m_appearancePage->normalFgButton, SIGNAL(changed(const QColor&)), SLOT(slotChanged()));
    connect(m_appearancePage->normalBgButton, SIGNAL(changed(const QColor&)), SLOT(slotChanged()));
    connect(m_appearancePage->selectedFgButton, SIGNAL(changed(const QColor&)), SLOT(slotChanged()));
    connect(m_appearancePage->selectedBgButton, SIGNAL(changed(const QColor&)), SLOT(slotChanged()));
}

void KatyPreferences::initIndentingPage()
{
    QFrame *page;
    QBoxLayout *layout;

    // Indenting page
    page = addPage(i18n("Indenting"), i18n("Indenting"));
    layout = new QVBoxLayout(page);
    m_indentingPage = new KatyPrefIndentingPage(page);

    m_indentingPage->tabSize->setValue(katyapp->readConfig_TabSize());
    m_indentingPage->useSpaces->setChecked(katyapp->readConfig_UseSpaces());
    m_indentingPage->indentSize->setValue(katyapp->readConfig_IndentSize());

    layout->addWidget(m_indentingPage);
    layout->addStretch();

    connect(m_indentingPage->tabSize, SIGNAL(valueChanged(int)), SLOT(slotChanged()));
    connect(m_indentingPage->useSpaces, SIGNAL(toggled(bool)), SLOT(slotChanged()));
    connect(m_indentingPage->indentSize, SIGNAL(valueChanged(int)), SLOT(slotChanged()));
}

void KatyPreferences::saveAppearancePage()
{
    katyapp->writeConfig_Font(m_appearancePage->fontDescriptionLabel->font());
    katyapp->writeConfig_NormalForeground(m_appearancePage->normalFgButton->color());
    katyapp->writeConfig_NormalBackground(m_appearancePage->normalBgButton->color());
    katyapp->writeConfig_SelectedForeground(m_appearancePage->selectedFgButton->color());
    katyapp->writeConfig_SelectedBackground(m_appearancePage->selectedBgButton->color());
}

void KatyPreferences::saveIndentingPage()
{
    katyapp->writeConfig_TabSize(m_indentingPage->tabSize->value());
    katyapp->writeConfig_UseSpaces(m_indentingPage->useSpaces->isChecked());
    katyapp->writeConfig_IndentSize(m_indentingPage->indentSize->value());
}

QString KatyPreferences::describeFont(QFont font)
{
    QFontDatabase fontDB;
    QString format("%1, %2, %3pt");
    return format.arg(font.family()).arg(fontDB.styleString(font)).arg(font.pointSize());
}

void KatyPreferences::slotApply()
{
    if (m_changed)
    {
        saveAppearancePage();
        saveIndentingPage();
        KGlobal::config()->sync();
    }

    enableButtonApply(FALSE);
    m_changed = FALSE;
}

void KatyPreferences::slotOk()
{
    slotApply();
    KDialogBase::slotOk();
}

void KatyPreferences::slotChanged()
{
    enableButtonApply(TRUE);
    m_changed = TRUE;
}

