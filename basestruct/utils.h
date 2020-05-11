#ifndef UTILS_H
#define UTILS_H
#include <QString>
#include <QStringList>
#include "commondef.h"


class Utils
{
public:
    Utils();
    static QString find_program_in_path(const QString &proname);
    static int executecmdwithartlist(const QString &strcmd, const QStringList  &strarg, QString &output, QString &error);
    static int executcmd(const QString &strcmd, QString &output, QString &error);
    static QString regexp_label(const QString &strtext, const QString &strpatter);
    static const QString get_partition_type_string(PartitionType type);
    static const QString FSTypeToString(FSType type);
    static FSType StringToFSType(const QString &fsname);
    static int get_mounted_filesystem_usage(const QString &mountpoint, Byte_Value &fs_size, Byte_Value &fs_free) ;
    static QString get_filesystem_software(FSType fstype);
    static QString format_size(Sector sectors, Byte_Value sector_size) ;
    static double sector_to_unit(Sector sectors, Byte_Value sector_size, SIZE_UNIT size_unit) ;
};

#endif // UTILS_H
