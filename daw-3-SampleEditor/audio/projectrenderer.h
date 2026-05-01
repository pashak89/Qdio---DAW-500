#ifndef PROJECTRENDERER_H
#define PROJECTRENDERER_H


#include "audioengine.h"
#include "audiofiledevice.h"
#include "engine.h"
#include "song.h"

#include <QThread>

#include <array>

class ProjectRenderer : public QThread {
    Q_OBJECT
public:
    enum ExportFileFormats : int {
        WaveFile,
        FlacFile,
        OggFile,
        MP3File,
        NumFileFormats
    };

    struct FileEncodeDevice {
        bool isAvailable() const { return m_getDevInst != nullptr; }

        ExportFileFormats m_fileFormat;
        const char* m_description;
        const char* m_extension;
        AudioFileDeviceInstantiaton m_getDevInst;
    };

    ProjectRenderer(const AudioEngine::qualitySettings& _qs,
        const OutputSettings& _os,
        ExportFileFormats _file_format,
        const QString& _out_file);
    ~ProjectRenderer() override = default;

    bool isReady() const
    {
        return m_fileDev != nullptr;
    }

    static ExportFileFormats getFileFormatFromExtension(
        const QString& _ext);

    static QString getFileExtensionFromFormat(ExportFileFormats fmt);

    static const std::array<FileEncodeDevice, 2> fileEncodeDevices;

public slots:
    void startProcessing();
    void abortProcessing();

    void updateConsoleProgress();

signals:
    void progressChanged(int);

private:
    void run() override;

    AudioFileDevice* m_fileDev;
    AudioEngine::qualitySettings m_qualitySettings;

    volatile int m_progress;
    volatile bool m_abort;
};
#endif // PROJECTRENDERER_H
