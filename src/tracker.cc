#include <fstream>
#include <filesystem>

#include "include/tracker.h"

#include <algorithm>
#include <iostream>

// Copied from stack overflow :3
bool is_subpath(const std::filesystem::path &path,
                const std::filesystem::path &base)
{
    const auto rel = std::filesystem::relative(path, base);
    return !rel.empty() && rel.native()[0] != '.';
}

bool checkIfIgnored(const std::string& _repositoryPath, const std::string &_path)
{
    if (is_subpath(_path, _repositoryPath + "/.fstm")) return true;
    std::ifstream blacklistFile(_repositoryPath + "/.fstm/blacklist",std::ios_base::app);

    std::filesystem::path targetPath = _path;

    if (!std::filesystem::exists(targetPath)) return true;

    if (!blacklistFile)
    {
        blacklistFile.close();
        return false;
    }

    std::string buffer;

    while (std::getline(blacklistFile, buffer))
    {
        if (buffer.empty()) continue;
        std::filesystem::path toCheck = buffer;
        if (!std::filesystem::exists(toCheck)) continue;

        if (is_subpath(_path, buffer) ||
            std::filesystem::equivalent(targetPath, toCheck)) {
            blacklistFile.close();
            return true;
        };
    }

    blacklistFile.close();
    return false;
}

std::vector<std::string> loadIgnored(const std::string& _repositoryPath)
{
    std::ifstream blacklistFile(_repositoryPath + "/.fstm/blacklist",std::ios_base::app);

    if (!blacklistFile)
    {
        return {};
    }

    std::vector<std::string> out;
    std::string buffer;

    while (std::getline(blacklistFile, buffer))
    {
        if (buffer.empty()) continue;
        out.push_back(buffer);
    }

    blacklistFile.close();
    return out;
}

bool checkIfIgnored(const std::string &_repositoryPath, const std::string &_path, const std::vector<std::string>& _cachedIgnoreList)
{
    if (is_subpath(_path, _repositoryPath + "/.fstm")) return true;
    std::filesystem::path targetPath = _path;

    if (!std::filesystem::exists(targetPath)) return true;

    if (_cachedIgnoreList.empty()) return false;

    return std::ranges::any_of(_cachedIgnoreList.begin(), _cachedIgnoreList.end(), [targetPath](const auto& a)
    {
        const std::filesystem::path toCheck = a;
        if (!std::filesystem::exists(toCheck)) return false;
        return is_subpath(targetPath, toCheck) || std::filesystem::equivalent(targetPath, toCheck);
    });
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
        if (buffer.empty()) continue;
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
        if (buffer.empty()) continue;
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

std::vector<std::string> getTrackableFilesRecursive(const std::string& _repositoryPath, const std::string &_path, const std::vector<std::string> &_cachedIgnored)
{
    std::vector<std::string> out;

    for (const auto &p : std::filesystem::directory_iterator(_path))
    {
        const std::string iPath = p.path().string();
        if (checkIfIgnored(_repositoryPath, iPath, _cachedIgnored)) continue;
        if (std::filesystem::is_directory(p))
        {
            auto u = getTrackableFilesRecursive(_repositoryPath, iPath, _cachedIgnored);
            out.insert(out.end(), u.begin(), u.end());
        }
        else
        {
            out.push_back(iPath);
        }
    }
    return out;
}

std::vector<std::string> getTrackableFiles(const std::string& _repositoryPath)
{
    auto ignored = loadIgnored(_repositoryPath);

    return getTrackableFilesRecursive(_repositoryPath, _repositoryPath, ignored);
}
