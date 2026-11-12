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

        Snapshot(const std::string &_id, Snapshot *_parent);
    };

    class FullSnapshot : public Snapshot
    {
    public:
        std::vector<std::string> filePaths;

        FullSnapshot(const std::string &_id, Snapshot *_parent, const std::vector<std::string> &_filePaths);
    };

    class DiffSnapshot : public Snapshot
    {
        std::vector<std::pair<std::string, std::vector<Diff::Diff>>> pathDiffs;

        DiffSnapshot(const std::string& _id, Snapshot *_parent, const std::vector<std::pair<std::string, std::vector<Diff::Diff>>> &_pathDiffs);
    };
}

#endif //FSTM_SNAPSHOT_H