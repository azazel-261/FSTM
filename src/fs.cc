#include "include/fs.h"

#include <fstream>
#include <iostream>
#include <utility>

#include "include/tracker.h"

namespace FSTM::Filesystem
{
    Filesystem::Filesystem()
    {
        this -> latestFileIds = {};
    }

    Filesystem::Filesystem(std::map<std::string, std::string> _latestFileIds)
    {
        this -> latestFileIds = std::move(_latestFileIds);
    }

    Filesystem Filesystem::fromSnapshot(const std::string &_repositoryPath, const Snapshots::Snapshot &_snapshot, const std::vector<std::string> &_snapshotParents)
    {
        Filesystem out;
        if (!_snapshotParents.empty())
        {
            if (!_snapshotParents[0].empty()) out = load(_repositoryPath + "/.fstm/snapshots/" + _snapshotParents[0]);
        }
        for (const auto &file : _snapshot.filePaths)
        {
            out.latestFileIds[file] = _snapshot.id;
        }
        auto deletedFiles = Diff::findDeletedFiles(_repositoryPath, Tracker::getTrackableFiles(_repositoryPath));
        for (const auto &deleted : deletedFiles)
        {
            out.latestFileIds.erase(deleted);
        }
        return out;
    }

    std::string Filesystem::emitYAML() const
    {
        ryml::Tree tree;
        ryml::NodeRef root = tree.rootref();
        root.set_map();

        ryml::NodeRef files = root.append_child();
        files.save_key("files");
        files.set_seq();

        for (const auto &[file, id] : this -> latestFileIds)
        {
            ryml::NodeRef fileNode = files.append_child();
            fileNode.set_map();
            fileNode["path"].save(ryml::csubstr(file.data(), file.size()));
            fileNode["id"].save(ryml::csubstr(id.data(), id.size()));
        }

        return ryml::emitrs_yaml<std::string>(tree);
    }

    Filesystem Filesystem::loadYAML(const std::string &_data)
    {
        ryml::Tree tree = ryml::parse_in_arena(ryml::csubstr(_data.data(), _data.size()));
        Filesystem out;

        for (ryml::NodeRef files = tree["files"]; auto entry : files.children())
        {
            ryml::csubstr yPath = entry["path"].val();
            std::string path(yPath.str, yPath.len);
            ryml::csubstr yId = entry["id"].val();
            std::string id(yId.str, yId.len);

            out.latestFileIds[path] = id;
        }

        return out;
    }

    Filesystem Filesystem::load(const std::string &_path)
    {
        std::ifstream fsFile(_path + "/.fstm/fs.yml");

        if (!fsFile)
        {
            throw std::runtime_error("FS file not present or failed to open");
        }

        Filesystem out;

        std::ostringstream data;
        std::string buffer;

        while (std::getline(fsFile, buffer))
        {
            if (buffer.empty()) continue;
            data << buffer << std::endl;
        }

        return loadYAML(data.str());
    }

    void Filesystem::write(const std::string &_snapshotPath) const
    {
        std::ofstream fsFile(_snapshotPath + "/.fstm/fs.yml");

        fsFile << this -> emitYAML();

        fsFile.close();
    }
}
