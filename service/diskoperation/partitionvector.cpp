#include "partitionvector.h"
#include <algorithm>
namespace DiskManager {

PartitionVector::PartitionVector(const PartitionVector &src)
{
    v.resize(src.size());
    for (unsigned int i = 0 ; i < src.size() ; i ++)
        v[i] = src[i].clone();
}

PartitionVector::~PartitionVector()
{
    for (unsigned int i = 0 ; i < v.size() ; i ++)
        delete v[i];
}

void PartitionVector::swap(PartitionVector &other)
{
    std::swap(this->v, other.v);
}

PartitionVector &PartitionVector::operator=(PartitionVector rhs)
{
    // Copy assignment implemented using copy-and-swap idiom.
    // 1) Pass-by-value to get rhs variable copy constructed from source;
    // 2) Swap contents of this PartitionVector with rhs variable copy;
    // 3) Method returns, rhs variable goes out of scope and destructor called on old
    //    this PartitionVector to delete owned Partitions.
    // Reference:
    //     Wikibooks: More C++ Idioms / Copy-and-swap
    //     https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Copy-and-swap
    rhs.swap(*this);
    return *this;
}

void PartitionVector::pop_back()
{
    delete v.back();
    v.pop_back();
}

void PartitionVector::erase(const iterator position)
{
    delete *position;
    v.erase(position);
}

void PartitionVector::clear()
{
    for (unsigned int i = 0 ; i < v.size() ; i ++)
        delete v[i];
    v.clear();
}

void PartitionVector::push_back_adopt(Partition *partition)
{
    v.push_back(partition);
}

void PartitionVector::insert_adopt(iterator position, Partition *partition)
{
    v.insert(position, partition);
}

void PartitionVector::replace_at(size_type n, const Partition *partition)
{
    Partition *p = partition->clone();
    delete v[n];
    v[n] = p;
}

// Return index of the extended partition or -1 when not found
int find_extended_partition(const PartitionVector &partitions)
{
//    for (unsigned int i = 0 ; i < partitions.size() ; i ++)
//        if (partitions[i].type == TYPE_EXTENDED)
//            return (int)i;
//    return -1;
}
}
