/*
 * Copyright (C) 2019 Vlad Zagorodniy <vladzzag@gmail.com>
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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

// Own
#include "DynamicWallpaperModel.h"
#include "DynamicWallpaperData.h"
#include "SunPath.h"
#include "SunPosition.h"
#include "utils.h"

// std
#include <algorithm>

static qreal computeTime(const SunPath* path, const QVector3D& position)
{
    return computeAngle(path->normal(), path->center(), position) / 360;
}

DynamicWallpaperModel::DynamicWallpaperModel(const DynamicWallpaperData* wallpaper, qreal latitude, qreal longitude)
    : m_dateTime(QDateTime::currentDateTime())
    , m_latitude(latitude)
    , m_longitude(longitude)
    , m_time(0)
{
    m_sunPath = std::make_unique<SunPath>(m_dateTime, latitude, longitude);
    if (!m_sunPath->isValid())
        return;

    const QVector<Image> images = wallpaper->images();
    for (const Image& image : images) {
        const QVector3D projection = m_sunPath->project(image.position);
        if (projection.isNull())
            continue;
        Knot knot { computeTime(m_sunPath.get(), projection), image.url };
        m_knots << knot;
    }
    std::sort(m_knots.begin(), m_knots.end());

    update();
}

DynamicWallpaperModel::~DynamicWallpaperModel()
{
}

bool DynamicWallpaperModel::isExpired() const
{
    // Rebuild the model each hour.
    const QDateTime now = QDateTime::currentDateTime();
    return qAbs(now.secsTo(m_dateTime)) > 3600;
}

bool DynamicWallpaperModel::isValid() const
{
    return m_sunPath->isValid();
}

QUrl DynamicWallpaperModel::bottomLayer() const
{
    return currentBottomKnot().url;
}

QUrl DynamicWallpaperModel::topLayer() const
{
    return currentTopKnot().url;
}

qreal DynamicWallpaperModel::blendFactor() const
{
    const Knot from = currentBottomKnot();
    const Knot to = currentTopKnot();

    if (from < to)
        return (m_time - from.time) / (to.time - from.time);

    const qreal duration = 1 - from.time + to.time;

    if (from.time <= m_time)
        return (m_time - from.time) / duration;

    return (1 - from.time + m_time) / duration;
}

DynamicWallpaperModel::Knot DynamicWallpaperModel::currentBottomKnot() const
{
    const Knot dummy { m_time, QUrl() };

    if (dummy < m_knots.first())
        return m_knots.last();

    if (m_knots.last() <= dummy)
        return m_knots.last();

    auto it = std::lower_bound(m_knots.begin(), m_knots.end(), dummy);
    if (dummy < *it)
        return *std::prev(it);

    return *it;
}

DynamicWallpaperModel::Knot DynamicWallpaperModel::currentTopKnot() const
{
    const Knot dummy { m_time, QUrl() };

    if (dummy < m_knots.first())
        return m_knots.first();

    if (m_knots.last() <= dummy)
        return m_knots.first();

    auto it = std::lower_bound(m_knots.begin(), m_knots.end(), dummy);
    if (dummy < *it)
        return *it;

    return *std::next(it);
}

void DynamicWallpaperModel::update()
{
    const QDateTime now(QDateTime::currentDateTime());
    const SunPosition position(now, m_latitude, m_longitude);
    const QVector3D projection(m_sunPath->project(position));
    if (!projection.isNull())
        m_time = computeTime(m_sunPath.get(), projection);
}