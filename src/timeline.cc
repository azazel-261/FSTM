#include "include/timeline.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include "external/include/rapidyaml.v0.16.0.singlesrc.hpp"

namespace FSTM::Timelines
{
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
}
