#ifndef PARTITION_H
#define PARTITION_H
#include "commondef.h"

namespace DiskManager {

class Partition
{
public:
    Partition();
    virtual ~Partition() {}
    virtual Partition *clone() const;

    void Reset() ;


};
}
#endif // PARTITION_H
