#include "partition.h"

namespace DiskManager {

Partition::Partition()
{

}

Partition *Partition::clone() const
{
    return new Partition(*this);
}

void Partition::Reset()
{

}

}//namespace DiskManager
