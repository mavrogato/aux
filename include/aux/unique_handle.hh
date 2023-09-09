#pragma once

#include <utility>

namespace aux
{
    template <class T, class D, T NIL>
    class [[nodiscard]] unique_handle {
    public:
        unique_handle(T handle, D deleter) noexcept
            : handle{handle}
            , deleter{deleter}
            {
            }
        ~unique_handle() noexcept {
            if (this->handle != NIL) {
                this->deleter(handle);
            }
        }

        unique_handle(unique_handle const&) = delete;
        unique_handle& operator=(unique_handle const&) = delete;

        unique_handle(unique_handle&& other) noexcept
            : handle{std::exchange(other.handle, NIL)}
            , deleter{other.deleter}
            {
            }

        unique_handle& operator=(unique_handle&& other) noexcept {
            if (auto dispose = std::exchange(this->handle, std::exchange(other.handle, NIL)); dispose != NIL) {
                this->deleter(dispose);
            }
            return *this;
        }

        auto get() const noexcept { return this->handle; }
        operator T() const noexcept { return this->get(); }
        explicit operator bool() const { return this->handle != NIL; }

    private:
        T handle;
        D deleter;
    };
} // ::aux
