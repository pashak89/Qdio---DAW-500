/* Copyright (C) 2024, Abbas Aliakbari - All Rights Reserved
 * SPDX-License-Identifier: MIT
 */
#pragma once

#include <QObject>

// Registered with QWebChannel so the Three.js scene can talk to the DAW.
// Signals flow C++ → JS. Q_INVOKABLE methods flow JS → C++.
class ThreeBridge : public QObject {
    Q_OBJECT

public:
    explicit ThreeBridge(QObject* parent = nullptr);

    // Called by JavaScript when a user drags an object in the Three.js scene.
    Q_INVOKABLE void entityMovedInScene(int trackIndex, double x, double y, double z);

public slots:
    // Drive the 3D scene from the DAW side.
    void setPlayhead(qint64 time);
    void setEntityPosition(int trackIndex, double x, double y, double z);
    void resetScene();

signals:
    // ---- Delivered to JavaScript via QWebChannel ----
    void playheadMoved(double time);
    void entityPositionChanged(int trackIndex, double x, double y, double z);
    void sceneReset();

    // ---- Internal: consumed by C++ (ObjectCreator) ----
    void entityMovedFromScene(int trackIndex, double x, double y, double z);
};
