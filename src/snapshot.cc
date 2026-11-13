#include <utility>

#include "include/snapshot.h"

namespace FSTM::Snapshots
{
    Snapshot::Snapshot(const std::string &_id, Snapshot *_parent, std::vector<std::string> _filePaths)
    {
        this -> id = _id;
        this -> parent = _parent;

        this -> filePaths = std::move(_filePaths);
    }
}
