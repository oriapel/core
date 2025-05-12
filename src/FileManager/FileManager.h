#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H
#include "IFileManager.h"

#include <iostream>
#include <fstream>

class FileManager : public IFileManager
{
public:
    // C'tor
    FileManager(const std::string& fileName)
    {
        m_file.open(fileName, std::ios::in | std::ios::out);
        if (!m_file.is_open())
        {
            printf("File is not open to writing! something went wrong\n");
        }
    }

    //D'tor
    virtual ~FileManager(){m_file.close();}

    /**
     * The function reads from the file into o_buff, which should be sized at least length +1
     */
    virtual ErrorCode Read(const uint32_t offset, const uint32_t length, std::string& o_buff) override;
    /**
     * The function write to the file the content of buff, which should be sized at least length +1
     */
    virtual ErrorCode Write(const uint32_t offset, const uint32_t length, const std::string& buff) override;
    /**
     * The funciton fill the memory of  in the file with zeros
     */
    virtual ErrorCode Erase(const uint32_t offset, const uint32_t length) override;

private:
    /**
     * The function seeks the file to the wanted location
     */
    ErrorCode SeekFile(const uint32_t offset);

private:
    std::fstream m_file;
};


#endif // FILE_MANAGER_H
