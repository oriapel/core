#ifndef FILE_MANAGER_HPP
#define FILE_MANAGER_HPP
#include "MemoryManager.h"
#include <limits>

static constexpr uint32_t SIZE_OF_FLAG{1};

template <uint32_t N>
ErrorCode MemoryManager<N>::InitalizeAllFields() const
{
    ErrorCode res = ErrorCode::ERR_OK;
    // Initialize all fields to default values (#)
    for (uint32_t i = 0; i < this->m_allFileFields.size(); ++i)
    {
        std::string emptyField((this->m_allFileFields[i].fieldSize), '#');
        std::string emptyHeader{"##"};
        const uint32_t fieldOffset = this->GetFieldOffsetInFile(i);
        emptyHeader[FIELD_IS_DATA_VALID_INDEX] = this->CalculateFieldChecksum(emptyField);
        emptyHeader[FIELD_WRITING_FLAG_INDEX] = WRITING_DONE;
        
        res = this->m_fileManager->Write(fieldOffset, INTERNAL_HEADER_SIZE, emptyHeader);
        if (res != ErrorCode::ERR_OK)
        {
            printf("Failed writing field %u to file!\n", i);
            return res;
        }
        
        res = this->m_fileManager->Write(fieldOffset + INTERNAL_HEADER_SIZE, (this->m_allFileFields[i].fieldSize), emptyField);
        if (res != ErrorCode::ERR_OK)
        {
            printf("Failed writing field %u to file!\n", i);
            return res;
        }
    }
    return ErrorCode::ERR_OK;
}

template <uint32_t N>
void MemoryManager<N>::ValidateAllFields()
{
    uint32_t invalidFieldsCounter= 0;
    for (uint32_t i = 0; i < this->m_allFileFields.size(); ++i)
    {
        // Get field with headers to validate field
        const uint32_t fieldOffset = this->GetFieldOffsetInFile(i);
        std::string fieldHeaders{"##"};
        
        // Read field to validate data
        ErrorCode res = ErrorCode::ERR_OK;
        res = this->m_fileManager->Read(fieldOffset, INTERNAL_HEADER_SIZE, fieldHeaders);
        if (res != ErrorCode::ERR_OK)
        {
            printf("Failed reading field %u headers!\n", i);
        }

        // Only check writing flag, can also add if we want validation on the checksum
        if (fieldHeaders[FIELD_WRITING_FLAG_INDEX] != WRITING_DONE)
        {
            printf("Found corrupted field %u!\n", i);
            ++invalidFieldsCounter;
            if (invalidFieldsCounter == 1)
            {
                this->RestoreField(i);
                // Can also assume that there can only be one curropted file and break here
            }
            else
            {
                printf("OH NO!\ntoo many corrupted fields, all hope is lost\nnum of corrupted files: %u\n", invalidFieldsCounter);
            }
        }
    
    }
}

template <uint32_t N>
ErrorCode MemoryManager<N>::RestoreField(const uint32_t index)
{
    std::string currData(this->m_biggestRecordSize, '\0');

    // Read backed up field from designated location
    ErrorCode res = ErrorCode::ERR_OK;
    res = this->ReadField(this->m_allFileFields.size(), currData);
    if (res != ErrorCode::ERR_OK)
    {
        printf("Could not read field %u from backup!\n", index);
        return res;
    }
    currData[GetFieldLength(index)] = 0;

    // Write without backup, as it is already backed up
    res = this->WriteField(index, currData, false);
    if (res != ErrorCode::ERR_OK)
    {
        printf("Could not write backup for field %u!\n", index);
        return res;
    }

    return ErrorCode::ERR_OK;
}


template <uint32_t N>
ErrorCode MemoryManager<N>::BackUpField(const uint32_t index)
{
    std::string currData(GetFieldLength(index), '\0');

    ErrorCode res = ErrorCode::ERR_OK;
    res = this->ReadField(index, currData);
    if (res != ErrorCode::ERR_OK)
    {
        printf("Could not read field %u to backup!\n", index);
        return res;
    }

    res = this->WriteField(this->m_allFileFields.size(), currData, false);
    if (res != ErrorCode::ERR_OK)
    {
        printf("Could not write backup for field %u!\n", index);
        return res;
    }

    return ErrorCode::ERR_OK;
}


template <uint32_t N>
inline uint32_t MemoryManager<N>::GetFieldLength(const uint32_t index) const
{
    if (index != this->m_allFileFields.size())
    {
        return this->m_allFileFields[index].fieldSize;
    }
    return this->m_biggestRecordSize;
}


template <uint32_t N>
uint8_t MemoryManager<N>::CalculateFieldChecksum(const std::string& field) const
{
    uint8_t checksum = 0;
    for (const char& tmp : field)
    {
        checksum += static_cast<uint8_t>(tmp);
    }
    return checksum % static_cast<uint8_t>(std::numeric_limits<uint8_t>::max());
}

template <uint32_t N>
ErrorCode MemoryManager<N>::EraseField(const uint32_t index)
{
    ErrorCode res = this->WriteField(index, std::string(this->m_allFileFields[index].fieldSize, '#'), false);

    if (res != ErrorCode::ERR_OK)
    {
        printf("Could not erase field %u!\n", index);
        return res;
    }

    return ErrorCode::ERR_OK;
}


template <uint32_t N>
ErrorCode MemoryManager<N>::ReadField(const uint32_t index, std::string& o_buff)
{
    // Get field with headers to validate field
    const uint32_t fieldOffset = this->GetFieldOffsetInFile(index);
    const uint32_t fieldSize = this->GetFieldLength(index);
    
    // Read field
    std::string field{};
    ErrorCode res = this->m_fileManager->Read(fieldOffset, fieldSize + INTERNAL_HEADER_SIZE, field);
    
    if (res != ErrorCode::ERR_OK)
    {
        printf("Failed reading field %u\n", index);
        return res;
    }

    // Check if writing flag is valid
    if (field[FIELD_WRITING_FLAG_INDEX] != WRITING_DONE)
    {
        printf("Field flags are invalid! writing did not finished\n");
        return ErrorCode::ERR_DATA_NOT_WRITTEN;
    }
    // fill o_buff with field data
    o_buff.resize(fieldSize + 1);
    for (size_t i = 0; i < fieldSize; i++)
    {
        o_buff[i] = field[i + INTERNAL_HEADER_SIZE];
    }
    o_buff[fieldSize] = 0;

    // Check if checksum is valid
    if (static_cast<uint8_t>(field[FIELD_IS_DATA_VALID_INDEX]) != this->CalculateFieldChecksum(o_buff))
    {
        printf("Checksum is invalid!\n");
        return ErrorCode::ERR_DATA_CORRUPTED;
    }

    return ErrorCode::ERR_OK;
}

template <uint32_t N>
ErrorCode MemoryManager<N>::WriteField(const uint32_t index, const std::string& buff, bool backup)
{
    const uint32_t fieldSize = this->GetFieldLength(index);
    
    // validate field size
    if (buff.size() != fieldSize && (index != this->m_allFileFields.size()))
    {
        printf("Field %u does not match to field length! size is %lu, max size is %u\n", index, buff.size(), fieldSize);
        return ErrorCode::ERR_GENERAL_ERROR;
    }
    
    // Write writing flag
    const uint32_t fieldOffset = this->GetFieldOffsetInFile(index);
    if (backup)
    {
        // Backup before writing
        this->BackUpField(index);
    }
    std::string writingNotDone{WRITING_NOT_DONE};

    // Indicate that we did not finish writing field
    ErrorCode res = this->m_fileManager->Write(fieldOffset + FIELD_WRITING_FLAG_INDEX, SIZE_OF_FLAG, writingNotDone);
    if (res != ErrorCode::ERR_OK)
    {
        printf("Failed writing field %u writing flag to file!\n", index);
        return res;
    }

    // Write checksum
    std::string checksumStr(SIZE_OF_FLAG, this->CalculateFieldChecksum(buff));
    res = this->m_fileManager->Write(fieldOffset + FIELD_IS_DATA_VALID_INDEX, SIZE_OF_FLAG, checksumStr);
    if (res != ErrorCode::ERR_OK)
    {
        printf("Failed writing field %u checksum to file!\n", index);
        return res;
    }

    // Write field data
    res = this->m_fileManager->Write(fieldOffset + INTERNAL_HEADER_SIZE, fieldSize, buff);
    if (res != ErrorCode::ERR_OK)
    {
        printf("Failed writing field %u!\n", index);
        return res;
    }

    // Indicate that we finished writing field
    std::string writingDone{WRITING_DONE};
    res = this->m_fileManager->Write(fieldOffset + FIELD_WRITING_FLAG_INDEX, SIZE_OF_FLAG, writingDone);
    if (res != ErrorCode::ERR_OK)
    {
        printf("Failed writing field %u writing flag to file!\n", index);
        return res;
    }

    return ErrorCode::ERR_OK;
}


template <uint32_t N>
inline void MemoryManager<N>::UpdateBiggestRecordSize()
{
    uint32_t curr_max_val{0};
    for (const auto& tmp : m_allFileFields)
    {
        if (curr_max_val < tmp.fieldSize)
        {
            curr_max_val = tmp.fieldSize;
        }
    }
    m_biggestRecordSize = curr_max_val;
}

template <uint32_t N>
bool MemoryManager<N>::ValidateTotalSizeOfFields()
{
    uint32_t totalSize{0};
    for (const auto& tmp : m_allFileFields)
    {
        totalSize += tmp.fieldSize + INTERNAL_HEADER_SIZE;
    }

    // if total size is valid, we also update the place in which we'll back up fields
    if ((totalSize + m_biggestRecordSize) <= MAX_FILE_CAPACITY)
    {
        m_backUpRecordOffset = totalSize;
        return true;
    }

    return true;
}

template <uint32_t N>
uint32_t MemoryManager<N>::GetFieldOffsetInFile(const uint32_t index) const
{
    uint32_t offset{0};
    for (uint32_t i = 0; i < index; ++i)
    {
        offset += (m_allFileFields[i].fieldSize + INTERNAL_HEADER_SIZE);
    }
    return offset;
}

#endif // FILE_MANAGER_HPP