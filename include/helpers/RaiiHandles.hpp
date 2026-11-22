#ifndef CPPNEOFETCH_RAII_HANDLES_HPP
#define CPPNEOFETCH_RAII_HANDLES_HPP

#include <dirent.h>
#include <cstdio>
#include <memory>

/**
 * @brief RAII wrapper managing a POSIX file descriptor.
 */
class CUniqueFd {
public:
    /**
     * @brief Construct a CUniqueFd owning the given file descriptor.
     *
     * @param i32_Fd - POSIX file descriptor (defaults to -1 / invalid)
     */
    explicit CUniqueFd(int i32_Fd = -1) noexcept;

    /**
     * @brief Destroy the CUniqueFd and close the managed descriptor if valid.
     */
    ~CUniqueFd();

    CUniqueFd(const CUniqueFd&) = delete;
    CUniqueFd& operator=(const CUniqueFd&) = delete;

    /**
     * @brief Move-construct a CUniqueFd, transferring ownership from other.
     *
     * @param other - Source CUniqueFd to transfer from
     */
    CUniqueFd(CUniqueFd&& other) noexcept;

    /**
     * @brief Move-assign a CUniqueFd, closing any currently owned descriptor.
     *
     * @param other - Source CUniqueFd to transfer from
     *
     * @return Reference to *this
     */
    CUniqueFd& operator=(CUniqueFd&& other) noexcept;

    /**
     * @brief Get the underlying raw file descriptor.
     *
     * @return Managed file descriptor, or -1 if invalid
     */
    int fi32_Get() const noexcept;

    /**
     * @brief Check whether the managed file descriptor is valid.
     *
     * @return true if descriptor >= 0, false otherwise
     */
    explicit operator bool() const noexcept;

    /**
     * @brief Close the current descriptor if valid and take ownership of i32_NewFd.
     *
     * @param i32_NewFd - Replacement file descriptor (defaults to -1)
     */
    void fv_Reset(int i32_NewFd = -1) noexcept;

private:
    int mi32_Fd;
};

/**
 * @brief Custom deleter for DIR* handles using closedir().
 */
struct dir_deleter_t {
    /**
     * @brief Close a non-null directory stream.
     *
     * @param p_Dir - Directory stream pointer
     */
    void operator()(DIR* p_Dir) const noexcept;
};
using UniqueDir = std::unique_ptr<DIR, dir_deleter_t>;

/**
 * @brief Custom deleter for popen() FILE* handles using pclose().
 */
struct pipe_deleter_t {
    /**
     * @brief Close a non-null pipe stream.
     *
     * @param p_Pipe - Pipe stream pointer
     */
    void operator()(FILE* p_Pipe) const noexcept;
};
using UniquePipe = std::unique_ptr<FILE, pipe_deleter_t>;

#endif // CPPNEOFETCH_RAII_HANDLES_HPP
