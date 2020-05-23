#ifndef PARTITION_H
#define PARTITION_H
#include "commondef.h"
#include "partitioninfo.h"

namespace DiskManager {

class Partition
{
public:
    Partition();
    virtual ~Partition() {}
    virtual Partition *clone() const;
    virtual bool sector_usage_known() const;
    virtual Sector get_sectors_unallocated() const;
    void Reset() ;
    void Reset(const PartitionInfo &info);
    //simple Set-functions.  only for convenience, since most members are public
    void Set(const QString &device_path, const QString &partition, int partition_number, PartitionType type, FSType fstype, Sector sector_start, Sector sector_end, Byte_Value sector_size, bool inside_extended, bool busy);
    void set_unpartitioned(const QString &device_path, const QString &partition_path, FSType fstype, Sector length, Byte_Value sector_size, bool busy);
    void Set_Unallocated(const QString &device_path, Sector sector_start, Sector sector_end, Byte_Value sector_size, bool inside_extended);
    bool filesystem_label_known() const;
    void set_filesystem_label(const QString &filesystem_label);
    void add_mountpoint(const QString &mountpoint);
    void add_mountpoints(const QVector<QString> &mountpoints);
    QVector<QString> get_mountpoints() const ;
    QString get_mountpoint() const ;
    void set_sector_usage(Sector sectors_fs_size, Sector sectors_fs_unused) ;
    inline QString get_path() const {return this->path;}
    inline void set_path(const QString &path) {this->path = path;}
    inline Sector get_sector() const {return (sector_start + sector_end) / 2 ;}
    Byte_Value get_byte_length() const ;
    Sector get_sector_length() const ;
    QString get_filesystem_label()const;
    PartitionInfo getPartitionInfo();

    QString device_path ;
    int partition_number;
    PartitionType type;// UNALLOCATED, PRIMARY, LOGICAL, etc...
    PartitionStatus status; //STAT_REAL, STAT_NEW, etc..
    PartitionAlignment alignment;   //ALIGN_CYLINDER, ALIGN_STRICT, etc
    FSType fstype;
    QString uuid ;
    QString name;
    Sector sector_start;
    Sector sector_end;
    Sector sectors_used;
    Sector sectors_unused;
    Sector sectors_unallocated;  //Difference between the size of the partition and the file system
    Sector significant_threshold;  //Threshold from intrinsic to significant unallocated sectors
    bool inside_extended;
    bool busy;
    bool fs_readonly;  // Is the file system mounted read-only?
    std::vector<QString> flags ;

    //PartitionVector logicals;
    QVector<Partition *> logicals;
    bool strict_start ; //Indicator if start sector must stay unchanged
    Sector free_space_before ;  //Free space preceding partition value

    Byte_Value sector_size ;  //Sector size of the disk device needed for converting to/from sectors and bytes.
    Byte_Value fs_block_size;  // Block size of of the file system, or -1 when unknown.

private:
    Sector calc_significant_unallocated_sectors() const ;
    QString path;
    QVector<QString> mountpoints ;
    bool have_filesystem_label;
    QString filesystem_label;


};
}
#endif // PARTITION_H
