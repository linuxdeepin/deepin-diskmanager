#ifndef PARTITIONVECTOR_H
#define PARTITIONVECTOR_H
#include "partition.h"

#include <cstddef>
#include <vector>
namespace DiskManager {

class Partition;
class PartitionVector;

class PartitionVector
{
public:
    typedef size_t size_type;
    typedef std::vector<Partition *>::iterator iterator;

    PartitionVector() {}
    PartitionVector(const PartitionVector &src);
    ~PartitionVector();
    void swap(PartitionVector &other);
    PartitionVector &operator=(PartitionVector rhs);

    // Iterators
    iterator begin() { return v.begin(); }

    // Capacity
    bool empty() const { return v.empty(); }

    // Element access
    Partition &operator[](size_type n) { return *v[n]; }
    const Partition &operator[](size_type n) const  { return *v[n]; }
    size_type size() const { return v.size(); }
    const Partition &front() const { return *v.front(); }
    const Partition &back() const { return *v.back(); }

    // Modifiers
    void pop_back();
    void erase(const iterator position);
    void clear();
    void push_back_adopt(Partition *partition);
    void insert_adopt(iterator position, Partition *partition);
    void replace_at(size_type n, const Partition *partition);

private:
    std::vector<Partition *> v;
};

int find_extended_partition(const PartitionVector &partitions);
}
#endif // PARTITIONVECTOR_H
