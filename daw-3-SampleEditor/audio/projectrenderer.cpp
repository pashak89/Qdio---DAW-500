
#include "projectrenderer.h"
#include "audiofilewave.h"

const std::array<ProjectRenderer::FileEncodeDevice, 2> ProjectRenderer::fileEncodeDevices {

    FileEncodeDevice { ProjectRenderer::WaveFile,
        QT_TRANSLATE_NOOP("ProjectRenderer", "WAV (*.wav)"),
        ".wav", &AudioFileWave::getInst },

    FileEncodeDevice { ProjectRenderer::NumFileFormats, nullptr, nullptr, nullptr }

};

ProjectRenderer::ProjectRenderer(const AudioEngine::qualitySettings& qualitySettings,
    const OutputSettings& outputSettings,
    ExportFileFormats exportFileFormat,
    const QString& outputFilename)
    : QThread(AudioManager::audioEngine())
    , m_fileDev(nullptr)
    , m_qualitySettings(qualitySettings)
    , m_progress(0)
    , m_abort(false)
{
    AudioFileDeviceInstantiaton audioEncoderFactory = fileEncodeDevices[exportFileFormat].m_getDevInst;

    if (audioEncoderFactory) {
        bool successful = false;

        m_fileDev = audioEncoderFactory(
            outputFilename, outputSettings, 2,
            AudioManager::audioEngine(), successful);

        qDebug() << "m_fileDev::m_fileDev" << successful << m_fileDev;
        if (!successful && m_fileDev) {
            delete m_fileDev;
            m_fileDev = nullptr;
        }
        qDebug() << "m_fileDev::m_fileDev2";
    }
}

// Little help function for getting file format from a file extension
// (only for registered file-encoders).
ProjectRenderer::ExportFileFormats ProjectRenderer::getFileFormatFromExtension(
    const QString& _ext)
{
    int idx = 0;
    while (fileEncodeDevices[idx].m_fileFormat != NumFileFormats) {
        if (QString(fileEncodeDevices[idx].m_extension) == _ext) {
            return (fileEncodeDevices[idx].m_fileFormat);
        }
        ++idx;
    }

    return (WaveFile); // Default.
}

QString ProjectRenderer::getFileExtensionFromFormat(
    ExportFileFormats fmt)
{
    return fileEncodeDevices[fmt].m_extension;
}

void ProjectRenderer::startProcessing()
{

    if (isReady()) {
        // Have to do audio engine stuff with GUI-thread affinity in order to
        // make slots connected to sampleRateChanged()-signals being called immediately.
        AudioManager::audioEngine()->setAudioDevice(m_fileDev, m_qualitySettings, false, false);

        start(QThread::HighPriority);
    }
}

void ProjectRenderer::run()
{

    AudioManager::getSong()->startExport();
    // Skip first empty buffer.
    AudioManager::audioEngine()->nextBuffer();

    m_progress = 0;

    // Now start processing
    AudioManager::audioEngine()->startProcessing(false);

    qDebug() << "Start Run";
    // Continually track and emit progress percentage to listeners.
    while (!AudioManager::getSong()->isExportDone() && !m_abort) {
        m_fileDev->processNextBuffer();
        const int nprog = AudioManager::getSong()->getExportProgress();
        if (m_progress != nprog) {
            m_progress = nprog;
            emit progressChanged(m_progress);
        }
    }

    m_fileDev->processFlush();

    qDebug() << "End Run";
    // Notify the audio engine of the end of processing.
    AudioManager::audioEngine()->stopProcessing();

    AudioManager::getSong()->stopExport();

    // If the user aborted export-process, the file has to be deleted.
    const QString f = m_fileDev->outputFile();
    if (m_abort) {
        QFile(f).remove();
    }
}

void ProjectRenderer::abortProcessing()
{
    m_abort = true;
    wait();
}

void ProjectRenderer::updateConsoleProgress()
{
    const int cols = 50;
    static int rot = 0;
    auto buf = std::array<char, 80> {};
    auto prog = std::array<char, cols + 1> {};

    for (int i = 0; i < cols; ++i) {
        prog[i] = (i * 100 / cols <= m_progress ? '-' : ' ');
    }
    prog[cols] = 0;

    const auto activity = (const char*)"|/-\\";
    std::fill(buf.begin(), buf.end(), 0);
    sprintf(buf.data(), "\r|%s|    %3d%%   %c  ", prog.data(), m_progress,
        activity[rot]);
    rot = (rot + 1) % 4;

    fprintf(stderr, "%s", buf.data());
    fflush(stderr);
}
