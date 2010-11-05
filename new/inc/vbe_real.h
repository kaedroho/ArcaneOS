/* 
 * File:   vbe_real.h
 * Author: Diggory
 *
 * Created on 05 November 2010, 12:02
 */

#ifndef VBE_REAL_H
#define	VBE_REAL_H

enum vbe_result {
    vbe_result_success = 0,
    vbe_result_failed = 1,
    vbe_result_not_supported = 2,
    vbe_result_invalid_call = 3
};

enum vbe_result vbe_get_controller_info();

#endif	/* VBE_REAL_H */

