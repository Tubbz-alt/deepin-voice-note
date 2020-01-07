#include "textnoteedit.h"

#include <QScrollBar>

#include <DApplicationHelper>

TextNoteEdit::TextNoteEdit(QWidget *parent)
    : DTextEdit(parent)
{
    DPalette pb = DApplicationHelper::instance()->palette(this);
    pb.setBrush(DPalette::Text,pb.color(DPalette::Active,DPalette::WindowText));
    pb.setBrush(DPalette::Button, pb.color(DPalette::ItemBackground));
    this->setPalette(pb);
}

void TextNoteEdit::focusInEvent(QFocusEvent *e)
{
    DTextEdit::focusInEvent(e);
    emit sigFocusIn();
}

void TextNoteEdit::focusOutEvent(QFocusEvent *e)
{
    DTextEdit::focusOutEvent(e);
    if(!m_menuPop){
        QScrollBar *bar = this->verticalScrollBar();
        bar->setValue(bar->minimum());
        emit sigFocusOut();
    }
}

void TextNoteEdit::enterEvent(QEvent *event)
{
    DTextEdit::enterEvent(event);
    DPalette pb = DApplicationHelper::instance()->palette(this);
    pb.setBrush(DPalette::Text,pb.color(DPalette::Active,DPalette::WindowText));
    pb.setBrush(DPalette::Button, pb.color(DPalette::Light));
    this->setPalette(pb);
}

void TextNoteEdit::leaveEvent(QEvent *event)
{
    DTextEdit::leaveEvent(event);
    DPalette pb = DApplicationHelper::instance()->palette(this);
    pb.setBrush(DPalette::Text,pb.color(DPalette::Active,DPalette::WindowText));
    pb.setBrush(DPalette::Button, pb.color(DPalette::ItemBackground));
    this->setPalette(pb);
}

void TextNoteEdit::contextMenuEvent(QContextMenuEvent *e)
{
    m_menuPop = true;
    DTextEdit::contextMenuEvent(e);
    m_menuPop = false;
}