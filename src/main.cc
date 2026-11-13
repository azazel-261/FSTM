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

    /*if (argc < 2)
        return -1;

    std::string repoPath = ".";
    trackFile(repoPath, "example_folder/example3.txt");*/

    std::string f1hash = FSTM::Diff::hashFile("./example.txt");
    std::string f2hash = FSTM::Diff::hashFile("./example2.txt");

    std::cout << f1hash << std::endl;
    std::cout << f2hash << std::endl;

    std::cout << (f1hash == f2hash) << std::endl;

    return 0;
}