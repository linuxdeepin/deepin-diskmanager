#include "lvmstruct.h"
#include <QDebug>
/*********************************** PVData *********************************************/
PVData::PVData()
{
    m_pvAct = LVMAction::LVM_Act_Unknow;
    m_type = LVMDevType::LVM_Dev_Unknow_Devices;
}

QDBusArgument &operator<<(QDBusArgument &argument, const PVData &data)
{
    argument.beginStructure();
    argument << data.m_devicePath
             << data.m_startSector
             << data.m_endSector
             << static_cast<int>(data.m_pvAct)
             << static_cast<int>(data.m_type);
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, PVData &data)
{
    argument.beginStructure();
    int pvAct, type;
    argument >> data.m_devicePath
             >> data.m_startSector
             >> data.m_endSector
             >> pvAct
             >> type;
    data.m_pvAct = static_cast<LVMAction>(pvAct);
    data.m_type = static_cast<LVMDevType>(type);
    argument.endStructure();
    return argument;
}

/*********************************** LVDATA *********************************************/
LVDATA::LVDATA()
{
    m_lvByteSize = 0;
}

QDBusArgument &operator<<(QDBusArgument &argument, const LVData &data)
{
    argument.beginStructure();
    argument << data.m_lvName
             << data.m_lvPath
             << data.m_lvSize
             << data.m_lvByteSize;
    argument.endStructure();

    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, LVData &data)
{
    argument.beginStructure();
    argument >> data.m_lvName
             >> data.m_lvPath
             >> data.m_lvSize
             >> data.m_lvByteSize;
    argument.endStructure();
    return argument;
}

/*********************************** VGDATA *********************************************/
VGDATA::VGDATA()
{
    m_vgByteSize = 0;
}

QDBusArgument &operator<<(QDBusArgument &argument, const VGData &data)
{
    argument.beginStructure();
    argument << data.m_vgName
             << data.m_vgSize
             << data.m_vgUuid
             << data.m_vgByteSize
             << data.m_lvList;
    argument.endStructure();
    return argument;
}


const QDBusArgument &operator>>(const QDBusArgument &argument, VGData &data)
{
    argument.beginStructure();
    argument >> data.m_vgName
             >> data.m_vgSize
             >> data.m_vgUuid
             >> data.m_vgByteSize
             >> data.m_lvList;
    argument.endStructure();
    return argument;
}

/*********************************** PVRANGES *********************************************/
PVRANGES::PVRANGES()
{
    m_start = 0;
    m_end = 0;
    m_used = false;
}

QDBusArgument &operator<<(QDBusArgument &argument, const PVRanges &data)
{
    argument.beginStructure();
    argument << data.m_lvName
             << data.m_devPath
             << data.m_vgName
             << data.m_vgUuid
             << data.m_start
             << data.m_end
             << data.m_used;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, PVRanges &data)
{
    argument.beginStructure();
    argument >> data.m_lvName
             >> data.m_devPath
             >> data.m_vgName
             >> data.m_vgUuid
             >> data.m_start
             >> data.m_end
             >> data.m_used;
    argument.endStructure();
    return argument;
}



/*********************************** PVInfo *********************************************/
PVInfo::PVInfo()
{
    m_pvMdaSize = 0;
    m_pvMdaCount = 0 ;
    m_pvUsedPE = 0;
    m_pvUnusedPE = 0;
    m_PESize = 0;
    m_pvError = LVMError::LVM_Err_Normal;
    m_lvmDevType = LVMDevType::LVM_Dev_Unknow_Devices;
}


QDBusArgument &operator<<(QDBusArgument &argument, const PVInfo &data)
{
    argument.beginStructure();
    argument << data.m_pvFmt
             << data.m_vgName
             << data.m_pvPath
             << data.m_pvUuid
             << data.m_vgUuid
             << data.m_pvMdaSize
             << data.m_pvMdaCount
             << data.m_pvSize
             << data.m_pvFree
             << data.m_pvUsedPE
             << data.m_pvUnusedPE
             << data.m_PESize
             << data.m_pvStatus
             << static_cast<int>(data.m_pvError)
             << data.m_lvRangesList
             << data.m_vgRangesList
             << static_cast<int>(data.m_lvmDevType);
    argument.endStructure();
    return argument;
}


const QDBusArgument &operator>>(const QDBusArgument &argument,  PVInfo &data)
{
    argument.beginStructure();
    int err, devType;
    argument >> data.m_pvFmt
             >> data.m_vgName
             >> data.m_pvPath
             >> data.m_pvUuid
             >> data.m_vgUuid
             >> data.m_pvMdaSize
             >> data.m_pvMdaCount
             >> data.m_pvSize
             >> data.m_pvFree
             >> data.m_pvUsedPE
             >> data.m_pvUnusedPE
             >> data.m_PESize
             >> data.m_pvStatus
             >> err
             >> data.m_lvRangesList
             >> data.m_vgRangesList
             >> devType;
    data.m_pvError = static_cast<LVMError>(err);
    data.m_lvmDevType = static_cast<LVMDevType>(devType);
    argument.endStructure();
    return argument;
}



/*********************************** LVInfo *********************************************/

LVInfo::LVInfo()
{

    m_lvLECount = 0;
    m_LESize = 0;
    m_busy = false;
    m_minReduceSize = 0;
    m_lvError = LVMError::LVM_Err_Normal;
}
QDBusArgument &operator<<(QDBusArgument &argument, const LVInfo &data)
{
    argument.beginStructure();
    argument << data.m_vgName
             << data.m_lvPath
             << data.m_lvUuid
             << data.m_lvName
             << static_cast<int>(data.m_lvFsType)
             << data.m_lvSize
             << data.m_lvLECount
             << data.m_fsUsed
             << data.m_fsUnused
             << data.m_LESize
             << data.m_busy
             << data.m_mountpoints
             << data.m_lvStatus
             << data.m_minReduceSize
             << static_cast<int>(data.m_lvError);
    argument.endStructure();
    return argument;
}


const QDBusArgument &operator>>(const QDBusArgument &argument, LVInfo &data)
{
    argument.beginStructure();
    int err, type;
    argument >> data.m_vgName
             >> data.m_lvPath
             >> data.m_lvUuid
             >> data.m_lvName
             >> type
             >> data.m_lvSize
             >> data.m_lvLECount
             >> data.m_fsUsed
             >> data.m_fsUnused
             >> data.m_LESize
             >> data.m_busy
             >> data.m_mountpoints
             >> data.m_lvStatus
             >> data.m_minReduceSize
             >> err;
    data.m_lvFsType = static_cast<FSType>(type);
    data.m_lvError = static_cast<LVMError>(err);

    argument.endStructure();
    return argument;
}
/*********************************** VGInfo *********************************************/
VGInfo::VGInfo()
{
    m_pvCount = 0;
    m_peCount = 0;
    m_peUsed = 0;
    m_peUnused = 0;
    m_PESize = 0;
    m_curLV = 0;
    m_vgError = LVMError::LVM_Err_Normal;

}


QDBusArgument &operator<<(QDBusArgument &argument, const VGInfo &data)
{
    argument.beginStructure();
    argument << data.m_vgName
             << data.m_vgUuid
             << data.m_vgSize
             << data.m_vgUsed
             << data.m_vgUnused
             << data.m_pvCount
             << data.m_peCount
             << data.m_peUsed
             << data.m_peUnused
             << data.m_PESize
             << data.m_curLV
             << data.m_vgStatus
             << static_cast<int>(data.m_vgError)
             << data.m_lvlist
             << data.m_pvList;
    argument.endStructure();
    return argument;
}


const QDBusArgument &operator>>(const QDBusArgument &argument,  VGInfo &data)
{
    argument.beginStructure();
    int err;
    argument >> data.m_vgName
             >> data.m_vgUuid
             >> data.m_vgSize
             >> data.m_vgUsed
             >> data.m_vgUnused
             >> data.m_pvCount
             >> data.m_peCount
             >> data.m_peUsed
             >> data.m_peUnused
             >> data.m_PESize
             >> data.m_curLV
             >> data.m_vgStatus
             >> err
             >> data.m_lvlist
             >> data.m_pvList;
    data.m_vgError = static_cast<LVMError>(err);
    argument.endStructure();
    return argument;
}


/*********************************** LVMInfo *********************************************/

LVMInfo::LVMInfo()
{
    m_lvmErr = LVMError::LVM_Err_Normal;
}


QDBusArgument &operator<<(QDBusArgument &argument, const LVMInfo &data)
{
    argument.beginStructure();
    argument << data.m_pvInfo
             << data.m_vgInfo
             << static_cast<int>(data.m_lvmErr);
    argument.endStructure();
    return argument;
}


const QDBusArgument &operator>>(const QDBusArgument &argument,  LVMInfo &data)
{
    argument.beginStructure();
    int err;
    argument >> data.m_pvInfo
             >> data.m_vgInfo
             >> err;
    data.m_lvmErr = static_cast<LVMError>(err);
    argument.endStructure();
    return argument;
}



