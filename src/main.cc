#include <iostream>
#include "include/diff.h"

int main()
{
    auto diff = FSTM::Diff::PartialFileDiff::findFileDiff({
        "hi", "hello", "hehehe"
    }, {
        "hi", "hellow", "new line", "newer line", "hehehe"
    });

    for (const auto& d : diff)
    {
        std::cout << d.targetLine << ": " << d.oldContent << " -> " << d.newContent << std::endl;
    }

    return 0;
}