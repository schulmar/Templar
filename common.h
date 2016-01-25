#ifndef COMMON_H
#define COMMON_H

#include <QString>
#include <QColor>

namespace common {

    namespace colors{
        extern QColor nodeColors[10];
        extern QString colorNames[10];
        extern const char* DEFAULT;
        extern const char* BEGIN;
        extern const char* END;
        extern const char* BLANK;
    } // namespace colors
    namespace attrs{
        extern const char* POLYSHAPE;
        extern const char* ELLIPSESHAPE;
        extern const char* STYLE;
        extern const char* SIDES;
    } // namespace colors

} // namespace common

#endif // COMMON_H
