#ifndef RENDERMANAGER_H
#define RENDERMANAGER_H

#include "audioengine.h"
#include "audiofiledevice.h"
#include "engine.h"
#include "projectrenderer.h"
#include "song.h"

#include <QObject>
#include <QThread>

#include <array>
#include <memory>

class AudioRenderManager : public QObject {
    Q_OBJECT
public:
    AudioRenderManager(
        const AudioEngine::qualitySettings& qualitySettings,
        const OutputSettings& outputSettings,
        ProjectRenderer::ExportFileFormats fmt,
        QString outputPath);

    ~AudioRenderManager() override;

    /// Export all unmuted tracks into a single file
    void renderProject();

    /// Export all unmuted tracks into individual file
    void renderTracks();

    void abortProcessing();

signals:
    void progressChanged(int);
    void finished();

private slots:
    void renderNextTrack();
    void updateConsoleProgress();

private:
    void restoreMutedState();
    QString pathForTrack(const Track* track, int num);
    void render(QString outputPath);

    const AudioEngine::qualitySettings m_qualitySettings;
    const AudioEngine::qualitySettings m_oldQualitySettings;
    const OutputSettings m_outputSettings;
    ProjectRenderer::ExportFileFormats m_format;
    QString m_outputPath;

    std::unique_ptr<ProjectRenderer> m_activeRenderer;

    QVector<Track*> m_tracksToRender;
    QVector<Track*> m_unmuted;
};

#endif // RENDERMANAGER_H
