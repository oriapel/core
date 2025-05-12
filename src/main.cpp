#include <stdio.h>
#include "MemoryManager/MemoryManager.h"
#include "FileManager/FileManager.h"

static constexpr uint16_t NUMBER_OF_FIELDS{4};

int main(void)
{
    constexpr std::array<FileFields, NUMBER_OF_FIELDS> myFileFields{{
        {0, 4},
        {1, 6},
        {2, 10},
        {3, 20}
    }};

    std::string aaaa {"aaaa"};
    std::string bbbb {"bbbbbb"};
    std::string cccc {"cccccccccc"};
    std::string dddd {"dddddddddddddddddddd"};
    FileManager bla{"batz"};

    MemoryManager<NUMBER_OF_FIELDS> fm{&bla, myFileFields};
    fm.InitalizeAllFields();
    fm.WriteField(0, aaaa, false);
    fm.WriteField(1, bbbb, false);
    fm.WriteField(2, cccc, false);
    fm.WriteField(3, dddd, false);

    std::string aaaa2{};
    fm.ReadField(0, aaaa2);
    printf("aaaa2: %s\n", aaaa2.c_str());
    fm.WriteField(0, bbbb, true);
    fm.ValidateAllFields();
    fm.ReadField(0, aaaa2);
    fm.EraseField(0);

    return 0;
}