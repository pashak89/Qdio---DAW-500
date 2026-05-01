#include "rendermanager.h"

#include <QDir>

AudioRenderManager::AudioRenderManager(
    const AudioEngine::qualitySettings& qualitySettings,
    const OutputSettings& outputSettings,
    ProjectRenderer::ExportFileFormats fmt,
    QString outputPath)
    : m_qualitySettings(qualitySettings)
    , m_oldQualitySettings(AudioManager::audioEngine()->currentQualitySettings())
    , m_outputSettings(outputSettings)
    , m_format(fmt)
    , m_outputPath(outputPath)
{
    AudioManager::audioEngine()->storeAudioDevice();
}

AudioRenderManager::~AudioRenderManager()
{
    AudioManager::audioEngine()->restoreAudioDevice(); // Also deletes audio dev.
    // AudioManager::audioEngine()->changeQuality(m_oldQualitySettings);
}

void AudioRenderManager::abortProcessing()
{
    if (m_activeRenderer) {
        disconnect(m_activeRenderer.get(), SIGNAL(finished()),
            this, SLOT(renderNextTrack()));
        m_activeRenderer->abortProcessing();
    }
    restoreMutedState();
}

// Render the song into a single track
void AudioRenderManager::renderProject()
{
    render(m_outputPath);
}

void AudioRenderManager::render(QString outputPath)
{
    qDebug() << "Render1";
    m_activeRenderer = std::make_unique<ProjectRenderer>(
        m_qualitySettings,
        m_outputSettings,
        m_format,
        outputPath);

    if (m_activeRenderer->isReady()) {
        // pass progress signals through
        connect(m_activeRenderer.get(), SIGNAL(progressChanged(int)),
            this, SIGNAL(progressChanged(int)));

        // when it is finished, render the next track.
        // if we have not queued any tracks, renderNextTrack will just clean up
        connect(m_activeRenderer.get(), SIGNAL(finished()),
            this, SLOT(renderNextTrack()));

        m_activeRenderer->startProcessing();
    } else {
        qDebug("Renderer failed to acquire a file device!");
        renderNextTrack();
    }
    qDebug() << "Render2";
}

// Called to render each new track when rendering tracks individually.
void AudioRenderManager::renderNextTrack()
{
    m_activeRenderer.reset();

    if (m_tracksToRender.isEmpty()) {
        // nothing left to render
        restoreMutedState();
        emit finished();
    } else {
        // pop the next track from our rendering queue
        Track* renderTrack = m_tracksToRender.back();
        m_tracksToRender.pop_back();

        // mute everything but the track we are about to render
        for (auto track : m_unmuted) {
            track->setMuted(track != renderTrack);
        }

        // for multi-render, prefix each output file with a different number
        int trackNum = m_tracksToRender.size() + 1;

        render(pathForTrack(renderTrack, trackNum));
    }
}

// Determine the output path for a track when rendering tracks individually
QString AudioRenderManager::pathForTrack(const Track* track, int num)
{
    QString extension = ProjectRenderer::getFileExtensionFromFormat(m_format);
    QString name = "test";

    name = QString("%1_%2%3").arg(num).arg(name).arg(extension);
    return QDir(m_outputPath).filePath(name);
}

// Unmute all tracks that were muted while rendering tracks
void AudioRenderManager::restoreMutedState()
{
    while (!m_unmuted.isEmpty()) {
        Track* restoreTrack = m_unmuted.back();
        m_unmuted.pop_back();
        restoreTrack->setMuted(false);
    }
}

void AudioRenderManager::updateConsoleProgress()
{
    if (m_activeRenderer) {
        m_activeRenderer->updateConsoleProgress();

        int totalNum = m_unmuted.size();
        if (totalNum > 0) {
            // we are rendering multiple tracks, append a track counter to the output
            int trackNum = totalNum - m_tracksToRender.size();
            fprintf(stderr, "(%d/%d)", trackNum, totalNum);
        }
    }
}
