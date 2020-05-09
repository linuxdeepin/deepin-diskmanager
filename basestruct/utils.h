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


};

#endif // UTILS_H
