// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only

#include "accessiblewidget.h"
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QDebug>

AccessibleWidget::AccessibleWidget(QWidget *widget)
    : QAccessibleWidget(widget)
    , m_widget(widget)
{
    qDebug() << "AccessibleWidget::AccessibleWidget constructor called.";
}

AccessibleWidget::~AccessibleWidget()
{
    // qDebug() << "AccessibleWidget::~AccessibleWidget destructor called.";
}

void *AccessibleWidget::interface_cast(QAccessible::InterfaceType t)
{
    qDebug() << "AccessibleWidget::interface_cast called with type:" << t;
    switch (t) {
    case QAccessible::ActionInterface:
        qDebug() << "Interface cast to ActionInterface.";
        return static_cast<QAccessibleActionInterface *>(this);
    case QAccessible::TextInterface:
        qDebug() << "Interface cast to TextInterface.";
        return static_cast<QAccessibleTextInterface *>(this);
    default:
        qDebug() << "Interface cast to unknown type.";
        return nullptr;
    }
}

QString AccessibleWidget::text(QAccessible::Text t) const
{
    qDebug() << "AccessibleWidget::text called with text type:" << t;
    switch (t) {
    case QAccessible::Name:
        qDebug() << "Returning accessible name.";
        return m_widget->accessibleName();
    case QAccessible::Description:
        qDebug() << "Returning accessible description.";
        return m_widget->accessibleDescription();
    default:
        qDebug() << "Returning empty string for unknown text type.";
        return QString();
    }
}

QString AccessibleWidget::text(int startOffset, int endOffset) const
{
    qDebug() << "AccessibleWidget::text(int, int) called.";
    Q_UNUSED(startOffset)
    Q_UNUSED(endOffset)

    return QString();
}

void AccessibleWidget::selection(int selectionIndex, int *startOffset, int *endOffset) const
{
    qDebug() << "AccessibleWidget::selection called.";
    Q_UNUSED(selectionIndex)
    Q_UNUSED(startOffset)
    Q_UNUSED(endOffset)
}

int AccessibleWidget::selectionCount() const
{
    qDebug() << "AccessibleWidget::selectionCount called.";
    return 0;
}

void AccessibleWidget::addSelection(int startOffset, int endOffset)
{
    qDebug() << "AccessibleWidget::addSelection called.";
    Q_UNUSED(startOffset)
    Q_UNUSED(endOffset)
}

void AccessibleWidget::removeSelection(int selectionIndex)
{
    qDebug() << "AccessibleWidget::removeSelection called.";
    Q_UNUSED(selectionIndex)
}

void AccessibleWidget::setSelection(int selectionIndex, int startOffset, int endOffset)
{
    qDebug() << "AccessibleWidget::setSelection called.";
    Q_UNUSED(selectionIndex)
    Q_UNUSED(startOffset)
    Q_UNUSED(endOffset)
}

int AccessibleWidget::cursorPosition() const
{
    qDebug() << "AccessibleWidget::cursorPosition called.";
    return 0;
}

void AccessibleWidget::setCursorPosition(int position)
{
    qDebug() << "AccessibleWidget::setCursorPosition called.";
    Q_UNUSED(position)
}

int AccessibleWidget::characterCount() const
{
    qDebug() << "AccessibleWidget::characterCount called.";
    return 0;
}

QRect AccessibleWidget::characterRect(int offset) const
{
    qDebug() << "AccessibleWidget::characterRect called.";
    Q_UNUSED(offset)

    return QRect();
}

int AccessibleWidget::offsetAtPoint(const QPoint &point) const
{
    qDebug() << "AccessibleWidget::offsetAtPoint called.";
    Q_UNUSED(point)

    return 0;
}

void AccessibleWidget::scrollToSubstring(int startIndex, int endIndex)
{
    qDebug() << "AccessibleWidget::scrollToSubstring called.";
    Q_UNUSED(startIndex)
    Q_UNUSED(endIndex)
}

QString AccessibleWidget::attributes(int offset, int *startOffset, int *endOffset) const
{
    qDebug() << "AccessibleWidget::attributes called.";
    Q_UNUSED(offset)
    Q_UNUSED(startOffset)
    Q_UNUSED(endOffset)

    return QString();
}

AccessibleLabel::AccessibleLabel(QLabel *label)
    : AccessibleWidget(label)
    , m_label(label)
{
    qDebug() << "AccessibleLabel::AccessibleLabel constructor called.";
}

AccessibleLabel::~AccessibleLabel()
{
    // qDebug() << "AccessibleLabel::~AccessibleLabel destructor called.";
}

QString AccessibleLabel::text(int startOffset, int endOffset) const
{
    qDebug() << "AccessibleLabel::text(int, int) called.";
    if (Q_NULLPTR != m_label) {
        qDebug() << "Label is not null.";
        if (m_label->objectName().contains("@==@")) {
            qDebug() << "Label object name contains '@==@', removing it.";
            return m_label->objectName().remove("@==@");
        } else {
            qDebug() << "Label object name does not contain '@==@', returning text.";
            return m_label->text();
        }
    }

    qDebug() << "Label is null, calling base class text.";
    return AccessibleWidget::text(startOffset, endOffset);
}

AccessibleButton::AccessibleButton(QPushButton *button)
    : AccessibleWidget(button)
    , m_button(button)
{
    qDebug() << "AccessibleButton::AccessibleButton constructor called.";
}

AccessibleButton::~AccessibleButton()
{
    qDebug() << "AccessibleButton::~AccessibleButton destructor called.";
}

QString AccessibleButton::text(int startOffset, int endOffset) const
{
    qDebug() << "AccessibleButton::text(int, int) called.";
    if (Q_NULLPTR != m_button) {
        qDebug() << "Button is not null, returning text.";
        return m_button->text();
    }

    qDebug() << "Button is null, calling base class text.";
    return AccessibleWidget::text(startOffset, endOffset);
}

AccessibleComboBox::AccessibleComboBox(QComboBox *comboBox)
    : AccessibleWidget(comboBox)
    , m_comboBox(comboBox)
{
    qDebug() << "AccessibleComboBox::AccessibleComboBox constructor called.";
}

AccessibleComboBox::~AccessibleComboBox()
{
    qDebug() << "AccessibleComboBox::~AccessibleComboBox destructor called.";
}

QString AccessibleComboBox::text(int startOffset, int endOffset) const
{
    qDebug() << "AccessibleComboBox::text(int, int) called.";
    if (Q_NULLPTR != m_comboBox) {
        qDebug() << "ComboBox is not null, returning current text.";
        return m_comboBox->currentText();
    }

    qDebug() << "ComboBox is null, calling base class text.";
    return AccessibleWidget::text(startOffset, endOffset);
}
