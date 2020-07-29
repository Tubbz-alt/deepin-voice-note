/*
* Copyright (C) 2019 ~ 2019 UnionTech Software Technology Co.,Ltd.
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
#include "setting.h"

#include <QStandardPaths>
#include <QFileInfo>
#include <QDir>
#include <QDebug>

#include <DSettingsOption>

static setting*  settinginstance = nullptr;

CustemBackend::CustemBackend(const QString &filepath, QObject *parent)
    :DSettingsBackend(parent)
{
   m_settings = new QSettings(filepath,QSettings::IniFormat);
}

CustemBackend::~CustemBackend()
{
    ;
}

QStringList CustemBackend::keys() const
{
    QStringList keyList = m_settings->allKeys();
    for(auto &it : keyList){
        if(it.indexOf('.') == -1){
            it.insert(0,"old.");
        }
    }
    return keyList;
}

QVariant CustemBackend::getOption(const QString &key) const
{
    if(key.startsWith("old.")){
        return  m_settings->value(key.right(key.length() - 4));
    }
    return m_settings->value(key);
}

void CustemBackend::doSync()
{
    m_settings->sync();
}

void CustemBackend::doSetOption(const QString &key, const QVariant &value)
{
    m_writeLock.lock();
    if(key.startsWith("old.")){
         m_settings->setValue(key.right(key.length() - 4),value);
    }else {
        m_settings->setValue(key, value);
    }
    m_settings->sync();
    m_writeLock.unlock();
}

setting::setting(QObject *parent) : QObject(parent)
{
    QString vnoteConfigBasePath =
            QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);

    QFileInfo configDir(vnoteConfigBasePath+QDir::separator());

    if (!configDir.exists()) {
        QDir().mkpath(configDir.filePath());
    }
    
    m_backend = new CustemBackend(configDir.filePath() + QString("config.conf"), this);
    m_setting = DSettings::fromJsonFile(":/deepin-voice-note-setting.json");
    m_setting->setBackend(m_backend);
}

void setting::setOption(const QString &key, const QVariant &value)
{
    if(getOption(key) != value){
        m_setting->setOption(key, value);
        m_backend->doSetOption(key, value);
    }
}

QVariant setting::getOption(const QString &key)
{
    return m_setting->getOption(key);
}

DSettings* setting::getSetting()
{
    return m_setting;
}

setting *setting::instance()
{
    if (settinginstance == nullptr) {
        settinginstance = new setting;
    }
    return settinginstance;
}

