#ifndef HELPER_BREAKSTRING_HPP_
#define HELPER_BREAKSTRING_HPP_

#include <QString>

namespace Helper {

QString breakString(const QString& str, const int lineLength = 128);

}

#endif /* HELPER_BREAKSTRING_HPP_ */
