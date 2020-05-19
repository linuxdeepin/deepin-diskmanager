#ifndef EXT2_H
#define EXT2_H
#include "filesystem.h"

namespace DiskManager {
class EXT2: public FileSystem
{
public:
    EXT2(FSType type);
    FS get_filesystem_support() ;
    bool create(const Partition &new_partition) override;


private:
    Byte_Value fs_block_size;  // Holds file system block size for the copy_progress() callback
    bool force_auto_64bit;
    const FSType specific_type;
    QString mkfs_cmd;
};

}


#endif // EXT2_H
