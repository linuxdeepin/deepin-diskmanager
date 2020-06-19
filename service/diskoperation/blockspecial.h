#ifndef BLOCKSPECIAL_H
#define BLOCKSPECIAL_H
#include <QString>

namespace DiskManager {

class BlockSpecial
{
public:
    BlockSpecial();
    BlockSpecial(const QString &name);
    ~BlockSpecial();

    QString m_name; // E.g. Block special file {"/dev/sda1", 8, 1},
    unsigned long m_major; // plain file {"FILENAME", 0, 0} and empty object
    unsigned long m_minor; // {"", 0, 0}.

    static void clear_cache();
    static void register_block_special(const QString &name,
                                       unsigned long major, unsigned long minor);
};

bool operator==(const BlockSpecial &lhs, const BlockSpecial &rhs);
bool operator<(const BlockSpecial &lhs, const BlockSpecial &rhs);

} // namespace DiskManager
#endif // BLOCKSPECIAL_H
