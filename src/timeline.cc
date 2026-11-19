#include "include/timeline.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include "external/include/rapidyaml.v0.16.0.singlesrc.hpp"

namespace FSTM::Timelines
{
    void Timeline::writeCurrent(const std::string &_repositoryPath, const std::string &_timelineName)
    {
        std::ifstream timelineFile(_repositoryPath + "/.fstm/timelines.yml");

        ryml::Tree tree;

        if (timelineFile)
        {
            std::ostringstream out;
            std::string buffer;

            while (std::getline(timelineFile, buffer))
            {
                out << buffer << std::endl;
            }

            tree = ryml::parse_in_arena(ryml::csubstr(out.str().data(), out.str().length()));
            timelineFile.close();
        }
        tree.rootref().set_map();

        if (tree.rootref().has_child("current"))
        {
            tree.rootref()["current"].set_val(ryml::csubstr(_timelineName.data(), _timelineName.length()));
        }
        else
        {
            ryml::NodeRef currentNode = tree.rootref().append_child();
            currentNode.set_key("current");
            currentNode.set_val(ryml::csubstr(_timelineName.data(), _timelineName.length()));
        }

        std::ofstream timeLineFileOut(_repositoryPath + "/.fstm/timelines.yml");

        timeLineFileOut << ryml::emitrs_yaml<std::string>(tree);
        timeLineFileOut.close();
    }

    std::string Timeline::getCurrentName(const std::string &_repositoryPath)
    {
        std::ifstream timelineFile(_repositoryPath + "/.fstm/timelines.yml");

        if (timelineFile)
        {
            std::ostringstream out;
            std::string buffer;

            while (std::getline(timelineFile, buffer))
            {
                out << buffer << std::endl;
            }

            ryml::Tree tree = ryml::parse_in_arena(ryml::csubstr(out.str().data(), out.str().length()));
            timelineFile.close();
            tree.rootref().set_map();
            if (tree.rootref().has_child("current"))
            {
                ryml::NodeRef currentNode = tree.rootref()["current"];
                ryml::csubstr yCurrentStr = currentNode.val();
                return {yCurrentStr.str, yCurrentStr.len};
            }
        }
        return "";
    }

    Timeline::Timeline(const std::string &_name, const std::string &_latestSnapshotId)
    {
        this -> name = _name;
        this -> latestSnapshotId = _latestSnapshotId;
    }

    void Timeline::writeInfoYAML(const std::string &_repositoryPath) const
    {
        std::ifstream timelineFile(_repositoryPath + "/.fstm/timelines.yml");

        ryml::Tree tree;

        ryml::NodeRef timelines;

        if (timelineFile)
        {
            std::ostringstream out;
            std::string buffer;

            while (std::getline(timelineFile, buffer))
            {
                out << buffer << std::endl;
            }

            tree = ryml::parse_in_arena(ryml::csubstr(out.str().data(), out.str().length()));
            timelineFile.close();
        }
        tree.rootref().set_map();
        if (!timelineFile)
        {
            ryml::NodeRef currentNode = tree.rootref().append_child();
            currentNode.set_key("current");
            currentNode.set_val("");
        }
        if (tree.rootref().has_child("timelines"))
        {
            timelines = tree.rootref()["timelines"];
        }
        else
        {
            timelines = tree.rootref().append_child();
            timelines.set_key("timelines");
            timelines.set_map();
        }

        bool override = false;

        for (auto child : timelines.children())
        {
            if (child.key() == this -> name)
            {
                override = true;
                child["latest"].set_val(ryml::csubstr(this -> latestSnapshotId.data(), this -> latestSnapshotId.length()));
            }
        }

        if (!override)
        {
            ryml::NodeRef newEntry = timelines.append_child();
            newEntry.set_key(ryml::csubstr(this -> name.data(), this -> name.length()));
            newEntry.set_map();
            ryml::NodeRef latestNode = newEntry.append_child();
            latestNode.set_key("latest");
            latestNode.set_val(ryml::csubstr(this -> latestSnapshotId.data(), this -> latestSnapshotId.length()));
        }

        std::ofstream timeLineFileOut(_repositoryPath + "/.fstm/timelines.yml");

        timeLineFileOut << ryml::emitrs_yaml<std::string>(tree);
        timeLineFileOut.close();
    }

    Timeline Timeline::loadByName(const std::string &_repositoryPath, const std::string &_timelineName)
    {
        if (std::ifstream timelineFile(_repositoryPath + "/.fstm/timelines.yml"); timelineFile)
        {
            std::ostringstream out;
            std::string buffer;

            while (std::getline(timelineFile, buffer))
            {
                out << buffer << std::endl;
            }

            ryml::Tree tree = ryml::parse_in_arena(ryml::csubstr(out.str().data(), out.str().length()));
            timelineFile.close();
            tree.rootref().set_map();
            if (tree.rootref().has_child("timelines"))
            {
                ryml::NodeRef timelinesNode = tree.rootref()["timelines"];
                for (const auto &c : timelinesNode.children())
                {
                    if (c.key() == _timelineName)
                    {
                        ryml::csubstr yLatestStr = c["latest"].val();
                        return Timeline(_timelineName, {yLatestStr.str, yLatestStr.len});
                    }
                }
            }
        }
        return Timeline("");
    }

    std::vector<Timeline> Timeline::loadTimelines(const std::string &_repositoryPath)
    {
        if (std::ifstream timelineFile(_repositoryPath + "/.fstm/timelines.yml"); timelineFile)
        {
            std::ostringstream stream;
            std::string buffer;

            while (std::getline(timelineFile, buffer))
            {
                stream << buffer << std::endl;
            }

            ryml::Tree tree = ryml::parse_in_arena(ryml::csubstr(stream.str().data(), stream.str().length()));
            timelineFile.close();
            tree.rootref().set_map();
            if (tree.rootref().has_child("timelines"))
            {
                ryml::NodeRef timelinesNode = tree.rootref()["timelines"];
                std::vector<Timeline> out;
                for (const auto &c : timelinesNode.children())
                {
                    ryml::csubstr yNameStr = c.key();
                    ryml::csubstr yLatestStr = c["latest"].val();
                    out.push_back(Timeline({yNameStr.str, yNameStr.len}, {yLatestStr.str, yLatestStr.len}));
                }
            }
        }
        return {};
    }
}
