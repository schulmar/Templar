/*
 * qt_version_switch.h
 *
 *  Created on: 20.12.2013
 *      Author: schulmar
 */

#ifndef QT_VERSION_SWITCH_H_
#define QT_VERSION_SWITCH_H_

#if USE_QT5
/// returns the char* of a QString reference
#define Q_TO_C_STRING(QSTRING) QSTRING.toLatin1().data()
#define QMATRIX_DETERMINANT(QMATRIX) QMATRIX.determinant()
#else
/// returns the char* of a QString reference
#define Q_TO_C_STRING(QSTRING) QSTRING.toAscii().data()
#define QMATRIX_DETERMINANT(QMATRIX) QMATRIX.det()
#endif

#endif /* QT_VERSION_SWITCH_H_ */
