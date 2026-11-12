#ifndef FSTM_TRACKER_H
#define FSTM_TRACKER_H

#include <vector>
#include <string>

bool checkIfIgnored(const std::string& _repositoryPath, const std::string &_filePath);

void trackFile(const std::string &_repositoryPath, const std::string &_filePath);
void untrackFile(const std::string &_repositoryPath, const std::string &_filePath);

std::vector<std::string> getTrackedFiles(std::string _repositoryPath);

#endif //FSTM_TRACKER_H
