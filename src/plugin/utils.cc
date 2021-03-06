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
#include "utils.h"

// Qt
#include <QtMath>

// std
#include <cmath>

qreal computeAngle(const QVector3D& normal, const QVector3D& v1, const QVector3D& v2)
{
    const float dot = QVector3D::dotProduct(v1, v2);
    const float det = QVector3D::dotProduct(normal, QVector3D::crossProduct(v1, v2));

    float angle = qRadiansToDegrees(std::atan2(det, dot));
    if (angle < 0)
        angle += 360;

    return static_cast<qreal>(angle);
}

QVector3D computeNormal(const QVector3D& center, const QVector3D& v1, const QVector3D& v2)
{
    const QVector3D cross = QVector3D::crossProduct(v1 - center, v2 - center);
    return cross.normalized();
}
