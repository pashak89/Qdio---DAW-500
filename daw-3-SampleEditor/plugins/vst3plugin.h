#ifndef VST3PLUGINH_H
#define VST3PLUGINH_H

#include "remoteplugin.h"
#include <QObject>
#include <QQuickWindow>
#include <QStringList>
#include <QtConcurrent>
#include <audio/audio_global.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include "pluginterfaces/vst/ivsttestplugprovider.h"
#include "public.sdk/source/vst/hosting/plugprovider.h"
#include "public.sdk/source/vst/moduleinfo/moduleinfo.h"
#include "public.sdk/source/vst/moduleinfo/moduleinfocreator.h"
#include "public.sdk/source/vst/moduleinfo/moduleinfoparser.h"

#include <pluginterfaces/gui/iplugview.h>
#include <pluginterfaces/vst/ivsteditcontroller.h>
#include <pluginterfaces/vst/ivstprocesscontext.h>
#include <public.sdk/source/vst/hosting/eventlist.h>
#include <public.sdk/source/vst/hosting/hostclasses.h>
#include <public.sdk/source/vst/hosting/module.h>
#include <public.sdk/source/vst/hosting/parameterchanges.h>
#include <public.sdk/source/vst/hosting/plugprovider.h>
#include <public.sdk/source/vst/hosting/processdata.h>
//#include <juce_audio_processors/processors/juce_AudioPluginInstance.h>

#include <QAbstractItemModel>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSharedPointer>
#include <QThread>
#include <QVariant>
#include <QVector>
class Effect;
class NewQuickView : public QQuickWindow {
    Q_OBJECT
public:
    NewQuickView()
        : QQuickWindow()
    {
    }

public:
    bool event(QEvent* event) override
    {
        if (event->type() == QEvent::Close) {
            Q_EMIT sigClose();
        }
        return QQuickWindow::event(event);
    }
signals:
    void sigClose();
};

class TreeItem {
public:
    explicit TreeItem(const QString& data, QSharedPointer<TreeItem> parent = nullptr, QString path = "", int index = 0)
        : itemData(data)
        , parentItem(parent)
        , _path(path)
        , _index(index)
    {
    }

    ~TreeItem() = default; // QSharedPointer will handle the cleanup of child items

    void appendChild(QSharedPointer<TreeItem> child)
    {
        childItems.append(child);
    }

    QSharedPointer<TreeItem> child(int row)
    {
        return childItems.value(row);
    }

    int childCount() const
    {
        return childItems.count();
    }

    int columnCount() const
    {
        return 1; // Only one column (Name)
    }

    QVariant data() const
    {
        return itemData;
    }

    QSharedPointer<TreeItem> parent()
    {
        return parentItem;
    }

    int row(QSharedPointer<TreeItem> item) const
    {
        if (parentItem) {
            return parentItem->childItems.indexOf(item);
        }
        return 0;
    }

    QString path() const
    {
        return _path;
    }

    int index() const
    {
        return _index;
    }

    void clearChildren()
    {
        childItems.clear();
    }

    QColor tagColor() const { return _tagColor; }

    void setTagColor(const QColor& tag) { _tagColor = tag; }

private:
    QString _path;
    QColor _tagColor;
    int _index;
    QString itemData;
    QSharedPointer<TreeItem> parentItem;
    QList<QSharedPointer<TreeItem>> childItems;
};

// TreeModel class implements QAbstractItemModel
class TreeModel : public QAbstractItemModel {
    Q_OBJECT

public:
    explicit TreeModel(QObject* parent = nullptr)
        : QAbstractItemModel(parent)
    {
        rootItem = QSharedPointer<TreeItem>(new TreeItem("Root"));
    }

    enum Roles {
        TagColorRole = Qt::UserRole + 1
    };

    Q_INVOKABLE QModelIndex rootIndex()
    {
        return {};
    }

    Q_INVOKABLE int depth(const QModelIndex& index) const
    {
        int count = 0;
        auto anchestor = index;
        if (!index.isValid()) {
            return 0;
        }
        while (anchestor.parent().isValid()) {
            anchestor = anchestor.parent();
            ++count;
        }

        return count;
    }

    Q_INVOKABLE void clear()
    {
        beginResetModel();
        rootItem->clearChildren();
        endResetModel();
    }

    ~TreeModel() override = default; // QSharedPointer will manage the deletion

    // Override rowCount
    int rowCount(const QModelIndex& parent = QModelIndex()) const override
    {
        auto parentItem = getItem(parent);
        return parentItem->childCount();
    }

    // Override columnCount
    int columnCount(const QModelIndex& parent = QModelIndex()) const override
    {
        return 1; // Single column (Name)
    }

    // Override data
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override
    {
        if (!index.isValid())
            return {};

        auto item = getItem(index);

        switch (role) {
        case Qt::DisplayRole:
            return item->data();
        case TagColorRole:
            return item->tagColor(); // returns QColor
        default:
            return {};
        }
    }

    bool setData(const QModelIndex& index, const QVariant& value, int role) override
    {
        if (!index.isValid())
            return false;

        auto item = getItem(index);

        if (role == TagColorRole) {
            QColor c;

            // QML may pass QColor or string like "#ff0000" / "red"
            if (value.canConvert<QColor>())
                c = value.value<QColor>();
            else
                c = QColor(value.toString());

            if (!c.isValid())
                return false;

            item->setTagColor(c);
            emit dataChanged(index, index, { role });
            return true;
        }
        return false;
    }

    // Override parent
    QModelIndex parent(const QModelIndex& index) const override
    {
        if (!index.isValid()) {
            return QModelIndex();
        }

        TreeItem* childItem = static_cast<TreeItem*>(index.internalPointer());
        TreeItem* parentItem = childItem->parent().data();

        if (parentItem == rootItem.data()) {
            return QModelIndex();
        }
        if (parentItem == Q_NULLPTR) {
            return QModelIndex();
        }

        return createIndex(parentItem->row(childItem->parent()), 0, parentItem);
    }

    // Override index
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override
    {
        if (!hasIndex(row, column, parent)) {
            return QModelIndex();
        }

        TreeItem* parentItem;
        if (!parent.isValid()) {
            parentItem = rootItem.data();
        } else {
            parentItem = static_cast<TreeItem*>(parent.internalPointer());
        }
        auto childItem = parentItem->child(row);

        if (childItem) {
            return createIndex(row, column, childItem.data());
        }
        return QModelIndex();
    }

    QSharedPointer<TreeItem> getRootItem() const
    {
        return rootItem;
    }

    Q_INVOKABLE QString getTreeItemName(const QModelIndex& index)
    {
        auto item = getItem(index);

        if (item && item->index() != 0) {
            return item->data().toString();
        }
        return "";
    }

    Q_INVOKABLE QString getTreeItemPath(const QModelIndex& index)
    {
        auto item = getItem(index);

        if (item && item->index() != 0) {
            return item->path();
        }
        return "";
    }

    Q_INVOKABLE int getTreeItemIndex(const QModelIndex& index)
    {
        auto item = getItem(index);

        if (item && item->index() != 0) {
            return item->index();
        }
        return -1;
    }

    void resetModel()
    {
        beginResetModel();
        endResetModel();
    }

    QSharedPointer<TreeItem> findItemByName(const QString& name) const
    {
        return findItemByName(rootItem, name);
    }

    QSharedPointer<TreeItem> findItemByName(QSharedPointer<TreeItem> parent, const QString& name) const
    {
        if (parent == nullptr) {
            return nullptr;
        }

        if (parent->data().toString() == name) {
            return parent;
        }

        for (int i = 0; i < parent->childCount(); ++i) {
            QSharedPointer<TreeItem> child = parent->child(i);
            QSharedPointer<TreeItem> result = findItemByName(child, name);
            if (result) {
                return result;
            }
        }

        return nullptr;
    }

    QHash<int, QByteArray> roleNames() const override
    {
        QHash<int, QByteArray> roles;
        roles[Qt::DisplayRole] = "display";
        roles[TagColorRole] = "tagColor";
        return roles;
    }

private:
    QSharedPointer<TreeItem> rootItem;

    TreeItem* getItem(const QModelIndex& index) const
    {
        if (index.isValid()) {
            return static_cast<TreeItem*>(index.internalPointer());
        }
        return rootItem.data();
    }
};

class AsyncEffectLoader : public QObject, public ChangeListener {
    Q_OBJECT
    void loadEffects();
    OwnedArray<PluginDescription> pluginDescriptions;

public:
    AsyncEffectLoader(QObject* parent = Q_NULLPTR);
    ~AsyncEffectLoader();
    struct EffectInfo {
        QString _type;
        QString _name;
        QString _category;
        QString _manufacturer;
        QString _path;
        int _index;
        QJsonObject toJson() const
        {
            QJsonObject jsonObj;
            jsonObj["type"] = _type;
            jsonObj["name"] = _name;
            jsonObj["category"] = _category;
            jsonObj["manufacturer"] = _manufacturer;
            jsonObj["path"] = _path;
            jsonObj["index"] = _index;
            return jsonObj;
        }

        // Create an EffectInfo from a QJsonObject for deserialization
        static EffectInfo fromJson(const QJsonObject& jsonObj)
        {
            EffectInfo info;
            info._type = jsonObj["type"].toString();
            info._name = jsonObj["name"].toString();
            info._category = jsonObj["category"].toString();
            info._manufacturer = jsonObj["manufacturer"].toString();
            info._path = jsonObj["path"].toString();
            info._index = jsonObj["index"].toInt();
            return info;
        }
    };
    QVector<EffectInfo> getEffectInfo() const
    {
        return _effectInfo;
    }

    void reload()
    {
        QtConcurrent::run([this]() {
            this->loadEffects();
        });
    }
    void stop()
    {
        _quit = true;
    }

private:
    KnownPluginList knownPluginList;
    juce::MessageManagerLock mml;
    QMap<int, bool> _scanned;
    QVector<EffectInfo> _effectInfo;
    bool _reload = false;
    bool _quit = false;

signals:
    void resultReady();
    void newPluginFound(QString formatName, QString name, QString category, QString manufacturerName, QString path, int uniqueId);

    // ChangeListener interface
public:
    void changeListenerCallback(ChangeBroadcaster* source);
};

class Vst3PluginManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(TreeModel* vst2modules READ getVst2modules NOTIFY vst2ModulesChanged)
    Q_PROPERTY(TreeModel* vst3modules READ getVst3modules NOTIFY vst3ModulesChanged)
    Q_PROPERTY(bool effectIsLoading READ effectIsLoading NOTIFY loadingChanged)
    Q_PROPERTY(QString vst2Path READ vst2Path NOTIFY vst2PathChanged)
    Q_PROPERTY(QString vst3Path READ vst3Path NOTIFY vst3PathChanged)
public:
    Vst3PluginManager(QObject* parent = Q_NULLPTR);
    ~Vst3PluginManager();

    QString vst3Path();
    QString vst2Path();

    Q_INVOKABLE void rescanEffects();

    bool saveToJson(const QString& filePath, QVector<AsyncEffectLoader::EffectInfo> _effectInfo)
    {
        QJsonArray jsonArray;

        // Convert each EffectInfo to a JSON object and add to the array
        for (const AsyncEffectLoader::EffectInfo& effect : _effectInfo) {
            jsonArray.append(effect.toJson());
        }

        // Create a QJsonDocument from the array
        QJsonDocument doc(jsonArray);

        // Write the JSON data to a file
        QFile file(filePath);
        if (!file.open(QIODevice::WriteOnly)) {
            qWarning() << "Could not open file for writing:" << filePath;
            return false;
        }

        file.write(doc.toJson());
        file.close();
        return true;
    }

    // Load _effectInfo from a JSON file
    bool loadFromJson(const QString& filePath, QVector<AsyncEffectLoader::EffectInfo>& _effectInfo)
    {

        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly)) {
            qWarning() << "Could not open file for reading:" << filePath;
            return false;
        }

        QByteArray fileData = file.readAll();
        file.close();

        QJsonDocument doc = QJsonDocument::fromJson(fileData);
        if (doc.isArray()) {
            QJsonArray jsonArray = doc.array();
            _effectInfo.clear(); // Clear the existing data

            // Convert each item in the JSON array to an EffectInfo and append it to _effectInfo
            for (const QJsonValue& value : jsonArray) {
                if (value.isObject()) {
                    _effectInfo.append(AsyncEffectLoader::EffectInfo::fromJson(value.toObject()));
                }
            }
            return true;
        } else {
            qWarning() << "Invalid JSON format in file:" << filePath;
            return false;
        }
    }

    QSharedPointer<TreeModel> vst2modules() const
    {
        return _vst2modules;
    }

    QSharedPointer<TreeModel> vst3modules() const
    {
        return _vst3modules;
    }

    TreeModel* getVst2modules() const
    {
        return _vst2modules.data();
    }

    TreeModel* getVst3modules() const
    {
        return _vst3modules.data();
    }

    bool effectIsLoading() const
    {
        return _effectIsLoading;
    }

private:
    bool _effectIsLoading = false;
    QSharedPointer<TreeModel> _vst2modules;
    QSharedPointer<TreeModel> _vst3modules;

    QSharedPointer<AsyncEffectLoader> _effectLoader;

    QString _vst2Path;
    QString _vst3Path;

    void fillModels(QVector<AsyncEffectLoader::EffectInfo> effectInfo);
    void addEffectInfo(AsyncEffectLoader::EffectInfo effectInfo);

private Q_SLOTS:
    void sltResultReady();
    void newPluginFound(QString formatName, QString name, QString category, QString manufacturerName, QString path, int uniqueId);

signals:
    void loadingChanged();
    void sigEffectIsLoading();
    void sigEffectLoaded();

    void modelChanged();
    void vst2ModulesChanged();
    void vst3ModulesChanged();

    void vst2PathChanged();
    void vst3PathChanged();
};

struct Parameter {
    int _id;
    QString _title;
    QString _units;
    int32 _stepCount;
    double _defaultNormalizedValue;
    QString _value;
    double _normalized;
    bool _active;
    bool _configured;
    bool _vstProberty;
};

class Vst3Plugin : public QObject, public juce::AudioProcessorListener, public juce::ComponentListener {
    Q_OBJECT

public:
    Vst3Plugin(QSharedPointer<Effect> effect, QObject* parent = Q_NULLPTR);
    ~Vst3Plugin();

    void addConfiguredParameter(int juceIndex);

    bool init(const std::string& path, const std::string& name, uint32 pluginIndex, int sampleRate, int maxBlockSize, int channelIn, int channelOut);
    void destroy();

    bool process(sampleFrame* _buf);

    void prepareToplay();
    bool createView();
    void destroyView();

    const std::string& name();

    bool failed() const;

    QVector<Parameter> parameters() const;

    bool parameter(int juceIndex, Parameter& parameter);

    void updateParameter(int index, double normalized);
    void setParameter(int index, uint32 id, double normalized);

    QString getParamValue(uint32 id, double normalized);

signals:
    void sigVisibleChanged(bool visible);

private:
    bool m_failed = false;
    int m_initialSampleRate;
    int m_maxBlockSize;

    unsigned int totalNumInputChannels;
    unsigned int totalNumOutputChannels;

private:
    QVector<Parameter> _parameters;

    std::string _path;
    std::string _name;

    QSharedPointer<NewQuickView> _window = Q_NULLPTR;
    AudioProcessorEditor* _editor = Q_NULLPTR;
    QSharedPointer<Effect> _effect;

private:
    // Effect interface

    std::unique_ptr<AudioPluginInstance> jplugin;
    std::unique_ptr<AudioPluginInstance> createPluginInstance(const String& pluginPath, const String& pluginName, int pluginIndex,
        double initialSampleRate, int initialBlockSize);

signals:
    void sigComponentRezied();
    void sigBeginEdit(uint32 id);
    void sigPerformEdit(uint32 id, double valueNormalized);
    void sigUpdateParams(int index, QString value, double valueNormalized);
    void sigEndEdit(uint32 id);
    void sigNewParameterAdded(int juceIndex);

    // AudioProcessorListener interface
public:
    void audioProcessorParameterChanged(AudioProcessor* processor, int parameterIndex, float newValue);
    void audioProcessorChanged(AudioProcessor* processor, const ChangeDetails& details);
    void sltWindowClosed();

    // ComponentListener interface
public:
    void componentMovedOrResized(Component& component, bool wasMoved, bool wasResized);

    // AudioProcessorListener interface
public:
    void audioProcessorParameterChangeGestureBegin(AudioProcessor* processor, int parameterIndex);
    void audioProcessorParameterChangeGestureEnd(AudioProcessor* processor, int parameterIndex);
};

#endif // VST3PLUGINH_H
