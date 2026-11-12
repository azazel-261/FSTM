#include "include/snapshot.h"

namespace FSTM::Snapshots
{
    Snapshot::Snapshot(const std::string &_id, Snapshot *_parent)
    {
        this -> id = _id;
        this -> parent = _parent;
    }

    FullSnapshot::FullSnapshot(const std::string &_id, Snapshot *_parent, const std::vector<std::string> &_filePaths) : Snapshot(_id, _parent)
    {
        this -> filePaths = _filePaths;
    }

    DiffSnapshot::DiffSnapshot(const std::string& _id, Snapshot *_parent, const std::vector<std::pair<std::string,
        std::vector<Diff::Diff> > > &_pathDiffs)
            : Snapshot(_id, _parent)
    {
        this -> pathDiffs = _pathDiffs;
    }
}
