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

#include "leftviewsortfilter.h"
#include "common/vnoteforlder.h"
#include "common/standarditemcommon.h"

/**
 * @brief LeftViewSortFilter::LeftViewSortFilter
 * @param parent
 */
LeftViewSortFilter::LeftViewSortFilter(QObject *parent)
    : QSortFilterProxyModel(parent)
{
}

/**
 * @brief LeftViewSortFilter::lessThan
 * @param source_left
 * @param source_right
 * @return true source_left小于source_right
 */
bool LeftViewSortFilter::lessThan(
    const QModelIndex &source_left,
    const QModelIndex &source_right) const
{
    StandardItemCommon::StandardItemType leftSourceType = StandardItemCommon::getStandardItemType(source_left);
    StandardItemCommon::StandardItemType rightSourceType = StandardItemCommon::getStandardItemType(source_left);

    if (leftSourceType == StandardItemCommon::NOTEPADITEM && rightSourceType == StandardItemCommon::NOTEPADITEM) {
        VNoteFolder *leftSource = reinterpret_cast<VNoteFolder *>(
            StandardItemCommon::getStandardItemData(source_left));

        VNoteFolder *rightSource = reinterpret_cast<VNoteFolder *>(
            StandardItemCommon::getStandardItemData(source_right));

        if (-1 != leftSource->sort_number && -1 != rightSource->sort_number){
            return leftSource->sort_number < rightSource->sort_number;
        } else {
            return leftSource->createTime < rightSource->createTime;
        }
    }

    return QSortFilterProxyModel::lessThan(source_left, source_right);
}
