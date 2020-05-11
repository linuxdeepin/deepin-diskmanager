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
    virtual void read_uuid(Partition &) {}
    virtual void set_used_sectors(Partition &) {}
};
}
#endif // FILESYSTEM_H
