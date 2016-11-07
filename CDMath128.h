////////////////////////////////////////////////////////////////////////////
////        (C) Copyright Dario Cortese                                 ////
//// This source code may only be used by Dario Cortese licensed users  ////
//// This source code may only be distributed to Dario Cortese licensed ////
//// users.  No other use, reproduction  or distribution is permitted   ////
//// without written permission.                                        ////
////                                                                    ////
//// Author: Dario Cortese                                              ////
//// Client: I.S.A. srl                                                 ////
//// User: Dario Cortese                                                ////
//// Created on 01/04/2013                                              ////
//// File: cdMath128.h                                                   ////
//// Description:                                                       ////
////    This file has the functions prototypes and data definitions     ////
////    to manage software math 128bit types                                 ////  
////////////////////////////////////////////////////////////////////////////
#ifndef _CDMATH128_C_
#define _CDMATH128_C_

#ifdef DOXYGEN
    #define section( YY )
#endif


typedef struct tagCDnum128{
	short neg;    
	uint64_t hi;
	uint64_t lo;
}CDnum128_t;

//init ot 0 a 128bit struct
void CDinit128 (CDnum128_t *Res);
//convert sint64 to CDnum128_t type
void CDconvert64to128 (sint64_t *A, CDnum128_t *Res);

//shift right entire 128bit for a number of indicated bit (max 127)
//this operation as is a div 2^numBits
void CDshiftRight128(CDnum128_t *val, uint8_t numBits);
//shift left entire 128bit for a number of indicated bit (max 127)
//this operation as is a mul 2^numBits
void CDshiftLeft128(CDnum128_t *val, uint8_t numBits);

//add 128bit A value to the B 128bit value nad store the result in B
void CDadd128_AtoB(CDnum128_t *A, CDnum128_t *B);
//add a 64bit signed with the 128bit B; before convert the signed 64bit to a 128bit and after add it to 128
void CDadd_A64toB128(sint64_t *A, CDnum128_t *B);

//convert CDnum128_t to signed int 64bit; remember that if num128 higher part (64bits) is a value bigger than 63 bit + sign
// then 64 destination will set to maximun positive or maximun negative
void CDconvert128HighToS64 (CDnum128_t *Org, sint64_t *Dest);
//convert CDnum128_t to signed inte 64bit; remember that if num128 is higher as value to the possible storable in a 63 bit (with sign) then
// 64 destination will set to maximun positive or maximun negative, depends on sign of 128bit
void CDconvert128ToS64 (CDnum128_t *Org, sint64_t *Dest);


#endif //_CDMATH128_C_
