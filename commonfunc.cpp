#include "commonfunc.h"

#include <climits>
#include <QRegularExpression>
#include <QString>

bool CommonFunc::checkPort(quint16 port)
{
    int minPort{ 1024 };
    return !(port < minPort || port > USHRT_MAX);
}

bool CommonFunc::checkAddressIP(const QString& addressIP)
{
    static const QRegularExpression ipRegex("^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}"
                                            "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$");
    return (ipRegex.match(addressIP).hasMatch() || addressIP == "localhost");
}
