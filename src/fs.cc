//
// Created by georg on 14.11.2026.
//

#include "include/fs.h"

#include <fstream>
#include <utility>

namespace FSTM::Filesystem
{
    std::string getCurrentId(const std::string &_repositoryPath)
    {
        std::ifstream currentFile(_repositoryPath + "/.fstm/current");

        if (!currentFile) return "";

        if (std::string buffer; std::getline(currentFile, buffer))
        {
            return buffer;
        }
        return "";
    }

    void setCurrent(const std::string &_repositoryPath, const std::string &_id)
    {
        std::ofstream currentFile(_repositoryPath + "/.fstm/current");

        currentFile << _id;

        currentFile.close();
    }

    Filesystem::Filesystem()
    {
        this -> latestFileIds = {};
    }

    Filesystem::Filesystem(std::map<std::string, std::string> _latestFileIds)
    {
        this -> latestFileIds = std::move(_latestFileIds);
    }

    Filesystem Filesystem::fromSnapshot(const std::string &_repositoryPath, const Snapshots::Snapshot &_snapshot)
    {
        Filesystem out;
        if (!_snapshot.parentId.empty())
        {
            out = load(_repositoryPath + "/.fstm/snapshots/" + _snapshot.parentId);
        }
        for (const auto &file : _snapshot.filePaths)
        {
            out.latestFileIds[file] = _snapshot.id;
        }
        return out;
    }

    Filesystem Filesystem::load(const std::string &_path)
    {
        std::ifstream fsFile(_path + "/.fstm/fs");

        if (!fsFile)
        {
            throw std::runtime_error("FS file not present or failed to open");
        }

        Filesystem out;

        std::string id;
        std::string buffer;

        while (std::getline(fsFile, buffer))
        {
            if (buffer.empty()) continue;
            if (id.empty()) id = buffer;
            else
            {
                out.latestFileIds[id] = buffer;
                id.clear();
            }
        }

        return out;
    }

    void Filesystem::write(const std::string &_path)
    {
        std::ofstream fsFile(_path + "/.fstm/fs");

        for (const auto &[file, id] : this -> latestFileIds)
        {
            fsFile << file << '\n';
            fsFile << id << std::endl;
        }

        fsFile.close();
    }
}
