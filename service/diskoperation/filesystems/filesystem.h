#ifndef FILESYSTEM_H
#define FILESYSTEM_H
#include "commondef.h"
#include "../partition.h"

namespace DiskManager {
class FileSystem
{
public:
    FileSystem();
    virtual ~FileSystem() {}

    virtual FS get_filesystem_support() = 0 ;
    virtual bool is_busy(const QString &) { return false ; }
    virtual void read_label(Partition &) {}
    virtual bool write_label(const Partition &) { return false; }
    virtual void read_uuid(Partition &) {}
    virtual bool write_uuid(const Partition &) { return false; }
    virtual void set_used_sectors(Partition &) {}
    virtual FS_Limits get_filesystem_limits(const Partition &partition) const  { return fs_limits; }
    virtual bool create(const Partition &new_partition) { return false; }


public:
    FS_Limits fs_limits;
private:

};
}
#endif // FILESYSTEM_H
