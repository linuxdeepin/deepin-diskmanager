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

    QString m_name;   // E.g. Block special file {"/dev/sda1", 8, 1},
    unsigned long m_major;  // plain file {"FILENAME", 0, 0} and empty object
    unsigned long m_minor;  // {"", 0, 0}.

    static void clear_cache();
    static void register_block_special(const QString &name,
                                       unsigned long major, unsigned long minor);

};

// Operator overloading > The Decision between Member and Non-member
// http://stackoverflow.com/questions/4421706/operator-overloading/4421729#4421729
//     "2. If a binary operator treats both operands equally (it leaves them unchanged),
//     implement this operator as a non-member function."
bool operator == (const BlockSpecial &lhs, const BlockSpecial &rhs);
bool operator < (const BlockSpecial &lhs, const BlockSpecial &rhs);

}
#endif // BLOCKSPECIAL_H
