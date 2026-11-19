#ifndef FSTM_COMMANDS_H
#define FSTM_COMMANDS_H

#include <string>

namespace FSTM::Commands
{
    int initCommand(const std::string &_repositoryPath);

    int returnCommand(const std::string &_repositoryPath);

    // Snapshots
    int snapshotCreateCommand(const std::string &_repositoryPath, const std::string &_message);
    int snapshotTravelCommand(const std::string &_repositoryPath, const std::string &_targetName);

    // Tracking
    int fileTrackCommand(const std::string &_repositoryPath, const std::string &_filename);
    int fileUntrackCommand(const std::string &_repositoryPath, const std::string &_filename);

    int fileBlacklistCommand(const std::string &_repositoryPath, const std::string &_filename);
    int fileUnblacklistCommand(const std::string &_repositoryPath, const std::string &_filename);

    // Timelines
    int timelineCreateCommand(const std::string &_repositoryPath, const std::string &_name);
    int timelineTravelCommand(const std::string &_repositoryPath, const std::string &_name);
    int timelineHistoryCommand(const std::string &_repositoryPath, int depth);
}

#endif //FSTM_COMMANDS_H
