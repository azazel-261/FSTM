#include <utility>
#include <openssl/sha.h>
#include "include/snapshot.h"

#include <iostream>

namespace FSTM::Snapshots
{
    Snapshot::Snapshot(const std::string &_id, const std::string &_parentId, std::vector<std::string> _filePaths)
    {
        this -> id = _id;
        this -> parentId = _parentId;

        this -> filePaths = std::move(_filePaths);
    }

    Snapshot::Snapshot(const std::string &_parentId, std::vector<std::string> _filePaths)
    {
        this -> parentId = _parentId;

        this -> filePaths = std::move(_filePaths);

        this -> id = generateSnapshotHash(this -> filePaths);
    }

    std::string generateSnapshotHash(const std::vector<std::string> &_filePaths)
    {
        SHA256_CTX context;

        SHA256_Init(&context);

        for (const auto &file : _filePaths)
        {
            std::string hash = Diff::hashFile(file);
            SHA256_Update(&context, hash.c_str(), hash.length());
        }

        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256_Final(hash, &context);

        std::ostringstream res;

        for (unsigned char byte : hash)
        {
            res << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
        }

        return res.str();
    }

    void Snapshot::record(const std::string &_repositoryPath) const
    {
        std::filesystem::create_directories(_repositoryPath + "/.fstm/snapshots/" + this -> id + "/.fstm");

        for (const auto &file : this -> filePaths)
        {
            std::filesystem::path path = file;
            std::filesystem::path from = std::filesystem::absolute(std::filesystem::path(file));
            std::filesystem::path to = absolute(std::filesystem::path(_repositoryPath + "/.fstm/snapshots/" + this -> id + path.relative_path().string()));
            if (!std::filesystem::exists(path))
            {
                continue;
            };
            if (std::filesystem::is_directory(path))
            {
                continue;
            }
            if (!std::filesystem::exists(to.parent_path()))
            {
                std::cout << std::filesystem::create_directories(to.parent_path()) << std::endl;
            }
            try
            {
                std::filesystem::copy(from, to);
            } catch (const std::filesystem::filesystem_error &e)
            {
                std::cout << e.what() << std::endl;
            }
        }
    }
}
