//
// Created by georg on 18.11.2026.
//

#include "include/commands.h"

#include <iostream>
#include <filesystem>

#include "include/init.h"
#include "include/tracker.h"
#include "include/diff.h"
#include "include/fs.h"

namespace FSTM::Commands
{
    int initCommand(const std::string &_repositoryPath)
    {
        initRepository(_repositoryPath);
        std::cout << "Repository created successfully" << std::endl;
        return 0;
    }

    int snapshotCreateCommand(const std::string &_repositoryPath, const std::string &_message)
    {
        auto changedFiles = Diff::findChangedFiles(_repositoryPath, Tracker::getTrackedFiles(_repositoryPath));
        if (changedFiles.empty())
        {
            std::cout << "No changes in the tracked files since last snapshot. Try using fstm track * to track all your files" << std::endl;
            return 0;
        }
        std::string currentId = Snapshots::getCurrentId(_repositoryPath);
        std::string hash = Snapshots::Snapshot::generateSnapshotHash(_repositoryPath, changedFiles);
        Snapshots::Snapshot s(hash, changedFiles);
        if (std::filesystem::exists(_repositoryPath + "/.fstm/snapshots/" + s.id))
        {
            std::cout << "A snapshot with the same hash already exists: " << s.id << std::endl;
            return -1;
        }
        Filesystem::Filesystem fs = Filesystem::Filesystem::fromSnapshot(_repositoryPath, s, {currentId});
        Snapshots::SnapshotData data("main", {currentId});
        s.write(_repositoryPath);
        fs.write(_repositoryPath + "/.fstm/snapshots/" + s.id);
        data.write(_repositoryPath + "/.fstm/snapshots/" + s.id);
        Snapshots::setCurrentId(_repositoryPath, s.id);
        std::cout << "Successfully created snapshot " << s.id << std::endl;
        Tracker::untrackAll(_repositoryPath);
        return 0;
    }

    int snapshotTravelCommand(const std::string &_repositoryPath, const std::string &_targetName)
    {
        std::string targetSnapshot = _targetName;
        Snapshots::Snapshot::snapshotUnsaved(_repositoryPath);
        if (std::filesystem::exists(_repositoryPath + "/.fstm/snapshots/" + _targetName + "_tmp"))
        {
            targetSnapshot += "_tmp";
        }
        if (!std::filesystem::exists(_repositoryPath + "/.fstm/snapshots/" + _targetName))
        {
            std::cout << "Could not find the provided id" << std::endl;
            return -1;
        }
        Snapshots::Snapshot::travelToSnapshot(_repositoryPath, targetSnapshot);
        return 0;
    }

    int fileTrackCommand(const std::string &_repositoryPath, const std::string &_filename)
    {
        if (_filename == "*")
        {
            const auto trackableFiles = Tracker::getTrackableFiles(_repositoryPath);
            for (const auto &file : trackableFiles)
            {
                Tracker::trackFile(_repositoryPath, file);
            }
            std::cout << "Tracking all files" << std::endl;
            return 0;
        }
        if (!std::filesystem::exists(_filename) || !Tracker::is_subpath(_filename, _repositoryPath))
        {
            std::cout << "File doesn't exist or is outside the repository scope" << std::endl;
            return -1;
        }
        Tracker::trackFile(_repositoryPath, _filename);
        std::cout << "Started tracking " << _filename << std::endl;
        return 0;
    }

    int fileUntrackCommand(const std::string &_repositoryPath, const std::string &_filename)
    {
        if (_filename == "*")
        {
            Tracker::untrackAll(_repositoryPath);
            std::cout << "Removed all files from tracking" << std::endl;
            return 0;
        }
        if (!std::filesystem::exists(_filename) || !Tracker::is_subpath(_filename, _repositoryPath))
        {
            std::cout << "File doesn't exist or is _repositoryPath the repository scope" << std::endl;
            return -1;
        }
        Tracker::untrackFile(_repositoryPath, _filename);
        std::cout << "Stopped tracking " << _filename << std::endl;
        return 0;
    }

    int fileBlacklistCommand(const std::string &_repositoryPath, const std::string &_filename)
    {
        if (!std::filesystem::exists(_filename) || !Tracker::is_subpath(_filename, _repositoryPath))
        {
            std::cout << "File doesn't exist or is outside the repository scope" << std::endl;
            return -1;
        }
        Tracker::blacklistFile(_repositoryPath, _filename);
        Tracker::untrackFile(_repositoryPath, _filename);
        std::cout << "Blacklisted " << _filename << std::endl;
        return 0;
    }

    int fileUnblacklistCommand(const std::string &_repositoryPath, const std::string &_filename)
    {
        if (!std::filesystem::exists(_filename) || !Tracker::is_subpath(_filename, _repositoryPath))
        {
            std::cout << "File doesn't exist or is outside the repository scope" << std::endl;
            return -1;
        }
        Tracker::unblacklistFile(_repositoryPath, _filename);
        std::cout << "Removed from blacklist " << _filename << std::endl;
        return 0;
    }

    int timelineCreateCommand(const std::string &_repositoryPath, const std::string &_name)
    {
        return 0;
    }

    int timelineTravelCommand(const std::string &_repositoryPath, const std::string &_name)
    {
        return 0;
    }
}
