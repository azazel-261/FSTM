#include <fstream>
#include <filesystem>

#include "include/tracker.h"

#include <iostream>

// Copied from stack overflow :3
bool is_subpath(const std::filesystem::path &path,
                const std::filesystem::path &base)
{
    const auto rel = std::filesystem::relative(path, base);
    return !rel.empty() && rel.native()[0] != '.';
}

bool checkIfIgnored(const std::string& _repositoryPath, const std::string &_filePath)
{
    std::ifstream blacklistFile(_repositoryPath + "/.fstm/blacklist",std::ios_base::app);

    std::filesystem::path targetFile = _filePath;

    if (!std::filesystem::exists(targetFile)) return true;

    if (!blacklistFile)
    {
        blacklistFile.close();
        return false;
    }

    std::string buffer;

    while (std::getline(blacklistFile, buffer))
    {
        std::filesystem::path toCheck = buffer;
        if (!std::filesystem::exists(toCheck)) continue;

        if (is_subpath(_filePath, buffer) ||
            std::filesystem::equivalent(targetFile, toCheck)) {
            blacklistFile.close();
            return true;
        };
    }

    blacklistFile.close();
    return false;
}

void trackFile(const std::string &_repositoryPath, const std::string &_filePath)
{
    if (!std::filesystem::exists(_filePath))
    {
        return;
    }
    if (checkIfIgnored(_repositoryPath, _filePath))
    {
        return;
    }
    std::fstream trackedFile(_repositoryPath + "/.fstm/tracked", std::ios::in | std::ios::app);

    std::string buffer;

    while (std::getline(trackedFile, buffer))
    {
        if (std::filesystem::path(buffer) == std::filesystem::path(_filePath))
        {
            trackedFile.close();
            return;
        }
    }

    trackedFile.clear();

    trackedFile << _filePath << std::endl;
    trackedFile.close();
}

void untrackFile(const std::string &_repositoryPath, const std::string &_filePath)
{
    std::ifstream oldTracked(_repositoryPath + "/.fstm/tracked");

    std::string buffer;

    std::ofstream newTracked(_repositoryPath + "/.fstm/tracked.tmp");

    while (std::getline(oldTracked, buffer))
    {
        if (std::filesystem::path(buffer) != std::filesystem::path(_filePath))
        {
            newTracked << buffer << std::endl;
        }
    }

    oldTracked.close();
    newTracked.close();

    remove((_repositoryPath + "/.fstm/tracked").c_str());
    rename((_repositoryPath + "/.fstm/tracked.tmp").c_str(), (_repositoryPath + "/.fstm/tracked").c_str());
}
