#ifndef FSTM_FS_H
#define FSTM_FS_H
#include <string>
#include <map>

#include "snapshot.h"
#include "../external/include/rapidyaml.v0.16.0.singlesrc.hpp"

namespace FSTM::Filesystem
{
    std::string getCurrentId(const std::string &_repositoryPath);
    void setCurrent(const std::string &_repositoryPath, const std::string &_id);

    class Filesystem
    {
    public:
        std::map<std::string, std::string> latestFileIds;

        Filesystem();
        explicit Filesystem(std::map<std::string, std::string> _latestFileIds);

        static Filesystem fromSnapshot(const std::string &_repositoryPath, const Snapshots::Snapshot &_snapshot);
        static Filesystem load(const std::string &_path);
        static Filesystem loadYAML(const std::string &_data);
        std::string emitYAML();
        void write(const std::string &_path);
    };
}

#endif //FSTM_FS_H
