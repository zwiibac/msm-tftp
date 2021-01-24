    #pragma once

#include <boost/filesystem/path.hpp>
#include <string>
#include <fstream>

#include <boost/asio.hpp>
#include <boost/filesystem.hpp>

namespace zwiibac {
namespace tftp {

class FilePath
{
public:
    std::optional<std::string> GetReadFilePath(const std::string_view& file_name) const;
    std::optional<std::string> GetWriteFilePath(const std::string_view& file_name) const;
    void SetServerRootPath(const std::string& root_path);
    inline void Reset() noexcept {};
private:
    boost::filesystem::path root_path_;
};

} // end namespace tftp
} // end namespace zwiibac
    