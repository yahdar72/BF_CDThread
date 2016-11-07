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
//// Created on 10/08/2012                                              ////
//// File: cdThread.c                                                   ////
//// Description:                                                       ////
//// THIS FILE HAVE ALL IMPLEMENTATION OF FUNCTION USED FOR CDTHREAD    ////
////   MANAGING; THREADS ARE USED ALSO IN CDMESSAGE, AND VICEVERSA      ////        
////////////////////////////////////////////////////////////////////////////

#ifndef _CDTHREAD_C_
#define _CDTHREAD_C_

#include "CDThread.h"
#include "CDEvents.h"

#ifdef DOXYGEN
    #define section(YY)
#endif

uint64_t	cdth_Absolute_timer;

//used to inform extern code which is the last executed thread function called by thread_engine
cdThdID_t LASTCALLEDTHREADIDBYENGINE; 


//advise external code which is the last called thread by user
cdThdID_t LASTCALLEDTHREADIDBYUSER;	



#ifdef  ENABLE_CDTHERRINF
   struct struct_cdThErrInf cdThErrInf;
#endif	//ENABLE_CDTHERRINF


//int cdthreadOVERFLOW;
//unsigned int cdthreadCREATED;
//unsigned int cdthreadDESTROYED;
//int cdthreadLOADED;
//int cdthreadENABLED;
struct struct_cdThCOUNTERS cdThCOUNTERS;



//is the system array that stores the cdthread structure used to manage threads
section("sdram0")  cdthreadStruct  cdthreadsSystemArray[CDTHREAD_MAX_NUM_THREADS];
//COMMENTED BECAUSE ALLOCATED IN memory_alloc.c


/*! \var cdThdID_t cdthreadsSystemOperations_ID
    This is a thread index used for a special thread always executed, every time a trheadmanager is called 
    \n The thread associated at the index is the first executed and affer starts other thread
    \n The use of this thread is for system operation and checks, for example keypad managment, serial port
       managment ans so on, that is all operation requiring constant check
*/
cdThdID_t cdthreadsSystemOperations_ID;

/*! \var cdThdID_t cdthreadsSystemOperations_ID
    

This is a thread index used for a special thread executed, every time a trheadmanager is called 
    \n The thread associated at the index is the first executed and affer starts other thread
    \n The use of this thread is for system operation and checks, for example keypad managment, serial port
       managment ans so on, that is all operation requiring constant check
*/
cdThdID_t cdthreadsBackgroundOperations_ID;


/*! \fn void cdthread_initAll(void)
	\author Dario Cortese
	\date 10-08-2012
	\brief must be called by initialization main to reset and init system threads array
	\version 1.00
*/
void cdthread_initAll(void){
	int idx;
	
	cdthreadsSystemOperations_ID = CDTHREADID_ERROR;
	cdthreadsBackgroundOperations_ID = CDTHREADID_ERROR;
	
	for(idx=0; idx < CDTHREAD_MAX_NUM_THREADS; idx++){
		cdthreadsSystemArray[idx].ID=CDTHREADID_ERROR;	//free/available position
		cdthreadsSystemArray[idx].Priority=CDTHREADPRIORITY_DISABLED;
		cdthreadsSystemArray[idx].cdtreadFunction = CDTHREADFUNCTION_NOFUNCTION;	//no associated function
		cdthreadsSystemArray[idx].LastExitState =0;			//default value
		cdthreadsSystemArray[idx].isTheFirstTime = true;	//so when vill be activated that condition will be true
		cdthreadsSystemArray[idx].MessagesCounter=0;		//no messages
		cdthreadsSystemArray[idx].FirstMsgID = CDMESSAGEID_ERROR;		//no first message 
		cdthreadsSystemArray[idx].LastMsgID = CDMESSAGEID_ERROR;		//no last message
		cdthreadsSystemArray[idx].WaitUpTo =0; 			//no wait absolute timer 
		cdthreadsSystemArray[idx].waitModeMask = 0;		//no wait mask, so every time shall be called
		cdthreadsSystemArray[idx].LoadTimVal = 0;	 	//no load WaitUpTo timer
		cdthreadsSystemArray[idx].eventPtr = 0;	  		//no global event pointer	
		cdthreadsSystemArray[idx].CountDown = 0;		//no wait calling befor run
		cdthreadsSystemArray[idx].LoadCountDown = 0; 	//no reload countdown
	}

   cdThCOUNTERS.OVERFLOW = 0;
   cdThCOUNTERS.CREATED = 0;
   cdThCOUNTERS.DESTROYED = 0;
   cdThCOUNTERS.LOADED = 0;
   cdThCOUNTERS.ENABLED = 0;
	
#ifdef  ENABLE_CDTHERRINF
	cdThErrInf.getFreeID_err =0;
	//cdThErrInf.getArrayIdxFromID_err =0;
	cdThErrInf.getPointerToStruct_err =0;
	cdThErrInf.cdthread_new.pThIdIsNotDestroyed =0;
	cdThErrInf.cdthread_new.freeid_err =0;
	cdThErrInf.newUserManaged.new_err =0;
	cdThErrInf.newUserManaged.id_err =0;
	cdThErrInf.changeFunction.realIdx_err =0;
	cdThErrInf.changeFunction.id_err =0;
	cdThErrInf.isFunction.realIdx_err =0;
	cdThErrInf.isFunction.id_err =0;
	cdThErrInf.DestroyThread_realIdx_warning =0;
	cdThErrInf.isDestroyed_realIdx_warning =0;
	cdThErrInf.signAsFirtsTime.realIdx_err =0;
	cdThErrInf.signAsFirtsTime.id_err =0;
	cdThErrInf.waitUpTo.realIdx_err =0;
	cdThErrInf.waitUpTo.id_err =0;
	cdThErrInf.Enable_realIdx_err =0;
	cdThErrInf.Disable_realIdx_err =0;
	cdThErrInf.isEnabled_realIdx_err =0;
	cdThErrInf.getPriority_realIdx_err =0;
	cdThErrInf.getPrevExitStatus_realIdx_err =0;
#endif	//ENABLE_CDTHERRINF
}



//********************************************************************************************************************************
//****THREAD ENGINE FUNCTIONS*****************************************************************************************************
//********************************************************************************************************************************


/*! \fn int cdthread_ThreadToRun(int LastRunnedIndex)
	\author Dario Cortese
	\date 01-05-2016
	\brief this function is called by cdthread Engines to determine if a thread (by index of thread array) is runnable
	Actualy this function check if is disabled and all waitmode, moreover check also if associated function
	 is not CDTHREADFUNCTION_NOFUNCTION.
	\n Moreover this function indicates that thix equal to cdthreadsSystemOperations_ID or cdthreadsBackgroundOperations_ID
	  is not valid thread to run, because this is used for special pourpose by thread manager
	\param thidx is the index of cdthreadsSystemArray indicating 
	\return true if indicated index of thread array is a thread that must be run
	\warning this function doesn't check if index is inside range, an index error generate unpredictable behaviour
	\warning thidx IS NOT a cdThdID_t, so don't use it with cdThreadFunction
	\version 2.00
*/
int cdThread_ValidateThreadToRun(int thidx){
   int ret_ok;
   int ModeMaskVal;
   cdthreadStruct* pths;
   
   ret_ok = false;
   pths = &cdthreadsSystemArray[thidx];
   //checks if actual analyzed thread is enabled, also disabled if destroyed so useless checks also if destroyed
   if( pths->Priority == CDTHREADPRIORITY_DISABLED )
      return false;
      
   //this function is called also from then userthread so check only if id is different from CDTHREADID_ERROR
   if( pths->ID == CDTHREADID_ERROR)
      return false;

   if( pths->cdtreadFunction == CDTHREADFUNCTION_NOFUNCTION )
      return false ;        

   ModeMaskVal = pths->waitModeMask;

   //checks if there isn't wait mode operation active, in this case if arrived here means that this thread
   //must be executed
   if( ModeMaskVal == cdTh_NOwaitMode )
      return true ;
      
//REMEMBER ALL WAIT MODE MUST BE TESTED OTHERWISE SOME WAIT FEATURE COULD NOT WORK PROPERLY
//REMEMBER ALL WAIT MODE MUST BE TESTED OTHERWISE SOME WAIT FEATURE COULD NOT WORK PROPERLY
   ret_ok = false ;

   //TEST WAIT COUNTS MODE AND ITS AUTORELAOD
   if ((ModeMaskVal & cdTh_waitModeCounts ) != 0){
      //this must be the first mode to be checked otherwise countdown could be altered by others modes
      if ((pths->CountDown) > 0 )
            pths->CountDown--;
         
      if ((pths->CountDown) <= 0 ){
         if(pths->LoadCountDown > 0 )  //tests if there is a reload value
            pths->CountDown = pths->LoadCountDown;
         else //if there isn't a reload value then disable count down mode
            pths->waitModeMask &= ( ~ cdTh_waitModeCounts );
         
         ret_ok = true; //indites to return thidx, but doesn't stop others test  
      }   
   }   

   //TEST WAIT TIMER MODE AND ITS AUTORELOAD
   if ((ModeMaskVal & cdTh_waitModeTimer ) != 0){
      if (pths->WaitUpTo <= cdth_Absolute_timer ){
         if( pths->LoadTimVal > 0 )
            pths->WaitUpTo += pths->LoadTimVal;   
         else //if there isn't a reload value then disable timer wait mode
            pths->waitModeMask &= ( ~ cdTh_waitModeTimer );
            
         ret_ok = true; //indites to return thidx, but doesn't stop others test  
      } 
   }
   
   //TEST WAIT MESSAGE MODE
   if ((ModeMaskVal & cdTh_waitModeMessage ) != 0){
      if (pths->MessagesCounter > 0 )
         ret_ok = true; //indites to return thidx, but doesn't stop others test  
   }      

   //TEST WAIT EVENT MODE
   if ((ModeMaskVal & cdTh_waitModeEvent ) != 0){
      if ( cdeventIsActive( (*(pths->eventPtr)) ) )
         ret_ok = true; //indites to return thidx, but doesn't stop others test  
   }

   return ret_ok;
   
} //end int cdThread_ValidateThreadToRun(int thidx)




/*! \fn int cdthread_Engine(void)
	\author Dario Cortese
	\date 09-08-2012 mod 2016-05-01
	\brief this function is the engine for thread and must be called continuosly
	every time this function is called, it call a single different active thread function, and after exit.
	\n If there isn't active thread return 0.
	\n If there is almost one thread active with a function callback then return 1 if she exit with a positive exit code, otherwise return -1
	\n To determine which thread run when it is called, it call a cdthread_ThreadToRun function; if there is to implement a new sistem to manage
	 thread then that function could be changed. 
	\return the index of executed thread, if any thread executed then return -1 
	\ver 2.0
*/
int cdthread_Engine(void){
	static int LastThIndex;
	int nextThIndex;
	cdthreadStruct* thptr;
	cdThdID_t cdthid;
	cdtreadFunctionType ptrFunc;
	int isFirstTime;
	int exitState;
   int ret_ok;	
   int icount;

   //if last values is
	if( LastThIndex >=0 ) 
      nextThIndex = LastThIndex;
   else //if <0
      nextThIndex=-1; //only for ensurance 
	
   cdtimer_getNOW(); //updates absolute timer
   ret_ok = false ;
   for(icount = 1; icount < CDTHREAD_MAX_NUM_THREADS; icount++ )
   {
      nextThIndex++;
      if (nextThIndex >= (CDTHREAD_MAX_NUM_THREADS - 1))
         nextThIndex = 0;
      
      ret_ok = cdThread_ValidateThreadToRun( nextThIndex ) ;             
      if (ret_ok){
         //check if this ID is a managed thread
   		thptr = &cdthreadsSystemArray[ nextThIndex ];
	   	cdthid = thptr->ID;
         if ( cdthread_isEngineManagedID(cdthid) )
            break; //exit for
      }
   } //end for(icount = 1; icount < CDTHREAD_MAX_NUM_THREADS; icount++ )
	//if ret_ok = false then means that no active thread (managed)
	
   if(ret_ok){
//		thptr = &cdthreadsSystemArray[ nextThIndex ];
//		//call the function with 
//		cdthid = thptr->ID;
		ptrFunc = thptr->cdtreadFunction;
		isFirstTime = thptr->isTheFirstTime;
		//call the callback function
		exitState = thptr->LastExitState;
		exitState = ptrFunc( isFirstTime, cdthid, exitState);
		if(isFirstTime)
		   thptr->isTheFirstTime = false;	//after now sign this thread as "not the first time" that is called
		//store actual exitcode for this thread
		thptr->LastExitState = exitState;
		LASTCALLEDTHREADIDBYENGINE = cdthid;	//advise external code which is the last called thread by engine

   	LastThIndex = nextThIndex;
   	return nextThIndex;
   } // end if (ret_ok)
   //if arrive here means that any thread was called
   return -1;    
	//store index for next call
}




/*! \fn int cdthread_UserCallThreadFunction(cdThdID_t pThId, int* exitCode)
	\author Dario Cortese
	\date 10-08-2012...16-09-2013...01-05-2016
	\brief call function of thread, if thread is enabled and id is good (no errors); return true if called, otherwise false
	checks if threadID is a valid ID, if thread is enables, if function pointer isn't CDTHREADFUNCTION_NOFUNCTION (no associated function to thread),
	 after, if there aren't wait modes actives, call the pointed thread function.
	\n If one or more wait modes are actives then checks all available wait modes (is timer elapsed, is elpased the number of call countdown, there is a message,
		is active the event) and if one, ore more, is satisfied then call thread function, otherwise exit without call it. 
	\n When call function read threadId, if is the first time that run, and pass this information to called function.
	\n When called function returns, the returned value is stored in thread LastExitState and thread id pubblicated in LASTCALLEDTHREADIDBYUSER; the returned
	value is also returned outside this function by exitCode parameter.
	\param pThId is cdthread (id) 
	\param exitCode is returned value by called thread_function
	
	\note only when thread is enabled the wait modes working, if thread is disabled then wait modes aren't tested
	\return true if function was called, false if for error, or simply because thread is disabled or waiting; in avery case when the function was not called
	\warning if cdTh_waitModeTimer (cdthread_waitUpTo or cdthread_RunEveryTicks) is active must be updated externaly the cdtimer_getNOW() or cdtimer_updateAbsoluteTimer()
	\warning cdtimer_getNOW(), or cdtimer_updateAbsoluteTimer(), is time consumer operation because use 64bit math with division
	\version 2.0
*/
section("L1_code")
int cdthread_UserCallThreadFunction(cdThdID_t pThId, int* exitCode){
	int realidx;
	cdthreadStruct* thptr;
	cdThdID_t cdthid;
	cdtreadFunctionType ptrFunc;
	int isFirstTime;
	int exitState;
    int ModeMaskVal;
    int ret_ok;
	
	//convert pThId in real index for cdthreadsSystemArray
	realidx = cdthread_getArrayIdxFromID(pThId);
	//if an error happened then return false
	if( realidx<0 )
	   return false; 

   ret_ok = cdThread_ValidateThreadToRun( realidx ) ;             
   if ( !ret_ok)
		return false; //if arrives here means no called function so singnaling an error

	thptr = &cdthreadsSystemArray[ realidx ];
	//if arrives here means call the thread function
	cdthid = thptr->ID;
	ptrFunc = thptr->cdtreadFunction;
	isFirstTime = thptr->isTheFirstTime;
	//call the callback function
	exitState = thptr->LastExitState;
	exitState = ptrFunc( isFirstTime, cdthid, exitState);
	if(isFirstTime)
	   thptr->isTheFirstTime = false;	//after now sign this thread as "not the first time" that is called
	//store actual exitcode for this thread
	thptr->LastExitState = exitState;
	LASTCALLEDTHREADIDBYUSER = cdthid;	//advise external code which is the last called thread by engine
	*exitCode = exitState;
	return true;
}




//********************************************************************************************************************************
//****THREAD  FUNCTIONS***********************************************************************************************************
//********************************************************************************************************************************




/*! \fn cdThdID_t cdthread_getFreeID(void)
	\author Dario Cortese
	\date 08-08-2012
	\brief searchs free/available position in system thread array
	Searchs inside cdthreadsSystemArray if there is a available/free position (searhs for every position if there is a ID==CDTHREADID_ERROR, 
	 this means that that position is free to use)
	\return CDTHREADID_ERROR if doesn't find any available position, or positive number that indicate ID (array position + 1) 
	\note ONLY FOR INTERNAL USE
	\version 1.00
*/
cdThdID_t cdthread_getFreeID(void){
	//uses static id so next time that a new thread position is requested, start from a new position and avoid to 
	//reuse position until array end is reached  
	static cdThdID_t idx=0;
	
	if(idx >=CDTHREAD_MAX_NUM_THREADS) idx=0;
	
	for(; idx < CDTHREAD_MAX_NUM_THREADS; idx++){
		if(cdthreadsSystemArray[idx].ID == CDTHREADID_ERROR){
			idx++;	//so next time that reenter use next new position
			//return (idx +1);	//ID is always the real array index +1, because id=0 means unused/free
			return idx;
		}
	}
	//if first search, searching from last idx has no positive result (otherwire exit with return and doesn't arrives here)
	// then execute a second search starting from 0
	for(idx=0; idx < CDTHREAD_MAX_NUM_THREADS; idx++){
		if(cdthreadsSystemArray[idx].ID == CDTHREADID_ERROR){
			idx++;	//so next time that reenter use next new position
			//return (idx +1);	//ID is always the real array index +1, because id=0 means unused/free
			return idx;
		}
	}
	//if also second search has no positive result then this means that there isn't available trread id free, at compiling time
	// increments CDTHREAD_MAX_NUM_THREADS define, or check if some thread is many time allocated without destroy	
#ifdef  ENABLE_CDTHERRINF
	cdThErrInf.getFreeID_err++;
#endif  //ENABLE_CDTHERRINF
   cdThCOUNTERS.OVERFLOW=1; //advices my thread system that a serious error happened (no available thread)
	return CDTHREADID_ERROR; //means no available threads
} 


/*! \fn int cdthread_getArrayIdxFromID(cdThdID_t pThId)
	\author Dario Cortese
	\date 09-08-2012
	\brief return the real index for cdthreadsSystemArray extracted from indicated cdThdID_t (pThId)
	\param pThId is cdthread id 
	\return -1 if thre is an error or positive number (0 or more) if all ok
	\note please don't use it to access thread struct data, prefere to use all other function with higher abstraction from internal data structure
	\warning DANGEROUS, because return a index that will be used to access directly an element of system thread array without any checks or value casting; possibility of data corruption
	\note ONLY FOR INTERNAL USE
	\version 1.00
*/
int cdthread_getArrayIdxFromID(cdThdID_t pThId){
	int realidx;
	if(!cdthread_checkValidID( pThId)){
//#ifdef  ENABLE_CDTHERRINF
//		cdThErrInf.getArrayIdxFromID_err++;
//#endif  //ENABLE_CDTHERRINF
		return -1;
	}
	//convert pThId in real index for cdthreadsSystemArray
	if(cdthread_isUserManagedID( pThId )){
		realidx = (int)pThId * (-1);
		realidx--;
	}else{
		realidx = (int)pThId;
		realidx--;
	}
	return realidx;
}



/*! \fn cdthreadStruct* cdthread_getPointerToStruct(cdThdID_t pThId){
	\author Dario Cortese
	\date 10-08-2012
	\brief return a cdthreadStruct type pointer directly to inndicated element of internal thread struct array; if error happen return a pointer to 0x0000000 memory address
	\param pThId is cdthread (id) to destroy
	\return a pointer to internal system array of stream struct; if an error happen return a pointer to 0x0000000 address 
	\note please don't use it, prefere to use all other function with higher abstraction from internal data structure
	\warning VERY DANGEROUS, because point directly to an element of private system thread array without any checks or value casting; possibility of data corruption
	\note ONLY FOR INTERNAL USE
	\version 1.00
*/
cdthreadStruct* cdthread_getPointerToStruct(cdThdID_t pThId){
	int realidx;
	//convert pThId in real index for cdthreadsSystemArray
	realidx = cdthread_getArrayIdxFromID(pThId);
	//if an error happened then return false
	if( realidx<0 ){
#ifdef  ENABLE_CDTHERRINF
		cdThErrInf.getPointerToStruct_err++;
#endif  //ENABLE_CDTHERRINF
		return (cdthreadStruct*)0x00000000; 
	}
	return &cdthreadsSystemArray[realidx];
} 




/*! \fn int cdthread_new(cdThdID_t* pThId, cdtreadFunctionType ptrFunction)
	\author Dario Cortese
	\date 08-08-2012, mod 06-Apr-2013
	\brief creates a new thread for Thread_Engine; after creation the thread is disabled, doesn't have messages and the firsttime is set to true
	Checks if passed id (pThId) is unused/ previously destroyed thread id and return an error if id already used.
	\n Searchs inside cdthreadsSystemArray if there is a available/free position (searhs for every position if there is a ID==CDTHREADID_ERROR, 
	 this means that that position is free to use), after initialize finded thread with ID = returned id value, priority as disabled thread,
	 cdtreadFunction as passed ptrFunction, LastExitState as 0, isTheFirstTime as true and message counter =0
	\n At the end, if all ok, return the new id by pThId parameter and true as responce.
	\param pThId is cdthread (id) to verify if was free (previously destroyed) and also is the returned new thread (id); if error happens this is set to CDTHREADID_ERROR 
	\param ptrFunction is the function pointer to a function that is the called thread_function when thread run; if no function pass CDTHREADFUNCTION_NOFUNCTION
	\return true if new thread created or false if happened a problem as thread id also used (not destroyed) or noone id is free for a new thread
	\version 1.01
*/
int cdthread_new(cdThdID_t* pThId, cdtreadFunctionType ptrFunction){
//prev define was: cdThdID_t cdthread_new( cdtreadFunctionType ptrFunction){
	//cdThdID_t idx;
	cdthreadStruct* thptr;
	
	//checks if *pThId in not destroyed only if his ID is not a CDTHREADID_ERROR
	if(*pThId != CDTHREADID_ERROR){
   	if(!cdthread_isDestroyed(*pThId)){
#ifdef  ENABLE_CDTHERRINF
	   	cdThErrInf.cdthread_new.pThIdIsNotDestroyed++;
#endif  //ENABLE_CDTHERRINF
		   return false;	//the thread id point to an existing thread (not previously destroyed and also actually used)
	   }
	} //if(*pThId != CDTHREADID_ERROR)
	
	*pThId=cdthread_getFreeID();
	if(*pThId == CDTHREADID_ERROR){
#ifdef  ENABLE_CDTHERRINF
		cdThErrInf.cdthread_new.freeid_err++;
#endif  //ENABLE_CDTHERRINF
		return false;	//means error, no available 
	}
	//the system array index is equal to |id|-1
	thptr = &cdthreadsSystemArray[*pThId - 1];
	thptr->ID=*pThId;
	thptr->Priority=CDTHREADPRIORITY_DISABLED;
	thptr->cdtreadFunction = ptrFunction;
	thptr->LastExitState =0;
	thptr->isTheFirstTime = true;
	thptr->MessagesCounter=0;
	thptr->FirstMsgID = CDMESSAGEID_ERROR;
	thptr->LastMsgID = CDMESSAGEID_ERROR;		
	thptr->WaitUpTo =0;
	
   cdThCOUNTERS.CREATED++;
   cdThCOUNTERS.LOADED++;
	return true;
} 


/*! \fn int cdthread_newUserManaged(cdThdID_t* pThId, cdtreadFunctionType ptrFunction) 
	\author Dario Cortese
	\date 08-08-2012, mod 06-Apr-2013
	\brief creates a new USER managed thread (not managed by Thread Engine); after creation the thread is disabled, doesn't have messages and the first time is set to true
	Checks if passed id (pThId) is unused/ previously destroyed thread id and return an error if id already used.
	\n Searchs inside cdthreadsSystemArray if there is a available/free position (searhs for every position if there is a ID==CDTHREADID_ERROR, 
	 this means that that position is free to use).
	\n If find a free position initialize that thread with ID = returned id value, priority as disabled thread, cdtreadFunction as passed ptrFunction,
	 LastExitState as 0, isTheFirstTime as true and message counter =0
	\n At the end, if all ok, return the new id by pThId parameter and true as responce.  
	\param pThId is cdthread (id) to verify if was free (previously destroyed) and also is the returned new thread (id); if error happens this is set to CDTHREADID_ERROR 
	\param ptrFunction is the function pointer to a function that is the called thread_function when thread run; if no function pass CDTHREADFUNCTION_NOFUNCTION
	\return true if new thread created or false if happened a problem as thread id also used (not destroyed) or noone id is free for a new thread
	\version 1.01
*/
int cdthread_newUserManaged(cdThdID_t* pThId, cdtreadFunctionType ptrFunction){
//prev was: cdThdID_t cdthread_newUserManaged(cdtreadFunctionType ptrFunction)
	int ires;
	int previdx;
	ires = cdthread_new( pThId, ptrFunction);
	if(!ires){
#ifdef  ENABLE_CDTHERRINF
		cdThErrInf.newUserManaged.new_err++;
#endif  //ENABLE_CDTHERRINF
		return false ; //return an error, cause must be passed id already used (not previously destroyed) or unavailable thread
	}
	if(*pThId == CDTHREADID_ERROR){
#ifdef  ENABLE_CDTHERRINF
		cdThErrInf.newUserManaged.id_err++;
#endif  //ENABLE_CDTHERRINF
		return false;	//means error, no available 
	}
	//the system array real id is equal to |id|-1
	previdx =  *pThId - 1;
	//the user managed threads have negative id to distinguish the thread engine managed that have positive id;
	// remeber that the system array real id is equal to |id|-1 (|id| is the id without sign)
	*pThId *= - 1;
	cdthreadsSystemArray[previdx].ID= *pThId;
	return true;
} 


/*! \fn int cdthread_changeFunction(cdThdID_t pThId, cdtreadFunctionType ptrFunction)
	\author Dario Cortese
	\date 18-09-2012
	\brief it changes only called function by indicated thread without change any other thread fields
	This method changes the called thread_function by thread, without change messages, last exit code, firsttime and the other fields.
	\n This is very usefull, and power, system to manage differents object that will be controlled by only one thread; a very simple examples could be a DAC, of 
	 three different available, that must be selected and drived while other two are stopped.
	\n Changing called function is possible to drive and manage any of three DAC without performance lack generated typically by series of if-else or switch instruction  
	\param pThId is cdthread (id) to change called thread_function
	\param ptrFunction is the function pointer to a function that is the called thread_function when thread run; if no function pass CDTHREADFUNCTION_NOFUNCTION
	\return true if action has succes
	\version 1.00
*/
int cdthread_changeFunction(cdThdID_t pThId, cdtreadFunctionType ptrFunction){
	int realidx;
	realidx = cdthread_getArrayIdxFromID(pThId);
	//if an error happened then return false
	if( realidx<0 ){
#ifdef  ENABLE_CDTHERRINF
		cdThErrInf.changeFunction.realIdx_err++;
#endif  //ENABLE_CDTHERRINF
		return false;
	}
	//checks id thread is destroyed and if destroyed return false
	if(cdthreadsSystemArray[realidx].ID == CDTHREADID_ERROR){
#ifdef  ENABLE_CDTHERRINF
		cdThErrInf.changeFunction.id_err++;
#endif  //ENABLE_CDTHERRINF
		return false;
	}
	//sets the new callback thread function to thread
	cdthreadsSystemArray[realidx].cdtreadFunction = ptrFunction;
	return true;
} 



/*! \fn int cdthread_isFunction(cdThdID_t pThId, cdtreadFunctionType ptrFunction)
	\author Dario Cortese
	\date 06-04-2013
	\brief checks if called function by indicated thread (pThId) is the same passed (ptrFunction)
	\param pThId is cdthread (id) to checks called thread_function
	\param ptrFunction is the function pointer to a function that theorically will be called by thread; if no function pass CDTHREADFUNCTION_NOFUNCTION
	\return true actually thread function is the same indcated (ptrFunction), return false if also id invalid or thread destroyed 
	\version 1.00
*/
int cdthread_isFunction(cdThdID_t pThId, cdtreadFunctionType ptrFunction){
	int realidx;
	realidx = cdthread_getArrayIdxFromID(pThId);
	//if an error happened then return false
	if( realidx<0 ){
#ifdef  ENABLE_CDTHERRINF
		cdThErrInf.isFunction.realIdx_err++;
#endif  //ENABLE_CDTHERRINF
		return false;
	}
	//checks id thread is destroyed and if destroyed return false
	if(cdthreadsSystemArray[realidx].ID == CDTHREADID_ERROR){
#ifdef  ENABLE_CDTHERRINF
		cdThErrInf.isFunction.id_err++;
#endif  //ENABLE_CDTHERRINF
		return false;
	}
	//checks the callback thread function is different by indicates
	if(cdthreadsSystemArray[realidx].cdtreadFunction != ptrFunction)
		return false;
	return true;
} 



/*! \fn int cdthread_DestroyThread(cdThdID_t* pThId)
	\author Dario Cortese
	\date 09-08-2012, last mod 06-Apr-2013
	\brief destroy the indicated thread (memory are free for another new thread), and every message associated
	sign inside cdthreadsSystemArray that indicated position is unusable/deleted/free (ID==CDTHREADID_ERROR), after
	checks if there are messages for indicated thread and destroy all they.
	Lastest action is to set passed pThId to CDTHREADID_ERROR, so is unusable
	\param pThId is cdthread (id) to destroy
	\return false if a error accurred, or true if indicated thread has been deleted 
	\version 1.01 
*/
int cdthread_DestroyThread(cdThdID_t* pThId){
	int realidx;
	//check if isn't a valid id
	//next line unusefull because cdthread_getArrayIdxFromID check it and return -1 in case of error 
	//if( !cdthread_checkValidID( *pThId ) return false;		   //error for unusable id

	//convert pThId in real index for cdthreadsSystemArray
	realidx = cdthread_getArrayIdxFromID( *pThId);
	//if an error happened then return false
	if( realidx<0 ){ 
#ifdef  ENABLE_CDTHERRINF
		cdThErrInf.DestroyThread_realIdx_warning++;
#endif  //ENABLE_CDTHERRINF
		return false;
	}
	//sign this thread as unusable/overvwritable	
	cdthreadsSystemArray[realidx].ID= CDTHREADID_ERROR; //delete this thread and sign as unusable/overwritable
	if(cdthreadsSystemArray[realidx].Priority != CDTHREADPRIORITY_DISABLED){
   	cdthreadsSystemArray[realidx].Priority=CDTHREADPRIORITY_DISABLED;
	   cdThCOUNTERS.ENABLED--;
	}
#ifdef ENABLE_DELALLMSGWITHTHREADID
	//always destroy all messages associated to this thread
	cdmessage_deleteAllMsgWithThreadID( *pThId );
#else  //ifndef ENABLE_DELALLMSGWITHTHREADID
   //only there are pending messages then destroy they
	if(cdthreadsSystemArray[realidx].MessagesCounter > 0){
   	//destroy all messages associated to this thread
   	cdmessage_deleteAllMsgWithThreadID( *pThId );
	} 
#endif //else ifndef ENABLE_DELALLMSGWITHTHREADID
	*pThId = CDTHREADID_ERROR;	//force passed thread id to an unexisting thread id
   cdThCOUNTERS.DESTROYED++;
   cdThCOUNTERS.LOADED--;
	return true;
} 






/*! \fn int cdthread_isDestroyed(cdThdID_t pThId)
	\author Dario Cortese
	\date 06-03-2013
	\brief checks if passed id is destroyed (invalid pThId or pThId signed as destroyed) and if it is then return true
	\param pThId is cdthread (id) to check
	\return true if pThId is invalid or thread indicated by pThId is signed as destroyed
	\version 1.00
*/
int cdthread_isDestroyed(cdThdID_t pThId){
	int realidx;
	//convert pThId in real index for cdthreadsSystemArray
	realidx = cdthread_getArrayIdxFromID(pThId);
	//if an error happened then return true to indicate that thread doesn't exist
	if( realidx<0 ){
#ifdef  ENABLE_CDTHERRINF
		cdThErrInf.isDestroyed_realIdx_warning++;
#endif  //ENABLE_CDTHERRINF
		return true;
	}
	//checks if thread was destroyed
	if( cdthreadsSystemArray[realidx].ID == CDTHREADID_ERROR)
		return true;
	//checks if pointed thread by pThId is different by pThId; this operation have a very limited utility because happens only if allocated thread is a different type thread, so removed
	//if( cdthreadsSystemArray[realidx].ID != pThId) return true;
	return false;	//to indicates that thread exist
}


/*! \fn int cdthread_signAsFirtsTime(cdThdID_t pThId)
	\author Dario Cortese
	\date 09-08-2012
	\brief set firsttime flag as true for indicated thread
	sign inside cdthreadsSystemArray at indicated position that isTheFirstTime = true, so the called function will reinitialize itself .
	\param pThId is cdthread (id) to sign as firsttime
	\return false if a error accurred, or true if indicated thread has been signed as firsttime 
	\version 1.00
*/
int cdthread_signAsFirtsTime(cdThdID_t pThId){
	int realidx;
	//convert pThId in real index for cdthreadsSystemArray
	realidx = cdthread_getArrayIdxFromID(pThId);
	//if an error happened then return false
	if( realidx<0 ){
#ifdef  ENABLE_CDTHERRINF
		cdThErrInf.signAsFirtsTime.realIdx_err++;
#endif  //ENABLE_CDTHERRINF
		return false;
	}
	//checks id thread is destroyed and if destroyed return false
	if(cdthreadsSystemArray[realidx].ID == CDTHREADID_ERROR){
#ifdef  ENABLE_CDTHERRINF
		cdThErrInf.signAsFirtsTime.id_err++;
#endif  //ENABLE_CDTHERRINF
		return false;
	}
	cdthreadsSystemArray[realidx].isTheFirstTime= true; 
	return true;
} 



/*! \fn int cdthread_waitUpTo(cdThdID_t pThId, cdTimerID pWaitUpTo)
	\author Dario Cortese
	\date 16-09-2013
	\brief sets when this thread will be recall indicating the absolute timer value (wait until)
	sign inside cdthreadsSystemArray at indicated position that isTheFirstTime = true, so the called function will reinitialize itself .
	\param pThId is cdthread (id) 
	\param pWaitUpTo is a cdTimerID that act for the absolute timer value (equal or above) when this thread will be recall 
	\return false if a error accurred, or true if operation terminate with success
	\note for pWaitUpTo use macros cdtimer_setup_s, cdtimer_setup_ms, cdtimer_setup_us 
   \note it doesn't run if thread disabled
	\version 1.00
*/
int cdthread_waitUpTo(cdThdID_t pThId, cdTimerID pWaitUpTo){
	int realidx;
	//convert pThId in real index for cdthreadsSystemArray
	realidx = cdthread_getArrayIdxFromID(pThId);
	//if an error happened then return false
	if( realidx<0 ){
#ifdef  ENABLE_CDTHERRINF
		cdThErrInf.waitUpTo.realIdx_err++;
#endif  //ENABLE_CDTHERRINF
		return false;
	}
	//checks id thread is destroyed and if destroyed return false
	if(cdthreadsSystemArray[realidx].ID == CDTHREADID_ERROR){
#ifdef  ENABLE_CDTHERRINF
		cdThErrInf.waitUpTo.id_err++;
#endif  //ENABLE_CDTHERRINF
		return false;
	}
	cdthreadsSystemArray[realidx].WaitUpTo= pWaitUpTo;
   cdthreadsSystemArray[realidx].LoadTimVal = 0;   
   cdthreadsSystemArray[realidx].waitModeMask |= cdTh_waitModeTimer;
	return true;
} 

/*! \fn int cdthread_RunEveryTicks(cdThdID_t pThId, cdLoadTimer_t pTicks)
	\author Dario Cortese
	\date 01-05-2016
	\brief sets when this thread will be recall after a number of ticks, continuosly
	\param pThId is cdthread (id) 
	\param pTicks is a the number of system ticks to wait before this thread will be call/executed. this value will automatically reload every time  
	\return false if a error accurred, or true if operation terminate with success
	\note for pTicks use CDTIMER_us_FOR_TICK, CDTIMER_1ms_TICKS, CDTIMER_1s_TICKS, absolutely DON'T USE cdtimer_setup_s, cdtimer_setup_ms, cdtimer_setup_us  
   \note doesn't run if thread disabled
	\version 1.00
*/
int cdthread_RunEveryTicks(cdThdID_t pThId, cdLoadTimer_t pTicks){
	int realidx;
	//convert pThId in real index for cdthreadsSystemArray
	realidx = cdthread_getArrayIdxFromID(pThId);
	//if an error happened then return false
	if( realidx<0 ){
#ifdef  ENABLE_CDTHERRINF
		cdThErrInf.waitUpTo.realIdx_err++;
#endif  //ENABLE_CDTHERRINF
		return false;
	}
	//checks id thread is destroyed and if destroyed return false
	if(cdthreadsSystemArray[realidx].ID == CDTHREADID_ERROR){
#ifdef  ENABLE_CDTHERRINF
		cdThErrInf.waitUpTo.id_err++;
#endif  //ENABLE_CDTHERRINF
		return false;
	}
	cdthreadsSystemArray[realidx].WaitUpTo= cdtimer_setupTicks(pTicks);
   cdthreadsSystemArray[realidx].LoadTimVal = pTicks;   
   cdthreadsSystemArray[realidx].waitModeMask |= cdTh_waitModeTimer;
 	return true;
} 


/*! \fn int cdthread_WaitCycles(cdThdID_t pThId, int pCycles, int pContinuosly)
	\author Dario Cortese
	\date 01-05-2016
	\brief sets after many thread cycles (thread index rounds inside thread manager) will be call one time, or continuosly  
	\param pThId is cdthread (id) 
	\param pCycles is a the number of cycles (thread index rounds inside thread manager) to wait before run this thread  
	\param pContinuosly is a boolean, that true reload the number of cycle to wait every time counting is exausted; if false the counting doesn't restart (one-time)  
	\return false if a error accurred, or true if operation terminate with success
	\note doesn't run if thread disabled
	\warning for user managed thread the cycles is the time that call cdthread_UserCallThreadFunction for indicated thread
	\version 1.00
*/
int cdthread_WaitCycles(cdThdID_t pThId, int pCycles, int pContinuosly){
	int realidx;
	//convert pThId in real index for cdthreadsSystemArray
	realidx = cdthread_getArrayIdxFromID(pThId);
	//if an error happened then return false
	if( realidx<0 ){
#ifdef  ENABLE_CDTHERRINF
		cdThErrInf.waitUpTo.realIdx_err++;
#endif  //ENABLE_CDTHERRINF
		return false;
	}
	//checks id thread is destroyed and if destroyed return false
	if(cdthreadsSystemArray[realidx].ID == CDTHREADID_ERROR){
#ifdef  ENABLE_CDTHERRINF
		cdThErrInf.waitUpTo.id_err++;
#endif  //ENABLE_CDTHERRINF
		return false;
	}
	cdthreadsSystemArray[realidx].CountDown= pCycles;
   if(pContinuosly)
      cdthreadsSystemArray[realidx].LoadCountDown = pCycles;
   else
      cdthreadsSystemArray[realidx].LoadCountDown = 0;

   cdthreadsSystemArray[realidx].waitModeMask |= cdTh_waitModeCounts;
	return true;
} 


/*! \fn cdthread_WaitMessage(cdThdID_t pThId)
	\author Dario Cortese
	\date 01-05-2016
	\brief sets thread to wait almost a message before to be called  
	\param pThId is cdthread (id) 
	\return false if a error accurred, or true if operation terminate with success
	\note doesn't run if thread disabled
	\warning not available for user managed thread 
	\version 1.00
*/
int cdthread_WaitMessage(cdThdID_t pThId){
	int realidx;
	//convert pThId in real index for cdthreadsSystemArray
	realidx = cdthread_getArrayIdxFromID(pThId);
	//if an error happened then return false
	if( realidx<0 ){
#ifdef  ENABLE_CDTHERRINF
		cdThErrInf.waitUpTo.realIdx_err++;
#endif  //ENABLE_CDTHERRINF
		return false;
	}
	//checks id thread is destroyed and if destroyed return false
	if(cdthreadsSystemArray[realidx].ID == CDTHREADID_ERROR){
#ifdef  ENABLE_CDTHERRINF
		cdThErrInf.waitUpTo.id_err++;
#endif  //ENABLE_CDTHERRINF
		return false;
	}
   cdthreadsSystemArray[realidx].waitModeMask |= cdTh_waitModeMessage;
   
	return true;
} 



/*! \fn int cdthread_WaitEvent(cdThdID_t pThId, cdevent_t* pEventPtr)
	\author Dario Cortese
	\date 01-05-2016
	\brief sets that this thread will be called when the indicated event is generated (active and enabled)  
	\param pThId is cdthread (id) 
	\param pEventPtr is the pointer to the event that will be used to wakeup the thread  
	\return false if a error accurred, or true if operation terminate with success
	\note doesn't run if thread disabled
   \warning reset the cdevent inside the thread atherwise could happens that generated event isn't detected by thread manager and thread does not called
	\warning not available for user managed thread 
	\version 1.00
*/
int cdthread_WaitEvent(cdThdID_t pThId, cdevent_t* pEventPtr){
	int realidx;
	//convert pThId in real index for cdthreadsSystemArray
	realidx = cdthread_getArrayIdxFromID(pThId);
	//if an error happened then return false
	if( realidx<0 ){
#ifdef  ENABLE_CDTHERRINF
		cdThErrInf.waitUpTo.realIdx_err++;
#endif  //ENABLE_CDTHERRINF
		return false;
	}
	//checks id thread is destroyed and if destroyed return false
	if(cdthreadsSystemArray[realidx].ID == CDTHREADID_ERROR){
#ifdef  ENABLE_CDTHERRINF
		cdThErrInf.waitUpTo.id_err++;
#endif  //ENABLE_CDTHERRINF
		return false;
	}
   cdthreadsSystemArray[realidx].eventPtr = pEventPtr;
   cdthreadsSystemArray[realidx].waitModeMask |= cdTh_waitModeEvent;
   
	return true;
} 


/*! \fn int cdthread_WaitDisable(cdThdID_t pThId, int pDisableCode)
	\author Dario Cortese
	\date 01-05-2016
	\brief it disables wait function for thread, alls (if cdTh_NOwaitMode, so run always) or one specific (if cdTh_waitMode...)  
	\param pThId is cdthread (id) 
	\param pDisableCode is could be cdTh_NOwaitMode, cdTh_waitModeMessage, cdTh_waitModeEvent, cdTh_waitModeTimer, cdTh_waitModeCounts  
	\return false if a error accurred, or true if operation terminate with success
	\warning not available for user managed thread
   \see cdthread_RunEveryTicks, cdthread_WaitCycles, cdthread_WaitMessage, cdthread_WaitEvent    
	\version 1.00
*/
int cdthread_WaitDisable(cdThdID_t pThId, int pDisableCode){
	int realidx;
	//convert pThId in real index for cdthreadsSystemArray
	realidx = cdthread_getArrayIdxFromID(pThId);
	//if an error happened then return false
	if( realidx<0 ){
#ifdef  ENABLE_CDTHERRINF
		cdThErrInf.waitUpTo.realIdx_err++;
#endif  //ENABLE_CDTHERRINF
		return false;
	}
	//checks id thread is destroyed and if destroyed return false
	if(cdthreadsSystemArray[realidx].ID == CDTHREADID_ERROR){
#ifdef  ENABLE_CDTHERRINF
		cdThErrInf.waitUpTo.id_err++;
#endif  //ENABLE_CDTHERRINF
		return false;
	}
   if (pDisableCode == 0)
      cdthreadsSystemArray[realidx].waitModeMask = 0;
   else 
      cdthreadsSystemArray[realidx].waitModeMask &= ( ~ pDisableCode);
   
	return true;
} 




/*! \fn int cdthread_Enable(cdThdID_t pThId, int pPriority)
	\author Dario Cortese
	\date 09-08-2012
	\brief enable indicated Thread, and set indicated priority, only if previously disabled
	sign inside cdthreadsSystemArray at indicated position that Priority = pPriority.
	\n If pPriority is under 1 or is CDTHREADPRIORITY_DISABLED will be changed into equal to 1
	\param pThId is cdthread (id) to enable
	\param pPriority is the priority level that must be higher than 1 (more higher = more priority)
	\return false if a error accurred, or true if indicated thread has been disabled 
	\version 1.00
*/
int cdthread_Enable(cdThdID_t pThId, int pPriority){
	int realidx;
	
	if ((pPriority < 1) || (pPriority == CDTHREADPRIORITY_DISABLED)) 
	   pPriority=1;
	   
	//convert pThId in real index for cdthreadsSystemArray
	realidx = cdthread_getArrayIdxFromID(pThId);
	//if an error happened then return false
	if( realidx<0 ){
#ifdef  ENABLE_CDTHERRINF
		cdThErrInf.Enable_realIdx_err++;
#endif  //ENABLE_CDTHERRINF
		return false;
	}
	//chenges value only if thread was disabled
	if( cdthreadsSystemArray[realidx].Priority == CDTHREADPRIORITY_DISABLED){
   	cdthreadsSystemArray[realidx].Priority = pPriority; 
      cdThCOUNTERS.ENABLED++;
	}
	return true;
} 



/*! \fn int cdthread_Disable(cdThdID_t pThId)
	\author Dario Cortese
	\date 09-08-2012
	\brief disable indicated Thread 
	sign inside cdthreadsSystemArray at indicated position that Priority = CDTHREADPRIORITY_DISABLED
	\param pThId is cdthread (id) to disable
	\return false if a error accurred, or true if indicated thread has been disabled 
	\version 1.00
*/
int cdthread_Disable(cdThdID_t pThId){
	int realidx;
	//convert pThId in real index for cdthreadsSystemArray
	realidx = cdthread_getArrayIdxFromID(pThId);
	//if an error happened then return false
	if( realidx<0 ){
#ifdef  ENABLE_CDTHERRINF
		cdThErrInf.Disable_realIdx_err++;
#endif  //ENABLE_CDTHERRINF
		return false;
	}
	if(cdthreadsSystemArray[realidx].Priority != CDTHREADPRIORITY_DISABLED){
	   cdthreadsSystemArray[realidx].Priority= CDTHREADPRIORITY_DISABLED; //disable this thread
	   cdThCOUNTERS.ENABLED--;
	}
	return true;
} 



/*! \fn int cdthread_isEnabled(cdThdID_t pThId)
	\author Dario Cortese
	\date 10-08-2012
	\brief checks if indicated Thread is enabled (priority over 0) and return true if enabled 
	checks inside cdthreadsSystemArray at indicated position that Priority != CDTHREADPRIORITY_DISABLED
	\param pThId is cdthread (id) to disable
	\return true if enabled or false if disabled or an error happened
	\version 1.00
*/
int cdthread_isEnabled(cdThdID_t pThId){
	int realidx;
	//convert pThId in real index for cdthreadsSystemArray
	realidx = cdthread_getArrayIdxFromID(pThId);
	//if an error happened then return false
	if( realidx<0 ){
#ifdef  ENABLE_CDTHERRINF
		cdThErrInf.isEnabled_realIdx_err++;
#endif  //ENABLE_CDTHERRINF
		return false;
	}
	if( cdthreadsSystemArray[realidx].Priority == CDTHREADPRIORITY_DISABLED)
		return false;

	return true;
} 


/*! \fn int cdthread_getPriority(cdThdID_t pThId)
	\author Dario Cortese
	\date 10-08-2012
	\brief return the actual pripority of thread
	check inside cdthreadsSystemArray at indicated position the Priority 
	\param pThId is cdthread (id) to disable
	\return priority (0=disabled) or -1 if an error happened
	\version 1.00
*/
int cdthread_getPriority(cdThdID_t pThId){
	int realidx;
	//convert pThId in real index for cdthreadsSystemArray
	realidx = cdthread_getArrayIdxFromID(pThId);
	//if an error happened then return false
	if( realidx<0 ){
#ifdef  ENABLE_CDTHERRINF
		cdThErrInf.getPriority_realIdx_err++;
#endif  //ENABLE_CDTHERRINF
		return -1; 
	}
	return cdthreadsSystemArray[realidx].Priority;
} 


/*! \fn int cdthread_getPrevExitStatus(cdThdID_t pThId)
	\author Dario Cortese
	\date 10-08-2012
	\brief return the last exit state; if hasn't a previous exit state (first time executed) return 0. if an error happen return -1
	\param pThId is cdthread (id) to check
	\return -1 for an error, if is the first time return  0(default val)
	\note is good practice don't use -1 in the possible exit state, use negative exit state for errors, and positive values for normal exit
	\version 1.00
*/
int cdthread_getPrevExitStatus(cdThdID_t pThId){
	int realidx;
	//convert pThId in real index for cdthreadsSystemArray
	realidx = cdthread_getArrayIdxFromID(pThId);
	//if an error happened then return false
	if( realidx<0 ){
#ifdef  ENABLE_CDTHERRINF
		cdThErrInf.getPrevExitStatus_realIdx_err++;
#endif  //ENABLE_CDTHERRINF
		return -1; 
	}
	return cdthreadsSystemArray[realidx].LastExitState; 
} 










/*! \fn int cdthread_isThrereMessages(cdThdID_t pThId)
	\author Dario Cortese
	\date 09-08-2012
	\brief check if indicated thread has almost a message
	\param pThId is cdthread (id) to check
	\return true if there is a message, otherwise return false if there isn't messages or an error accurred
	\version 1.00
*/
int cdthread_isThrereMessages(cdThdID_t pThId){
	int realidx;
	//convert pThId in real index for cdthreadsSystemArray
	realidx = cdthread_getArrayIdxFromID(pThId);
	//if an error happened then return false
	if( realidx<0 )
	   return false; 
	if( cdthreadsSystemArray[realidx].MessagesCounter> 0)
	   return true; 
	return false;
} 




/*! \fn cdMsgID_t cdthread_getMessage(cdThdID_t pThId)
	\author Dario Cortese
	\date 09-08-2012
	\brief return the first message (id) on msg queue of indicated thread, and sign it as read; if there isn't or an error occours then return CDMESSAGEID_ERROR
	\param pThId is cdthread (id) to check
	\return the cdMsgID_t of available message, otherwise return CDMESSAGEID_ERROR if there isn't messages or an error accurred
	\note this function doesn't remove message from queue, and so if you doesn't remove it by cdthread_removeMessage(thid), next time reread this message
	\version 1.00
*/
cdMsgID_t cdthread_getMessage(cdThdID_t pThId){
	int realidx;
	cdMsgID_t msgid;
	//convert pThId in real index for cdthreadsSystemArray
	realidx = cdthread_getArrayIdxFromID(pThId);
	//if an error happened then return false
	if( realidx<0 ) 
	      return CDMESSAGEID_ERROR; 
	      
	if( cdthreadsSystemArray[realidx].MessagesCounter > 0){
		msgid = cdthreadsSystemArray[realidx].FirstMsgID;
		//sign indicated message as read, but doesnt remove it from queue
		cdmessage_sigMsgAsRead( msgid);
		return msgid;
	}
	return CDMESSAGEID_ERROR;
} 


/*! \fn int cdthread_removeMessage(cdThdID_t pThId)
	\author Dario Cortese
	\date 09-08-2012
	\brief remove first message from thread msg queue and return true if action has success, otherwise return false
	\param pThId is cdthread (id) to check
	\return true if remove msg from thread queue or false if an error happen
	\version 1.00
*/
int cdthread_removeMessage(cdThdID_t pThId){
	int realidx;
	cdMsgID_t msgid;
	//convert pThId in real index for cdthreadsSystemArray
	realidx = cdthread_getArrayIdxFromID(pThId);
	//if an error happened then return false
	if( realidx<0 )
	   return false; 
	if( cdthreadsSystemArray[realidx].MessagesCounter> 0){
		msgid = cdthreadsSystemArray[realidx].FirstMsgID;
		if (msgid == CDMESSAGEID_ERROR){
#ifdef ENABLE_DELALLMSGWITHTHREADID		   
		   cdmessage_deleteAllMsgWithThreadID(pThId);
#endif //ENABLE_DELALLMSGWITHTHREADID
		   cdthreadsSystemArray[realidx].MessagesCounter =0;
		}
		return cdmessage_deleteMsg( msgid );
	}
	return CDMESSAGEID_ERROR;
} 






/*! \fn int cdthread_setDescription(cdThdID_t pThId , const char* desc_ptr, int num_ch)
	\author Dario Cortese
	\date 24-01-2014
	\brief add description at indicated thread and return true if action has success, otherwise return false
	\param pThId is cdthread (id) to check
	\param desc_ptr is the origin description char array
	\param num_ch is the number of char that mu be copied; min 1, max 8
	\return true if description added at the indicated thread or false if an error happen
	\version 1.00
*/
   int cdthread_setDescription(cdThdID_t pThId , const char* desc_ptr, int num_ch)
   {
#ifdef ENABLE_THREAD_DESCRIPTION
   	int realidx;
      char* ptrch;
      int icount;
   	cdMsgID_t msgid;

   	if( num_ch < 1)  	   
         return false;
   	//convert pThId in real index for cdthreadsSystemArray
   	realidx = cdthread_getArrayIdxFromID(pThId);
   	//if an error happened then return false
   	if( realidx<0 )
   	   return false;
   	   
      if( num_ch > 8)
         num_ch = 8;
         
   	ptrch = &cdthreadsSystemArray[realidx].Description[0];
   	//simple copy without checks
   	for(; num_ch>0; num_ch--){
      	*ptrch = *desc_ptr;
        	ptrch++; desc_ptr++;
   	}   	
   	//always terminator char at the end of copy
   	*ptrch =0; 
#endif //ENABLE_THREAD_DESCRIPTION
   	return true; 
   }	



   
#endif //_CDTHREAD_C_

