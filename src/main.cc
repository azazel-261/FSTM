#include <iostream>
#include "include/diff.h"
#include "include/snapshot.h"
#include "include/init.h"
#include "include/tracker.h"

using namespace FSTM;

int main(int argc, char *argv[])
{
    /*auto diff = FSTM::Diff::PartialFileDiff::findFileDiff({
        "hi", "hello", "hehehe"
    }, {
        "hi", "hellow", "new line", "newer line", "hehehe"
    });

    for (const auto& d : diff)
    {
        std::cout << d.targetLine << ": " << d.oldContent << " -> " << d.newContent << std::endl;
    }*/

    /*if (argc < 2)
        return -1;*/

    std::string repoPath = ".";
    // trackFile(repoPath, "example_folder/example3.txt");

    const auto trackableFiles = Tracker::getTrackableFiles(repoPath);

    /*for (const auto &p : trackableFiles)
    {
        std::cout << p << std::endl;
        std::cout << Diff::hashFile(p) << std::endl;
    }*/

    std::cout << Snapshots::generateSnapshotHash(trackableFiles) << std::endl;

    Snapshots::Snapshot s("", trackableFiles);

    s.record(repoPath);

    return 0;
}