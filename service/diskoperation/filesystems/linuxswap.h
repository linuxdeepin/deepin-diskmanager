#ifndef LINUXSWAP_H
#define LINUXSWAP_H
#include "filesystem.h"

namespace DiskManager {
class LinuxSwap : public FileSystem
{
public:
    FS get_filesystem_support();
    void set_used_sectors(Partition &partition);
    void read_label(Partition &partition);
    bool write_label(const Partition &partition);
    void read_uuid(Partition &partition);
    bool write_uuid(const Partition &partition);
    bool create(const Partition &new_partition);
    bool resize(const Partition &partition_new, bool fill_partition);
};

} // namespace DiskManager
#endif // LINUXSWAP_H
