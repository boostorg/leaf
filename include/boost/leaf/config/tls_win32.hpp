#ifndef BOOST_LEAF_CONFIG_TLS_WIN32_HPP_INCLUDED
#define BOOST_LEAF_CONFIG_TLS_WIN32_HPP_INCLUDED

// Copyright 2025 Emil Dotchevski and Reverge Studios, Inc.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// This header implements the TLS API specified in tls.hpp using Win32 TLS
// functions, allowing error objects to cross DLL boundaries on Windows. This
// implementation is enabled by defining BOOST_LEAF_CFG_WIN32=2 before including
// any LEAF headers.

#ifndef _WIN32
#   error "This header is only for Windows"
#endif

#include <atomic>
#include <unordered_map>
#include <cstdint>
#include <new>
#include <stdexcept>
#include <windows.h>
#ifdef min
#   undef min
#endif
#ifdef max
#   undef max
#endif

namespace boost { namespace leaf {

// Thrown on TLS allocation failure.
class win32_tls_error:
    public std::runtime_error
{
public:
    explicit win32_tls_error(char const * what) noexcept:
        std::runtime_error(what)
    {
    }
};

namespace detail
{
    __declspec(noreturn) inline void raise_fail_fast(NTSTATUS status) noexcept
    {
        EXCEPTION_RECORD rec = {};
        rec.ExceptionCode = status;
        rec.ExceptionFlags = EXCEPTION_NONCONTINUABLE;
        RaiseFailFastException(&rec, nullptr, 0);
        BOOST_LEAF_UNREACHABLE;
    }

    template <class T, class... Args>
    T * heap_new(Args && ... args) noexcept
    {
        void * mem = HeapAlloc(GetProcessHeap(), 0, sizeof(T));
        if (!mem)
        {
            raise_fail_fast(STATUS_NO_MEMORY);
            BOOST_LEAF_UNREACHABLE;
        }
        return new (mem) T(static_cast<Args &&>(args)...);
    }

    template <class T>
    void heap_delete(T * p) noexcept
    {
        if (p)
        {
            p->~T();
            BOOL r = HeapFree(GetProcessHeap(), 0, p);
            BOOST_LEAF_ASSERT(r), (void) r;
        }
    }

    template <class T>
    class heap_allocator
    {
    public:

        using value_type = T;

        heap_allocator() noexcept = default;

        template <class U>
        heap_allocator(heap_allocator<U> const &) noexcept
        {
        }

        T * allocate(std::size_t n) noexcept
        {
            if (void * p = HeapAlloc(GetProcessHeap(), 0, n * sizeof(T)))
                return static_cast<T *>(p);
            raise_fail_fast(STATUS_NO_MEMORY);
            BOOST_LEAF_UNREACHABLE;
        }

        void deallocate(T * p, std::size_t) noexcept
        {
            BOOL r = HeapFree(GetProcessHeap(), 0, p);
            BOOST_LEAF_ASSERT(r), (void) r;
        }

        friend bool operator==(heap_allocator const &, heap_allocator const &) noexcept { return true; }
        friend bool operator!=(heap_allocator const &, heap_allocator const &) noexcept { return false; }
    };

    class critical_section_lock
    {
        critical_section_lock(critical_section_lock const &) = delete;
        critical_section_lock & operator=(critical_section_lock const &) = delete;

        CRITICAL_SECTION & cs_;

    public:

        explicit critical_section_lock(CRITICAL_SECTION & cs) noexcept:
            cs_(cs)
        {
            EnterCriticalSection(&cs_);
        }

        ~critical_section_lock() noexcept
        {
            LeaveCriticalSection(&cs_);
        }
    };

    using atomic_unsigned_int = std::atomic<unsigned int>;

    template <int N, int I>
    struct cpp11_hash_step
    {
        BOOST_LEAF_ALWAYS_INLINE constexpr static std::uint32_t compute(char const (&str)[N], std::uint32_t hash) noexcept
        {
            return cpp11_hash_step<N, I - 1>::compute(str, (hash ^ static_cast<std::uint32_t>(str[I])) * 16777619u);
        }
    };

    template <int N>
    struct cpp11_hash_step<N, -1>
    {
        BOOST_LEAF_ALWAYS_INLINE constexpr static std::uint32_t compute(char const (&)[N], std::uint32_t hash) noexcept
        {
            return hash;
        }
    };

    template <int N>
    BOOST_LEAF_ALWAYS_INLINE constexpr std::uint32_t cpp11_hash_string(char const (&str)[N]) noexcept
    {
        return cpp11_hash_step<N, N - 2>::compute(str, 2166136261u); // str[N-2] is the last character before the \0.
    }
} // namespace detail

namespace n
{
    template <class T>
    BOOST_LEAF_ALWAYS_INLINE constexpr std::uint32_t __cdecl h() noexcept
    {
        return detail::cpp11_hash_string(BOOST_LEAF_PRETTY_FUNCTION);
    }
}

namespace detail
{
    template<class T>
    BOOST_LEAF_ALWAYS_INLINE constexpr std::uint32_t type_hash() noexcept
    {
        return n::h<T>();
    }
}

} } // namespace boost::leaf

////////////////////////////////////////

namespace boost { namespace leaf {

namespace detail
{
    class slot_map
    {
        slot_map(slot_map const &) = delete;
        slot_map & operator=(slot_map const &) = delete;

        class tls_slot_index
        {
            tls_slot_index(tls_slot_index const &) = delete;
            tls_slot_index & operator=(tls_slot_index const &) = delete;
            tls_slot_index & operator=(tls_slot_index &&) = delete;

            DWORD idx_;

        public:

            BOOST_LEAF_ALWAYS_INLINE tls_slot_index():
                idx_(TlsAlloc())
            {
                if (idx_ == TLS_OUT_OF_INDEXES)
                    throw_exception_(win32_tls_error("TLS_OUT_OF_INDEXES"));
            }

            BOOST_LEAF_ALWAYS_INLINE ~tls_slot_index() noexcept
            {
                if (idx_ == TLS_OUT_OF_INDEXES)
                    return;
                BOOL r = TlsFree(idx_);
                BOOST_LEAF_ASSERT(r), (void) r;
            }

            BOOST_LEAF_ALWAYS_INLINE tls_slot_index(tls_slot_index && other) noexcept:
                idx_(other.idx_)
            {
                other.idx_ = TLS_OUT_OF_INDEXES;
            }

            BOOST_LEAF_ALWAYS_INLINE DWORD get() const noexcept
            {
                BOOST_LEAF_ASSERT(idx_ != TLS_OUT_OF_INDEXES);
                return idx_;
            }
        };

        int refcount_;
        HANDLE const mapping_;
        tls_slot_index const error_id_slot_;
        mutable CRITICAL_SECTION cs_;
        std::unordered_map<
            std::uint32_t,
            tls_slot_index,
            std::hash<std::uint32_t>,
            std::equal_to<std::uint32_t>,
            heap_allocator<std::pair<std::uint32_t const, tls_slot_index>>> map_;
        atomic_unsigned_int error_id_storage_;

    public:

        explicit slot_map(HANDLE mapping) noexcept:
            refcount_(1),
            mapping_(mapping),
            error_id_storage_(1)
        {
            BOOST_LEAF_ASSERT(mapping != INVALID_HANDLE_VALUE);
            InitializeCriticalSection(&cs_);
        }

        ~slot_map() noexcept
        {
            DeleteCriticalSection(&cs_);
            BOOL r = CloseHandle(mapping_);
            BOOST_LEAF_ASSERT(r), (void) r;
        }

        BOOST_LEAF_ALWAYS_INLINE void add_ref() noexcept
        {
            BOOST_LEAF_ASSERT(refcount_ >= 1);
            ++refcount_;
        }

        BOOST_LEAF_ALWAYS_INLINE void release() noexcept
        {
            --refcount_;
            BOOST_LEAF_ASSERT(refcount_ >= 0);
            if (refcount_ == 0)
                heap_delete(this);
        }

        DWORD check(std::uint32_t type_hash) const noexcept
        {
            critical_section_lock lock(cs_);
            auto it = map_.find(type_hash);
            return (it != map_.end()) ? it->second.get() : TLS_OUT_OF_INDEXES;
        }

        DWORD get(std::uint32_t type_hash)
        {
            critical_section_lock lock(cs_);
            DWORD idx = map_[type_hash].get();
            BOOST_LEAF_ASSERT(idx != TLS_OUT_OF_INDEXES);
            return idx;
        }

        BOOST_LEAF_ALWAYS_INLINE DWORD error_id_slot() const noexcept
        {
            return error_id_slot_.get();
        }

        BOOST_LEAF_ALWAYS_INLINE atomic_unsigned_int & error_id_storage() noexcept
        {
            return error_id_storage_;
        }
    }; // class slot_map

    class module_state
    {
        module_state(module_state const &) = delete;
        module_state & operator=(module_state const &) = delete;

        static constexpr unsigned tls_failure_create_mapping = 0x01;
        static constexpr unsigned tls_failure_map_view = 0x02;

        void * hinstance_;
        unsigned tls_failures_;
        slot_map * sm_;

    public:

        constexpr module_state() noexcept:
            hinstance_(nullptr),
            tls_failures_(0),
            sm_(nullptr)
        {
        }

        BOOST_LEAF_ALWAYS_INLINE slot_map & sm() const noexcept
        {
            BOOST_LEAF_ASSERT(hinstance_);
            BOOST_LEAF_ASSERT(!(tls_failures_ & tls_failure_create_mapping));
            BOOST_LEAF_ASSERT(!(tls_failures_ & tls_failure_map_view));
            BOOST_LEAF_ASSERT(sm_);
            return *sm_;
        }

        BOOST_LEAF_ALWAYS_INLINE void update(PVOID hinstDLL, DWORD dwReason) noexcept
        {
            if (dwReason == DLL_PROCESS_ATTACH)
            {
                hinstance_ = hinstDLL;
                char name[32] = "Local\\boost_leaf_";
                {
                    constexpr static char const hex[] = "0123456789ABCDEF";
                    DWORD pid = GetCurrentProcessId();
                    for (int i = 7; i >= 0; --i)
                    {
                        name[17 + i] = hex[pid & 0xf];
                        pid >>= 4;
                    }
                    name[25] = '\0';
                }
                HANDLE mapping = CreateFileMappingA(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, sizeof(slot_map *), name);
                DWORD mapping_status = GetLastError();
                if (!mapping)
                {
                    tls_failures_ |= tls_failure_create_mapping;
                    return;
                }
                BOOST_LEAF_ASSERT(mapping_status == ERROR_ALREADY_EXISTS || mapping_status == ERROR_SUCCESS);
                bool is_first_module = (mapping_status == ERROR_SUCCESS);
                slot_map * * mapped_ptr = static_cast<slot_map * *>(MapViewOfFile(mapping, FILE_MAP_WRITE, 0, 0, sizeof(slot_map *)));
                if (!mapped_ptr)
                {
                    tls_failures_ |= tls_failure_map_view;
                    BOOL r = CloseHandle(mapping);
                    BOOST_LEAF_ASSERT(r), (void) r;
                    return;
                }
                if (is_first_module)
                    sm_ = *mapped_ptr = heap_new<slot_map>(mapping);
                else
                {
                    sm_ = *mapped_ptr;
                    sm_->add_ref();
                    BOOL r = CloseHandle(mapping);
                    BOOST_LEAF_ASSERT(r), (void) r;
                }
                UnmapViewOfFile(mapped_ptr);
            }
            else if (dwReason == DLL_PROCESS_DETACH)
            {
                BOOST_LEAF_ASSERT(sm_ || tls_failures_);
                if (sm_)
                {
                    sm_->release();
                    sm_ = nullptr;
                }
            }
        }
    }; // class module_state

    template<int = 0>
    struct module
    {
        static module_state state;
    };

    template<int N>
    module_state module<N>::state;

    BOOST_LEAF_ALWAYS_INLINE unsigned generate_next_error_id() noexcept
    {
        static atomic_unsigned_int & counter = module<>::state.sm().error_id_storage();
        unsigned id = (counter += 4);
        BOOST_LEAF_ASSERT((id&3) == 1);
        return id;
    }

    inline void NTAPI tls_callback(PVOID hinstDLL, DWORD dwReason, PVOID) noexcept
    {
        module<>::state.update(hinstDLL, dwReason);
    }

#ifdef _MSC_VER
#   pragma section(".CRT$XLB", long, read)
#   pragma data_seg(push, ".CRT$XLB")

extern "C" __declspec(selectany) PIMAGE_TLS_CALLBACK boost_leaf_tls_callback = tls_callback;

#   pragma data_seg(pop)
#   ifdef _WIN64
#       pragma comment(linker, "/INCLUDE:boost_leaf_tls_callback")
#   else
#       pragma comment(linker, "/INCLUDE:_boost_leaf_tls_callback")
#   endif
#elif defined(__GNUC__)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wattributes"

extern "C" __attribute__((used, selectany)) PIMAGE_TLS_CALLBACK boost_leaf_tls_callback __attribute__((section(".CRT$XLB"))) = tls_callback;

#   pragma GCC diagnostic pop
#else
#   error Unknown compiler, unable to define .CRT$XLB section
#endif
} // namespace detail

} } // namespace boost::leaf

////////////////////////////////////////

namespace boost { namespace leaf {

namespace tls
{
    BOOST_LEAF_ALWAYS_INLINE unsigned generate_next_error_id() noexcept
    {
        return detail::generate_next_error_id();
    }

    BOOST_LEAF_ALWAYS_INLINE void write_current_error_id(unsigned x) noexcept
    {
        using namespace detail;
        DWORD slot = module<>::state.sm().error_id_slot();
        BOOL r = TlsSetValue(slot, reinterpret_cast<void *>(static_cast<std::uintptr_t>(x)));
        BOOST_LEAF_ASSERT(r), (void) r;
    }

    BOOST_LEAF_ALWAYS_INLINE unsigned read_current_error_id() noexcept
    {
        using namespace detail;
        DWORD slot = module<>::state.sm().error_id_slot();
        LPVOID value = TlsGetValue(slot);
        BOOST_LEAF_ASSERT(GetLastError() == ERROR_SUCCESS);
        return static_cast<unsigned>(reinterpret_cast<std::uintptr_t>(value));
    }

    template <class T>
    BOOST_LEAF_ALWAYS_INLINE void reserve_ptr()
    {
        using namespace detail;
        thread_local DWORD const cached_slot = module<>::state.sm().get(type_hash<T>());
        BOOST_LEAF_ASSERT(cached_slot != TLS_OUT_OF_INDEXES), (void) cached_slot;
    }

    template <class T>
    BOOST_LEAF_ALWAYS_INLINE void write_ptr(T * p) noexcept
    {
        using namespace detail;
        thread_local DWORD const cached_slot = module<>::state.sm().check(type_hash<T>());
        DWORD slot = cached_slot;
        BOOST_LEAF_ASSERT(slot != TLS_OUT_OF_INDEXES);
        BOOL r = TlsSetValue(slot, p);
        BOOST_LEAF_ASSERT(r), (void) r;
    }

    template <class T>
    BOOST_LEAF_ALWAYS_INLINE T * read_ptr() noexcept
    {
        using namespace detail;
        thread_local DWORD cached_slot = TLS_OUT_OF_INDEXES;
        if (cached_slot == TLS_OUT_OF_INDEXES)
            cached_slot = module<>::state.sm().check(type_hash<T>());
        DWORD slot = cached_slot;
        if (slot == TLS_OUT_OF_INDEXES)
            return nullptr;
        LPVOID value = TlsGetValue(slot);
        BOOST_LEAF_ASSERT(GetLastError() == ERROR_SUCCESS);
        return static_cast<T *>(value);
    }
} // namespace tls

} } // namespace boost::leaf

#endif // #ifndef BOOST_LEAF_CONFIG_TLS_WIN32_HPP_INCLUDED
