#include <iostream>
#include "include/diff.h"
#include "include/snapshot.h"
#include "include/init.h"
#include "include/tracker.h"

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

    if (argc < 2)
        return -1;

    std::string repoPath = argv[1];
    trackFile(repoPath, "example.txt");

    return 0;
}