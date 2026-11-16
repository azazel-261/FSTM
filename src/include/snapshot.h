#ifndef FSTM_SNAPSHOT_H
#define FSTM_SNAPSHOT_H

#include <filesystem>
#include <vector>

#include "diff.h"

namespace FSTM::Snapshots
{
    class Snapshot
    {
    public:
        std::string id;
        std::vector<std::string> filePaths;

        Snapshot(const std::string &_id, std::vector<std::string> _filePaths);
        void write(const std::string &_repositoryPath) const;

        static std::string readMessage(const std::string &_snapshotPath);
        static void writeMessage(const std::string &_snapshotPath, const std::string &_message);

        static std::string generateSnapshotHash(const std::string &_repositoryPath, const std::vector<std::string> &_filePaths);
    };
    class SnapshotData
    {
    public:
        std::vector<std::string> parentIds;

        SnapshotData();
        explicit SnapshotData(std::vector<std::string> _parentIds);

        static SnapshotData loadYAML(const std::string &_data);
        [[nodiscard]] std::string emitYAML() const;

        static SnapshotData load(const std::string &_snapshotPath);
        void write(const std::string &_snapshotPath) const;
    };
}

#endif //FSTM_SNAPSHOT_H