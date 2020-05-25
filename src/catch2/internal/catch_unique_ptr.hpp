#ifndef CATCH_UNIQUE_PTR_HPP_INCLUDED
#define CATCH_UNIQUE_PTR_HPP_INCLUDED

#include <type_traits>

namespace Catch {
namespace Detail {
    // reimplementation of unique_ptr for improved compilation times
    // Does not support custom deleters (and thus does not require EBO)
    // Does not support arrays
    template <typename T>
    class unique_ptr {
        T* m_ptr;
    public:
        constexpr unique_ptr(std::nullptr_t = nullptr):
            m_ptr{}
        {}
        explicit constexpr unique_ptr(T* ptr):
            m_ptr(ptr)
        {}

        //template <typename U, typename = std::enable_if_t<std::is_base_of<T, U>::value>>
        template <typename U>
        unique_ptr(unique_ptr<U>&& from):
            m_ptr(from.release())
        {}

        //template <typename U, typename = std::enable_if_t<std::is_base_of<T, U>::value>>
        template <typename U>
        unique_ptr& operator=(unique_ptr<U>&& from) {
            // Guard self-assignment?
            delete m_ptr;
            m_ptr = from.release();

            return *this;
        }

        unique_ptr(unique_ptr const&) = delete;
        unique_ptr& operator=(unique_ptr const&) = delete;

        unique_ptr(unique_ptr&& rhs) noexcept:
            m_ptr(rhs.m_ptr) {
            rhs.m_ptr = nullptr;
        }
        unique_ptr& operator=(unique_ptr&& rhs) noexcept {
            // Using swap here is not standard behaviour of std::unique_ptr,
            // but it let's us avoid branching and complicated logic here.
            // The danger is that the allocation will potentially hang around
            // longer than it should, but without custom deleters this should
            // not matter in practice.

            // Annoyingly, swap is in <utility>, which is... big
            auto temp = rhs.m_ptr;
            rhs.m_ptr = m_ptr;
            m_ptr = temp;

            return *this;
        }

        ~unique_ptr() {
            delete m_ptr;
        }

        // TODO: assert non-null?
        T& operator*() { return *m_ptr; }
        T const& operator*() const { return *m_ptr; }
        T* operator->() const noexcept { return m_ptr; }

        T* get() { return m_ptr; }
        T const* get() const { return m_ptr; }

        void reset(T* ptr = nullptr) {
            delete m_ptr;
            m_ptr = ptr;
        }

        T* release() {
            auto temp = m_ptr;
            m_ptr = nullptr;
            return temp;
        }

        explicit operator bool() const {
            return m_ptr;
        }
    };

    // Purposefully doesn't exist
    // We could also rely on compiler warning + werror for calling plain delete
    // on a T[], but this seems better.
    // Maybe add definition and a static assert?
    template <typename T>
    class unique_ptr<T[]>;

    template <typename T, typename... Args>
    unique_ptr<T> make_unique(Args&&... args) {
        // Static cast to avoid bringing in utility instead?
        return unique_ptr<T>(new T(static_cast<Args&&>(args)...));
    }


} // end namespace Detail
} // end namespace Catch

#endif // CATCH_UNIQUE_PTR_HPP_INCLUDED
