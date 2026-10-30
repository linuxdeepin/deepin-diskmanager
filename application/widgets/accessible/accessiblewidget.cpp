// SPDX-FileCopyrightText: 2022 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only

#include "ddlog.h"
#include "accessiblewidget.h"
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QDebug>

AccessibleWidget::AccessibleWidget(QWidget *widget)
    : QAccessibleWidget(widget)
    , m_widget(widget)
{
    qCDebug(appLog) << "AccessibleWidget::AccessibleWidget constructor called.";
}

AccessibleWidget::~AccessibleWidget()
{
    // qCDebug(appLog) << "AccessibleWidget::~AccessibleWidget destructor called.";
}

void *AccessibleWidget::interface_cast(QAccessible::InterfaceType t)
{
    qCDebug(appLog) << "AccessibleWidget::interface_cast called with type:" << t;
    switch (t) {
    case QAccessible::ActionInterface:
        qCDebug(appLog) << "Interface cast to ActionInterface.";
        return static_cast<QAccessibleActionInterface *>(this);
    case QAccessible::TextInterface:
        qCDebug(appLog) << "Interface cast to TextInterface.";
        return static_cast<QAccessibleTextInterface *>(this);
    default:
        qCDebug(appLog) << "Interface cast to unknown type.";
        return nullptr;
    }
}

QString AccessibleWidget::text(QAccessible::Text t) const
{
    qCDebug(appLog) << "AccessibleWidget::text called with text type:" << t;
    switch (t) {
    case QAccessible::Name:
        qCDebug(appLog) << "Returning accessible name.";
        return m_widget->accessibleName();
    case QAccessible::Description:
        qCDebug(appLog) << "Returning accessible description.";
        return m_widget->accessibleDescription();
    default:
        qCDebug(appLog) << "Returning empty string for unknown text type.";
        return QString();
    }
}

QString AccessibleWidget::text(int startOffset, int endOffset) const
{
    qCDebug(appLog) << "AccessibleWidget::text(int, int) called.";
    Q_UNUSED(startOffset)
    Q_UNUSED(endOffset)

    return QString();
}

void AccessibleWidget::selection(int selectionIndex, int *startOffset, int *endOffset) const
{
    qCDebug(appLog) << "AccessibleWidget::selection called.";
    Q_UNUSED(selectionIndex)
    Q_UNUSED(startOffset)
    Q_UNUSED(endOffset)
}

int AccessibleWidget::selectionCount() const
{
    qCDebug(appLog) << "AccessibleWidget::selectionCount called.";
    return 0;
}

void AccessibleWidget::addSelection(int startOffset, int endOffset)
{
    qCDebug(appLog) << "AccessibleWidget::addSelection called.";
    Q_UNUSED(startOffset)
    Q_UNUSED(endOffset)
}

void AccessibleWidget::removeSelection(int selectionIndex)
{
    qCDebug(appLog) << "AccessibleWidget::removeSelection called.";
    Q_UNUSED(selectionIndex)
}

void AccessibleWidget::setSelection(int selectionIndex, int startOffset, int endOffset)
{
    qCDebug(appLog) << "AccessibleWidget::setSelection called.";
    Q_UNUSED(selectionIndex)
    Q_UNUSED(startOffset)
    Q_UNUSED(endOffset)
}

int AccessibleWidget::cursorPosition() const
{
    qCDebug(appLog) << "AccessibleWidget::cursorPosition called.";
    return 0;
}

void AccessibleWidget::setCursorPosition(int position)
{
    qCDebug(appLog) << "AccessibleWidget::setCursorPosition called.";
    Q_UNUSED(position)
}

int AccessibleWidget::characterCount() const
{
    qCDebug(appLog) << "AccessibleWidget::characterCount called.";
    return 0;
}

QRect AccessibleWidget::characterRect(int offset) const
{
    qCDebug(appLog) << "AccessibleWidget::characterRect called.";
    Q_UNUSED(offset)

    return QRect();
}

int AccessibleWidget::offsetAtPoint(const QPoint &point) const
{
    qCDebug(appLog) << "AccessibleWidget::offsetAtPoint called.";
    Q_UNUSED(point)

    return 0;
}

void AccessibleWidget::scrollToSubstring(int startIndex, int endIndex)
{
    qCDebug(appLog) << "AccessibleWidget::scrollToSubstring called.";
    Q_UNUSED(startIndex)
    Q_UNUSED(endIndex)
}

QString AccessibleWidget::attributes(int offset, int *startOffset, int *endOffset) const
{
    qCDebug(appLog) << "AccessibleWidget::attributes called.";
    Q_UNUSED(offset)
    Q_UNUSED(startOffset)
    Q_UNUSED(endOffset)

    return QString();
}

AccessibleLabel::AccessibleLabel(QLabel *label)
    : AccessibleWidget(label)
    , m_label(label)
{
    qCDebug(appLog) << "AccessibleLabel::AccessibleLabel constructor called.";
}

AccessibleLabel::~AccessibleLabel()
{
    // qCDebug(appLog) << "AccessibleLabel::~AccessibleLabel destructor called.";
}

QString AccessibleLabel::text(int startOffset, int endOffset) const
{
    qCDebug(appLog) << "AccessibleLabel::text(int, int) called.";
    if (Q_NULLPTR != m_label) {
        qCDebug(appLog) << "Label is not null.";
        if (m_label->objectName().contains("@==@")) {
            qCDebug(appLog) << "Label object name contains '@==@', removing it.";
            return m_label->objectName().remove("@==@");
        } else {
            qCDebug(appLog) << "Label object name does not contain '@==@', returning text.";
            return m_label->text();
        }
    }

    qCDebug(appLog) << "Label is null, calling base class text.";
    return AccessibleWidget::text(startOffset, endOffset);
}

AccessibleButton::AccessibleButton(QPushButton *button)
    : AccessibleWidget(button)
    , m_button(button)
{
    qCDebug(appLog) << "AccessibleButton::AccessibleButton constructor called.";
}

AccessibleButton::~AccessibleButton()
{
    qCDebug(appLog) << "AccessibleButton::~AccessibleButton destructor called.";
}

QString AccessibleButton::text(int startOffset, int endOffset) const
{
    qCDebug(appLog) << "AccessibleButton::text(int, int) called.";
    if (Q_NULLPTR != m_button) {
        qCDebug(appLog) << "Button is not null, returning text.";
        return m_button->text();
    }

    qCDebug(appLog) << "Button is null, calling base class text.";
    return AccessibleWidget::text(startOffset, endOffset);
}

AccessibleComboBox::AccessibleComboBox(QComboBox *comboBox)
    : AccessibleWidget(comboBox)
    , m_comboBox(comboBox)
{
    qCDebug(appLog) << "AccessibleComboBox::AccessibleComboBox constructor called.";
}

AccessibleComboBox::~AccessibleComboBox()
{
    qCDebug(appLog) << "AccessibleComboBox::~AccessibleComboBox destructor called.";
}

QString AccessibleComboBox::text(int startOffset, int endOffset) const
{
    qCDebug(appLog) << "AccessibleComboBox::text(int, int) called.";
    if (Q_NULLPTR != m_comboBox) {
        qCDebug(appLog) << "ComboBox is not null, returning current text.";
        return m_comboBox->currentText();
    }

    qCDebug(appLog) << "ComboBox is null, calling base class text.";
    return AccessibleWidget::text(startOffset, endOffset);
}
