#ifndef CLIP_H
#define CLIP_H

#include "journallingobject.h"
#include "timepos.h"

#include <QColor>
#include <QObject>

class Track;

class Clip : public QObject, public JournallingObject {
    Q_OBJECT

public:
    Clip(Track* track, bool isFake = false, QObject* parent = Q_NULLPTR);
    Clip(const Clip& clip, Track* track, bool isFake = false, QObject* parent = Q_NULLPTR);
    ~Clip() override;

    inline Track* getTrack() const
    {
        return m_track;
    }

    inline const QString& name() const
    {
        return m_name;
    }

    inline const TimePos& startPosition() const
    {
        return m_startPosition;
    }

    inline TimePos endPosition() const
    {
        const double sp = m_startPosition;
        return sp + m_length;
    }

    inline const TimePos& length() const
    {
        return m_length;
    }

    /*! \brief Specify whether or not a TCO automatically resizes.
     *
     *  If a TCO does automatically resize, it cannot be manually
     *  resized by clicking and dragging its edge.
     *
     */
    inline void setAutoResize(const bool r)
    {
        m_autoResize = r;
    }

    inline const bool getAutoResize() const
    {
        return m_autoResize;
    }

    QColor color() const
    {
        return m_color;
    }

    void setColor(const QColor& c)
    {
        m_color = c;
    }

    bool hasColor();

    void useCustomClipColor(bool b);

    bool usesCustomClipColor()
    {
        return m_useCustomClipColor;
    }

    virtual void movePosition(const TimePos& pos);
    virtual void changeLength(const TimePos& length);

    inline void selectViewOnCreate(bool select)
    {
        m_selectViewOnCreate = select;
    }

    inline bool getSelectViewOnCreate()
    {
        return m_selectViewOnCreate;
    }
    /*! Returns whether note is muted */
    bool isMuted() const
    {
        return m_muted;
    }

    /// Returns true if and only if a->startPosition() < b->startPosition()
    static bool comparePosition(const Clip* a, const Clip* b);

    TimePos startTimeOffset() const;
    void setStartTimeOffset(const TimePos& startTimeOffset);

    // Will copy the state of a clip to another clip
    static void copyStateTo(Clip* src, Clip* dst);

    bool isFake() const;
    void setFake(bool isFake);

    QString clipIndex() const;

    virtual void setClipIndex(const QString& clipIndex);

public slots:
    void toggleMute();

signals:
    void lengthChanged(QString clipIndex);
    void positionChanged(QString clipIndex);
    void destroyedClip();
    void colorChanged();
    void dataChanged();

protected:
    bool _isFake;

    QString m_clipIndex = "";

    TimePos m_startPosition;
    TimePos m_length;
    TimePos m_startTimeOffset;

private:
    enum Actions {
        NoAction,
        Move,
        Resize
    };

    Track* m_track;

    QString m_name;

    bool m_mutedModel;
    bool m_soloModel;
    bool m_autoResize;

    bool m_muted;

    bool m_selectViewOnCreate;

    QColor m_color;
    bool m_useCustomClipColor;

    friend class ClipView;
};

#endif // CLIP_H
