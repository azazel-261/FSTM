#include "include/commands.h"

#include <iostream>
#include <filesystem>

#include "include/init.h"
#include "include/tracker.h"
#include "include/diff.h"
#include "include/fs.h"
#include "include/timeline.h"

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

        if (!Timelines::Timeline::checkIfLatest(_repositoryPath, currentId))
        {
            std::cout << "You're not on the latest snapshot of the current timeline! Please switch to another timeline or create a new one" << std::endl;
            return -1;
        }
        std::string hash = Snapshots::Snapshot::generateSnapshotHash(_repositoryPath, changedFiles);

        Snapshots::Snapshot s(hash, changedFiles);

        if (std::filesystem::exists(_repositoryPath + "/.fstm/snapshots/" + s.id))
        {
            std::cout << "A snapshot with the same hash already exists: " << s.id << std::endl;
            return -1;
        }

        std::cout << "Initiating snapshot..." << std::endl;
        Filesystem::Filesystem fs = Filesystem::Filesystem::fromSnapshot(_repositoryPath, s, {currentId});
        std::string currentTimeline = Timelines::Timeline::getCurrentName(_repositoryPath);
        std::vector<std::string> parentIds = {};
        if (!currentId.empty()) parentIds.push_back(currentId);
        Snapshots::SnapshotData data(currentTimeline, parentIds);
        Timelines::Timeline tm = Timelines::Timeline::loadByName(_repositoryPath, currentTimeline);

        std::cout << "Writing..." << std::endl;
        s.write(_repositoryPath);
        fs.write(_repositoryPath + "/.fstm/snapshots/" + s.id);
        data.write(_repositoryPath + "/.fstm/snapshots/" + s.id);
        Snapshots::Snapshot::writeMessage(_repositoryPath + "/.fstm/snapshots/" + s.id, _message);

        tm.latestSnapshotId = s.id;
        tm.writeInfoYAML(_repositoryPath);

        Snapshots::setCurrentId(_repositoryPath, s.id);

        std::cout << "Successfully created snapshot " << s.id << std::endl;

        Tracker::untrackAll(_repositoryPath);
        return 0;
    }

    int snapshotTravelCommand(const std::string &_repositoryPath, const std::string &_targetName)
    {
        if (_targetName.ends_with("_tmp"))
        {
            std::cout << "This is a temporary snapshot created by the program itself, cannot proceed" << std::endl;
            return -1;
        }
        if (_targetName == Snapshots::getCurrentId(_repositoryPath))
        {
            std::cout << "Already on this snapshot" << std::endl;
            return 0;
        }

        std::string targetSnapshot = _targetName;
        Snapshots::Snapshot::snapshotUnsaved(_repositoryPath);

        bool temporary = false;

        std::cout << "Searching..." << std::endl;
        if (std::filesystem::exists(_repositoryPath + "/.fstm/snapshots/" + _targetName + "_tmp"))
        {
            targetSnapshot += "_tmp";
            temporary = true;
        }
        if (!std::filesystem::exists(_repositoryPath + "/.fstm/snapshots/" + _targetName))
        {
            std::cout << "Could not find the provided id: " << _targetName << std::endl;
            return -1;
        }

        std::cout << "Travelling to " << _targetName << "..." << std::endl;
        Snapshots::Snapshot::travelToSnapshot(_repositoryPath, targetSnapshot, _targetName);

        if (temporary)
        {
            std::cout << "Cleaning up..." << std::endl;
            std::filesystem::remove_all(_repositoryPath + "/.fstm/snapshots/" + _targetName + "_tmp");
        }

        std::cout << "Done!" << std::endl;
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
        Timelines::Timeline search = Timelines::Timeline::loadByName(_repositoryPath, _name);
        if (!search.name.empty())
        {
            std::cout << "A timeline with this name already exists!" << std::endl;
            return -1;
        }

        std::cout << "Generating timeline data..." << std::endl;
        std::string currentSnapshotId = Snapshots::getCurrentId(_repositoryPath);
        if (currentSnapshotId.empty())
        {
            std::cout << "Cannot retreive a snapshot to start the timeline. Please create a snapshot before creating a new timeline!" << std::endl;
            return -1;
        }
        Timelines::Timeline tm(_name, currentSnapshotId);

        std::cout << "Writing..." << std::endl;
        tm.writeInfoYAML(_repositoryPath);
        Timelines::Timeline::writeCurrent(_repositoryPath, tm.name);

        std::cout << "Created timeline " << tm.name << " and set it as current" << std::endl;
        return 0;
    }

    int timelineTravelCommand(const std::string &_repositoryPath, const std::string &_name)
    {
        Timelines::Timeline search = Timelines::Timeline::loadByName(_repositoryPath, _name);

        if (search.name.empty())
        {
            std::cout << "Cannot find a timeline with this name!" << std::endl;
            return -1;
        }
        if (Timelines::Timeline::getCurrentName(_repositoryPath) == search.name)
        {
            std::cout << "Already on the timeline!" << std::endl;
            return 0;
        }
        std::string targetId = search.latestSnapshotId;

        std::string targetSnapshot = targetId;
        Snapshots::Snapshot::snapshotUnsaved(_repositoryPath);

        bool temporary = false;

        std::cout << "Searching..." << std::endl;
        if (std::filesystem::exists(_repositoryPath + "/.fstm/snapshots/" + targetId + "_tmp"))
        {
            targetSnapshot += "_tmp";
            temporary = true;
        }
        if (!std::filesystem::exists(_repositoryPath + "/.fstm/snapshots/" + targetSnapshot))
        {
            std::cout << "Could not find the provided id: " << targetSnapshot << std::endl;
            return -1;
        }

        std::cout << "Travelling to " << targetId << "..." << std::endl;
        Snapshots::Snapshot::travelToSnapshot(_repositoryPath, targetSnapshot, targetId);

        if (temporary)
        {
            std::cout << "Cleaning up..." << std::endl;
            std::filesystem::remove_all(_repositoryPath + "/.fstm/snapshots/" + targetId + "_tmp");
        }

        std::cout << "Done!" << std::endl;

        Timelines::Timeline::writeCurrent(_repositoryPath, search.name);
        return 0;
    }

    int timelineHistoryCommand(const std::string &_repositoryPath, int depth)
    {
        std::string currentId = Snapshots::getCurrentId(_repositoryPath);
        if (currentId.empty())
        {
            std::cout << "Nothing to show" << std::endl;
            return 0;
        }
        Snapshots::SnapshotData currentData = Snapshots::SnapshotData::load(_repositoryPath + "/.fstm/snapshots/" + currentId);
        std::string currentTimeline = currentData.timeline;
        int currentDepth = 0;
        std::cout << "+-[" << currentTimeline << "]- " << Snapshots::Snapshot::readMessage(_repositoryPath + "/.fstm/snapshots/" + currentId);
        while (currentDepth < depth && !currentData.parentIds.empty())
        {
            currentId = currentData.parentIds[0];
            currentData = Snapshots::SnapshotData::load(_repositoryPath + "/.fstm/snapshots/" + currentId);
            std::cout << "|" << std::endl;
            std::cout << "+-";
            if (currentData.timeline != currentTimeline)
            {
                currentTimeline = currentData.timeline;
                std::cout << "[" << currentTimeline << "]";
            }
            std::cout << "- " << Snapshots::Snapshot::readMessage(_repositoryPath + "/.fstm/snapshots/" + currentId);
            currentDepth++;
        }
        if (!currentData.parentIds.empty())
        {
            std::cout << "|" << std::endl << "..." << std::endl;
        }
        return 0;
    }

    int returnCommand(const std::string &_repositoryPath)
    {
        return 0;
    }
}
