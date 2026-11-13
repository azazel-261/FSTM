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
        Snapshot *parent;

        std::vector<std::string> filePaths;

        Snapshot(const std::string &_id, Snapshot *_parent, std::vector<std::string> _filePaths);
    };
}

#endif //FSTM_SNAPSHOT_H