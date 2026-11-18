#ifndef FSTM_TIMELINE_H
#define FSTM_TIMELINE_H
#include <string>
#include <vector>

namespace FSTM::Timelines
{
    class Timeline
    {
    public:
        std::string name;
        std::string latestSnapshotId;

        explicit Timeline(const std::string &_name, const std::string &_latestSnapshotId = "");

        void writeInfoYAML(const std::string &_repositoryPath) const;

        static std::vector<Timeline> loadTimelines(const std::string &_repositoryPath);
    };
}

#endif //FSTM_TIMELINE_H
