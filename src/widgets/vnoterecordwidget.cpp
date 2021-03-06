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

#include "vnoterecordwidget.h"
#include "common/utils.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QStandardPaths>
#include <QDir>
#include <QDateTime>
#include <QDebug>

/**
 * @brief VNoteRecordWidget::VNoteRecordWidget
 * @param parent
 */
VNoteRecordWidget::VNoteRecordWidget(QWidget *parent)
    : DFloatingWidget(parent)
{
    initUi();
    initRecord();
    initConnection();
}

/**
 * @brief VNoteRecordWidget::initUi
 */
void VNoteRecordWidget::initUi()
{
    m_pauseBtn = new VNoteIconButton(this, "pause_rec_normal.svg", "pause_rec_hover.svg", "pause_rec_press.svg");
    m_pauseBtn->setIconSize(QSize(60, 60));
    m_pauseBtn->setFlat(true);

    m_continueBtn = new VNoteIconButton(this, "record_normal.svg", "record_hover.svg", "record_press.svg");
    m_continueBtn->setIconSize(QSize(60, 60));
    m_continueBtn->setFlat(true);

    m_finshBtn = new VNoteIconButton(this, "stop_rec_normal.svg", "stop_rec_hover.svg", "stop_rec_press.svg");
    m_finshBtn->setIconSize(QSize(60, 60));
    m_finshBtn->setFlat(true);

    QFont recordTimeFont;
    recordTimeFont.setPixelSize(14);
    m_timeLabel = new DLabel(this);
    m_timeLabel->setFont(recordTimeFont);
    m_timeLabel->setText("00:00");

    m_waveForm = new VNWaveform(this);
    QVBoxLayout *waveLayout = new QVBoxLayout;
    waveLayout->addWidget(m_waveForm);
    waveLayout->setContentsMargins(0, 15, 10, 15);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    QGridLayout *btnLayout = new QGridLayout;
    btnLayout->addWidget(m_pauseBtn, 0, 0);
    btnLayout->addWidget(m_continueBtn, 0, 0);
    mainLayout->addLayout(btnLayout);
    mainLayout->addLayout(waveLayout, 1);
    mainLayout->addWidget(m_timeLabel);
    mainLayout->addWidget(m_finshBtn);
    mainLayout->setContentsMargins(6, 0, 6, 3);
    this->setLayout(mainLayout);
    m_continueBtn->setVisible(false);
}

/**
 * @brief VNoteRecordWidget::initRecord
 */
void VNoteRecordWidget::initRecord()
{
    m_audioRecoder = new GstreamRecorder(this);
}

/**
 * @brief VNoteRecordWidget::initConnection
 */
void VNoteRecordWidget::initConnection()
{
    connect(m_pauseBtn, &VNoteIconButton::clicked, this, &VNoteRecordWidget::onPauseRecord);
    connect(m_continueBtn, &VNoteIconButton::clicked, this, &VNoteRecordWidget::onContinueRecord);
    connect(m_finshBtn, &VNoteIconButton::clicked, this, &VNoteRecordWidget::stopRecord);
    connect(m_audioRecoder, SIGNAL(audioBufferProbed(const QAudioBuffer &)),
            this, SLOT(onAudioBufferProbed(const QAudioBuffer &)));
    connect(m_audioRecoder, &GstreamRecorder::recordFinshed, this, &VNoteRecordWidget::onGstreamerFinshRecord, Qt::QueuedConnection);
}

/**
 * @brief VNoteRecordWidget::initRecordPath
 */
void VNoteRecordWidget::initRecordPath()
{
    m_recordDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    m_recordDir += "/voicenote/";
    QDir dir(m_recordDir);
    if (!dir.exists()) {
        dir.mkdir(m_recordDir);
    }
}

/**
 * @brief VNoteRecordWidget::onFinshRecord
 */
void VNoteRecordWidget::stopRecord()
{
    m_audioRecoder->stopRecord();
}

/**
 * @brief VNoteRecordWidget::onPauseRecord
 */
void VNoteRecordWidget::onPauseRecord()
{
    m_pauseBtn->setVisible(false);
    m_continueBtn->setVisible(true);
    m_audioRecoder->pauseRecord();
}

/**
 * @brief VNoteRecordWidget::onContinueRecord
 * @return true 成功
 */
bool VNoteRecordWidget::onContinueRecord()
{
    m_pauseBtn->setVisible(true);
    m_continueBtn->setVisible(false);
    if (!m_audioRecoder->startRecord()) {
        stopRecord();

        return false;
    }
    return true;
}

/**
 * @brief VNoteRecordWidget::startRecord
 * @return true 成功
 */
bool VNoteRecordWidget::startRecord()
{
    QString fileName = QDateTime::currentDateTime()
                           .toString("yyyyMMddhhmmss")
                       + ".mp3";
    initRecordPath();
    m_recordMsec = 0;
    m_recordPath = m_recordDir + fileName;
    m_audioRecoder->setOutputFile(m_recordPath);
    m_timeLabel->setText("00:00");
    return onContinueRecord();
}

/**
 * @brief VNoteRecordWidget::getRecordPath
 * @return 录音文件路径
 */
QString VNoteRecordWidget::getRecordPath() const
{
    return m_recordPath;
}

/**
 * @brief VNoteRecordWidget::onAudioBufferProbed
 * @param buffer
 */
void VNoteRecordWidget::onAudioBufferProbed(const QAudioBuffer &buffer)
{
    qint64 msec = buffer.startTime();
    if (msec != m_recordMsec) {
        onRecordDurationChange(msec);
    }
    m_waveForm->onAudioBufferProbed(buffer);
}

/**
 * @brief VNoteRecordWidget::onRecordDurationChange
 * @param duration
 */
void VNoteRecordWidget::onRecordDurationChange(qint64 duration)
{
    m_recordMsec = duration;
    QString strTime = Utils::formatMillisecond(duration, 0);
    m_timeLabel->setText(strTime);

    if (duration >= (60 * 60 * 1000)) {
        stopRecord();
    }
}

/**
 * @brief VNoteRecordWidget::setAudioDevice
 * @param device
 */
void VNoteRecordWidget::setAudioDevice(QString device)
{
    m_audioRecoder->setDevice(device);
}

/**
 * @brief VNoteRecordWidget::onGstreamerFinshRecord
 */
void VNoteRecordWidget::onGstreamerFinshRecord()
{
    m_audioRecoder->setStateToNull();
    emit sigFinshRecord(m_recordPath, m_recordMsec);
}
