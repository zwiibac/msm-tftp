#include <filesystem>
#include <limits>
#include <optional>
#include <algorithm>
#include <string_view>
#include <system_error>
#include <zwiibac/tftp/data/file_path.h>

namespace zwiibac {
namespace tftp {

namespace fs = std::filesystem;

std::optional<std::string> FilePath::GetReadFilePath(const std::string_view& file_name) const 
{
    std::error_code ec;
    auto file_path = fs::canonical(root_path_ / std::string(file_name), ec);

    return !ec 
            && (std::mismatch(root_path_.begin(), root_path_.end(), file_path.begin(), file_path.end()).first == root_path_.end())
            ? std::optional(file_path.string()) : std::nullopt;
}

std::optional<std::string> FilePath::GetWriteFilePath(const std::string_view& file_name) const 
{
    fs::path relativ_file_name_path{file_name.cbegin(), file_name.cend()};

    std::error_code ec;
    auto file_path = fs::canonical(root_path_ / relativ_file_name_path.parent_path(), ec);
    file_path /= relativ_file_name_path.filename();

    return !ec 
            && (std::mismatch(root_path_.begin(), root_path_.end(), file_path.begin(), file_path.end()).first == root_path_.end())
            ? std::optional(file_path.string()) : std::nullopt;
}

std::optional<size_t> FilePath::GetFileSize(const std::string& file) const 
{
    std::error_code ec;
    auto size = std::filesystem::file_size({file}, ec);
    return !ec
            ? std::optional(size) : std::nullopt;
};

void FilePath::SetServerRootPath(const std::string& root_path) 
{
    root_path_ = fs::canonical(root_path);
}

} // end namespace tftp
} // end namespace zwiibac