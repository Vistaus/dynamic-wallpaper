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
#include "DynamicWallpaper.h"
#include "DynamicWallpaperData.h"
#include "DynamicWallpaperModel.h"

DynamicWallpaper::DynamicWallpaper(QObject* parent)
    : QObject(parent)
{
}

DynamicWallpaper::~DynamicWallpaper()
{
}

QUrl DynamicWallpaper::bottomLayer() const
{
    return m_bottomLayer;
}

QUrl DynamicWallpaper::topLayer() const
{
    return m_topLayer;
}

qreal DynamicWallpaper::blendFactor() const
{
    return m_blendFactor;
}

DynamicWallpaper::Status DynamicWallpaper::status() const
{
    return m_status;
}

QString DynamicWallpaper::error() const
{
    return m_error;
}

QString DynamicWallpaper::wallpaperId() const
{
    return m_wallpaperId;
}

void DynamicWallpaper::setWallpaperId(const QString& id)
{
    if (m_wallpaperId == id)
        return;
    m_wallpaperId = id;
    emit wallpaperIdChanged();
    reloadWallpaper();
    reloadModel();
    scheduleUpdate();
}

qreal DynamicWallpaper::latitude() const
{
    return m_latitude;
}

void DynamicWallpaper::setLatitude(qreal latitude)
{
    if (m_latitude == latitude)
        return;
    m_latitude = latitude;
    emit latitudeChanged();
    reloadModel();
    scheduleUpdate();
}

qreal DynamicWallpaper::longitude() const
{
    return m_longitude;
}

void DynamicWallpaper::setLongitude(qreal longitude)
{
    if (m_longitude == longitude)
        return;
    m_longitude = longitude;
    emit longitudeChanged();
    reloadModel();
    scheduleUpdate();
}

void DynamicWallpaper::update()
{
    if (m_status == Status::Error)
        return;
    if (m_model->isExpired())
        reloadModel();
    if (m_status == Status::Error)
        return;
    m_model->update();
    setBottomLayer(m_model->bottomLayer());
    setTopLayer(m_model->topLayer());
    setBlendFactor(m_model->blendFactor());
}

void DynamicWallpaper::setBottomLayer(const QUrl& url)
{
    if (m_bottomLayer == url)
        return;
    m_bottomLayer = url;
    emit bottomLayerChanged();
}

void DynamicWallpaper::setTopLayer(const QUrl& url)
{
    if (m_topLayer == url)
        return;
    m_topLayer = url;
    emit topLayerChanged();
}

void DynamicWallpaper::setBlendFactor(qreal factor)
{
    if (m_blendFactor == factor)
        return;
    m_blendFactor = factor;
    emit blendFactorChanged();
}

void DynamicWallpaper::setStatus(Status status)
{
    if (m_status == status)
        return;
    m_status = status;
    emit statusChanged();
}

void DynamicWallpaper::setError(const QString& error)
{
    if (m_error == error)
        return;
    m_error = error;
    emit errorChanged();
}

void DynamicWallpaper::reloadModel()
{
    m_model.reset();

    if (!m_wallpaper)
        return;

    if (qAbs(m_latitude) > 86.5) {
        const QString pole = m_latitude > 0 ? QStringLiteral("North") : QStringLiteral("South");
        setError(QStringLiteral("Argh, darn it... It looks like you are located near the %1 pole.").arg(pole));
        setStatus(Status::Error);
        return;
    }

    auto model = std::make_unique<DynamicWallpaperModel>(m_wallpaper.get(), m_latitude, m_longitude);
    if (!model->isValid()) {
        setError(QStringLiteral("Couldn't construct path of the Sun."));
        setStatus(Status::Error);
        return;
    }

    m_model = std::move(model);
    setStatus(Status::Ok);
}

void DynamicWallpaper::reloadWallpaper()
{
    m_wallpaper.reset();

    std::unique_ptr<DynamicWallpaperData> wallpaper = DynamicWallpaperData::load(m_wallpaperId);
    if (!wallpaper) {
        setError(QStringLiteral("Couldn't load dynamic wallpaper with id '%1'.").arg(m_wallpaperId));
        setStatus(Status::Error);
        return;
    }

    if (wallpaper->images().count() < 2) {
        setError(QStringLiteral("The dynamic wallpaper doesn't have enough pictures."));
        setStatus(Status::Error);
        return;
    }

    m_wallpaper = std::move(wallpaper);
    setStatus(Status::Ok);
}

void DynamicWallpaper::scheduleUpdate()
{
    QMetaObject::invokeMethod(this, &DynamicWallpaper::update, Qt::QueuedConnection);
}
