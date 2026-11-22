#include "helpers/RaiiHandles.hpp"

#include <unistd.h>

/**
 * @brief Construct a CUniqueFd owning the given file descriptor.
 *
 * @param i32_Fd - POSIX file descriptor (defaults to -1 / invalid)
 */
CUniqueFd::CUniqueFd(int i32_Fd) noexcept : mi32_Fd(i32_Fd) {}

/**
 * @brief Destroy the CUniqueFd and close the managed descriptor if valid.
 */
CUniqueFd::~CUniqueFd() {
    fv_Reset();
}

/**
 * @brief Move-construct a CUniqueFd, transferring ownership from other.
 *
 * @param other - Source CUniqueFd to transfer from
 */
CUniqueFd::CUniqueFd(CUniqueFd&& other) noexcept : mi32_Fd(other.mi32_Fd) {
    other.mi32_Fd = -1;
}

/**
 * @brief Move-assign a CUniqueFd, closing any currently owned descriptor.
 *
 * @param other - Source CUniqueFd to transfer from
 *
 * @return Reference to *this
 */
CUniqueFd& CUniqueFd::operator=(CUniqueFd&& other) noexcept {
    if (&other != this) {
        fv_Reset();
        mi32_Fd = other.mi32_Fd;
        other.mi32_Fd = -1;
    }
    return *this;
}

/**
 * @brief Get the underlying raw file descriptor.
 *
 * @return Managed file descriptor, or -1 if invalid
 */
int CUniqueFd::fi32_Get() const noexcept {
    return mi32_Fd;
}

/**
 * @brief Check whether the managed file descriptor is valid.
 *
 * @return true if descriptor >= 0, false otherwise
 */
CUniqueFd::operator bool() const noexcept {
    return (0 <= mi32_Fd);
}

/**
 * @brief Close the current descriptor if valid and take ownership of i32_NewFd.
 *
 * @param i32_NewFd - Replacement file descriptor (defaults to -1)
 */
void CUniqueFd::fv_Reset(int i32_NewFd) noexcept {
    if (0 <= mi32_Fd) {
        ::close(mi32_Fd);
    }
    mi32_Fd = i32_NewFd;
}

/**
 * @brief Close a non-null directory stream.
 *
 * @param p_Dir - Directory stream pointer
 */
void dir_deleter_t::operator()(DIR* p_Dir) const noexcept {
    if (nullptr != p_Dir) {
        ::closedir(p_Dir);
    }
}

/**
 * @brief Close a non-null pipe stream.
 *
 * @param p_Pipe - Pipe stream pointer
 */
void pipe_deleter_t::operator()(FILE* p_Pipe) const noexcept {
    if (nullptr != p_Pipe) {
        ::pclose(p_Pipe);
    }
}
