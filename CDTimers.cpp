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
//// Created on 26/08/2012                                              ////
//// File: cdtimers.c                                                   ////
//// Description:                                                       ////
//// THIS FILE HAS ALL IMPLEMENTATION OF FUNCTIONS USED FOR CDTIMERS    ////
////   MANAGING; TIMERS ARE USED WITH CDTHREAD, AND VICEVERSA           ////        
////////////////////////////////////////////////////////////////////////////

#ifndef _CDTIMERS_C_
#define _CDTIMERS_C_

#include "CDThread.h"

#ifdef DOXYGEN
    #define section(YY)
#endif


//is the system array that stores the cdmessage structure used to manage messages
section("L1_data_b")  uint64_t        cdtimersSystemArray[CDTHREAD_MAX_NUM_THREADS];
//COMMENTED BECAUSE ALLOCATED IN memory_alloc.c



#endif //_CDTIMERS_C_

