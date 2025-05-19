#ifndef I_FILE_MANAGER
#define I_FILE_MANAGER

#include "../ErrorCodes.h"
#include <string>

// This class will assume that all of the paramters are valid!
// should only be called from the MemoryManager class after validating the parameters
class IFileManager
{
public:
    /**
     * The function reads from the file into o_buff
     */
    virtual ErrorCode Read(const uint32_t offset, const uint32_t length, std::string& o_buff) = 0;
    /**
     * The function write to the file the content of buff
     */
    virtual ErrorCode Write(const uint32_t offset, const uint32_t length, const std::string& buff) = 0;
    /**
     * The funciton fill the memory of file with '#'
     */
    virtual ErrorCode Erase(const uint32_t offset, const uint32_t length) = 0;
};

#endif // I_FILE_MANAGER