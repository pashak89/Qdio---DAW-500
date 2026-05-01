#ifndef PLUGIN_H
#define PLUGIN_H

#include <QMap>
#include <QObject>
class Plugin : public QObject {
    Q_OBJECT
public:
    enum PluginTypes {
        Undefined,
        Effect, // effect-plugin for effect-board

    };

    //! Descriptor holds information about a plugin - every external plugin
    //! has to instantiate such a Descriptor in an extern "C"-section so that
    //! the plugin-loader is able to access information about the plugin
    struct Descriptor {
        const char* name;
        const char* displayName;
        const char* description;
        PluginTypes type;

        /**
            Access to non-key-data of a sub plugin

            If you consider sub plugin keys as keys in a
            key-value-map, this is the lookup for the corresponding
            values. In order to have flexibility between different
            plugin APIs, this is rather an array of fixed data,
            but a bunch of virtual functions taking the key and
            returning some values (or modifying objects of other
            classes).
         */
        class SubPluginFeatures {
        public:
            /**
                Key reference a Plugin::Descriptor, and,
                if the plugin has sub plugins, also reference
                its sub plugin (using the attributes).
                When keys are saved, those attributes are
                written to XML in order to find the right sub
                plugin when realoading.

                @note Any data that is not required to reference
                    the right Plugin or sub plugin should
                    not be here (but rather in
                    SubPluginFeatures, which are like values
                    in a key-value map).
            */
            struct Key {
                using AttributeMap = QMap<QString, QString>;

                inline Key(const Plugin::Descriptor* desc = nullptr,
                    const QString& name = QString(),
                    const AttributeMap& am = AttributeMap())
                    : desc(desc)
                    , name(name)
                    , attributes(am)
                {
                }

                inline bool isValid() const
                {
                    return desc != nullptr;
                }

                //! Key to subplugin: reference to parent descriptor
                //! Key to plugin: reference to its descriptor
                const Plugin::Descriptor* desc;
                //! Descriptive name like "Calf Phaser".
                //! Not required for key lookup and not saved
                //! only used sometimes to temporary store descriptive names
                //! @todo This is a bug, there should be a function
                //!   in SubPluginFeatures (to get the name) instead
                QString name;
                //! Attributes that make up the key and identify
                //! the sub plugin. They are being loaded and saved
                AttributeMap attributes;

                // helper functions to retrieve data that is
                // not part of the key, but mapped via desc->subPluginFeatures
                QString additionalFileExtensions() const;
                QString displayName() const;
                QString description() const;
            };

            using KeyList = QList<Key>;

            SubPluginFeatures(Plugin::PluginTypes type)
                : m_type(type)
            {
            }

            virtual ~SubPluginFeatures() = default;

            virtual void fillDescriptionWidget(QWidget*, const Key*) const
            {
            }

            //! While PluginFactory only collects the plugins,
            //! this function is used by widgets like EffectSelectDialog
            //! to find all possible sub plugins
            virtual void listSubPluginKeys(const Plugin::Descriptor*, KeyList&) const
            {
            }

        private:
            // You can add values mapped by "Key" below
            // The defaults are sane, i.e. redirect to sub plugin's
            // supererior descriptor

            virtual QString additionalFileExtensions(const Key&) const
            {
                return QString();
            }

            virtual QString displayName(const Key& k) const
            {
                return k.isValid() ? k.name : QString();
            }

            virtual QString description(const Key& k) const
            {
                return k.isValid() ? k.desc->description : QString();
            }

        protected:
            const Plugin::PluginTypes m_type;
        };

        SubPluginFeatures* subPluginFeatures;
    };
    // typedef a list so we can easily work with list of plugin descriptors
    using DescriptorList = QList<Descriptor*>;

    //! Constructor of a plugin
    //! @param key Sub plugins must pass a key here, optional otherwise.
    //!   See the key() function
    Plugin(const Descriptor* descriptor,
        const Descriptor::SubPluginFeatures::Key* key = nullptr, QObject* parent = 0);
    ~Plugin() override = default;

    //! Return plugin type
    inline PluginTypes type() const
    {
        return m_descriptor->type;
    }

    //! Return plugin Descriptor
    inline const Descriptor* descriptor() const
    {
        return m_descriptor;
    }

    //! Return the key referencing this plugin. If the Plugin has no
    //! sub plugin features, the key is pretty useless. If it has,
    //! this key will also contain the sub plugin attributes, and will be
    //! a key to those SubPluginFeatures.
    inline const Descriptor::SubPluginFeatures::Key& key() const
    {
        return m_key;
    }

private:
    const Descriptor* m_descriptor;

    Descriptor::SubPluginFeatures::Key m_key;
};

#endif // PLUGIN_H
