#include "utils.h"
#include <QProcess>
#include <QRegularExpression>
#include <QDebug>

Utils::Utils()
{

}

QString Utils::find_program_in_path(const QString &proname)
{
    if (proname.isEmpty())
        return QString();
    QStringList strarg;
    strarg << proname;
    QString strout, strerr;
    executecmdwithartlist("which", strarg, strout, strerr);
    return strout;
}

int Utils::executecmdwithartlist(const QString &strcmd, const QStringList &strarg, QString &output, QString &error)
{
    QProcess proc;
    proc.start(strcmd, strarg);
    proc.waitForFinished(-1);
    output = proc.readAllStandardOutput().data();
    error = proc.errorString();
    int exitcode = proc.exitCode();
    proc.close();
    return exitcode;
}

int Utils::executcmd(const QString &strcmd, QString &output, QString &error)
{
    QProcess proc;
    proc.start(strcmd);
    proc.waitForFinished(-1);
    output = proc.readAllStandardOutput().data();
    error = proc.errorString();
    int exitcode = proc.exitCode();
    proc.close();
    return exitcode;
}

QString Utils::regexp_label(const QString &strtext, const QString &strpatter)
{
    QString strsource = strtext;
    QString result;
    QStringList strlist = strsource.split(QRegularExpression(strpatter, QRegularExpression::MultilineOption | QRegularExpression::CaseInsensitiveOption));
    qDebug() << strlist;
    if (strlist.size() >= 2)
        result = strlist.at(1);
    return result;
}

