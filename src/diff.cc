#include "include/diff.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <openssl/sha.h>

namespace FSTM::Diff
{
    std::string hashFile(const std::string &_path)
    {
        std::ifstream stream(_path, std::ios::binary);

        if (!stream)
        {
            throw std::runtime_error("Failed to open file for hashing");
        }

        SHA256_CTX context;

        SHA256_Init(&context);

        char buffer[8192];

        while (stream.read(buffer, sizeof(buffer)) || stream.gcount() > 0)
        {
            SHA256_Update(&context, buffer, stream.gcount());
        }

        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256_Final(hash, &context);

        std::ostringstream res;

        for (unsigned char byte : hash)
        {
            res << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
        }

        stream.close();

        return res.str();
    }

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

    bool FileDiff::compare(const std::string &src, const std::string &mod)
    {
        std::ifstream srcStream(src, std::ios::binary | std::ios::ate);
        std::ifstream modStream(mod, std::ios::binary | std::ios::ate);

        if (srcStream.fail() || modStream.fail()) return false;

        if (srcStream.tellg() != modStream.tellg()) return false;

        srcStream.close(); modStream.close();

        return hashFile(src) == hashFile(mod);
    }
}
