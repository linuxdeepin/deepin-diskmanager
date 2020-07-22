#include "partedcore.h"
#include "fsinfo.h"
#include "mountinfo.h"
#include "partition.h"
#include "procpartitionsinfo.h"
#include "filesystems/filesystem.h"
#include <unistd.h>
#include <fcntl.h>
#include <QDebug>

namespace DiskManager {
//hdparm可检测，显示与设定IDE或SCSI硬盘的参数。
//udevadm可检测设备热插拔
static bool udevadm_found = false;
static bool hdparm_found = false;
const std::time_t SETTLE_DEVICE_PROBE_MAX_WAIT_SECONDS = 1;
const std::time_t SETTLE_DEVICE_APPLY_MAX_WAIT_SECONDS = 10;
SupportedFileSystems *PartedCore::supported_filesystems = nullptr;

PartedCore::PartedCore(QObject *parent)
    : QObject(parent)
{
    connect(this, &PartedCore::sigRefreshDeviceInfo, this, &PartedCore::slotRefreshDeviceInfo);
    qDebug() << __FUNCTION__ << "^^1";
    for (PedPartitionFlag flag = ped_partition_flag_next(static_cast<PedPartitionFlag>(NULL));
            flag; flag = ped_partition_flag_next(flag))
        flags.push_back(flag);
    qDebug() << __FUNCTION__ << "^^2";
    find_supported_core();
    qDebug() << __FUNCTION__ << "^^3";
    supported_filesystems = new SupportedFileSystems();
    //Determine file system support capabilities for the first time
    supported_filesystems->find_supported_filesystems();
    qDebug() << __FUNCTION__ << "^^4";
    probedeviceinfo();
    qDebug() << __FUNCTION__ << "^^5";
}

PartedCore::~PartedCore()
{
    delete supported_filesystems;
    supported_filesystems = NULL;
}

void PartedCore::find_supported_core()
{
    udevadm_found = !Utils::find_program_in_path("udevadm").isEmpty();
    hdparm_found = !Utils::find_program_in_path("hdparm").isEmpty();
}

bool PartedCore::supported_filesystem(FSType fstype)
{
    return supported_filesystems->get_fs_object(fstype) != NULL;
}

const FS &PartedCore::get_fs(FSType fstype) const
{
    return supported_filesystems->get_fs_support(fstype);
}

FileSystem *PartedCore::get_filesystem_object(FSType fstype)
{
    return supported_filesystems->get_fs_object(fstype);
}

bool PartedCore::filesystem_resize_disallowed(const Partition &partition)
{
    if (partition.fstype == FS_LVM2_PV) {
        //        //The LVM2 PV can't be resized when it's a member of an export VG
        //        QString vgname = LVM2_PV_Info::get_vg_name(partition.get_path());
        //        if (vgname .isEmpty())
        //            return false ;
        //        return LVM2_PV_Info::is_vg_exported(vgname);
    }
    return false;
}

void PartedCore::insert_unallocated(const QString &device_path, QVector<Partition *> &partitions, Sector start, Sector end, Byte_Value sector_size, bool inside_extended)
{
    //if there are no partitions at all..
    if (partitions.empty()) {
        Partition *partition_temp = new Partition();
        partition_temp->Set_Unallocated(device_path, start, end, sector_size, inside_extended);
        partitions.push_back(partition_temp);
        return;
    }

    //start <---> first partition start
    if ((partitions.front()->sector_start - start) > (MEBIBYTE / sector_size)) {
        Sector temp_end = partitions.front()->sector_start - 1;
        Partition *partition_temp = new Partition();
        partition_temp->Set_Unallocated(device_path, start, temp_end, sector_size, inside_extended);
        partitions.insert(partitions.begin(), partition_temp);
    }

    //look for gaps in between
    for (int t = 0; t < partitions.size() - 1; t++) {
        if (((partitions.at(t + 1)->sector_start - partitions.at(t)->sector_end - 1) > (MEBIBYTE / sector_size))
                || ((partitions.at(t + 1)->type != TYPE_LOGICAL) // Only show exactly 1 MiB if following partition is not logical.
                    && ((partitions.at(t + 1)->sector_start - partitions.at(t)->sector_end - 1) == (MEBIBYTE / sector_size)))) {
            Sector temp_start = partitions.at(t)->sector_end + 1;
            Sector temp_end = partitions.at(t + 1)->sector_start - 1;
            Partition *partition_temp = new Partition();
            partition_temp->Set_Unallocated(device_path, temp_start, temp_end,
                                            sector_size, inside_extended);
            partitions.insert(partitions.begin() + (++t), partition_temp);
            // partitions.insert_adopt(partitions.begin() + ++t, partition_temp);
        }
    }
    partitions.back();
    //last partition end <---> end
    if ((end - partitions.back()->sector_end) >= (MEBIBYTE / sector_size)) {
        Sector temp_start = partitions.back()->sector_end + 1;
        Partition *partition_temp = new Partition();
        partition_temp->Set_Unallocated(device_path, temp_start, end, sector_size, inside_extended);
        partitions.push_back(partition_temp);
    }
}

void PartedCore::set_flags(Partition &partition, PedPartition *lp_partition)
{
    for (int t = 0; t < flags.size(); t++) {
        if (ped_partition_is_flag_available(lp_partition, flags[t]) && ped_partition_get_flag(lp_partition, flags[t]))
            partition.flags.push_back(ped_partition_flag_get_name(flags[t]));
    }
}

FS_Limits PartedCore::get_filesystem_limits(FSType fstype, const Partition &partition)
{
    FileSystem *p_filesystem = supported_filesystems->get_fs_object(fstype);
    FS_Limits fs_limits;
    if (p_filesystem != NULL)
        fs_limits = p_filesystem->get_filesystem_limits(partition);
    return fs_limits;
}

void PartedCore::probedeviceinfo(const QString &)
{
    QVector<QString> devicepaths;
    qDebug() << __FUNCTION__ << "**1";
    devicepaths.clear();
    BlockSpecial::clear_cache();
    qDebug() << __FUNCTION__ << "**2";
    ProcPartitionsInfo::load_cache();
    qDebug() << __FUNCTION__ << "**3";
    FsInfo::load_cache();
    qDebug() << __FUNCTION__ << "**4";
    MountInfo::load_cache();
    qDebug() << __FUNCTION__ << "**6";
    ped_device_probe_all();
    qDebug() << __FUNCTION__ << "**7";
    PedDevice *lp_device = ped_device_get_next(NULL);
    while (lp_device) {
        /* TO TRANSLATORS: looks like   Confirming /dev/sda */
        qDebug() << QString("Confirming %1").arg(lp_device->path);

        //only add this device if we can read the first sector (which means it's a real device)
        if (useable_device(lp_device))
            devicepaths.push_back(lp_device->path);
        qDebug() << lp_device->path;
        lp_device = ped_device_get_next(lp_device);
    }
    qDebug() << __FUNCTION__ << "devicepaths size=" << devicepaths.size();
    std::sort(devicepaths.begin(), devicepaths.end());
    qDebug() << __FUNCTION__ << "**8";
    for (int t = 0; t < devicepaths.size(); t++) {
        /*TO TRANSLATORS: looks like Searching /dev/sda partitions */
        Device temp_device;
        set_device_from_disk(temp_device, devicepaths[t]);
        devicemap.insert(devicepaths.at(t), temp_device);
    }
    qDebug() << __FUNCTION__ << "**9";
    for (auto it = devicemap.begin(); it != devicemap.end(); it++) {
        DeviceInfo devinfo = it.value().getDeviceInfo();
        for (int i = 0; i < it.value().partitions.size(); i++) {
            Partition pat = *(it.value().partitions.at(i));
            PartitionInfo partinfo = pat.getPartitionInfo();
            if (pat.type == TYPE_EXTENDED) {
                devinfo.partition.push_back(partinfo);
                for (int k = 0; k < pat.logicals.size(); k++) {
                    Partition plogic = *(pat.logicals.at(k));
                    partinfo = plogic.getPartitionInfo();
                    devinfo.partition.push_back(partinfo);
                }
            } else {
                devinfo.partition.push_back(partinfo);
            }
        }
        inforesult.insert(devinfo.m_path, devinfo);
    }
    qDebug() << __FUNCTION__ << "**10";
}

bool PartedCore::useable_device(const PedDevice *lp_device)
{
    Q_ASSERT(nullptr != lp_device);
    char *buf = static_cast<char *>(malloc(lp_device->sector_size));
    if (!buf)
        return false;

    // Must be able to read from the first sector before the disk device is considered
    // useable in GParted.
    bool success = false;
    int fd = open(lp_device->path, O_RDONLY | O_NONBLOCK);
    if (fd >= 0) {
        ssize_t bytes_read = read(fd, buf, lp_device->sector_size);
        success = (bytes_read == lp_device->sector_size);
        close(fd);
    }
    free(buf);
    return success;
}

void PartedCore::set_device_from_disk(Device &device, const QString &device_path)
{
    PedDevice *lp_device = NULL;
    PedDisk *lp_disk = NULL;
    if (get_device(device_path, lp_device, true)) {
        device.heads = lp_device->bios_geom.heads;
        device.length = lp_device->length;
        device.m_path = device_path;
        device.model = lp_device->model;
        device.sector_size = lp_device->sector_size;
        device.sectors = lp_device->bios_geom.sectors;
        device.cylinders = lp_device->bios_geom.cylinders;
        device.cylsize = device.heads * device.sectors;
        set_device_serial_number(device);
        if (device.cylsize < (MEBIBYTE / device.sector_size))
            device.cylsize = MEBIBYTE / device.sector_size;

        FSType fstype = detect_filesystem(lp_device, NULL);
        if (fstype != FS_UNKNOWN) {
            device.disktype = "none";
            device.max_prims = 1;
            set_device_one_partition(device, lp_device, fstype);
        } else if (get_disk(lp_device, lp_disk, false)) {
            // Partitioned drive (excluding "loop"), as recognised by libparted
            if (lp_disk && lp_disk->type && lp_disk->type->name && strcmp(lp_disk->type->name, "loop") != 0) {
                device.disktype = lp_disk->type->name;
                device.max_prims = ped_disk_get_max_primary_partition_count(lp_disk);

                // Determine if partition naming is supported.
                if (ped_disk_type_check_feature(lp_disk->type, PED_DISK_TYPE_PARTITION_NAME)) {
                    device.enable_partition_naming(
                        Utils::get_max_partition_name_length(device.disktype));
                }

                set_device_partitions(device, lp_device, lp_disk);

                if (device.highest_busy) {
                    device.readonly = !commit_to_os(lp_disk, SETTLE_DEVICE_PROBE_MAX_WAIT_SECONDS);
                }
            }
            // Drive just containing libparted "loop" signature and nothing
            // else.  (Actually any drive reported by libparted as "loop" but
            // not recognised by blkid on the whole disk device).
            else if (lp_disk && lp_disk->type && lp_disk->type->name && strcmp(lp_disk->type->name, "loop") == 0) {
                device.disktype = lp_disk->type->name;
                device.max_prims = 1;

                // Create virtual partition covering the whole disk device
                // with unknown contents.
                Partition *partition_temp = new Partition();
                partition_temp->set_unpartitioned(device.m_path,
                                                  lp_device->path,
                                                  FS_UNKNOWN,
                                                  device.length,
                                                  device.sector_size,
                                                  false);
                // Place unknown file system message in this partition.
                device.partitions.push_back(partition_temp);
            }
            // Unrecognised, unpartitioned drive.
            else {
                device.disktype = "unrecognized";
                device.max_prims = 1;

                Partition *partition_temp = new Partition();
                partition_temp->set_unpartitioned(device.m_path,
                                                  "", // Overridden with "unallocated"
                                                  FS_UNALLOCATED,
                                                  device.length,
                                                  device.sector_size,
                                                  false);
                device.partitions.push_back(partition_temp);
            }
        }
        destroy_device_and_disk(lp_device, lp_disk);
    }
}

bool PartedCore::get_device(const QString &device_path, PedDevice *&lp_device, bool flush)
{
    lp_device = ped_device_get(device_path.toStdString().c_str());
    if (lp_device) {
        if (flush)
            // Force cache coherency before going on to read the partition
            // table so that libparted reading the whole disk device and the
            // file system tools reading the partition devices read the same
            // data.
            flush_device(lp_device);
        return true;
    }
    return false;
}

bool PartedCore::get_disk(PedDevice *&lp_device, PedDisk *&lp_disk, bool strict)
{
    if (lp_device) {
        lp_disk = ped_disk_new(lp_device);

        // (#762941)(!46) After ped_disk_new() wait for triggered udev rules to
        // to complete which remove and re-add all the partition specific /dev
        // entries to avoid FS specific commands failing because they happen to
        // be running when the needed /dev/PTN entries don't exist.
        settle_device(SETTLE_DEVICE_PROBE_MAX_WAIT_SECONDS);

        // if ! disk and writable it's probably a HD without disklabel.
        // We return true here and deal with them in
        // GParted_Core::set_device_from_disk().
        if (lp_disk || (!strict && !lp_device->read_only))
            return true;

        destroy_device_and_disk(lp_device, lp_disk);
    }

    return false;
}

void PartedCore::destroy_device_and_disk(PedDevice *&lp_device, PedDisk *&lp_disk)
{
    if (lp_disk)
        ped_disk_destroy(lp_disk);
    lp_disk = NULL;

    if (lp_device)
        ped_device_destroy(lp_device);
    lp_device = NULL;
}

bool PartedCore::infoBelongToPartition(const Partition &partition, const PartitionInfo info)
{
    bool belong = false;
    if (info.sector_end == partition.sector_end && info.sector_start == partition.sector_start)
        belong = true;
    return belong;
}

bool PartedCore::get_device_and_disk(const QString &device_path, PedDevice *&lp_device, PedDisk *&lp_disk, bool strict, bool flush)
{
    if (get_device(device_path, lp_device, flush)) {
        return get_disk(lp_device, lp_disk, strict);
    }

    return false;
}

bool PartedCore::commit(PedDisk *lp_disk)
{
    bool opened = ped_device_open(lp_disk->dev);

    bool succes = ped_disk_commit_to_dev(lp_disk);

    succes = commit_to_os(lp_disk, SETTLE_DEVICE_APPLY_MAX_WAIT_SECONDS) && succes;

    if (opened) {
        ped_device_close(lp_disk->dev);
        // Wait for udev rules to complete and partition device nodes to settle
        // from this ped_device_close().
        settle_device(SETTLE_DEVICE_APPLY_MAX_WAIT_SECONDS);
    }

    return succes;
}

PedPartition *PartedCore::get_lp_partition(const PedDisk *lp_disk, const Partition &partition)
{
    if (partition.type == TYPE_EXTENDED)
        return ped_disk_extended_partition(lp_disk);
    return ped_disk_get_partition_by_sector(lp_disk, partition.get_sector());
}

void PartedCore::set_device_serial_number(Device &device)
{
    if (!hdparm_found)
        // Serial number left blank when the hdparm command is not installed.
        return;

    QString output, error;
    Utils::executcmd(QString("hdparm -I %1").arg(device.m_path), output, error);
    if (!error.isEmpty()) {
        // hdparm reported an error message to stderr.  Assume it's a device
        // without a hard drive serial number.
        //
        // Using hdparm -I to query Linux software RAID arrays and BIOS fake RAID
        // arrays, both devices without their own hard drive serial numbers,
        // produce this error:
        //     HDIO_DRIVE_CMD(identify) failed: Inappropriate ioctl for device
        //
        // And querying USB flash drives, also a device type without their own
        // hard drive serial numbers, generates this error:
        //     SG_IO: bad/missing sense data, sb[]:  70 00 05 00 00 00 00 0a ...
        device.serial_number = "none";
    } else {
        QString serial_number = Utils::regexp_label(output, "^[[:blank:]]*Serial Number:[[:blank:]]*(.*)[[:blank:]]*$");
        if (!serial_number.isEmpty())
            device.serial_number = serial_number;
    }
    // Otherwise serial number left blank when not found in the hdparm output.
}

void PartedCore::set_device_one_partition(Device &device, PedDevice *lp_device, FSType fstype)
{
    device.partitions.clear();
    QString path(lp_device->path);
    bool partition_is_busy = is_busy(fstype, path);

    Partition *partition_temp = NULL;
    if (fstype == FS_LUKS) {
        partition_temp = NULL; //= new PartitionLUKS();
    } else
        partition_temp = new Partition();
    if (NULL == partition_temp)
        return;
    partition_temp->set_unpartitioned(device.m_path,
                                      path,
                                      fstype,
                                      device.length,
                                      device.sector_size,
                                      partition_is_busy);

    //        if ( fstype == FS_LUKS )
    //            set_luks_partition( *dynamic_cast<PartitionLUKS *>( partition_temp ) );

    if (partition_temp->busy)
        device.highest_busy = 1;

    set_partition_label_and_uuid(*partition_temp);
    set_mountpoints(*partition_temp);
    set_used_sectors(*partition_temp, NULL);
    device.partitions.push_back(partition_temp);
}

void PartedCore::set_partition_label_and_uuid(Partition &partition)
{
    QString partition_path = partition.get_path();
    read_label(partition);
    if (!partition.filesystem_label_known()) {
        bool label_found = false;
        QString label = FsInfo::get_label(partition_path, label_found);
        if (label_found)
            partition.set_filesystem_label(label);
    }

    // Retrieve file system UUID.  Use cached method first in an effort to speed up
    // device scanning.
    partition.uuid = FsInfo::get_uuid(partition_path);
    if (partition.uuid.isEmpty()) {
        read_uuid(partition);
    }
}

bool PartedCore::is_busy(FSType fstype, const QString &path, const PedPartition *lp_partition)
{
    FileSystem *p_filesystem = NULL;
    bool busy = false;
    if (nullptr != lp_partition) {
        busy = ped_partition_is_busy(lp_partition);
    }
    if (!busy && supported_filesystem(fstype)) {
        switch (get_fs(fstype).busy) {
        case FS::GPARTED:
            //Search GParted internal mounted partitions map
            busy = MountInfo::is_dev_mounted(path);
            break;
        case FS::EXTERNAL:
            //Call file system specific method
            p_filesystem = get_filesystem_object(fstype);
            if (p_filesystem)
                busy = p_filesystem->is_busy(path);
            break;

        default:
            break;
        }
    }

    return busy;
}

void PartedCore::read_label(Partition &partition)
{
    FileSystem *p_filesystem = NULL;
    switch (get_fs(partition.fstype).read_label) {
    case FS::EXTERNAL:
        p_filesystem = get_filesystem_object(partition.fstype);
        if (p_filesystem)
            p_filesystem->read_label(partition);
        break;

    default:
        break;
    }
}

void PartedCore::read_uuid(Partition &partition)
{
    FileSystem *p_filesystem = NULL;
    switch (get_fs(partition.fstype).read_uuid) {
    case FS::EXTERNAL:
        p_filesystem = get_filesystem_object(partition.fstype);
        if (p_filesystem)
            p_filesystem->read_uuid(partition);
        break;

    default:
        break;
    }
}

void PartedCore::set_mountpoints(Partition &partition)
{
    //DMRaid dmraid ; //Use cache of dmraid device information
    if (partition.fstype == FS_LVM2_PV) {
        //        QString vgname = LVM2_PV_Info::get_vg_name(partition.get_path());
        //        if (! vgname.isEmpty())
        //            partition.add_mountpoint(vgname);
    } else if (partition.fstype == FS_LINUX_SWRAID) {
        //        QString array_path = SWRaid_Info::get_array(partition.get_path());
        //        if (! array_path.isEmpty())
        //            partition.add_mountpoint(array_path);
    } else if (partition.fstype == FS_ATARAID) {
        //        QString array_path = SWRaid_Info::get_array(partition.get_path());
        //        if (! array_path.isEmpty()) {
        //            partition.add_mountpoint(array_path);
        //        } else {
        //            array_path = dmraid.get_array(partition.get_path());
        //            if (! array_path.isEmpty())
        //                partition.add_mountpoint(array_path);
        //        }
    } else if (partition.fstype == FS_LUKS) {
        //        LUKS_Mapping mapping = LUKS_Info::get_cache_entry(partition.get_path());
        //        if (! mapping.name.isEmpty())
        //            partition.add_mountpoint(DEV_MAPPER_PATH + mapping.name);
    }
    // Swap spaces don't have mount points so don't bother trying to add them.
    else if (partition.fstype != FS_LINUX_SWAP) {
        if (partition.busy) {
            // Normal device, not DMRaid device
            if (set_mountpoints_helper(partition, partition.get_path()))
                return;

            qDebug() << __FUNCTION__ << "xxxUnable to find mount point";
        } else { // Not busy file system
            partition.add_mountpoints(MountInfo::get_fstab_mountpoints(partition.get_path()));
        }
    }
}

bool PartedCore::set_mountpoints_helper(Partition &partition, const QString &path)
{
    QString search_path;
    if (partition.fstype == FS_BTRFS)
        search_path = path; //btrfs::get_mount_device( path ) ;
    else
        search_path = path;

    const QVector<QString> &mountpoints = MountInfo::get_mounted_mountpoints(search_path);
    if (mountpoints.size()) {
        partition.add_mountpoints(mountpoints);
        partition.fs_readonly = MountInfo::is_dev_mounted_readonly(search_path);
        return true;
    }

    return false;
}

void PartedCore::set_used_sectors(Partition &partition, PedDisk *lp_disk)
{
    Q_UNUSED(lp_disk)
    if (supported_filesystem(partition.fstype)) {
        FileSystem *p_filesystem = NULL;
        if (partition.busy) {
            switch (get_fs(partition.fstype).online_read) {
            case FS::EXTERNAL:
                p_filesystem = get_filesystem_object(partition.fstype);
                if (p_filesystem)
                    p_filesystem->set_used_sectors(partition);
                break;
            case FS::GPARTED:
                mounted_fs_set_used_sectors(partition);
                break;
            default:
                break;
            }
        } else { // Not busy file system
            switch (get_fs(partition.fstype).read) {
            case FS::EXTERNAL:
                p_filesystem = get_filesystem_object(partition.fstype);
                if (p_filesystem)
                    p_filesystem->set_used_sectors(partition);
                break;
#ifdef HAVE_LIBPARTED_FS_RESIZE
            case FS::LIBPARTED:
                if (lp_disk)
                    LP_set_used_sectors(partition, lp_disk);
                break;
#endif
            default:
                break;
            }
        }

        Sector unallocated;
        // Only confirm that the above code succeeded in setting the sector usage
        // values for this base Partition object, hence the explicit call to the
        // base Partition class sector_usage_known() method.  For LUKS this avoids
        // calling derived PartitionLUKS class sector_usage_known() which also
        // checks for known sector usage in the encrypted file system.  But that
        // wasn't set by the above code so in the case of luks/unknown would
        // produce a false positive.
        if (!partition.sector_usage_known()) {
            if (!Utils::get_filesystem_software(partition.fstype).isEmpty()) {
                QString msg("The following list of software packages is required for %1 file system support:  %2.");
                msg = msg.arg(Utils::FSTypeToString(partition.fstype)).arg(Utils::get_filesystem_software(partition.fstype));
                qDebug() << __FUNCTION__ << msg;
            }

        } else if ((unallocated = partition.get_sectors_unallocated()) > 0) {
            /* TO TRANSLATORS: looks like   1.28GiB of unallocated space within the partition. */
            QString temp("%1 of unallocated space within the partition.");
            temp = temp.arg(Utils::format_size(unallocated, partition.sector_size));
            FS fs = get_fs(partition.fstype);
            if (fs.check != FS::NONE && fs.grow != FS::NONE) {
                temp.append("To grow the file system to fill the partition, select the partition and choose the menu item:\n");
                temp.append("Partition --> Check.");
            }
        }

        if (filesystem_resize_disallowed(partition)) {
            //            QString temp = get_filesystem_object(partition.fstype)
            //                                 ->get_custom_text(CTEXT_RESIZE_DISALLOWED_WARNING);
        }
    } else {
        // Set usage of mounted but unsupported file systems.
        if (partition.busy)
            mounted_fs_set_used_sectors(partition);
    }
}

void PartedCore::mounted_fs_set_used_sectors(Partition &partition)
{
    if (partition.get_mountpoints().size() > 0 && MountInfo::is_dev_mounted(partition.get_path())) {
        Byte_Value fs_size;
        Byte_Value fs_free;
        if (Utils::get_mounted_filesystem_usage(partition.get_mountpoint(), fs_size, fs_free) == 0)
            partition.set_sector_usage(fs_size / partition.sector_size,
                                       fs_free / partition.sector_size);
    }
}

void PartedCore::set_device_partitions(Device &device, PedDevice *lp_device, PedDisk *lp_disk)
{
    int EXT_INDEX = -1;
    device.partitions.clear();

    PedPartition *lp_partition = ped_disk_next_partition(lp_disk, NULL);
    while (lp_partition) {
        Partition *partition_temp = NULL;
        bool partition_is_busy = false;
        FSType fstype = FS_UNKNOWN;
        QString partition_path;
        switch (lp_partition->type) {
        case PED_PARTITION_NORMAL:
        case PED_PARTITION_LOGICAL:
            fstype = detect_filesystem(lp_device, lp_partition);
            partition_path = get_partition_path(lp_partition);
            partition_is_busy = is_busy(fstype, partition_path, lp_partition);
            qDebug() << partition_is_busy << lp_partition->num << lp_partition->disk->dev->path;
            //            if (fstype == FS_LUKS)
            //                partition_temp = new PartitionLUKS();
            //            else
            partition_temp = new Partition();
            partition_temp->Set(device.m_path,
                                partition_path,
                                lp_partition->num,
                                (lp_partition->type == PED_PARTITION_NORMAL) ? TYPE_PRIMARY
                                : TYPE_LOGICAL,
                                fstype,
                                lp_partition->geom.start,
                                lp_partition->geom.end,
                                device.sector_size,
                                (lp_partition->type == PED_PARTITION_LOGICAL),
                                partition_is_busy);

            set_flags(*partition_temp, lp_partition);

            //if (fstype == FS_LUKS)
            // set_luks_partition(*dynamic_cast<PartitionLUKS *>(partition_temp));

            if (partition_temp->busy && partition_temp->partition_number > device.highest_busy)
                device.highest_busy = partition_temp->partition_number;
            break;

        case PED_PARTITION_EXTENDED:
            partition_path = get_partition_path(lp_partition);

            partition_temp = new Partition();
            partition_temp->Set(device.m_path,
                                partition_path,
                                lp_partition->num,
                                TYPE_EXTENDED,
                                FS_EXTENDED,
                                lp_partition->geom.start,
                                lp_partition->geom.end,
                                device.sector_size,
                                false,
                                false);

            set_flags(*partition_temp, lp_partition);

            EXT_INDEX = device.partitions.size();
            break;

        default:
            qDebug() << ped_partition_is_busy(lp_partition) << lp_partition->num << lp_partition->disk->dev->path;
            // Ignore libparted reported partitions with other type
            // bits set.
            break;
        }

        // Only for libparted reported partition types that we care about: NORMAL,
        // LOGICAL, EXTENDED
        if (partition_temp != NULL) {
            set_partition_label_and_uuid(*partition_temp);
            set_mountpoints(*partition_temp);
            set_used_sectors(*partition_temp, lp_disk);

            // Retrieve partition name
            if (device.partition_naming_supported())
                partition_temp->name = ped_partition_get_name(lp_partition);

            if (!partition_temp->inside_extended)
                device.partitions.push_back(partition_temp);
            else
                device.partitions[EXT_INDEX]->logicals.push_back(partition_temp);
        }

        //next partition (if any)
        lp_partition = ped_disk_next_partition(lp_disk, lp_partition);
    }

    if (EXT_INDEX > -1) {
        insert_unallocated(device.m_path,
                           device.partitions.at(EXT_INDEX)->logicals,
                           device.partitions.at(EXT_INDEX)->sector_start,
                           device.partitions.at(EXT_INDEX)->sector_end,
                           device.sector_size,
                           true);

        //Set busy status of extended partition if and only if
        //  there is at least one busy logical partition.
        for (int t = 0; t < device.partitions.at(EXT_INDEX)->logicals.size(); t++) {
            if (device.partitions.at(EXT_INDEX)->logicals.at(t)->busy) {
                device.partitions.at(EXT_INDEX)->busy = true;
                break;
            }
        }
    }

    insert_unallocated(device.m_path, device.partitions, 0, device.length - 1, device.sector_size, false);
}

bool PartedCore::flush_device(PedDevice *lp_device)
{
    bool success = false;
    if (ped_device_open(lp_device)) {
        success = ped_device_sync(lp_device);
        ped_device_close(lp_device);
        // (!46) Wait for udev rules to complete after this ped_device_open() and
        // ped_device_close() pair to avoid busy /dev/DISK entry when running file
        // system specific querying commands on the whole disk device in the call
        // sequence after get_device() in set_device_from_disk().
        settle_device(SETTLE_DEVICE_PROBE_MAX_WAIT_SECONDS);
    }
    return success;
}

void PartedCore::settle_device(std::time_t timeout)
{
    //如果支持udevadm
    //udevadm settle [options]　　查看udev事件队列，如果所有事件全部处理完就退出。timeout超时时间
    if (udevadm_found) {
        QString out, err;
        Utils::executcmd(QString("udevadm settle --timeout=%1").arg(timeout), out, err);
    } else
        sleep(timeout);
}

bool PartedCore::commit_to_os(PedDisk *lp_disk, time_t timeout)
{
    bool succes;
    succes = ped_disk_commit_to_os(lp_disk);
    // Wait for udev rules to complete and partition device nodes to settle from above
    // ped_disk_commit_to_os() initiated kernel update of the partitions.
    settle_device(timeout);
    return succes;
}

FSType PartedCore::detect_filesystem(PedDevice *lp_device, PedPartition *lp_partition)
{
    QString fsname = "";
    QString path;
    // Will query whole disk device using methods: (Q1) RAID, (Q2) blkid,
    // (Q4) internal
    if (lp_partition)
        path = get_partition_path(lp_partition);
    else
        path = lp_device->path;

    fsname = FsInfo::get_fs_type(path);
    FSType fstype = FS_UNKNOWN;
    if (fsname.isEmpty() && lp_partition && lp_partition->fs_type)
        fsname = lp_partition->fs_type->name;
    if (!fsname.isEmpty()) {
        fstype = Utils::StringToFSType(fsname);
        qDebug() << fstype;
        if (fstype != FS_UNKNOWN)
            return fstype;
    }

    fstype = detect_filesystem_internal(path, lp_device->sector_size);
    if (fstype != FS_UNKNOWN)
        return fstype;

    //no file system found....
    QString temp("Unable to detect file system! Possible reasons are:\n- ");
    temp.append("The file system is damaged \n- ");
    temp.append("The file system is unknown to GParted \n-");
    temp.append("There is no file system available (unformatted) \n- ");
    temp.append(QString("The device entry %1 is missing").arg(path));
    qDebug() << __FUNCTION__ << temp;
    return FS_UNKNOWN;
}

FSType PartedCore::detect_filesystem_internal(const QString &path, Byte_Value sector_size)
{
    char magic1[16]; // Big enough for largest signatures[].sig1 or sig2
    char magic2[16];
    FSType fstype = FS_UNKNOWN;

    char *buf = static_cast<char *>(malloc(sector_size));
    if (!buf)
        return FS_UNKNOWN;

    int fd = open(path.toStdString().c_str(), O_RDONLY | O_NONBLOCK);
    if (fd == -1) {
        free(buf);
        return FS_UNKNOWN;
    }

    struct {
        Byte_Value offset1;
        const char *sig1;
        Byte_Value offset2;
        const char *sig2;
        FSType fstype;
    } signatures[] = {
        //offset1, sig1              , offset2, sig2  , fstype
        {0LL, "LUKS\xBA\xBE", 0LL, NULL, FS_LUKS},
        {3LL, "-FVE-FS-", 0LL, NULL, FS_BITLOCKER},
        {0LL, "\x52\x56\xBE\x1B", 0LL, NULL, FS_GRUB2_CORE_IMG},
        {0LL, "\x52\x56\xBE\x6F", 0LL, NULL, FS_GRUB2_CORE_IMG},
        {0LL, "\x52\xE8\x28\x01", 0LL, NULL, FS_GRUB2_CORE_IMG},
        {0LL, "\x52\xBF\xF4\x81", 0LL, NULL, FS_GRUB2_CORE_IMG},
        {0LL, "\x52\x56\xBE\x63", 0LL, NULL, FS_GRUB2_CORE_IMG},
        {0LL, "\x52\x56\xBE\x56", 0LL, NULL, FS_GRUB2_CORE_IMG},
        {24LL, "\x01\x00", 32LL, "NXSB", FS_APFS},
        {512LL, "LABELONE", 536LL, "LVM2", FS_LVM2_PV},
        {1030LL, "\x34\x34", 0LL, NULL, FS_NILFS2},
        {65536LL, "ReIsEr4", 0LL, NULL, FS_REISER4},
        {65600LL, "_BHRfS_M", 0LL, NULL, FS_BTRFS}
    };
    // For simple BitLocker recognition consider validation of BIOS Parameter block
    // fields unnecessary.
    // *   Detecting BitLocker
    //     http://blogs.msdn.com/b/si_team/archive/2006/10/26/detecting-bitlocker.aspx
    //
    // Recognise GRUB2 core.img just by any of the possible first 4 bytes of x86 CPU
    // instructions it starts with.
    // *   bootinfoscript v0.77 line 1990  [GRUB2 core.img possible staring 4 bytes]
    //     https://github.com/arvidjaar/bootinfoscript/blob/009f509d59e2f0d39b8d44692e2a81720f5af7b6/bootinfoscript#L1990
    //
    // Simple APFS recognition based on matching the following fields in the
    // superblock:
    // 1)  Object type is OBJECT_TYPE_NX_SUPERBLOCK, lower 16-bits of the object type
    //     field is 0x0001 stored as little endian bytes 0x01, 0x00.
    //     WARNING: The magic signatures are defined as NUL terminated strings so the
    //     below code only does a 1-byte match for 0x01, rather than a 2-byte match
    //     for 0x01, 0x00.
    // 2)  4 byte magic "NXSB".
    // *   Apple File System Reference
    //     https://developer.apple.com/support/apple-file-system/Apple-File-System-Reference.pdf

    Byte_Value prev_read_offset = -1;
    memset(buf, 0, sector_size);

    for (unsigned int i = 0; i < sizeof(signatures) / sizeof(signatures[0]); i++) {
        const size_t len1 = std::min((signatures[i].sig1 == NULL) ? 0U : strlen(signatures[i].sig1),
                                     sizeof(magic1));
        const size_t len2 = std::min((signatures[i].sig2 == NULL) ? 0U : strlen(signatures[i].sig2),
                                     sizeof(magic2));
        // NOTE: From this point onwards signatures[].sig1 and .sig2 are treated
        // as character buffers of known lengths len1 and len2, not NUL terminated
        // strings.
        if (len1 == 0UL || (signatures[i].sig2 != NULL && len2 == 0UL))
            continue; // Don't allow 0 length signatures to match

        Byte_Value read_offset = signatures[i].offset1 / sector_size * sector_size;

        // Optimisation: only read new sector when it is different to the
        // previously read sector.
        if (read_offset != prev_read_offset) {
            if (lseek(fd, read_offset, SEEK_SET) == read_offset && read(fd, buf, sector_size) == sector_size) {
                prev_read_offset = read_offset;
            } else {
                // Outside block device boundaries or other error.
                continue;
            }
        }

        memcpy(magic1, buf + signatures[i].offset1 % sector_size, len1);

        // WARNING: This assumes offset2 is in the same sector as offset1
        if (signatures[i].sig2 != NULL)
            memcpy(magic2, buf + signatures[i].offset2 % sector_size, len2);

        if (memcmp(magic1, signatures[i].sig1, len1) == 0 && (signatures[i].sig2 == NULL || memcmp(magic2, signatures[i].sig2, len2) == 0)) {
            fstype = signatures[i].fstype;
            break;
        }
    }

    close(fd);
    free(buf);

    return fstype;
}

QString PartedCore::get_partition_path(PedPartition *lp_partition)
{
    char *lp_path; //we have to free the result of ped_partition_get_path()
    QString partition_path("Partition path not found");

    lp_path = ped_partition_get_path(lp_partition);
    if (lp_path != NULL) {
        partition_path = lp_path;
        free(lp_path);
    }
    return partition_path;
}

void PartedCore::LP_set_used_sectors(Partition &partition, PedDisk *lp_disk)
{
    PedFileSystem *fs = NULL;
    PedConstraint *constraint = NULL;

    if (lp_disk) {
        PedPartition *lp_partition = ped_disk_get_partition_by_sector(lp_disk, partition.get_sector());

        if (lp_partition) {
            fs = ped_file_system_open(&lp_partition->geom);

            if (fs) {
                constraint = ped_file_system_get_resize_constraint(fs);
                if (constraint) {
                    partition.set_sector_usage(fs->geom->length,
                                               fs->geom->length - constraint->min_size);

                    ped_constraint_destroy(constraint);
                }
                ped_file_system_close(fs);
            }
        }
    }
}

bool PartedCore::name_partition(const Partition &partition)
{
    QString msg;
    if (partition.name.isEmpty())
        msg = QString("Clear partition name on %1").arg(partition.get_path());
    else
        msg = QString("Set partition name to \"%1\" on %2").arg(partition.name).arg(partition.get_path());
    qDebug() << __FUNCTION__ << msg;

    bool success = false;
    PedDevice *lp_device = NULL;
    PedDisk *lp_disk = NULL;
    if (get_device_and_disk(partition.device_path, lp_device, lp_disk)) {
        PedPartition *lp_partition = ped_disk_get_partition_by_sector(lp_disk, partition.get_sector());
        if (lp_partition) {
            success = ped_partition_set_name(lp_partition, partition.name.toLatin1())
                      && commit(lp_disk);
        }
    }
    return success;
}

bool PartedCore::erase_filesystem_signatures(const Partition &partition)
{
    if (partition.fstype == FS_LUKS && partition.busy) {
        qDebug() << __FUNCTION__ << "partition contains open LUKS encryption for an erase file system signatures only step";
        return false;
    }

    bool overall_success = false;
    qDebug() << __FUNCTION__ << QString("clear old file system signatures in %1").arg(partition.get_path());

    //Get device, disk & partition and open the device.  Allocate buffer and fill with
    //  zeros.  Buffer size is the greater of 4 KiB and the sector size.
    PedDevice *lp_device = NULL;
    PedDisk *lp_disk = NULL;
    PedPartition *lp_partition = NULL;
    bool device_is_open = false;
    Byte_Value bufsize = 4LL * KIBIBYTE;
    char *buf = NULL;
    if (get_device(partition.device_path, lp_device)) {
        if (partition.type == TYPE_UNPARTITIONED) {
            // Virtual partition spanning whole disk device
            overall_success = true;
        } else if (get_disk(lp_device, lp_disk)) {
            // Partitioned device
            lp_partition = ped_disk_get_partition_by_sector(lp_disk, partition.get_sector());
            overall_success = (lp_partition != NULL);
        }

        if (overall_success && ped_device_open(lp_device)) {
            device_is_open = true;

            bufsize = std::max(bufsize, lp_device->sector_size);
            buf = static_cast<char *>(malloc(bufsize));
            if (buf)
                memset(buf, 0, bufsize);
        }
        overall_success &= device_is_open;
    }
    struct {
        Byte_Value offset; //Negative offsets work backwards from the end of the partition
        Byte_Value rounding; //Minimum desired rounding for offset
        Byte_Value length;
    } ranges[] = {
        //offset           , rounding       , length
        {0LL, 1LL, 512LL * KIBIBYTE}, // All primary super blocks
        {64LL * MEBIBYTE, 1LL, 4LL * KIBIBYTE}, // Btrfs super block mirror copy
        {256LL * GIBIBYTE, 1LL, 4LL * KIBIBYTE}, // Btrfs super block mirror copy
        {1LL * PEBIBYTE, 1LL, 4LL * KIBIBYTE}, // Btrfs super block mirror copy
        {-512LL * KIBIBYTE, 256LL * KIBIBYTE, 512LL * KIBIBYTE}, // ZFS labels L2 and L3
        {-64LL * KIBIBYTE, 64LL * KIBIBYTE, 4LL * KIBIBYTE}, // SWRaid metadata 0.90 super block
        {-8LL * KIBIBYTE, 4LL * KIBIBYTE, 8LL * KIBIBYTE} // @-8K SWRaid metadata 1.0 super block
        // and @-4K Nilfs2 secondary super block
    };
    for (unsigned int i = 0; overall_success && i < sizeof(ranges) / sizeof(ranges[0]); i++) {
        //Rounding is performed in multiples of the sector size because writes are in whole sectors.

        Byte_Value rounding_size = Utils::ceil_size(ranges[i].rounding, lp_device->sector_size);
        Byte_Value byte_offset;
        Byte_Value byte_len;
        if (ranges[i].offset >= 0LL) {
            byte_offset = Utils::floor_size(ranges[i].offset, rounding_size);
            byte_len = Utils::ceil_size(ranges[i].offset + ranges[i].length, lp_device->sector_size) - byte_offset;
        } else { //Negative offsets
            Byte_Value notional_offset = Utils::floor_size(partition.get_byte_length() + ranges[i].offset, ranges[i].rounding);
            byte_offset = Utils::floor_size(notional_offset, rounding_size);
            byte_len = Utils::ceil_size(notional_offset + ranges[i].length, lp_device->sector_size) - byte_offset;
        }
        //Limit range to partition size.
        if (byte_offset + byte_len <= 0LL) {
            //Byte range entirely before partition start.  Programmer error!
            continue;
        } else if (byte_offset < 0) {
            //Byte range spans partition start.  Trim to fit.
            byte_len += byte_offset;
            byte_offset = 0LL;
        }
        if (byte_offset >= partition.get_byte_length()) {
            //Byte range entirely after partition end.  Ignore.
            continue;
        } else if (byte_offset + byte_len > partition.get_byte_length()) {
            //Byte range spans partition end.  Trim to fit.
            byte_len = partition.get_byte_length() - byte_offset;
        }

        Byte_Value written = 0LL;
        bool zero_success = false;
        if (device_is_open && buf) {
            // Start sector of the whole disk device or the partition
            Sector ptn_start = 0LL;
            if (lp_partition)
                ptn_start = lp_partition->geom.start;

            while (written < byte_len) {
                //Write in bufsize amounts.  Last write may be smaller but
                //  will still be a whole number of sectors.
                Byte_Value amount = std::min(bufsize, byte_len - written);
                zero_success = ped_device_write(lp_device, buf,
                                                ptn_start + (byte_offset + written) / lp_device->sector_size,
                                                amount / lp_device->sector_size);
                if (!zero_success)
                    break;
                written += amount;
            }
        }
        overall_success &= zero_success;
    }
    if (buf)
        free(buf);

    if (overall_success) {
        bool flush_success = false;
        if (device_is_open) {
            flush_success = ped_device_sync(lp_device);
            ped_device_close(lp_device);
            settle_device(SETTLE_DEVICE_APPLY_MAX_WAIT_SECONDS);
        }
        overall_success &= flush_success;
    }
    destroy_device_and_disk(lp_device, lp_disk);
    return overall_success;
}

bool PartedCore::set_partition_type(const Partition &partition)
{
    if (partition.type == TYPE_UNPARTITIONED)
        // Trying to set the type of a partition on a non-partitioned whole disk
        // device is a successful non-operation.
        return true;
    qDebug() << __FUNCTION__ << QString("set partition type on %1").arg(partition.get_path());
    //Set partition type appropriately for the type of file system stored in the partition.
    //  Libparted treats every type as a file system, except LVM which it treats as a flag.

    bool return_value = false;

    PedDevice *lp_device = NULL;
    PedDisk *lp_disk = NULL;
    if (get_device_and_disk(partition.device_path, lp_device, lp_disk)) {
        PedPartition *lp_partition = ped_disk_get_partition_by_sector(lp_disk, partition.get_sector());
        if (lp_partition) {
            QString fs_type = Utils::FSTypeToString(partition.fstype);

            // Lookup libparted file system type using GParted's name, as most
            // match.  Exclude cleared as the name won't be recognised by
            // libparted and get_filesystem_string() has also translated it.
            PedFileSystemType *lp_fs_type = NULL;
            if (partition.fstype != FS_CLEARED)
                lp_fs_type = ped_file_system_type_get(fs_type.toLatin1());

            // If not found, and FS is udf, then try ntfs.
            // Actually MBR 07 IFS (Microsoft Installable File System) or
            // GPT BDP (Windows Basic Data Partition).
            // Ref: https://serverfault.com/a/829172
            if (!lp_fs_type && partition.fstype == FS_UDF)
                lp_fs_type = ped_file_system_type_get("ntfs");

            // default is Linux (83)
            if (!lp_fs_type)
                lp_fs_type = ped_file_system_type_get("ext2");

            bool supports_lvm_flag = ped_partition_is_flag_available(lp_partition, PED_PARTITION_LVM);

            if (lp_fs_type && partition.fstype != FS_LVM2_PV) {
                // Also clear any libparted LVM flag so that it doesn't
                // override the file system type
                if ((!supports_lvm_flag || ped_partition_set_flag(lp_partition, PED_PARTITION_LVM, 0)) && ped_partition_set_system(lp_partition, lp_fs_type) && commit(lp_disk)) {
                    qDebug() << __FUNCTION__ << QString("new partition type: %1").arg(lp_partition->fs_type->name);
                    return_value = true;
                }
            } else if (partition.fstype == FS_LVM2_PV) {
                if (supports_lvm_flag && ped_partition_set_flag(lp_partition, PED_PARTITION_LVM, 1) && commit(lp_disk)) {
                    return_value = true;
                } else if (!supports_lvm_flag) {
                    // Skip setting the lvm flag because the partition
                    // table type doesn't support it.  Applies to dvh
                    // and pc98 disk labels.
                    return_value = true;
                }
            }
        }

        destroy_device_and_disk(lp_device, lp_disk);
    }
    return return_value;
}

bool PartedCore::create_filesystem(const Partition &partition)
{
    if (partition.fstype == FS_LUKS && partition.busy) {
        qDebug() << __FUNCTION__ << QString("partition contains open LUKS encryption for a create file system only step");
        return false;
    }
    qDebug() << __FUNCTION__ << QString("create new %1 file system").arg(partition.fstype);
    bool succes = false;
    FileSystem *p_filesystem = NULL;
    switch (get_fs(partition.fstype).create) {
    case FS::NONE:
        break;
    case FS::GPARTED:
        break;
    case FS::LIBPARTED:
        break;
    case FS::EXTERNAL: {
        succes = (p_filesystem = get_filesystem_object(partition.fstype)) && p_filesystem->create(partition);
        if (succes && !partition.get_filesystem_label().isEmpty()) {
            p_filesystem->write_label(partition);
        }
    }
    break;
    default:
        break;
    }
    return succes;
}

bool PartedCore::formatpartition(const Partition &partition)
{
    bool bsuccess = false;
    if (partition.fstype == FS_LUKS && partition.busy) {
        qDebug() << __FUNCTION__ << QString("partition contains open LUKS encryption for a format files system only step");
        return false;
    }

    if (partition.fstype == FS_CLEARED)
        bsuccess = erase_filesystem_signatures(partition)
                   && set_partition_type(partition);
    else
        bsuccess = erase_filesystem_signatures(partition)
                   && set_partition_type(partition)
                   && create_filesystem(partition);

    return bsuccess;
}

bool PartedCore::resize(const Partition &partition_new)
{
    //ToDo fs linux-swap
    if (partition_new.fstype == FS_LINUX_SWAP) {
        // linux-swap is recreated, not resized
        //        return    resize_move_partition(partition_old, partition_new, operationdetail, true)
        //                  && recreate_linux_swap_filesystem(partition_new, operationdetail);
    }
    Sector delta = partition_new.get_sector_length() - curpartition.get_sector_length();
    if (delta < 0LL) { // shrink
        //        return    check_repair_filesystem(partition_new)
        //                  && shrink_filesystem(curpartition, partition_new)
        //                  && resize_move_partition(curpartition, partition_new, false);
    } else if (delta > 0LL) { // grow
        return check_repair_filesystem(partition_new)
               && resize_move_partition(curpartition, partition_new, true)
               && maximize_filesystem(partition_new);
    }
    return true;
}

bool PartedCore::check_repair_filesystem(const Partition &partition)
{
    if (partition.fstype == FS_LUKS && partition.busy) {
        qDebug() << "partition contains open LUKS encryption for a check file system only step";
        return false;
    }

    if (partition.busy)
        // Trying to check an online file system is a successful non-operation.
        return true;
    qDebug() << QString("PartedCore::check_repair_filesystem:check file system on %1"
                        " for errors and (if possible) fix them")
             .arg(partition.get_path());

    bool succes = false;
    FileSystem *p_filesystem = NULL;
    switch (get_fs(partition.fstype).check) {
    case FS::NONE:
        qDebug() << "PartedCore::check_repair_filesystem ,checking is not available for this file system";
        break;
    case FS::GPARTED:
        break;
    case FS::LIBPARTED:
        break;
    case FS::EXTERNAL:
        succes = (p_filesystem = get_filesystem_object(partition.fstype)) && p_filesystem->check_repair(partition);
        break;
    default:
        break;
    }

    return succes;
}

bool PartedCore::resize_move_partition(const Partition &partition_old, const Partition &partition_new, bool rollback_on_fail)
{
    Action action = NONE;
    if (partition_new.get_sector_length() > partition_old.get_sector_length())
        action = GROW;
    else if (partition_new.get_sector_length() < partition_old.get_sector_length())
        action = SHRINK;

    if (partition_new.sector_start > partition_old.sector_start && partition_new.sector_end > partition_old.sector_end)
        action = action == GROW ? MOVE_RIGHT_GROW : action == SHRINK ? MOVE_RIGHT_SHRINK : MOVE_RIGHT;
    else if (partition_new.sector_start < partition_old.sector_start && partition_new.sector_end < partition_old.sector_end)
        action = action == GROW ? MOVE_LEFT_GROW : action == SHRINK ? MOVE_LEFT_SHRINK : MOVE_LEFT;

    Sector new_start = -1;
    Sector new_end = -1;
    bool bsuccess = resize_move_partition_implement(partition_old, partition_new, new_start, new_end);
    if (!bsuccess && rollback_on_fail) {
        Partition *partition_intersection = partition_new.clone();
        partition_intersection->sector_start = std::max(partition_old.sector_start,
                                                        partition_new.sector_start);
        partition_intersection->sector_end = std::min(partition_old.sector_end,
                                                      partition_new.sector_end);

        Partition *partition_restore = partition_old.clone();
        // Ensure that old partition boundaries are not modified
        partition_restore->alignment = ALIGN_STRICT;

        resize_move_partition_implement(*partition_intersection, *partition_restore, new_start, new_end);
        delete partition_restore;
        partition_restore = NULL;
        delete partition_intersection;
        partition_intersection = NULL;
    }

    return bsuccess;
}

bool PartedCore::resize_move_partition_implement(const Partition &partition_old, const Partition &partition_new, Sector &new_start, Sector &new_end)
{
    bool success = false;
    PedDevice *lp_device = NULL;
    PedDisk *lp_disk = NULL;
    if (get_device_and_disk(partition_old.device_path, lp_device, lp_disk)) {
        PedPartition *lp_partition = get_lp_partition(lp_disk, partition_old);
        if (lp_partition) {
            PedConstraint *constraint = NULL;
            if (partition_new.alignment == ALIGN_STRICT || partition_new.alignment == ALIGN_MEBIBYTE || partition_new.strict_start) {
                PedGeometry *geom = ped_geometry_new(lp_device,
                                                     partition_new.sector_start,
                                                     partition_new.get_sector_length());
                if (geom) {
                    constraint = ped_constraint_exact(geom);
                    ped_geometry_destroy(geom);
                }
            } else {
                constraint = ped_constraint_any(lp_device);
            }

            if (constraint) {
                if (ped_disk_set_partition_geom(lp_disk,
                                                lp_partition,
                                                constraint,
                                                partition_new.sector_start,
                                                partition_new.sector_end)) {
                    new_start = lp_partition->geom.start;
                    new_end = lp_partition->geom.end;

                    success = commit(lp_disk);
                }

                ped_constraint_destroy(constraint);
            }
        }
        destroy_device_and_disk(lp_device, lp_disk);
    }

    return success;
}

bool PartedCore::maximize_filesystem(const Partition &partition)
{
    if (partition.fstype == FS_LUKS && partition.busy) {
        qDebug() << "PartedCore::maximize_filesystem: partition contains open LUKS encryption for a maximize file system only step";
        return false;
    }
    qDebug() << "PartedCore::maximize_filesystem: grow file system to fill the partition";

    // Checking if growing is available or allowed is only relevant for the check
    // repair operation to inform the user why the grow step is being skipped.  For a
    // resize/move operation these growing checks are merely retesting those performed
    // to allow the operation to be queued in the first place.  See
    // Win_GParted::set_valid_operations() and
    // Dialog_Partition_Resize_Move::Resize_Move_Normal().
    if (get_fs(partition.fstype).grow == FS::NONE) {
        qDebug() << "PartedCore::maximize_filesystem:growing is not available for this file system";
        return true;
    }
    bool success = resize_filesystem_implement(partition, partition);

    return success;
}

bool PartedCore::resize_filesystem_implement(const Partition &partition_old, const Partition &partition_new)
{
    bool fill_partition = false;
    const FS &fs_cap = get_fs(partition_new.fstype);
    FS::Support action = FS::NONE;
    if (partition_new.get_sector_length() >= partition_old.get_sector_length()) {
        // grow (always maximises the file system to fill the partition)
        fill_partition = true;
        action = (partition_old.busy) ? fs_cap.online_grow : fs_cap.grow;
    } else {
        // shrink
        fill_partition = false;
        action = (partition_old.busy) ? fs_cap.online_shrink : fs_cap.shrink;
    }
    bool success = false;
    FileSystem *p_filesystem = NULL;
    switch (action) {
    case FS::NONE:
        break;
    case FS::GPARTED:
        break;
    case FS::LIBPARTED:
        success = resize_move_filesystem_using_libparted(partition_old, partition_new);
        break;
    case FS::EXTERNAL:
        success = (p_filesystem = get_filesystem_object(partition_new.fstype)) && p_filesystem->resize(partition_new, fill_partition);
        break;
    default:
        break;
    }

    return success;
}

bool PartedCore::resize_move_filesystem_using_libparted(const Partition &partition_old, const Partition &partition_new)
{
    bool return_value = false;
    PedDevice *lp_device = NULL;
    PedDisk *lp_disk = NULL;
    if (get_device_and_disk(partition_old.device_path, lp_device, lp_disk)) {
        PedFileSystem *fs = NULL;
        PedGeometry *lp_geom = NULL;

        lp_geom = ped_geometry_new(lp_device,
                                   partition_old.sector_start,
                                   partition_old.get_sector_length());
        if (lp_geom) {
            fs = ped_file_system_open(lp_geom);

            ped_geometry_destroy(lp_geom);
            lp_geom = NULL;

            if (fs) {
                lp_geom = ped_geometry_new(lp_device,
                                           partition_new.sector_start,
                                           partition_new.get_sector_length());
                if (lp_geom) {
                    if (return_value)
                        commit(lp_disk);

                    ped_geometry_destroy(lp_geom);
                }
                ped_file_system_close(fs);
            }
        }
        destroy_device_and_disk(lp_device, lp_disk);
    }

    return return_value;
}

void PartedCore::slotRefreshDeviceInfo()
{
    probedeviceinfo();
    emit sigUpdateDeviceInfo(inforesult);
}

bool PartedCore::mount(const QString &mountpath)
{
    bool success = true;
    QString output, errstr;
    QString cmd ;
    FSType type = curpartition.fstype;
    QString partitionpath = curpartition.get_path();
    if (type == FS_FAT32 ||
            type == FS_FAT16) {
        cmd = QString("mount -v %1 %2 -o -o dmask=000,fmask=111").arg(partitionpath).arg(mountpath);
    } else if (type == FS_HFS) {
        cmd = QString("mount -v %1 %2 -o -o dir_umask=000,file_umask=111").arg(partitionpath).arg(mountpath);
    } else {
        cmd = QString("mount -v %1 %2").arg(partitionpath).arg(mountpath);
    }
    int exitcode = Utils::executcmd(cmd, output, errstr);
    if (exitcode != 0) {
        // QString type = Utils::get_filesystem_kernel_name(curpartition.fstype);
        // cmd = QString("mount -v -t %1 %2 %3").arg("").arg(partitionpath).arg(mountpath);
        success = false;
    }
    emit sigRefreshDeviceInfo();
    return success;
}

bool PartedCore::unmount()
{
    QString output, errstr;
    bool bsuccess = true;
    QVector<QString> mountpoints = curpartition.get_mountpoints();
    for (QString path : mountpoints) {
        QString cmd = QString("umount -v %1").arg(path);
        int exitcode = Utils::executcmd(cmd, output, errstr);
        if (0 != exitcode)
            bsuccess = false;
    }
    emit sigRefreshDeviceInfo();
    return bsuccess;
}

bool PartedCore::create(const PartitionVec &infovec)
{
    bool bsuccess = true;
    for (PartitionInfo info : infovec) {
        Partition new_partition;
        new_partition.Reset(info);
        if (!create(new_partition)) {
            bsuccess = false;
            break;
        }
    }
    emit sigRefreshDeviceInfo();
    return bsuccess;
}

bool PartedCore::create(Partition &new_partition)
{
    bool bsuccess = false;
    if (new_partition.type == TYPE_EXTENDED) {
        bsuccess = create_partition(new_partition);
    } else {
        FS_Limits fs_limits = get_filesystem_limits(new_partition.fstype, new_partition);
        bsuccess = create_partition(new_partition, fs_limits.min_size / new_partition.sector_size);
    }
    if (!bsuccess)
        return false;

    if (!new_partition.name.isEmpty()) {
        if (!name_partition(new_partition))
            return false;
    }

    if (new_partition.type == TYPE_EXTENDED || new_partition.fstype == FS_UNFORMATTED)
        return true;
    else if (new_partition.fstype == FS_CLEARED)
        return erase_filesystem_signatures(new_partition);
    else
        return erase_filesystem_signatures(new_partition)
               && set_partition_type(new_partition)
               && create_filesystem(new_partition);

    return false;
}

bool PartedCore::create_partition(Partition &new_partition, Sector min_size)
{
    qDebug() << __FUNCTION__ << "create empty partition";
    new_partition.partition_number = 0;
    PedDevice *lp_device = NULL;
    PedDisk *lp_disk = NULL;
    if (get_device_and_disk(new_partition.device_path, lp_device, lp_disk)) {
        PedPartitionType type;
        PedConstraint *constraint = NULL;
        PedFileSystemType *fs_type = NULL;
        //create new partition
        switch (new_partition.type) {
        case TYPE_PRIMARY:
            type = PED_PARTITION_NORMAL;
            break;
        case TYPE_LOGICAL:
            type = PED_PARTITION_LOGICAL;
            break;
        case TYPE_EXTENDED:
            type = PED_PARTITION_EXTENDED;
            break;

        default:
            type = PED_PARTITION_FREESPACE;
        }
        if (new_partition.type != TYPE_EXTENDED)
            fs_type = ped_file_system_type_get("ext2");

        PedPartition *lp_partition = ped_partition_new(lp_disk, type, fs_type,
                                                       new_partition.sector_start, new_partition.sector_end);

        if (lp_partition) {
            if (new_partition.alignment == ALIGN_STRICT
                    || new_partition.alignment == ALIGN_MEBIBYTE) {
                PedGeometry *geom = ped_geometry_new(lp_device, new_partition.sector_start, new_partition.get_sector_length());
                if (geom) {
                    constraint = ped_constraint_exact(geom);
                    ped_geometry_destroy(geom);
                }
            } else
                constraint = ped_constraint_any(lp_device);

            if (constraint) {
                if (min_size > 0 && new_partition.fstype != FS_XFS) // Permit copying to smaller xfs partition
                    constraint->min_size = min_size;

                if (ped_disk_add_partition(lp_disk, lp_partition, constraint) && commit(lp_disk)) {
                    new_partition.set_path(get_partition_path(lp_partition));

                    new_partition.partition_number = lp_partition->num;
                    new_partition.sector_start = lp_partition->geom.start;
                    new_partition.sector_end = lp_partition->geom.end;
                }
                ped_constraint_destroy(constraint);
            }
        }
        destroy_device_and_disk(lp_device, lp_disk);
    }
    bool bsucces = new_partition.partition_number > 0;

    return bsucces;
}

bool PartedCore::format(const QString &fstype, const QString &name)
{
    Partition part = curpartition;
    part.fstype = Utils::StringToFSType(fstype);
    part.set_filesystem_label(name);
    bool bsuccess = formatpartition(part);
    emit sigRefreshDeviceInfo();
    return bsuccess;
}

bool PartedCore::resize(const PartitionInfo &info)
{
    Partition new_partition = curpartition;
    new_partition.Reset(info);
    bool bsuccess = resize(new_partition);
    emit sigRefreshDeviceInfo();
    return bsuccess;
}

QStringList PartedCore::getallsupportfs()
{
    return supported_filesystems->get_all_fsname();
}

HardDiskInfo PartedCore::getDeviceHardInfo(const QString &devicepath)
{
    qDebug() << __FUNCTION__ << "get Device Hard Info Start";
    HardDiskInfo hdinfo;
    //QString devicepath = curpartition.device_path;
    if(devicepath.isEmpty()) {
        qDebug() << "disk path is empty";
        return hdinfo;
    }
    QString cmd = QString("smartctl -i %1").arg(devicepath);
    FILE *fd = nullptr;
    fd = popen(cmd.toStdString().data(), "r");
    char pb[1024];
    memset(pb, 0, 1024);
    if(fd == nullptr) {
        qDebug() << "exeuted cmd failed";
        return hdinfo;
    }
    while(fgets(pb, 1024, fd) != nullptr) {
        if(strstr(pb, "Device Model:") != nullptr) {
           hdinfo.m_deviceModel += pb+(sizeof("Device Model:")-1);
           hdinfo.m_deviceModel.remove(QRegExp("^ +\\s*"));
           hdinfo.m_deviceModel = hdinfo.m_deviceModel.replace(QRegExp("\\\n"), "\0");
           continue;
        }
        if(strstr(pb, "Serial Number:") != nullptr) {
           hdinfo.m_serialNumber += pb+(sizeof("Serial Number:")-1);
           //去除空格
           hdinfo.m_serialNumber.remove(QRegExp("^ +\\s*"));
           //替换换行符
           hdinfo.m_serialNumber = hdinfo.m_serialNumber.replace(QRegExp("\\\n"), "\0");
           continue;
        }
        if(strstr(pb, "LU WWN Device Id:") != nullptr) {
           hdinfo.m_deviceId += pb+(sizeof("LU WWN Device Id:")-1);
           //去除空格
           hdinfo.m_deviceId.remove(QRegExp("^ +\\s*"));
           //替换换行符
           hdinfo.m_deviceId = hdinfo.m_deviceId.replace(QRegExp("\\\n"), "\0");
           continue;
        }
        if(strstr(pb, "Firmware Version:") != nullptr) {
           hdinfo.m_firmwareVersion += pb+(sizeof("Firmware Version:")-1);
           //去除空格
           hdinfo.m_firmwareVersion.remove(QRegExp("^ +\\s*"));
           //替换换行符
           hdinfo.m_firmwareVersion = hdinfo.m_firmwareVersion.replace(QRegExp("\\\n"), "\0");
           continue;
        }
        if(strstr(pb, "User Capacity:") != nullptr) {
           hdinfo.m_userCapacity += pb+(sizeof("User Capacity:")-1);
           //去除空格
           hdinfo.m_userCapacity.remove(QRegExp("^ +\\s*"));
           //替换换行符
           hdinfo.m_userCapacity = hdinfo.m_userCapacity.replace(QRegExp("\\\n"), "\0");
           continue;
        }
        if(strstr(pb, "Sector Size:") != nullptr) {
           hdinfo.m_sectorSize += pb+(sizeof("Sector Size:")-1);
           //去除空格
           hdinfo.m_sectorSize.remove(QRegExp("^ +\\s*"));
           //替换换行符
           hdinfo.m_sectorSize = hdinfo.m_sectorSize.replace(QRegExp("\\\n"), "\0");
           continue;
        }
        if(strstr(pb, "Rotation Rate:") != nullptr) {
           hdinfo.m_rotationRate += pb+(sizeof("Rotation Rate:")-1);
           //去除空格
           hdinfo.m_rotationRate.remove(QRegExp("^ +\\s*"));
           //替换换行符
           hdinfo.m_rotationRate = hdinfo.m_rotationRate.replace(QRegExp("\\\n"), "\0");
           continue;
        }
        if(strstr(pb, "Form Factor:") != nullptr) {
           hdinfo.m_formFactor += pb+(sizeof("Form Factor:")-1);
           //去除空格
           hdinfo.m_formFactor.remove(QRegExp("^ +\\s*"));
           //替换换行符
           hdinfo.m_formFactor = hdinfo.m_formFactor.replace(QRegExp("\\\n"), "\0");
           continue;
        }
        if(strstr(pb, "Device is:") != nullptr) {
           hdinfo.m_deviceis += pb+(sizeof("Device is:")-1);
           //去除空格
           hdinfo.m_deviceis.remove(QRegExp("^ +\\s*"));
           //替换换行符
           hdinfo.m_deviceis = hdinfo.m_deviceis.replace(QRegExp("\\\n"), "\0");
           continue;
        }
        if(strstr(pb, "ATA Version is:") != nullptr) {
           if(strstr(pb, "SATA Version is:") != nullptr) {
               hdinfo.m_sataVersionIs += pb+(sizeof("SATA Version is:")-1);
               //去除空格
               hdinfo.m_sataVersionIs.remove(QRegExp("^ +\\s*"));
               //替换换行符
               hdinfo.m_sataVersionIs = hdinfo.m_sataVersionIs.replace(QRegExp("\\\n"), "\0");
           }
           else {
               hdinfo.m_ataVersionIs += pb+(sizeof("ATA Version is:")-1);
               //去除空格
               hdinfo.m_ataVersionIs.remove(QRegExp("^ +\\s*"));
               //替换换行符
               hdinfo.m_ataVersionIs = hdinfo.m_ataVersionIs.replace(QRegExp("\\\n"), "\0");
           }
           continue;
        }
        if(strstr(pb, "Local Time is:") != nullptr) {
           hdinfo.m_localTime += pb+(sizeof("Local Time is:")-1);
           //去除空格
           hdinfo.m_localTime.remove(QRegExp("^ +\\s*"));
           //替换换行符
           hdinfo.m_localTime = hdinfo.m_localTime.replace(QRegExp("\\\n"), "\0");
           continue;
        }
        if(strstr(pb, "SMART support is:") != nullptr) {
           if(strstr(pb, "Enabled") != nullptr || strstr(pb, "Disabled") != nullptr) {
               hdinfo.m_smartSupportOn_Off += pb+(sizeof("SMART support is:")-1);
               //去除空格
               hdinfo.m_smartSupportOn_Off.remove(QRegExp("^ +\\s*"));
               //替换换行符
               hdinfo.m_smartSupportOn_Off = hdinfo.m_smartSupportOn_Off.replace(QRegExp("\\\n"), "\0");
           }
           else {
               hdinfo.m_smartSupport += pb+(sizeof("SMART support is:")-1);
               //去除空格
               hdinfo.m_smartSupport.remove(QRegExp("^ +\\s*"));
               //替换换行符
               hdinfo.m_smartSupport = hdinfo.m_smartSupport.replace(QRegExp("\\\n"), "\0");
           }
           continue;
        }

    }
    pclose(fd);
    qDebug() << __FUNCTION__ << "get Device Hard Info End";
    return hdinfo;
}

DeviceInfo PartedCore::getDeviceinfo()
{
    DeviceInfo info;
    qDebug() << __FUNCTION__ << "#########";
    qDebug() << info.m_path << info.heads << info.cylinders << info.serial_number << info.max_prims;
    return info;
}

DeviceInfoMap PartedCore::getAllDeviceinfo()
{
    return inforesult;
}

void PartedCore::setCurSelect(const PartitionInfo &info)
{
    bool bfind = false;
    for (auto it = devicemap.begin(); it != devicemap.end() && !bfind; it++) {
        if (it.key() == info.device_path) {
            Device dev = it.value();
            for (auto itpart = dev.partitions.begin(); itpart != dev.partitions.end() && !bfind; itpart++) {
                Partition part = *(*itpart);
                if (infoBelongToPartition(part, info)) {
                    curpartition = part;
                    bfind = true;
                }
                if (part.inside_extended && !bfind) {
                    for (auto itextend = part.logicals.begin(); itextend != part.logicals.end(); itextend++) {
                        Partition partlogical = *(*itextend);
                        if (infoBelongToPartition(partlogical, info)) {
                            curpartition = partlogical;
                            bfind = true;
                        }
                    }
                }
            }
        }
    }
}

QString PartedCore::getDeviceHardStatus(const QString &devicepath)
{
    qDebug() << __FUNCTION__ << "get Device Hard Status Start";
    QString status;
    //QString devicepath = curpartition.device_path;
    if(devicepath.isEmpty()) {
        qDebug() << "disk path is empty";
        return status;
    }
    QString cmd = QString("smartctl -H %1").arg(devicepath);
    FILE *fd = nullptr;
    fd = popen(cmd.toStdString().data(), "r");
    char pb[1024];
    memset(pb, 0, 1024);
    if(fd == nullptr) {
        qDebug() << "exeuted cmd failed";
        return status;
    }
    while(fgets(pb, 1024, fd) != nullptr) {
        if(strstr(pb, "SMART overall-health self-assessment test result:") != nullptr)
        {
            status += pb+(sizeof("SMART overall-health self-assessment test result:")-1);
            //去除空格
            status.remove(QRegExp("^ +\\s*"));
            //替换换行符
            status = status.replace(QRegExp("\\\n"), "\0");
            break;
        }
    }
    qDebug() << __FUNCTION__ << "get Device Hard Status End";
    return status;
}

HardDiskStatusInfoList PartedCore::getDeviceHardStatusInfo(const QString &devicepath)
{
    qDebug() << __FUNCTION__ << "getDeviceHardStatusInfo Start";
    HardDiskStatusInfoList hdsilist;
    //QString devicepath = curpartition.device_path;
    if(devicepath.isEmpty()) {
        qDebug() << "disk path is empty";
        return hdsilist;
    }
    QString cmd = QString("smartctl -A %1").arg(devicepath);
    FILE *fd = nullptr;
    fd = popen(cmd.toStdString().data(), "r");
    char pb[1024];
    memset(pb, 0, 1024);
    if(fd == nullptr) {
        qDebug() << "exeuted cmd failed";
        return hdsilist;
    }
    int i = 0;
    while(fgets(pb, 1024, fd) != nullptr) {
        if(strstr(pb, "ID# ATTRIBUTE_NAME          FLAG     VALUE WORST THRESH TYPE      UPDATED  WHEN_FAILED RAW_VALUE") != nullptr)
        {
            break;
        }
        i++;
    }
    pclose(fd);
    fd = popen(cmd.toStdString().data(), "r");
    if(fd == nullptr) {
        qDebug() << "exeuted cmd failed";
        return hdsilist;
    }
    int j = 0;
    while(fgets(pb, 1024, fd) != nullptr) {
        if(j > i)
        {
            QString printbuf;
            printbuf.clear();
            HardDiskStatusInfo hdsinfo;
            printbuf += pb;
            //替换换行符
            printbuf = printbuf.replace(QRegExp("\\\n"), "\0");
            QStringList list = printbuf.split(' ');
            list.removeAll("");
            if(list.size() != 10)
            {
                qDebug() << "get DeviceHardStatus Info size error";
                break;
            }
            hdsinfo.m_id = list.at(0);
            hdsinfo.m_attributeName = list.at(1);
            hdsinfo.m_flag = list.at(2);
            hdsinfo.m_value = list.at(3);
            hdsinfo.m_worst = list.at(4);
            hdsinfo.m_thresh = list.at(5);
            hdsinfo.m_type = list.at(6);
            hdsinfo.m_updated = list.at(7);
            hdsinfo.m_whenFailed = list.at(8);
            hdsinfo.m_rawValue = list.at(9);
            hdsilist.append(hdsinfo);
        }
        j++;
    }
    pclose(fd);
    qDebug() << __FUNCTION__ << "getDeviceHardStatusInfo End";
    return hdsilist;
}

void PartedCore::test()
{
    QString devicepath = curpartition.device_path;
    qDebug() << devicepath;
}
} // namespace DiskManager
