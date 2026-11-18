#ifndef FSTM_SNAPSHOT_H
#define FSTM_SNAPSHOT_H

#include <filesystem>
#include <vector>

#include "diff.h"

namespace FSTM::Snapshots
{
    std::string getCurrentId(const std::string &_repositoryPath);
    void setCurrentId(const std::string &_repositoryPath, const std::string &_id);

    class Snapshot
    {
    public:
        std::string id;
        std::vector<std::string> filePaths;

        Snapshot(const std::string &_id, std::vector<std::string> _filePaths);
        void write(const std::string &_repositoryPath) const;

        static std::string readMessage(const std::string &_snapshotPath);
        static void writeMessage(const std::string &_snapshotPath, const std::string &_message);

        static void travelToSnapshot(const std::string &_repoPath, const std::string &_snapshotId);

        static void snapshotUnsaved(const std::string &_repoPath);

        static std::string generateSnapshotHash(const std::string &_repositoryPath, const std::vector<std::string> &_filePaths);
    };
    class SnapshotData
    {
    public:
        std::vector<std::string> parentIds;
        std::string timeline;

        SnapshotData();
        SnapshotData(std::string _timeline, std::vector<std::string> _parentIds);

        static SnapshotData loadYAML(const std::string &_data);
        [[nodiscard]] std::string emitYAML() const;

        static SnapshotData load(const std::string &_snapshotPath);
        void write(const std::string &_snapshotPath) const;
    };
}

#endif //FSTM_SNAPSHOT_H