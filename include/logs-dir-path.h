#pragma once

#include <filesystem>
#include <filesystem_helper/special_folder_path.h>

namespace Brand
{
    inline std::filesystem::path GetLogsDirPath() { return FilesystemHelper::SpecialFolderPath::GetCommonAppDataPath() / "Avanquest" / "DriverManager" / "logs"; }
}  // namespace Brand
