#include "include/init.h"

#include <filesystem>
#include <fstream>

void initRepository(const std::string &path)
{
    std::filesystem::create_directories(path + "/.fstm/snapshots");

    std::ofstream trackedFile(path + "/.fstm/tracked", std::ios_base::app);
    trackedFile.close();

    std::ofstream blacklistFile(path + "/.fstm/blacklist", std::ios_base::app);
    blacklistFile.close();
}
