#ifndef I_MEMORY_MANAGER_H
#define I_MEMORY_MANAGER_H

#include "../ErrorCodes.h" // for ErrorCode
#include "../FileFields.h"
#include "../FileManager/IFileManager.h"
#include <string> // for String

#include <array>

// can add validation, not really important for the exercise
static constexpr uint32_t MAX_FILE_PATH_LEN{20};
static constexpr uint32_t MAX_FILE_CAPACITY{100}; // can be 4MB for exercise, does not really matter
static constexpr uint32_t INTERNAL_HEADER_SIZE{2};
static constexpr uint32_t FIELD_WRITING_FLAG_INDEX{0};
static constexpr uint32_t FIELD_IS_DATA_VALID_INDEX{1};
static constexpr char WRITING_DONE{'y'};
static constexpr char WRITING_NOT_DONE{'n'};

template <uint32_t N>
class MemoryManager
{
public:
    // C'tor
    MemoryManager(IFileManager *fileManager, std::array<FileFields, N> allFileFields) : m_fileManager(fileManager), m_allFileFields(allFileFields)
    {
        // Save biggest record size
        UpdateBiggestRecordSize();

        // Check if we have enough space for the fields
        if (!ValidateTotalSizeOfFields())
        {
            // need to decide if failure here is exeption, need KAPAT for documentation
            printf("Fuck\n");
        }

        // Check if there is a corrupted fields, and restore it
        ValidateAllFields();
    }

    /**
     * The function reads from the file into o_buff
     */
    virtual ErrorCode ReadField(const uint32_t index, std::string &o_buff, bool validateChecksum = true);
    /**
     * The function write to the file the content of buff.
     * DO NOT CHECK DATA INTEGRITY! I just added it so later I can add it fully, not interesting for now
     */
    virtual ErrorCode WriteField(const uint32_t index, const std::string &buff, bool backup = true, bool doNotFinishWritingFlag = false);
    /**
     * The funciton fill the memory of field in the file with zeros
     */
    virtual ErrorCode EraseField(const uint32_t index);

    /**
     * Initialize all fields to default values (#)
     */
    ErrorCode InitalizeAllFields() const;

    /**
     * This function iterate all fields, and check if one is corrupted. if there is a corrupted field, it restores it
     * from the designated back up storage
     */
    void ValidateAllFields();

    
private:
    /**
     * This function calculate the checksum of field.
     */
    uint8_t CalculateFieldChecksum(const std::string &field) const;

    /**
     * This function gets an index of field, and return the field's size
     */
    inline uint32_t GetFieldLength(const uint32_t index) const;

    /**
     * This function back up a filed before writing to it, to not lose the data in case of failure
     */
    ErrorCode BackUpField(const uint32_t index);

    /**
     * This function restore corrupted field, if writing to it failed for some reason
     */
    ErrorCode RestoreField(const uint32_t index);

    /**
     * In order to backup records when writing them, we need to save space for one record each time,
     * so we need to know how much space to keep. for this, we keep a member that tells us that size, and here we're updating it
     */
    void UpdateBiggestRecordSize();

    /**
     * This function make sure that we have enough available memory for the required fields
     */
    bool ValidateTotalSizeOfFields();

    /**
     * This function gets an index of field, and return the field's offset in the file
     */
    uint32_t GetFieldOffsetInFile(const uint32_t index) const;

private:
    uint32_t m_backUpRecordOffset = 0;
    uint32_t m_biggestRecordSize = 0;
    std::array<FileFields, N> m_allFileFields;
    IFileManager *m_fileManager;
};

#include "MemoryManager.hpp"
#endif // I_MEMORY_MANAGER_H