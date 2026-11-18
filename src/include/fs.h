#ifndef FSTM_FS_H
#define FSTM_FS_H
#include <string>
#include <map>

#include "snapshot.h"
#include "../external/include/rapidyaml.v0.16.0.singlesrc.hpp"

namespace FSTM::Filesystem
{
    class Filesystem
    {
    public:
        std::map<std::string, std::string> latestFileIds;

        Filesystem();
        explicit Filesystem(std::map<std::string, std::string> _latestFileIds);

        static Filesystem loadYAML(const std::string &_data);
        [[nodiscard]] std::string emitYAML() const;

        static Filesystem fromSnapshot(const std::string &_repositoryPath, const Snapshots::Snapshot &_snapshot, const std::vector<std::string> &_snapshotParents);
        static Filesystem load(const std::string &_path);

        void write(const std::string &_snapshotPath) const;
    };
}

#endif //FSTM_FS_H
