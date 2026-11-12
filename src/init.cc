#include "include/init.h"

#include <filesystem>
#include <fstream>

void initRepository(const std::string &path)
{
    std::filesystem::create_directories(path + "/.fstm");

    std::ofstream trackedFile(path + "/.fstm/tracked");
    trackedFile.close();

    std::ofstream blacklistFile(path + "/.fstm/blacklist");
    blacklistFile.close();
}
