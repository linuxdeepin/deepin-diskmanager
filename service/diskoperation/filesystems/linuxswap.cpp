#include "linuxswap.h"
#include "utils.h"
#include "diskoperation/blockspecial.h"
#include <QFile>
#include <QDebug>


namespace DiskManager {

FS LinuxSwap::get_filesystem_support()
{
    FS fs(FS_LINUX_SWAP);

    fs .busy = FS::GPARTED ;
    fs .read = FS::EXTERNAL ;
    fs .online_read = FS::EXTERNAL ;

    if (!Utils::find_program_in_path("mkswap") .isEmpty()) {
        fs.create = FS::EXTERNAL;
        fs.create_with_label = FS::EXTERNAL;
        fs.grow = FS::EXTERNAL;
        fs.shrink = FS::EXTERNAL;
        fs.copy = FS::EXTERNAL;
        fs.move = FS::EXTERNAL;
    }

    if (! Utils::find_program_in_path("swaplabel") .isEmpty()) {
        fs .read_label = FS::EXTERNAL ;
        fs .write_label = FS::EXTERNAL ;
        fs .read_uuid = FS::EXTERNAL ;
        fs .write_uuid = FS::EXTERNAL ;
    }

    return fs ;
}

void LinuxSwap::set_used_sectors(Partition &partition)
{
    if (partition .busy) {
        N = -1;
        QFile file("/proc/swaps");
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QString line = file.readLine();
            BlockSpecial bs_path = BlockSpecial(partition.get_path());
            while (!file.atEnd() || !line.isEmpty()) {
                QString filename = Utils::regexp_label(line, ".*?(?= )");
                if (bs_path == BlockSpecial(filename)) {
                    sscanf(line.toLatin1(), "%*s %*s %*d %lld", &N);
                    break ;
                }
                line = file.readLine();
            }
        }
        if (N > -1) {
            // Ignore swap space reported size to ignore 1 page format
            // overhead.  Instead use partition size as sectors_fs_size so
            // reported used figure for active swap space starts from 0
            // upwards, matching what 'swapon -s' reports.
            T = partition.get_sector_length();
            N = qRound(N * (KIBIBYTE / double(partition .sector_size))) ;
            partition .set_sector_usage(T, T - N) ;
        }
    } else {
        //By definition inactive swap space is 100% free
        Sector size = partition .get_sector_length() ;
        partition .set_sector_usage(size, size) ;
    }
}

void LinuxSwap::read_label(Partition &partition)
{
    QString output, error, label;
    Utils::executcmd(QString("swaplabel %1").arg(partition.get_path()), output, error);
    label = Utils::regexp_label(output, "(?<=LABEL:).*(?=\n)");
    partition.set_filesystem_label(label);

    qDebug() << output << error << "----" << label;
}

bool LinuxSwap::write_label(const Partition &partition)
{
    QString output, error;
    int exitcode = Utils::executcmd(QString("swaplabel -L %1 %2").arg(partition.get_filesystem_label()).arg(partition.get_path()),
                                    output, error);
    return exitcode == 0 && error.compare("Unknown error") == 0;
}

void LinuxSwap::read_uuid(Partition &partition)
{
    QString output, error;
    Utils::executcmd(QString("swaplabel %1").arg(partition.get_path()), output, error);
    partition .uuid = Utils::regexp_label(output, "(?<=UUID:).*(?=\n)") ;
}

bool LinuxSwap::write_uuid(const Partition &partition)
{
    QString output, error;
    int exitcode = Utils::executcmd(QString("swaplabel -L %1 %2").arg(Utils::CreateUUid()).arg(partition.get_path()),
                                    output, error);
    return exitcode == 0 && error.compare("Unknown error") == 0;
}

bool LinuxSwap::create(const Partition &new_partition)
{
    QString output, error;
    int exitcode = Utils::executcmd(QString("mkswap -L %1 %2").arg(new_partition.get_filesystem_label()).arg(new_partition.get_path()),
                                    output, error);
    return exitcode == 0 && error.compare("Unknown error") == 0;
}

bool LinuxSwap::resize(const Partition &partition_new, bool)
{
    QString output, error;
    QString command = QString("mkswap -L %1 ").arg(partition_new.get_filesystem_label());
    if (! partition_new .uuid .isEmpty())
        command.append(QString(" -U %1 ").arg(partition_new.uuid));
    command.append(partition_new.get_path());
    int exitcode = Utils::executcmd(command, output, error);
    return exitcode == 0 && error.compare("Unknown error") == 0;
}


}
