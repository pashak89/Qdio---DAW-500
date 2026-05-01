#ifndef COORDINATEPLANE_H
#define COORDINATEPLANE_H

#include <QAbstractListModel>
#include <QBrush>
#include <QCursor>
#include <QFontMetrics>
#include <QJsonObject>
#include <QObject>
#include <QPainter>
#include <QPainterPath>
#include <QQuickPaintedItem>
#include <QtMath>
#include <complex.h>
#include <complex>
#include <math.h>

#include "multieq.h"

class AreaInfo;
class Filter {
public:
    Filter(int type, bool enabled, QString key, QString unit, QString nameFilter, bool decimal,
        double min, double max, double value, double dValue,
        bool variance, bool automated, bool active)
        : m_type(type)
        , m_enabled(enabled)
        , m_key(std::move(key))
        , m_unit(std::move(unit))
        , m_nameFilter(std::move(nameFilter))
        , m_decimal(decimal)
        , m_min(min)
        , m_max(max)
        , m_value(value)
        , m_dValue(dValue)
        , m_variance(variance)
        , m_automated(automated)
        , m_active(active)
    {
    }
    int type() const { return m_type; }
    bool enabled() const { return m_enabled; }
    QString key() const { return m_key; }
    QString unit() const { return m_unit; }
    QString nameFilter() const { return m_nameFilter; }
    bool decimal() const { return m_decimal; }
    double min() const { return m_min; }
    double max() const { return m_max; }
    double value() const { return m_value; }
    void setValue(double value) { m_value = value; }
    double dValue() const { return m_dValue; }
    void setEnabled(bool enabled) { m_enabled = enabled; }
    bool variance() const { return m_variance; }

    bool automated() const { return m_automated; }
    void setAutomated(bool value) { m_automated = value; }

    bool active() const { return m_active; }
    void setActive(bool active) { m_active = active; }

private:
    int m_type;
    bool m_enabled;
    QString m_key;
    QString m_unit;
    QString m_nameFilter;
    bool m_decimal;
    double m_min;
    double m_max;
    double m_value;
    double m_dValue;
    bool m_variance;

    bool m_active = false;
    bool m_automated = false;
};

class FiltersModel : public QAbstractListModel {
    Q_OBJECT

public:
    enum FilterRoles {
        ROLE_FilterType = Qt::UserRole + 1,
        ROLE_Enabled,
        ROLE_Key,
        ROLE_Unit,
        ROLE_NameFilter,
        ROLE_Decimal,
        ROLE_Min,
        ROLE_Max,
        ROLE_Value,
        ROLE_dValue,
        ROLE_Variance,
        ROLE_Automated,
        ROLE_Active,

    };

    Q_ENUM(FilterRoles)
    FiltersModel(QObject* parent = nullptr)
        : QAbstractListModel(parent)
    {
    }

    // Add a filter to the model
    void addFilter(const Filter& filter)
    {
        beginInsertRows(QModelIndex(), rowCount(), rowCount());
        m_filters.append(filter);
        endInsertRows();
    }

    int rowCount(const QModelIndex& parent = QModelIndex()) const override
    {
        Q_UNUSED(parent);
        return m_filters.count();
    }

    void setEnabled(int i, bool enabled)
    {
        if (i >= 0 && i < m_filters.size()) {

            m_filters[i].setEnabled(enabled);
            Q_EMIT dataChanged(this->index(i, 0), this->index(i, 0), QVector<int>() << ROLE_Enabled);
        }
    }

    void setValue(int i, double value)
    {
        if (i >= 0 && i < m_filters.size()) {
            m_filters[i].setValue(value);

            Q_EMIT dataChanged(this->index(i, 0), this->index(i, 0), QVector<int>() << ROLE_Value);
        }
    }

    void setAutomated(int i, bool automated)
    {
        if (i >= 0 && i < m_filters.size()) {
            m_filters[i].setAutomated(automated);

            Q_EMIT dataChanged(this->index(i, 0), this->index(i, 0), QVector<int>() << ROLE_Automated);
        }
    }

    void setActive(int i, bool active)
    {
        if (i >= 0 && i < m_filters.size()) {

            if (active != m_filters[i].active()) {
                m_filters[i].setActive(active);

                Q_EMIT dataChanged(this->index(i, 0), this->index(i, 0), QVector<int>() << ROLE_Active);
            }
        }
    }
    bool setData(const QModelIndex& index, const QVariant& value, int role)
    {

        if (m_filters.size() == 0 || index.row() < 0 || index.row() > m_filters.size())
            return false;

        if (index.isValid() && role == ROLE_Value) {

            m_filters[index.row()].setValue(value.toDouble());
            Q_EMIT dataChanged(index, index, QVector<int>() << ROLE_Value);
            return true;
        }

        return false;
    }

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override
    {
        if (!index.isValid() || index.row() >= m_filters.size())
            return QVariant();

        const Filter& filter = m_filters[index.row()];

        switch (role) {
        case ROLE_FilterType:
            return filter.type();
        case ROLE_Enabled:
            return filter.enabled();
        case ROLE_Key:
            return filter.key();
        case ROLE_Unit:
            return filter.unit();
        case ROLE_NameFilter:
            return filter.nameFilter();
        case ROLE_Decimal:
            return filter.decimal();
        case ROLE_Min:
            return filter.min();
        case ROLE_Max:
            return filter.max();
        case ROLE_Value: {
            return filter.value();
        }
        case ROLE_dValue:
            return filter.dValue();
        case ROLE_Variance:
            return filter.variance();

        case ROLE_Active: {
            return filter.active();
        }
        case ROLE_Automated:
            return filter.automated();

        default:
            return QVariant();
        }
    }

    QHash<int, QByteArray> roleNames() const override
    {
        QHash<int, QByteArray> roles;
        roles[ROLE_FilterType] = "filterType";
        roles[ROLE_Enabled] = "enabled";
        roles[ROLE_Key] = "key";
        roles[ROLE_Unit] = "unit";
        roles[ROLE_NameFilter] = "nameFilter";
        roles[ROLE_Decimal] = "decimal";
        roles[ROLE_Min] = "min";
        roles[ROLE_Max] = "max";
        roles[ROLE_Value] = "value";
        roles[ROLE_dValue] = "dValue";
        roles[ROLE_Variance] = "variance";

        roles[ROLE_Active] = "active";
        roles[ROLE_Automated] = "automated";

        return roles;
    }

private:
    QVector<Filter> m_filters;
};

class BandModel : public QAbstractListModel {
    Q_OBJECT

public:
    enum BandRole {
        ROLE_Filter = Qt::UserRole + 1,
        ROLE_Enabled,
        ROLE_Active,
        ROLE_Name,
        ROLE_PosX,
        ROLE_PosY,

    };
    Q_ENUM(BandRole)

    struct Band {
        int defaultFilter;
        bool enabled;
        bool active;
        QString name;
        int posX;
        int posY;
    };

    BandModel(QObject* parent = nullptr)
        : QAbstractListModel(parent)
    {
    }

    // Add a filter to the model
    void addBand(const Band& filter)
    {
        beginInsertRows(QModelIndex(), rowCount(), rowCount());
        m_bands.append(filter);
        endInsertRows();
    }
    void setEnable(int i, bool enabled)
    {

        if (i >= 0 && i < m_bands.size()) {
            m_bands[i].enabled = enabled;

            Q_EMIT dataChanged(this->index(i, 0), this->index(i, 0), QVector<int>() << ROLE_Enabled);
        }
    }
    void setActive(int i, bool active)
    {

        if (i >= 0 && i < m_bands.size()) {

            m_bands[i].active = active;

            Q_EMIT dataChanged(this->index(i, 0), this->index(i, 0), QVector<int>() << ROLE_Active);
        }
    }

    void setPos(int i, int x, int y)
    {

        if (i >= 0 && i < m_bands.size()) {

            m_bands[i].posX = x;
            m_bands[i].posY = y;

            Q_EMIT dataChanged(this->index(i, 0), this->index(i, 0), QVector<int>() << ROLE_PosX);
            Q_EMIT dataChanged(this->index(i, 0), this->index(i, 0), QVector<int>() << ROLE_PosY);
        }
    }

    int rowCount(const QModelIndex& parent = QModelIndex()) const override
    {
        Q_UNUSED(parent);
        return m_bands.count();
    }

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override
    {
        if (!index.isValid() || index.row() >= m_bands.size())
            return QVariant();

        const Band& band = m_bands[index.row()];

        switch (role) {
        case ROLE_Filter:
            return band.defaultFilter;
        case ROLE_Enabled:
            return band.enabled;
        case ROLE_Active:
            return band.active;
        case ROLE_Name:
            return band.name;
        case ROLE_PosX:
            return band.posX;
        case ROLE_PosY:
            return band.posY;

        default:
            return QVariant();
        }
    }

    QHash<int, QByteArray> roleNames() const override
    {
        QHash<int, QByteArray> roles;
        roles[ROLE_Filter] = "Filter";
        roles[ROLE_Enabled] = "Enabled";
        roles[ROLE_Active] = "Active";
        roles[ROLE_Name] = "Name";
        roles[ROLE_PosX] = "PosX";
        roles[ROLE_PosY] = "PosY";

        return roles;
    }

private:
    QVector<Band> m_bands;
};

class CustomCircle : public QQuickPaintedItem {
public:
    explicit CustomCircle(QQuickItem* parent = nullptr)
        : QQuickPaintedItem(parent)
    {
    }

    // QQuickPaintedItem interface
public:
    void paint(QPainter* painter)
    {
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setRenderHint(QPainter::SmoothPixmapTransform, true);
        painter->setRenderHint(QPainter::TextAntialiasing, true);

        brush.setColor(m_color);

        QPen pen(m_color);
        pen.setWidth(1.2);

        painter->setBrush(brush);
        painter->setPen(pen);
        painter->drawEllipse(0, 0, width(), height());
    }

    void setColor(const QColor& color)
    {
        m_color = color;
        update();
    }

    void setStyle(const Qt::BrushStyle& style)
    {
        brush.setStyle(style);
        update();
    }

    QColor color() const
    {
        return m_color;
    }

private:
    QColor m_color = QColor("#15d0e9");
    QBrush brush = Qt::SolidPattern;
    bool m_visible = true;
};

class CustomLabel : public QQuickPaintedItem {
    Q_OBJECT
public:
    explicit CustomLabel(QQuickItem* parent = nullptr)
        : QQuickPaintedItem(parent)
    {
        m_textColor = QColor("#eaeaea");
        setAcceptHoverEvents(true);
        setEnabled(false);
        connect(this, &CustomLabel::enabledChanged, [this]() {
            update();
        });
    }

    // QQuickPaintedItem interface
public:
    void paint(QPainter* painter)
    {
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setRenderHint(QPainter::SmoothPixmapTransform, true);
        painter->setRenderHint(QPainter::TextAntialiasing, true);

        if (isEnabled()) {
            painter->setPen(m_textColor);
        } else {
            painter->setPen(m_disableColor);
        }

        // painter->fillRect(0, 0, width(), height(), Qt::red);

        painter->drawText(QRect(0, 0, width(), height()), m_alignment, QString(m_text));
    }

    void setText(const QString& text)
    {
        m_text = text;
    }

    void setTextColor(const QColor& textColor)
    {
        m_textColor = textColor;
        update();
    }

    void setDisableColor(const QColor& disableColor)
    {
        m_disableColor = disableColor;
        update();
    }

private:
    QString m_text;
    QColor m_disableColor;
    QColor m_textColor;
    Qt::Alignment m_alignment = Qt::AlignCenter;
};

class FilterVisualizer : public QQuickPaintedItem {
    Q_OBJECT

    Q_PROPERTY(BandModel* bandModel READ bandModel NOTIFY bandModelChanged)

    Q_PROPERTY(MultiEQ* multieq READ multieq WRITE setMultiEq NOTIFY multieqChanged)

    Q_PROPERTY(AreaInfo* areaInfo READ areaInfo WRITE setAreaInfo NOTIFY areaInfoChanged)

    Q_PROPERTY(int EQ4_FilterType READ filterType WRITE setFilterType NOTIFY filterTypeChanged)
    Q_PROPERTY(float EQ4_Freq READ freq WRITE setFreq NOTIFY freqChanged)
    Q_PROPERTY(float EQ4_Gain READ gain WRITE setGain NOTIFY gainChanged)
    Q_PROPERTY(float EQ4_QFactor READ qFactor WRITE setQfactor NOTIFY qFactorChanged)
    Q_PROPERTY(int EQ4_activeBand READ activeIndex WRITE setActiveIndex NOTIFY activeIndexChanged)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)

    Q_PROPERTY(QSize size READ size WRITE setSize NOTIFY sizeChanged)

    Q_PROPERTY(int circleSize READ circleSize WRITE setCircleSize NOTIFY circleSizeChanged)

    Q_PROPERTY(bool deviceOnValue READ deviceOnValue NOTIFY deviceOnValueChanged)
    Q_PROPERTY(bool deviceOnActive READ deviceOnActive NOTIFY deviceOnActiveChanged)
    Q_PROPERTY(bool deviceOnAutomated READ deviceOnAutomated NOTIFY deviceOnAutomatedChanged)

    struct Settings {
        double fMin = 20.0f; // minimum displayed frequency
        double fMax = 22000.0f; // maximum displayed frequency
        double dbMin = -20.0f; // min displayed dB
        double dbMax = 20.0f; // max displayed dB
        double qMin = -20.0f; // min displayed dB
        double qMax = 20.0f; // max displayed dB
        double gridDiv = 5.0f; // how many dB per divisions (between grid lines)
        bool gainHandleLin = false; // are the filter gain sliders linear?
    };

    //    const float mL = 0.0f;
    //    const float mR = 13.0f;
    //    const float mT = 0.0f;
    //    const float mB = 13.0f;
    //    const float OH = 0.0f;
    float mL = 7.0f;
    float mR = 7.0f;
    float mT = 7.0f;
    float mB = 7.0f;
    float OH = 0.0f;

public:
    explicit FilterVisualizer(QQuickItem* parent = nullptr);
    void setSampleRate(const double newSampleRate);

    Q_INVOKABLE void setBandEnable(int index, bool active);
    Q_INVOKABLE bool isBandEnabled(int index);

    void setActiveIndex(int index);
    int activeIndex() const;

    Q_INVOKABLE void setFilterEnabled(int index, bool enabled);
    Q_INVOKABLE void setAutomatedEnabled(int index);

    Q_INVOKABLE void initialized(int itemWidth, int itemHeight);
    Q_INVOKABLE void mouseMoveEvent(int x, int y);
    Q_INVOKABLE void mousePressEvent(int x, int y);
    // Q_INVOKABLE void mouseReleasedEvent(int x, int y);

    Q_INVOKABLE void deviceOnAutomatedEnable();

    Q_INVOKABLE void deviceOnEnable(bool enabled);

    Q_INVOKABLE void saveUndoOperation(int op = 0);

public:
    void bandUpdate();
    void paint(QPainter* painter);
    void setMultiEq(MultiEQ* multiEq);
    MultiEQ* multieq() const;

    Q_INVOKABLE FiltersModel* filtersModel(int i) const;
    BandModel* bandModel() const;

    int filterType();
    void setFilterType(int filterType);

    float gain();
    void setGain(float gain);

    int freq();
    void setFreq(float freq);

    float qFactor();
    void setQfactor(float q);

    bool enabled() const;
    void setEnabled(bool newEnabled);

    QSize size() const;
    void setSize(QSize size);

    int circleSize() const;
    void setCircleSize(int size);

    bool deviceOnValue() const;
    void setDeviceOnValue(int value);

    bool deviceOnActive() const;
    void setDeviceOnActive(bool value);

    bool deviceOnAutomated() const;
    void setDeviceOnAutomated(bool value);

    void setAreaInfo(AreaInfo* areaInfo);
    AreaInfo* areaInfo();

public slots:

    void sltUpdateStatus(QJsonObject status);

private:
    bool _deviceOnValue = false;
    bool _deviceOnActive = false;
    bool _deviceOnAutomated = false;

    int m_circleSize = 7;
    bool m_initialized = false;
    QSize m_size;
    FiltersModel* m_filtersModel[4];
    BandModel* m_bandModel;

    int m_itemWidth = 1;
    int m_itemHeight = 1;

    QRectF m_rects[4];

    QColor m_lineColor;
    QColor m_lineGlowColor;
    QColor m_circleBgColor;

    int m_activeIndex = 0;

    Settings _settings;

    double sampleRate = 48000;

    float dyn, zero, scale;

    QPainterPath dbGridPath;
    QPainterPath hzGridPath;
    QPainterPath hzGridPathBold;

    QVector<double> frequencies;
    QVector<double> magnitudes;
    QVector<double> phases;
    int numPixels;

    float overallGainInDb { 0.0 };

    QVector<std::complex<double>> complexMagnitudes;
    QVector<float> allMagnitudesInDb;
    MultiEQ* _multiEq = nullptr;

    bool m_enabled = true;
    // QQuickItem interface

    AreaInfo* _areaInfo = Q_NULLPTR;

private:
    void resized();

    int drawLevelMark(QPainter* g, int x, int width, const int level, const QString& label, int lastTextDrawPos = -1);

    int dbToY(const float dB);

    float dbToYFloat(const float dB);

    float yToDb(const float y);

    float yToQ(const float y);

    float qToY(const float q);

    int hzToX(float hz);

    float xToHz(int x);

    enum { defaultMinusInfinitydB = -100 };
    double gainToDecibels(double gain,
        double minusInfinityDb = double(defaultMinusInfinitydB));

    double jlimit(double lowerLimit,
        double upperLimit,
        double valueToConstrain);

    static double decibelsToGain(double decibels,
        double minusInfinityDb = double(defaultMinusInfinitydB));

    //    // QQuickItem interface
    // protected:
    //    void geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry);

    //    // QQuickItem interface
    // protected:
    //    void mouseMoveEvent(QMouseEvent* event);
    //    void mousePressEvent(QMouseEvent* event);
    //    void mouseReleaseEvent(QMouseEvent* event);

private Q_SLOTS:

    void sltUpdate();

    void sltGainChanged();
    void sltQChanged();
    void sltFreqChanged();
    void sltParameterChanged();

    void sltParameterActiveChanged(int index, bool active);
    void sltParameterAutomated(int index, bool automated);
    void sltParameterUpdated(int index);

    void sltDeviceOnActiveChanged(bool active);
    void sltDeviceOnAutomatedChanged(bool active);
    void sltDeviceOnValueChanged(bool value);

    void saveRedoOperation();
Q_SIGNALS:

    void deviceOnValueChanged();
    void deviceOnActiveChanged();
    void deviceOnAutomatedChanged();

    void bandEnabled(int band, bool enabled);
    void sigCircleMoved();
    void circleYChanged();
    void sigUpdate();
    void filtersModelChanged();
    void sigAutomationEnableChanged();
    void sigLinesChanged();
    void sigBandMoved(int bandNumber);
    void multieqChanged();
    void filterTypeChanged();
    void freqChanged();
    void gainChanged();
    void qFactorChanged();
    void activeIndexChanged();

    void sigParamersChanged();
    void enabledChanged();

    void bandModelChanged();
    void sizeChanged();
    void circleSizeChanged();

    void areaInfoChanged();
};

#endif // COORDINATEPLANE_H
