#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <optional>
#include <algorithm>
#include <string_view>
#include <system_error>
#include <zwiibac/tftp/data/file_path.h>

namespace zwiibac {
namespace tftp {

namespace fs = boost::filesystem;

std::optional<std::string> FilePath::GetReadFilePath(const std::string_view& file_name) const 
{
    boost::system::error_code ec;
    auto file_path = fs::canonical({file_name.cbegin(), file_name.cend()}, root_path_, ec);

    return !ec 
            && file_path.size() > root_path_.size() 
            && (std::mismatch(root_path_.begin(), root_path_.end(), file_path.begin(), file_path.end()).first == root_path_.end())
            ? std::optional(file_path.string()) : std::nullopt;
}

std::optional<std::string> FilePath::GetWriteFilePath(const std::string_view& file_name) const 
{
    fs::path relativ_file_name_path{file_name.cbegin(), file_name.cend()};

    boost::system::error_code ec;
    auto file_path = fs::canonical(relativ_file_name_path.parent_path(), root_path_, ec);
    file_path /= relativ_file_name_path.filename();

    return !ec 
            && file_path.size() > root_path_.size() 
            && (std::mismatch(root_path_.begin(), root_path_.end(), file_path.begin(), file_path.end()).first == root_path_.end())
            ? std::optional(file_path.string()) : std::nullopt;
}

void FilePath::SetServerRootPath(const std::string& root_path) 
{
    root_path_ = fs::canonical(root_path);
}

} // end namespace tftp
} // end namespace zwiibac