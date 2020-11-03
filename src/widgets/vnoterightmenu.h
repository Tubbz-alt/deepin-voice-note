#pragma once

#include <DMenu>

#include <QMouseEvent>
#include <QDebug>
#include <QTimer>

DWIDGET_USE_NAMESPACE
class VNoteRightMenu : public DMenu
{
    Q_OBJECT
public:
    explicit VNoteRightMenu(QWidget *parent = nullptr);
    ~VNoteRightMenu() override;
    void setPressPointY(QPoint point);

protected:
    //初始化信号槽连接
    void initConnections();
    //处理鼠标move事件
    void mouseMoveEvent(QMouseEvent *eve)override;
    //处理鼠标release事件
    void mouseReleaseEvent(QMouseEvent *eve)override;
private:
    QPoint m_touchPoint;
    bool m_moved {false};
    QTimer *m_timer;
signals:
    //触摸移动信号
    void menuTouchMoved();
    //触摸释放信号
    void menuTouchReleased();
private slots:

};
