#pragma once

#include <filesystem>
#include <string>
#include <fstream>

#include <boost/asio.hpp>

namespace zwiibac {
namespace tftp {

class FilePath
{
public:
    std::optional<std::string> GetReadFilePath(const std::string_view& file_name) const;
    std::optional<std::string> GetWriteFilePath(const std::string_view& file_name) const;
    std::optional<size_t> GetFileSize(const std::string& file) const;
    void SetServerRootPath(const std::string& root_path);
    inline void Reset() noexcept {};
private:
    std::filesystem::path root_path_;
};

} // end namespace tftp
} // end namespace zwiibac
    