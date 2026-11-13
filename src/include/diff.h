#ifndef FSTM_DIFF_H
#define FSTM_DIFF_H

#include <string>
#include <vector>

using TextFile = std::vector<std::string>;

namespace FSTM::Diff
{
    enum DiffAction
    {
        NO_ACTION = 0,
        INSERT = 1,
        REMOVE = 2,
        MODIFY = 3
    };

    struct ModifiedSegment
    {
        size_t sourceStart;
        size_t sourceEnd;
        size_t modifiedStart;
        size_t modifiedEnd;
    };

    std::string hashFile(const std::string &_path);

    class Diff
    {
    public:
        DiffAction action;

        Diff(DiffAction _action);
    };

    class FileDiff : public Diff
    {
    public:
        char *oldContent;
        size_t oldContentLength;
        char *newContent;
        size_t newContentLength;

        FileDiff(DiffAction _action,
            char *_oldContent, size_t _oldContentLength,
            char *_newContent, size_t _newContentLength);

        static bool compare(const std::string &src, const std::string &mod);
    };

    class PartialFileDiff : public Diff
    {
    public:
        unsigned int targetLine;
        std::string oldContent;
        std::string newContent;

        PartialFileDiff(DiffAction _action,
            unsigned int _targetLine,
            const std::string& _oldContent,
            const std::string& _newContent);

        static std::vector<PartialFileDiff> findFileDiff(const TextFile& source,
            const TextFile& modified);
    };
}

#endif
