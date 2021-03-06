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

import QtQuick 2.1
import QtQuick.Layouts 1.0
import QtQuick.Controls 1.0 as QtControls

import org.kde.kcm 1.1 as KCM
import org.kde.plasma.core 2.0 as PlasmaCore

import com.github.zzag.private.wallpaper 1.0

ColumnLayout {
    id: root

    property int cfg_FillMode
    property int cfg_UpdateInterval
    property string cfg_WallpaperId

    property alias cfg_Latitude: latitudeSpinbox.value
    property alias cfg_Longitude: longitudeSpinbox.value

    // This is a very ugly hack to make width of our input fields same as width
    // of the Wallpaper Type combo box. A better way to align input elements has
    // to be found.
    readonly property int inputFieldWidth: wallpaperComboBox.width

    Row {
        spacing: units.largeSpacing / 2

        QtControls.Label {
            anchors.verticalCenter: positioningComboBox.verticalCenter
            width: formAlignment - units.largeSpacing
            horizontalAlignment: Text.AlignRight
            text: i18nd("plasma_wallpaper_com.github.zzag.wallpaper", "Fill Mode:")
        }

        QtControls.ComboBox {
            id: positioningComboBox
            width: inputFieldWidth
            model: [
                {
                    "label": i18nd("plasma_wallpaper_com.github.zzag.wallpaper", "Scaled and Cropped"),
                    "fillMode": Image.PreserveAspectCrop
                },
                {
                    "label": i18nd("plasma_wallpaper_com.github.zzag.wallpaper", "Scaled"),
                    "fillMode": Image.Stretch
                },
                {
                    "label": i18nd("plasma_wallpaper_com.github.zzag.wallpaper", "Scaled, Keep Proportions"),
                    "fillMode": Image.PreserveAspectFit
                },
                {
                    "label": i18nd("plasma_wallpaper_com.github.zzag.wallpaper", "Centered"),
                    "fillMode": Image.Pad
                },
                {
                    "label": i18nd("plasma_wallpaper_com.github.zzag.wallpaper", "Tiled"),
                    "fillMode": Image.Tile
                }
            ]

            textRole: "label"
            onCurrentIndexChanged: cfg_FillMode = model[currentIndex]["fillMode"]

            Component.onCompleted: {
                for (var i = 0; i < model.length; i++) {
                    if (model[i]["fillMode"] == wallpaper.configuration.FillMode) {
                        positioningComboBox.currentIndex = i;
                        break;
                    }
                }
            }
        }
    }

    Row {
        spacing: units.largeSpacing / 2

        QtControls.Label {
            anchors.verticalCenter: latitudeSpinbox.verticalCenter
            width: formAlignment - units.largeSpacing
            horizontalAlignment: Text.AlignRight
            text: i18nd("plasma_wallpaper_com.github.zzag.wallpaper", "Latitude:")
        }

        QtControls.SpinBox {
            id: latitudeSpinbox
            width: inputFieldWidth
            decimals: 2
            minimumValue: -90
            maximumValue: 90
        }
    }

    Row {
        spacing: units.largeSpacing / 2

        QtControls.Label {
            anchors.verticalCenter: longitudeSpinbox.verticalCenter
            width: formAlignment - units.largeSpacing
            horizontalAlignment: Text.AlignRight
            text: i18nd("plasma_wallpaper_com.github.zzag.wallpaper", "Longitude:")
        }

        QtControls.SpinBox {
            id: longitudeSpinbox
            width: inputFieldWidth
            decimals: 2
            minimumValue: -180
            maximumValue: 180
        }
    }

    Row {
        spacing: units.largeSpacing / 2

        QtControls.Label {
            anchors.verticalCenter: updateIntervalSpinbox.verticalCenter
            width: formAlignment - units.largeSpacing
            horizontalAlignment: Text.AlignRight
            text: i18nd("plasma_wallpaper_com.github.zzag.wallpaper", "Update Every:")
        }

        QtControls.SpinBox {
            id: updateIntervalSpinbox
            width: inputFieldWidth
            maximumValue: 59
            minimumValue: 1
            suffix: i18ndp("plasma_wallpaper_com.github.zzag.wallpaper", " min", " min", value)
            onValueChanged: cfg_UpdateInterval = value * 60000
            Component.onCompleted: value = wallpaper.configuration.UpdateInterval / 60000
        }
    }

    KCM.GridView {
        id: wallpapersGrid

        Layout.fillWidth: true
        Layout.fillHeight: true

        view.currentIndex: wallpapersModel.indexOf(cfg_WallpaperId)
        view.model: wallpapersModel
        view.delegate: KCM.GridDelegate {
            hoverEnabled: true
            text: model.name
            toolTip: model.name

            thumbnail: Image {
                anchors.fill: parent
                source: model.previewUrl
            }

            onClicked: {
                cfg_WallpaperId = model.id;
                wallpapersGrid.forceActiveFocus();
            }
        }
    }

    WallpapersModel {
        id: wallpapersModel
    }
}
