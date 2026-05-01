/* Copyright (C) 2024, Abbas Aliakbari - All Rights Reserved
 * SPDX-License-Identifier: MIT
 */
#include "ThreeBridge.h"

ThreeBridge::ThreeBridge(QObject* parent)
    : QObject(parent)
{
    setObjectName("bridge"); // Must match channel.objects.bridge in the JS side
}

void ThreeBridge::entityMovedInScene(int trackIndex, double x, double y, double z)
{
    Q_EMIT entityMovedFromScene(trackIndex, x, y, z);
}

void ThreeBridge::setPlayhead(qint64 time)
{
    Q_EMIT playheadMoved(static_cast<double>(time));
}

void ThreeBridge::setEntityPosition(int trackIndex, double x, double y, double z)
{
    Q_EMIT entityPositionChanged(trackIndex, x, y, z);
}

void ThreeBridge::resetScene()
{
    Q_EMIT sceneReset();
}
