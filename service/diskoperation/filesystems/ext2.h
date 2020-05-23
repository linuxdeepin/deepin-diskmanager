#ifndef EXT2_H
#define EXT2_H
#include "filesystem.h"

namespace DiskManager {
class EXT2: public FileSystem
{
public:
    EXT2(FSType type);
    FS get_filesystem_support() ;
    void set_used_sectors(Partition &partition)override;
    void read_label(Partition &partition) override;
    bool write_label(const Partition &partition) override;
    void read_uuid(Partition &partition) override;
    virtual bool write_uuid(const Partition &partition)override;
    bool create(const Partition &new_partition) override;
    bool resize(const Partition &partition_new, bool fill_partition)override;
    bool check_repair(const Partition &partition)override ;

    Sector T, N, S ;  //File system [T]otal num of blocks, [N]um of free (or used) blocks, block [S]ize
private:
    Byte_Value fs_block_size;  // Holds file system block size for the copy_progress() callback
    bool force_auto_64bit;
    const FSType specific_type;
    QString mkfs_cmd;
};

}


#endif // EXT2_H
