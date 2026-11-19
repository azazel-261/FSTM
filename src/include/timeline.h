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

        static void writeCurrent(const std::string &_repositoryPath, const std::string &_timelineName);
        static std::string getCurrentName(const std::string &_repositoryPath);

        static bool checkIfLatest(const std::string &_repositoryPath, const std::string &_snapshotId);
        static std::vector<Timeline> loadTimelines(const std::string &_repositoryPath);
        static Timeline loadByName(const std::string &_repositoryPath, const std::string &_timelineName);
    };
}

#endif //FSTM_TIMELINE_H
