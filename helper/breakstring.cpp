#include "breakstring.hpp"

namespace Helper {
QString breakString(const QString& str, const int lineLength)
{
    int i = 0;
    QString ret = str;

    while ( (i += lineLength) < str.length() )
    {
        ret.insert(i, '\n');
    }

    return ret;
}
}
