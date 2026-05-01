//#include "sharedmemory.h"

//#include <QSharedMemory>
//#include <QString>

//class SharedMemoryImpl {
//public:
//    SharedMemoryImpl(const std::string& key, bool readOnly)
//        : m_shm { QString::fromStdString(key) }
//    {
//        const auto mode = readOnly ? QSharedMemory::ReadOnly : QSharedMemory::ReadWrite;
//        if (!m_shm.attach(mode)) {
//            throw std::runtime_error { "SharedMemoryImpl: QSharedMemory::attach() failed" };
//        }
//    }

//    SharedMemoryImpl(const std::string& key, std::size_t size, bool readOnly)
//        : m_shm { QString::fromStdString(key) }
//    {
//        const auto mode = readOnly ? QSharedMemory::ReadOnly : QSharedMemory::ReadWrite;
//        if (!m_shm.create(size, mode)) {
//            throw std::runtime_error { "SharedMemoryImpl: QSharedMemory::create() failed" };
//        }
//    }

//    SharedMemoryImpl(const SharedMemoryImpl&) = delete;
//    SharedMemoryImpl& operator=(const SharedMemoryImpl&) = delete;

//    void* get() { return m_shm.data(); }

//private:
//    QSharedMemory m_shm;
//};

//SharedMemoryData::SharedMemoryData() noexcept = default;

//SharedMemoryData::SharedMemoryData(std::string&& key, bool readOnly)
//    : m_key { std::move(key) }
//    , m_impl { std::make_unique<SharedMemoryImpl>(m_key, readOnly) }
//    , m_ptr { m_impl->get() }
//{
//}

//SharedMemoryData::SharedMemoryData(std::string&& key, std::size_t size, bool readOnly)
//    : m_key { std::move(key) }
//    , m_impl { std::make_unique<SharedMemoryImpl>(m_key, std::max(size, std::size_t { 1 }), readOnly) }
//    , m_ptr { m_impl->get() }
//{
//}

//SharedMemoryData::~SharedMemoryData() = default;

//SharedMemoryData::SharedMemoryData(SharedMemoryData&& other) noexcept
//    : m_key { std::move(other.m_key) }
//    , m_impl { std::move(other.m_impl) }
//    , m_ptr { other.m_ptr }
//{
//}
