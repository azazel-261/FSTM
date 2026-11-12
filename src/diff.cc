#include "include/diff.h"

#include <iostream>

namespace FSTM::Diff
{
    Diff::Diff(const DiffAction _action)
    {
        this -> action = _action;
    }

    FileDiff::FileDiff(DiffAction _action,
        char *_oldContent, size_t _oldContentLength,
        char *_newContent, size_t _newContentLength)
        : Diff(_action)
    {
        this -> oldContent = _oldContent;
        this -> oldContentLength = _oldContentLength;
        this -> newContent = _newContent;
        this -> newContentLength = _newContentLength;
    }

    PartialFileDiff::PartialFileDiff(DiffAction _action,
        const unsigned int _targetLine,
        const std::string& _oldContent,
        const std::string& _newContent) : Diff(_action)
    {
        this -> targetLine = _targetLine;
        this -> oldContent = _oldContent;
        this -> newContent = _newContent;
    }
/*
 * hello
 * bye
 *
 * hello
 * hellow
 * bye
 */
    ModifiedSegment findAffectedSegment(const TextFile& source, const TextFile& modified, size_t &iSource, size_t &iModified)
    {
        if (source.at(iSource) != modified.at(iModified))
        {
            // Find a point where we can reconnect to source
            const auto modifiedSegmentStart = iModified;
            const auto sourceSegmentStart = iSource;

            // For each next line of the modified file
            for (;iModified < modified.size(); ++iModified)
            {
                iSource = sourceSegmentStart;
                // See if we landed back onto a part of the source
                for (;iSource < source.size(); ++iSource)
                {
                    if (source.at(iSource) == modified.at(iModified))
                    {
                        return {sourceSegmentStart, iSource, modifiedSegmentStart, iModified};
                    }
                }
            }

            // If not, we just say that the modifications affect the rest of the file
            return {sourceSegmentStart, iSource, modifiedSegmentStart, iModified};
        }

        return {iSource, iSource, iModified, iModified};
    }

    std::vector<PartialFileDiff> PartialFileDiff::findFileDiff(const TextFile& source, const TextFile& modified)
    {
        std::vector<PartialFileDiff> out;

        size_t iSource = 0;
        size_t iModified = 0;

        while (true)
        {
            if (source.at(iSource) != modified.at(iModified))
            {
                // Find a point where we can reconnect to source
                ModifiedSegment segment = findAffectedSegment(source, modified, iSource, iModified);

                if (segment.sourceStart != segment.sourceEnd)
                {
                    for (size_t i = segment.sourceStart; i < segment.sourceEnd; ++i)
                    {
                        out.emplace_back(REMOVE, i, source.at(i), "");
                    }
                }
                if (segment.modifiedStart != segment.modifiedEnd)
                {
                    for (size_t i = segment.modifiedStart; i < segment.modifiedEnd; ++i)
                    {
                        out.emplace_back(INSERT, i, "", modified.at(i));
                    }
                }
            }
            ++iSource;
            ++iModified;

            if (iSource >= source.size())
            {
                if (iModified < modified.size())
                {
                    for (; iModified < modified.size(); ++iModified)
                    {
                        out.emplace_back(INSERT, iModified, "", modified.at(iModified));
                    }
                }

                break;
            }
            if (iModified >= modified.size())
            {
                if (iSource < source.size())
                {
                    for (; iSource < source.size(); ++iSource)
                    {
                        out.emplace_back(REMOVE, iSource, source.at(iModified), "");
                    }
                }

                break;
            }
        }

        return out;
    }

    bool FileDiff::compare(const char *_oldContent, const size_t _oldContentLength,
        const char *_newContent, const size_t _newContentLength)
    {
        if (_oldContentLength != _newContentLength) return true;

        return memcmp(_oldContent, _newContent, _oldContentLength);
    }
}
