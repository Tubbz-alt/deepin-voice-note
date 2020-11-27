/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     liuyanga <liuyanga@uniontech.com>
*
* Maintainer: liuyanga <liuyanga@uniontech.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "vnoteplaywidget.h"
#include "vnote2siconbutton.h"

#include "common/vnoteitem.h"
#include "common/utils.h"

#include <QGridLayout>
#include <QDebug>

#include <DDialogCloseButton>

/**
 * @brief VNotePlayWidget::VNotePlayWidget
 * @param parent
 */
VNotePlayWidget::VNotePlayWidget(QWidget *parent)
    : DFloatingWidget(parent)
{
    initUI();
    initPlayer();
    initConnection();
}

/**
 * @brief VNotePlayWidget::initUI
 */
void VNotePlayWidget::initUI()
{
    m_playerBtn = new VNote2SIconButton("play.svg", "pause.svg", this);
    m_playerBtn->setIconSize(QSize(40, 40));
    m_playerBtn->setFixedSize(QSize(48, 48));
    m_playerBtn->setFocusPolicy(Qt::NoFocus);

    m_timeLab = new DLabel(this);
    m_timeLab->setText("00:00/00:00");
    m_timeLab->setFixedWidth(124);
    m_sliderHover = new DWidget(this);
    m_nameLab = new DLabel(m_sliderHover);
    m_nameLab->setContentsMargins(8, 0, 0, 0);
    m_slider = new DSlider(Qt::Horizontal, m_sliderHover);
    m_slider->setMinimum(0);
    m_slider->setValue(0);

    m_closeBtn = new DDialogCloseButton(this);
    m_closeBtn->setIconSize(QSize(26, 26));
    DStyle::setFocusRectVisible(m_closeBtn, false);

    QVBoxLayout *sliderLayout = new QVBoxLayout;
    sliderLayout->addWidget(m_nameLab, Qt::AlignLeft);
    sliderLayout->addWidget(m_slider, Qt::AlignTop);
    sliderLayout->setSpacing(0);
    m_sliderHover->setLayout(sliderLayout);
    sliderLayout->setContentsMargins(0, 5, 0, 5);
    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(m_playerBtn);
    mainLayout->addWidget(m_sliderHover);
    mainLayout->addWidget(m_timeLab, 0, Qt::AlignLeft);
    mainLayout->addWidget(m_closeBtn);
    mainLayout->setContentsMargins(13, 0, 13, 0);
    mainLayout->setSizeConstraint(QLayout::SetNoConstraint);
    this->setLayout(mainLayout);
    m_sliderHover->installEventFilter(this);
    //m_slider->installEventFilter(this);
}

/**
 * @brief VNotePlayWidget::initPlayer
 */
void VNotePlayWidget::initPlayer()
{
    m_player = new VlcPalyer(this);
}

/**
 * @brief VNotePlayWidget::initConnection
 */
void VNotePlayWidget::initConnection()
{
    connect(m_player, &VlcPalyer::positionChanged,
            this, &VNotePlayWidget::onVoicePlayPosChange, Qt::QueuedConnection);
    connect(m_player, &VlcPalyer::durationChanged,
            this, &VNotePlayWidget::onDurationChanged, Qt::QueuedConnection);
    connect(m_player, &VlcPalyer::playEnd,
            this, &VNotePlayWidget::onCloseBtnClicked, Qt::QueuedConnection);

    connect(m_playerBtn, &VNote2SIconButton::clicked,
            this, &VNotePlayWidget::onPlayerBtnClicked);

    connect(m_closeBtn, &DIconButton::clicked,
            this, &VNotePlayWidget::onCloseBtnClicked);

    connect(m_slider, &DSlider::sliderPressed,
            this, &VNotePlayWidget::onSliderPressed);
    connect(m_slider, &DSlider::sliderReleased,
            this, &VNotePlayWidget::onSliderReleased);
    connect(m_slider, &DSlider::sliderMoved,
            this, &VNotePlayWidget::onSliderMove);
}

/**
 * @brief VNotePlayWidget::onVoicePlayPosChange
 * @param pos
 */
void VNotePlayWidget::onVoicePlayPosChange(qint64 pos)
{
    if (m_sliderReleased == true) {
        onSliderMove(static_cast<int>(pos));
    }
}

/**
 * @brief VNotePlayWidget::onCloseBtnClicked
 */
void VNotePlayWidget::onCloseBtnClicked()
{
    m_slider->setValue(0);
    m_player->stop();
    m_sliderReleased = true;
    emit sigStopVoice();
}

/**
 * @brief VNotePlayWidget::onSliderPressed
 */
void VNotePlayWidget::onSliderPressed()
{
    m_sliderReleased = false;
}

/**
 * @brief VNotePlayWidget::onSliderReleased
 */
void VNotePlayWidget::onSliderReleased()
{
    m_sliderReleased = true;
    VlcPalyer::VlcState state = m_player->getState();
    if (state == VlcPalyer::Playing || state == VlcPalyer::Paused) {
        int pos = m_slider->value();
        if (pos >= m_slider->maximum()) {
            onCloseBtnClicked();
        } else {
            m_player->setPosition(pos);
        }
    }
}

/**
 * @brief VNotePlayWidget::onSliderMove
 * @param pos
 */
void VNotePlayWidget::onSliderMove(int pos)
{
    if (m_voiceBlock) {
        qint64 tmpPos = pos > m_voiceBlock->voiceSize ? m_voiceBlock->voiceSize : pos;
        m_timeLab->setText(Utils::formatMillisecond(tmpPos, 0) + "/" + Utils::formatMillisecond(m_voiceBlock->voiceSize));
    }

    if (m_sliderReleased == true) {
        m_slider->setValue(pos);
    }
}

/**
 * @brief VNotePlayWidget::eventFilter
 * @param o
 * @param e
 * @return false 不过滤
 */
bool VNotePlayWidget::eventFilter(QObject *o, QEvent *e)
{
    Q_UNUSED(o)
    if (e->type() == QEvent::Enter) {
        m_nameLab->setVisible(false);
    } else if (e->type() == QEvent::Leave) {
        m_nameLab->setVisible(true);
    }
    return false;
}

/**
 * @brief VNotePlayWidget::getPlayerStatus
 * @return 播放状态
 */
VlcPalyer::VlcState VNotePlayWidget::getPlayerStatus()
{
    return m_player->getState();
}

/**
 * @brief VNotePlayWidget::onDurationChanged
 * @param duration
 */
void VNotePlayWidget::onDurationChanged(qint64 duration)
{
    if (duration && m_slider->maximum() != duration) {
        m_slider->setMaximum(static_cast<int>(duration));
    }
}

/**
 * @brief VNotePlayWidget::onPlayerBtnClicked
 */
void VNotePlayWidget::onPlayerBtnClicked()
{
   playVideo(m_voiceBlock, true);
}

void VNotePlayWidget::playVideo(const VNVoiceBlock *voiceData, const bool &bIsSame)
{
    if(bIsSame){
        VlcPalyer::VlcState status = getPlayerStatus();
        if (status == VlcPalyer::Playing) {
            m_player->pause();
            m_playerBtn->setState(VNote2SIconButton::Normal);
            emit sigPauseVoice();
        } else if(VlcPalyer::Paused){
            m_player->play();
            m_playerBtn->setState(VNote2SIconButton::Press);
            emit sigPlayVoice();
        }
    }else {
        m_slider->setValue(0);
        m_voiceBlock = voiceData;
        m_player->setFilePath(m_voiceBlock->voicePath);
        m_nameLab->setText(voiceData->voiceTitle);
        m_timeLab->setText(Utils::formatMillisecond(0, 0) + "/" + Utils::formatMillisecond(voiceData->voiceSize));
        m_playerBtn->setState(VNote2SIconButton::Press);
        m_player->play();
        emit sigPlayVoice();
    }
}
