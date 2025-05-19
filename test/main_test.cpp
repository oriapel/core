#include <gtest/gtest.h>

#include <stdio.h>
#include "../src/MemoryManager/MemoryManager.h"
#include "../src/FileManager/FileManager.h"

static constexpr uint16_t NUMBER_OF_FIELDS{4};
constexpr std::array<FileFields, NUMBER_OF_FIELDS> myFileFields{{
        {0, 4},
        {1, 6},
        {2, 10},
        {3, 20}
    }};

    FileManager fm{"batz"};
    MemoryManager<NUMBER_OF_FIELDS> memoryManager{&fm, myFileFields};

TEST(SimpleTest, WriteGoodField) {
    std::string aaaa {"aaaa"};
    ASSERT_TRUE(memoryManager.InitalizeAllFields() == ErrorCode::ERR_OK);
    ASSERT_TRUE(memoryManager.WriteField(0, aaaa) == ErrorCode::ERR_OK);
}

TEST(SimpleTest, WriteWrongSizeField) {
    std::string aaaa {"aaaa"};
    ASSERT_TRUE(memoryManager.WriteField(1, aaaa) != ErrorCode::ERR_OK);
}

TEST(SimpleTest, WriteRead) {
    std::string aaaa {"aaaa"};
    std::string aaaa2{};
    ASSERT_TRUE(memoryManager.WriteField(0, aaaa) == ErrorCode::ERR_OK);
    ASSERT_TRUE(memoryManager.ReadField(0, aaaa2) == ErrorCode::ERR_OK);
    for (size_t i = 0; i < aaaa.size(); i++)
    {
        ASSERT_EQ(aaaa[i], aaaa2[i]);
    }  
}

TEST(SimpleTest, WriteTwiceRead) {
    std::string aaaa {"aaaa"};
    std::string bbbb {"bbbb"};
    std::string aaaa2{};
    ASSERT_TRUE(memoryManager.WriteField(0, aaaa) == ErrorCode::ERR_OK);
    ASSERT_TRUE(memoryManager.WriteField(0, bbbb) == ErrorCode::ERR_OK);
    ASSERT_TRUE(memoryManager.ReadField(0, aaaa2) == ErrorCode::ERR_OK);
    for (size_t i = 0; i < aaaa.size(); i++)
    {
        ASSERT_NE(aaaa[i], aaaa2[i]);
    }  
}

TEST(SimpleTest, WriteAllFields) {
    std::string aaaa {"aaaa"};
    std::string bbbb {"bbbbbb"};
    std::string cccc {"cccccccccc"};
    std::string dddd {"dddddddddddddddddddd"};
    ASSERT_TRUE(memoryManager.InitalizeAllFields() == ErrorCode::ERR_OK);
    ASSERT_TRUE(memoryManager.WriteField(0, aaaa) == ErrorCode::ERR_OK);
    ASSERT_TRUE(memoryManager.WriteField(1, bbbb) == ErrorCode::ERR_OK);
    ASSERT_TRUE(memoryManager.WriteField(2, cccc) == ErrorCode::ERR_OK);
    ASSERT_TRUE(memoryManager.WriteField(3, dddd) == ErrorCode::ERR_OK);
}


TEST(SimpleTest, EraseField) {
    std::string aaaa {"aaaa"};
    std::string aaaa2{};
    std::string emptyStr {"####"};
    ASSERT_TRUE(memoryManager.WriteField(0, aaaa) == ErrorCode::ERR_OK);
    ASSERT_TRUE(memoryManager.EraseField(0) == ErrorCode::ERR_OK);
    ASSERT_TRUE(memoryManager.ReadField(0, aaaa2) == ErrorCode::ERR_OK);

    for (size_t i = 0; i < aaaa2.size(); i++)
    {
        ASSERT_EQ(emptyStr[i], aaaa2[i]);
    }
}

TEST(SimpleTest, ReadEmptyField) {
    memoryManager.InitalizeAllFields();
    std::string aaaa2{};
    std::string emptyStr {"####"};
    ASSERT_TRUE(memoryManager.ReadField(0, aaaa2) == ErrorCode::ERR_OK);

    for (size_t i = 0; i < aaaa2.size(); i++)
    {
        ASSERT_EQ(emptyStr[i], aaaa2[i]);
    }
}

TEST(simpleTest, WriteFieldThenInitialize) {
    std::string aaaa {"aaaa"};
    std::string aaaa2{};
    std::string emptyStr {"####"};
    ASSERT_TRUE(memoryManager.WriteField(0, aaaa) == ErrorCode::ERR_OK);
    ASSERT_TRUE(memoryManager.InitalizeAllFields() == ErrorCode::ERR_OK);
    ASSERT_TRUE(memoryManager.ReadField(0, aaaa2) == ErrorCode::ERR_OK);

    for (size_t i = 0; i < aaaa2.size(); i++)
    {
        ASSERT_EQ(emptyStr[i], aaaa2[i]);
    }
}
TEST(simpleTest, WriteFieldThenInitializeThenWrite) {
    std::string aaaa {"aaaaaa"};
    std::string bbbb {"bbbbbb"};
    std::string aaaa2{};
    std::string emptyStr {"######"};
    ASSERT_TRUE(memoryManager.WriteField(1, aaaa) == ErrorCode::ERR_OK);
    ASSERT_TRUE(memoryManager.InitalizeAllFields() == ErrorCode::ERR_OK);
    ASSERT_TRUE(memoryManager.WriteField(1, bbbb) == ErrorCode::ERR_OK);
    ASSERT_TRUE(memoryManager.ReadField(1, aaaa2) == ErrorCode::ERR_OK);

    for (size_t i = 0; i < aaaa2.size(); i++)
    {
        ASSERT_EQ(bbbb[i], aaaa2[i]);
    }
}

TEST(simpleTest, TestCorruptedField) {
    std::string aaaa {"aaaa"};
    std::string bbbb {"bbbb"};
    std::string aaaa2{};
    std::string emptyStr {"######"};
    ASSERT_TRUE(memoryManager.WriteField(0, aaaa) == ErrorCode::ERR_OK);
    for (size_t i = 0; i < aaaa2.size(); i++)
    {
        ASSERT_EQ(aaaa[i], aaaa2[i]);
    }
    ASSERT_TRUE(memoryManager.WriteField(0, bbbb, true, true) == ErrorCode::ERR_OK);
    // Validate all fields also restores the corrupted field
    memoryManager.ValidateAllFields();
    ASSERT_TRUE(memoryManager.ReadField(0, aaaa) == ErrorCode::ERR_OK);;

    for (size_t i = 0; i < aaaa2.size(); i++)
    {
        ASSERT_EQ(aaaa[i], aaaa2[i]);
    }
}

TEST(simpleTest, TestTwoCorruptedField) {
    std::string aaaa {"aaaa"};
    std::string bbbb {"bbbb"};
    std::string cccc {"cccccccccc"};
    std::string aaaa2{};
    std::string emptyStr {"######"};
    ASSERT_TRUE(memoryManager.WriteField(0, aaaa) == ErrorCode::ERR_OK);
    for (size_t i = 0; i < aaaa2.size(); i++)
    {
        ASSERT_EQ(aaaa[i], aaaa2[i]);
    }
    ASSERT_TRUE(memoryManager.WriteField(0, bbbb, true, true) == ErrorCode::ERR_OK);

    ASSERT_TRUE(memoryManager.WriteField(2, cccc, true, true) == ErrorCode::ERR_OK);
    // Validate all fields also restores the corrupted field
    memoryManager.ValidateAllFields();
    ASSERT_TRUE(memoryManager.ReadField(0, aaaa) == ErrorCode::ERR_OK);
    ASSERT_TRUE(memoryManager.ReadField(2, aaaa2) == ErrorCode::ERR_DATA_NOT_WRITTEN);

    for (size_t i = 0; i < aaaa2.size(); i++)
    {
        ASSERT_EQ(aaaa[i], aaaa2[i]);
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}