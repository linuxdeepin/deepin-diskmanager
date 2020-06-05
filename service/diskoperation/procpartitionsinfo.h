#ifndef PROCPARTITIONSINFO_H
#define PROCPARTITIONSINFO_H
#include <QVector>
#include <QString>

namespace DiskManager {

class ProcPartitionsInfo
{
public:
    static void load_cache();
    static const QVector<QString> &get_device_paths();

private:
    static void initialize_if_required();
    static void load_proc_partitions_info_cache();
    static bool proc_partitions_info_cache_initialized;
    static QVector<QString> device_paths_cache;
};

} // namespace DiskManager
#endif // PROCPARTITIONSINFO_H
