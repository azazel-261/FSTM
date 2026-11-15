#ifndef FSTM_SNAPSHOT_H
#define FSTM_SNAPSHOT_H

#include <filesystem>
#include <vector>

#include "diff.h"

namespace FSTM::Snapshots
{
    std::string generateSnapshotHash(const std::vector<std::string> &_filePaths);

    class Snapshot
    {
    public:
        std::string id;
        std::string parentId;

        std::vector<std::string> filePaths;

        Snapshot(const std::string &_id, const std::string &_parentId, std::vector<std::string> _filePaths);
        Snapshot(const std::string &_parentId, std::vector<std::string> _filePaths);

        void write(const std::string &_repositoryPath) const;
    };
}

#endif //FSTM_SNAPSHOT_H