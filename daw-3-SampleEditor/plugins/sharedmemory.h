//#ifndef SHAREDMEMORY_H
//#define SHAREDMEMORY_H
//#include <memory>
//#include <string>
//#include <type_traits>

//class SharedMemoryImpl;

//class SharedMemoryData {
//public:
//    SharedMemoryData() noexcept;
//    SharedMemoryData(std::string&& key, bool readOnly);
//    SharedMemoryData(std::string&& key, std::size_t size, bool readOnly);
//    ~SharedMemoryData();

//    SharedMemoryData(SharedMemoryData&& other) noexcept;
//    SharedMemoryData& operator=(SharedMemoryData&& other) noexcept
//    {
//        auto temp = std::move(other);
//        swap(*this, temp);
//        return *this;
//    }

//    friend void swap(SharedMemoryData& a, SharedMemoryData& b) noexcept
//    {
//        using std::swap;
//        swap(a.m_key, b.m_key);
//        swap(a.m_impl, b.m_impl);
//        swap(a.m_ptr, b.m_ptr);
//    }

//    const std::string& key() const noexcept { return m_key; }
//    void* get() const noexcept { return m_ptr; }

//private:
//    std::string m_key;
//    std::unique_ptr<SharedMemoryImpl> m_impl;
//    void* m_ptr = nullptr;
//};

//template <typename T>
//class SharedMemory {
//    // This is stricter than necessary, but keeps things easy for now
//    static_assert(std::is_trivial_v<T>, "objects held in shared memory must be trivial");

//public:
//    SharedMemory() = default;
//    SharedMemory(SharedMemory&&) = default;
//    SharedMemory& operator=(SharedMemory&&) = default;

//    void attach(std::string key)
//    {
//        m_data = SharedMemoryData { std::move(key), std::is_const_v<T> };
//    }

//    void create(std::string key)
//    {
//        m_data = SharedMemoryData { std::move(key), sizeof(T), std::is_const_v<T> };
//    }

//    void detach() noexcept
//    {
//        m_data = SharedMemoryData {};
//    }

//    const std::string& key() const noexcept { return m_data.key(); }
//    T* get() const noexcept { return static_cast<T*>(m_data.get()); }

//    T* operator->() const noexcept { return get(); }
//    T& operator*() const noexcept { return *get(); }
//    explicit operator bool() const noexcept { return get() != nullptr; }

//private:
//    SharedMemoryData m_data;
//};

//template <typename T>
//class SharedMemory<T[]> {
//    // This is stricter than necessary, but keeps things easy for now
//    static_assert(std::is_trivial_v<T>, "objects held in shared memory must be trivial");

//public:
//    SharedMemory() = default;
//    SharedMemory(SharedMemory&&) = default;
//    SharedMemory& operator=(SharedMemory&&) = default;

//    void attach(std::string key)
//    {
//        m_data = SharedMemoryData { std::move(key), std::is_const_v<T> };
//    }

//    void create(std::string key, std::size_t size)
//    {
//        m_data = SharedMemoryData { std::move(key), size * sizeof(T), std::is_const_v<T> };
//    }

//    void detach() noexcept
//    {
//        m_data = SharedMemoryData {};
//    }

//    const std::string& key() const noexcept { return m_data.key(); }
//    T* get() const noexcept { return static_cast<T*>(m_data.get()); }

//    T& operator[](std::size_t index) const noexcept { return get()[index]; }
//    explicit operator bool() const noexcept { return get() != nullptr; }

//private:
//    SharedMemoryData m_data;
//};

//#endif // SHAREDMEMORY_H
