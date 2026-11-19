#include "include/init.h"

#include <filesystem>
#include <fstream>

#include "include/timeline.h"

void initRepository(const std::string &_repositoryPath)
{
    std::filesystem::create_directories(_repositoryPath + "/.fstm/snapshots");

    std::ofstream trackedFile(_repositoryPath + "/.fstm/tracked", std::ios_base::app);
    trackedFile.close();

    std::ofstream blacklistFile(_repositoryPath + "/.fstm/blacklist", std::ios_base::app);
    blacklistFile.close();

    std::ofstream currentFile(_repositoryPath + "/.fstm/current", std::ios_base::app);
    currentFile.close();

    FSTM::Timelines::Timeline tm("main", "");
    FSTM::Timelines::Timeline::writeCurrent(_repositoryPath, tm.name);
    tm.writeInfoYAML(_repositoryPath);
}
