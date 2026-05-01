#include "plugin.h"

static Plugin::Descriptor dummyPluginDescriptor = {
    "dummy",
    "dummy",
    "no description",
    Plugin::Undefined,
    nullptr,

};

Plugin::Plugin(const Descriptor* descriptor, const Descriptor::SubPluginFeatures::Key* key, QObject* parent)
    : QObject(parent)

    , m_descriptor(descriptor)
    , m_key(key ? *key : Descriptor::SubPluginFeatures::Key(m_descriptor))
{
    if (m_descriptor == nullptr) {
        m_descriptor = &dummyPluginDescriptor;
    }
}

template <class T>
T use_this_or(T this_param, T or_param)
{
    return this_param ? this_param : or_param;
}

QString use_this_or(QString this_param, QString or_param)
{
    return this_param.isNull() ? or_param : this_param;
}

QString Plugin::Descriptor::SubPluginFeatures::Key::additionalFileExtensions() const
{
    Q_ASSERT(isValid());
    return desc->subPluginFeatures
        // get from sub plugin
        ? desc->subPluginFeatures->additionalFileExtensions(*this)
        // no sub plugin, so no *additional* file extensions
        : QString();
}

QString Plugin::Descriptor::SubPluginFeatures::Key::displayName() const
{
    Q_ASSERT(isValid());
    return desc->subPluginFeatures
        // get from sub plugin
        ? use_this_or(desc->subPluginFeatures->displayName(*this),
            QString::fromUtf8(desc->displayName))
        // get from plugin
        : desc->displayName;
}

QString Plugin::Descriptor::SubPluginFeatures::Key::description() const
{
    Q_ASSERT(isValid());
    return desc->subPluginFeatures
        ? use_this_or(desc->subPluginFeatures->description(*this),
            QString::fromUtf8(desc->description))
        : desc->description;
}
