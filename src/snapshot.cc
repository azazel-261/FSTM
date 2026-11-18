#include <utility>
#include <openssl/sha.h>
#include <fstream>
#include <iostream>

#include "include/diff.h"
#include "include/snapshot.h"
#include "external/include/rapidyaml.v0.16.0.singlesrc.hpp"
#include "include/fs.h"
#include "include/tracker.h"

namespace FSTM::Snapshots
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

    void setCurrentId(const std::string &_repositoryPath, const std::string &_id)
    {
        std::ofstream currentFile(_repositoryPath + "/.fstm/current");

        currentFile << _id;

        currentFile.close();
    }

    Snapshot::Snapshot(const std::string &_id, std::vector<std::string> _filePaths)
    {
        this -> id = _id;
        this -> filePaths = std::move(_filePaths);
    }

    std::string Snapshot::generateSnapshotHash(const std::string &_repositoryPath, const std::vector<std::string> &_filePaths)
    {
        SHA256_CTX context;

        SHA256_Init(&context);

        for (const auto &file : _filePaths)
        {
            std::string hash = Diff::hashFile(_repositoryPath + "/" + file);
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

    void Snapshot::write(const std::string &_repositoryPath) const
    {
        std::filesystem::create_directories(_repositoryPath + "/.fstm/snapshots/" + this -> id + "/.fstm");
        for (const auto &file : this -> filePaths)
        {
            std::filesystem::path path = _repositoryPath + "/" + file;
            std::filesystem::path from = std::filesystem::absolute(std::filesystem::path(_repositoryPath + "/" + file));
            std::filesystem::path to = absolute(std::filesystem::path(_repositoryPath + "/.fstm/snapshots/" + this -> id + "/" + std::filesystem::relative(path, _repositoryPath).string()));
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

    void Snapshot::writeMessage(const std::string &_snapshotPath, const std::string &_message)
    {
        std::ofstream messageFile(_snapshotPath + "/.fstm/message.txt");
        messageFile << _message;
        messageFile.close();
    }

    std::string Snapshot::readMessage(const std::string &_snapshotPath)
    {
        std::ifstream messageFile(_snapshotPath + "/.fstm/message.txt");
        if (!messageFile) return {};

        std::ostringstream out;
        std::string buffer;

        while (std::getline(messageFile, buffer))
        {
            out << buffer << std::endl;
        }

        return out.str();
    }

    SnapshotData::SnapshotData()
    {
        this -> parentIds = {};
    }

    SnapshotData::SnapshotData(std::string _timeline, std::vector<std::string> _parentIds)
    {
        this -> parentIds = std::move(_parentIds);
        this -> timeline = std::move(_timeline);
    }

    std::string SnapshotData::emitYAML() const
    {
        ryml::Tree tree;
        ryml::NodeRef root = tree.rootref();
        root.set_map();

        ryml::NodeRef parentIdsNode = root.append_child();
        parentIdsNode.set_seq();
        parentIdsNode.set_key("parent_ids");

        for (const auto &parent : parentIds)
        {
            ryml::NodeRef entry = parentIdsNode.append_child();
            entry.set_val(ryml::csubstr(parent.data(), parent.length()));
        }

        ryml::NodeRef timelineNode = root.append_child();
        timelineNode.set_key("timeline");
        timelineNode.set_val(ryml::csubstr(this -> timeline.data(), this -> timeline.length()));

        return ryml::emitrs_yaml<std::string>(tree);
    }

    SnapshotData SnapshotData::loadYAML(const std::string &_data)
    {
        SnapshotData out;
        ryml::Tree tree = ryml::parse_in_arena(ryml::csubstr(_data.data(), _data.size()));

        for (ryml::NodeRef parentIdsNode = tree["parent_ids"]; const auto &c : parentIdsNode.children())
        {
            const ryml::csubstr yParentStr = c.val();
            std::string parent(yParentStr.str, yParentStr.len);
            out.parentIds.push_back(parent);
        }

        const ryml::NodeRef timelineNode = tree["timeline"];
        const ryml::csubstr yTimelineStr = timelineNode.val();
        out.timeline = std::string(yTimelineStr.str, yTimelineStr.len);

        return out;
    }

    void SnapshotData::write(const std::string &_snapshotPath) const
    {
        std::ofstream snapshotDataFile(_snapshotPath + "/.fstm/snapshot.yml");
        snapshotDataFile << this -> emitYAML();
        snapshotDataFile.close();
    }

    SnapshotData SnapshotData::load(const std::string &_snapshotPath)
    {
        std::ifstream fsFile(_snapshotPath + "/.fstm/fs.yml");

        if (!fsFile)
        {
            throw std::runtime_error("Snapshot data file not present or failed to open");
        }

        SnapshotData out;

        std::ostringstream data;
        std::string buffer;

        while (std::getline(fsFile, buffer))
        {
            if (buffer.empty()) continue;
            data << buffer << std::endl;
        }

        return loadYAML(data.str());
    }

    void Snapshot::snapshotUnsaved(const std::string &_repoPath)
    {
        std::string tempId = getCurrentId(_repoPath) + "_tmp";
        const auto files = Tracker::getTrackableFiles(_repoPath);
        const auto changedFiles = Diff::findChangedFiles(_repoPath, files);
        if (changedFiles.empty()) return;
        Snapshot s(tempId, changedFiles);
        Filesystem::Filesystem fs = Filesystem::Filesystem::fromSnapshot(_repoPath, s, {getCurrentId(_repoPath)});
        SnapshotData data("main", {getCurrentId(_repoPath)});
        s.write(_repoPath);
        fs.write(_repoPath + "/.fstm/snapshots/" + tempId);
        data.write(_repoPath + "/.fstm/snapshots/" + tempId);
        setCurrentId(_repoPath, tempId);
    }

    void Snapshot::travelToSnapshot(const std::string &_repoPath, const std::string &_snapshotId)
    {
        if (getCurrentId(_repoPath) == _snapshotId) return;

        Filesystem::Filesystem fs = Filesystem::Filesystem::load(_repoPath + "/.fstm/snapshots/" + _snapshotId);

        // Scary, delete all
        for (const auto &file: Tracker::getTrackableFiles(_repoPath))
        {
            std::filesystem::path filepath = file;
            std::filesystem::remove(filepath);
            if (std::filesystem::is_empty(std::filesystem::absolute(filepath).parent_path()))
            {
                if (!std::filesystem::equivalent(std::filesystem::absolute(filepath).parent_path(), _repoPath)
                    && Tracker::checkIfIgnored(_repoPath, std::filesystem::relative(filepath, _repoPath).parent_path().string()))
                {
                    std::filesystem::remove(filepath.parent_path());
                }
            }
        }

        for (const auto &[file, id] : fs.latestFileIds)
        {
            std::filesystem::path from = _repoPath;
            from /= ".fstm/snapshots"; from /= id; from /= file;
            std::filesystem::path to = _repoPath;
            to /= file;
            std::filesystem::create_directories(to.parent_path());
            std::filesystem::copy(from, to);
        }

        setCurrentId(_repoPath, _snapshotId);
    }
}
