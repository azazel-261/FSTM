#ifndef FSTM_TRACKER_H
#define FSTM_TRACKER_H

#include <vector>
#include <string>

namespace FSTM::Tracker
{
    bool is_subpath(const std::filesystem::path &path,
                   const std::filesystem::path &base);
    bool checkIfIgnored(const std::string& _repositoryPath, const std::string &_path);
    bool checkIfIgnored(const std::string &_repositoryPath, const std::string &_path, const std::vector<std::string>& _cachedIgnoreList);

    void trackFile(const std::string &_repositoryPath, const std::string &_filePath);
    void untrackFile(const std::string &_repositoryPath, const std::string &_filePath);
    void untrackAll(const std::string &_repositoryPath);

    std::vector<std::string> loadIgnored(const std::string& _repositoryPath);
    std::vector<std::string> getTrackableFiles(const std::string& _repositoryPath, const std::string &_startingPath = ".");
}

#endif //FSTM_TRACKER_H
