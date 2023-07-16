#ifndef INCLUDE_AUX_IO_HH_5046A96A_9833_49B3_BAAD_52853B11B1BD
#define INCLUDE_AUX_IO_HH_5046A96A_9833_49B3_BAAD_52853B11B1BD

#include <filesystem>
#include <span>
#include <utility>
#include <stdexcept>

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

namespace aux::inline io::inline posix
{
    class [[nodiscard]] unique_fd {
    public:
        unique_fd(unique_fd const&) = delete;
        unique_fd& operator=(unique_fd const&) = delete;

        explicit unique_fd(int fd = -1) noexcept
            : fd{fd}
            {
            }
        explicit unique_fd(char const* path, int oflag, auto... args)
            : fd{::open(path, oflag, args...)}
            {
                if (this->fd == -1) {
                    std::runtime_error("open failed...");
                }
            }
        unique_fd(unique_fd&& other) noexcept
            : fd{std::exchange(other.fd, -1)}
            {
            }
        ~unique_fd() noexcept {
            if (this->fd != -1) {
                ::close(fd);
                this->fd = -1;
            }
        }
        auto& operator=(unique_fd&& other) noexcept {
            if (this != &other) {
                std::swap(this->fd, other.fd);
            }
            return *this;
        }
        operator int() const noexcept { return this->fd; }

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
                             void* target = nullptr)
            : base_type{}
            , ptr{static_cast<T*>(::mmap(target, length * sizeof (T), prot, flags, fd, offset * sizeof (T)))}
            {
                if (this->ptr == MAP_FAILED) {
                    throw std::runtime_error("mmap failed...");
                }
                this->base() = base_type(this->ptr, length);
            }
        unique_mmap(unique_mmap&& other) noexcept
            : base_type{std::exchange(other.base(), std::span<T>{})}
            , ptr{std::exchange(other.ptr, reinterpret_cast<T*>(MAP_FAILED))}
            {
            }

        ~unique_mmap() noexcept {
            if (this->ptr != MAP_FAILED) {
                ::munmap(this->ptr, this->size() * sizeof (T));
                this->ptr = nullptr;
            }
        }

        auto& operator=(unique_mmap&& other) noexcept {
            if (this != &other) {
                std::swap(this->base(), other.base());
                std::swap(this->ptr, other.ptr);
            }
            return *this;
        }

    public:
        operator T const*() const noexcept { return this->data(); }
        operator T*() noexcept { return this->data(); }

    private:
        T* ptr;
    };

} // ::aux::inline io::inline posix

#endif/*INCLUDE_AUX_IO_HH_5046A96A_9833_49B3_BAAD_52853B11B1BD*/
