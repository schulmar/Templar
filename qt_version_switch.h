/*
 * qt_version_switch.h
 *
 *  Created on: 20.12.2013
 *      Author: schulmar
 */

#ifndef QT_VERSION_SWITCH_H_
#define QT_VERSION_SWITCH_H_

#if USE_QT5
#define QMATRIX_DETERMINANT(QMATRIX) QMATRIX.determinant()
#else
#define QMATRIX_DETERMINANT(QMATRIX) QMATRIX.det()
#endif

#endif /* QT_VERSION_SWITCH_H_ */
