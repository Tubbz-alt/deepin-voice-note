#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#include<DTextEdit>
DWIDGET_USE_NAMESPACE
struct VNoteItem;
struct VNoteBlock;

class TextNoteEditPrivate;

class TextNoteEdit : public DTextEdit
{
    Q_OBJECT
public:
     explicit TextNoteEdit(QWidget *parent = nullptr);
     void selectText(const QPoint &globalPos,QTextCursor::MoveOperation op);
     void clearSelection();
     void removeSelectText();
     QString getSelectFragment();
     bool hasSelection();

public slots:
     void onChangeTheme();

signals:
    void sigFocusIn();
    void sigFocusOut();

private:
     bool m_menuPop {false};
protected:
    void focusInEvent(QFocusEvent *e) override;
    void focusOutEvent(QFocusEvent *e) override;
    void wheelEvent(QWheelEvent *e) override;
    void contextMenuEvent(QContextMenuEvent *e) override;
    void keyPressEvent(QKeyEvent *e) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    bool eventFilter(QObject *o, QEvent *e) override;
};

#endif // TEXTEDITITEM_H