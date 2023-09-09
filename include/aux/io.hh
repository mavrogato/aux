#ifndef INCLUDE_AUX_IO_HH_5046A96A_9833_49B3_BAAD_52853B11B1BD
#define INCLUDE_AUX_IO_HH_5046A96A_9833_49B3_BAAD_52853B11B1BD

#include <filesystem>
#include <span>
#include <utility>

#include <cassert>

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "unique_handle.hh"

namespace aux::inline io::inline posix
{
    // T.B.D.: unique_handle should not contains the deleter in the instance...
    // class [[nodiscard]] unique_fd : public unique_handle<int, &::close, -1> {
    //     //...
    // };

    class [[nodiscard]] unique_fd {
    public:
        unique_fd(unique_fd const&) = delete;
        unique_fd& operator=(unique_fd const&) = delete;

        explicit unique_fd(int fd = -1) noexcept
            : fd{fd}
            {
            }
        unique_fd(unique_fd&& other) noexcept
            : fd{std::exchange(other.fd, -1)}
            {
            }
        ~unique_fd() noexcept {
            if (this->fd != -1) {
                [[maybe_unused]] auto ret = ::close(fd);
                assert(ret != -1);
                this->fd = -1;
            }
        }
        auto& operator=(unique_fd&& other) noexcept {
            if (auto dispose = std::exchange(this->fd, std::exchange(other.fd, -1)); dispose != -1) {
                [[maybe_unused]] auto ret = ::close(dispose);
                assert(ret != -1);
            }
            return *this;
        }
        operator int() const noexcept { return this->fd; }
        explicit operator bool() const noexcept { return this->fd != -1; }

    private:
        int fd;
    };

    template <class T>
    class [[nodiscard]] unique_mmap : public std::span<T> {
        using base_type = std::span<T>;
        auto& base() noexcept { return static_cast<base_type&>(*this); }

    public:
        unique_mmap(unique_mmap const&) = delete;
        unique_mmap& operator=(unique_mmap const&) = delete;

        explicit unique_mmap(int fd,
                             size_t length,
                             int prot = PROT_READ | PROT_WRITE,
                             int flags = MAP_SHARED,
                             size_t offset = 0,
                             void* target = nullptr) noexcept
            : base_type{}
            {
                if (void* addr = ::mmap(target, length * sizeof (T), prot, flags, fd, offset * sizeof (T)); addr != MAP_FAILED) {
                    static_cast<base_type&>(*this) = {static_cast<T*>(addr), length};
                }
            }
        unique_mmap(unique_mmap&& other) noexcept
            : base_type{std::exchange(other.base(), std::span<T>{})}
            {
            }

        ~unique_mmap() noexcept {
            if (!this->empty() && this->data() != MAP_FAILED) {
                [[maybe_unused]] auto ret = ::munmap(this->data(), this->size() * sizeof (T));
                assert(ret != -1);
            }
        }

        auto& operator=(unique_mmap&& other) noexcept {
            if (auto dispose = std::exchange(this->base(), std::exchange(other.base(), {})); !dispose.empty())
            {
                [[maybe_unused]] auto ret = ::munmap(dispose.data(), dispose.size() * sizeof (T));
                assert(ret != -1);
            }
            return *this;
        }

    public:
        operator T const*() const noexcept { return this->data(); }
        operator T*() noexcept { return this->data(); }
        explicit operator bool() const noexcept { return !this->empty(); }
    };

} // ::aux::inline io::inline posix

#endif/*INCLUDE_AUX_IO_HH_5046A96A_9833_49B3_BAAD_52853B11B1BD*/
