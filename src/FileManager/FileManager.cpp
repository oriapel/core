#include "FileManager.h"

ErrorCode FileManager::SeekFile(const uint32_t offset)
{
    if (!m_file.is_open())
    {
        printf("File is not open to writing! somwthing went wrong\n");
        return ErrorCode::ERR_FILE_PROBLEM;
    }

    m_file.seekp(offset);

    if (!m_file.good())
    {
        printf("Failed to seek file to wanted location!\n");
        return ErrorCode::ERR_FILE_PROBLEM;
    }
    return ErrorCode::ERR_OK;
}

ErrorCode FileManager::Read(const uint32_t offset, const uint32_t length, std::string& o_buff)
{
    ErrorCode res = this->SeekFile(offset);
    if (res != ErrorCode::ERR_OK)
    {
        printf("Failed seek file :(\n");
        return res;
    }

    o_buff[length] = 0;
    m_file.read(&o_buff[0], length);

    if (length != m_file.gcount())
    {
        printf("Could not read all bytes! wanted number is %u, actual is %lu\n", length, m_file.gcount());
        return ErrorCode::ERR_FILE_PROBLEM;
    }

    return ErrorCode::ERR_OK;   
}

ErrorCode FileManager::Write(const uint32_t offset, const uint32_t length, const std::string& buff)
{
    ErrorCode res = this->SeekFile(offset);
    if (res != ErrorCode::ERR_OK)
    {
        printf("Failed seek file :(\n");
        return res;
    }

    m_file.write(buff.c_str(), length);
    if (!m_file.good())
    {
        printf("Could not write to file!\n");
        return ErrorCode::ERR_FILE_PROBLEM;
    }

    return ErrorCode::ERR_OK;
}

ErrorCode FileManager::Erase(const uint32_t offset, const uint32_t length)
{
    std::string emptyBuff(length, '0');
    return this->Write(offset, length, emptyBuff);
}