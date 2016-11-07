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
//// Created on 02/08/2012                                              ////
//// File: cdthread.h                                                   ////
//// Description:                                                       ////
////    This file has the variable, structure and function definition   ////
////     to manage a generic cdthread ().                               ////
////                                                                    ////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
//////////////////////////////THREADS///////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
//// THIS TYPE OF THREAD IS A STRUCTURE THAT MANAGES COOPERATIVE THREADS.///
//// EVERY THREAD COULD BE ENABLED OR DISABLED AND, TODAY, DOESN'T HAVE ////
////   PRIORITY.                                                        ////
//// NO PRIORITY MEANS THAT EVERY THREAD ENABLED, IS EXECUTED BY THREAD ////
////   ENGINE IN SEQUENCE.                                              ////
//// THE RULE IS THAT THREAD ENGINE,WHEN CALLED, EXECUTE IN SEQUENCE    ////
////   ONLY ACTIVE, AND NON USER, THREADS.                              ////
//// Remember that thread engine manager function, cdthread_Engine(),   ////
//// must be called continuosly, so place it in main infinite loop....  ////
//// REMEMBER TO INITIALIZE THREAD SYSTEM ARRAY BEFORE TO CALL FIRST    ////
////   TIME A THREAD OR THE ENGINE MANAGER FUNCTION                     ////
////                                                                    ////
////   void main(void){                                                 ////
////     .......                                                        ////
////     cdthread_initAll();                                            ////
////     .......                                                        ////
////     while(true){                                                   ////
////       .......                                                      ////
////       cdthread_Engine();                                           ////
////       .......                                                      ////
////     }                                                              ////
////   }                                                                ////
////                                                                    ////
//// THE THREAD ENGINE WHEN MEETS A ACTIVE, AND ENGINE MANAGED, THREAD  ////
////   CALL THE FUNCTION INDICATED BY THREAD_FUNCTION POINTER, AFTER    ////
////   WAIT THE ENDS OF THAT FUNCTION (COLLABORATIVE SYSTEM) AND STORE  ////
////   RETURN VALUE.                                                    ////
//// AFTER THAT ANALYZE ITS THREAD LIST AND EXECUTES THE NEXT ACTIVE    ////
////   THREAD....AND SO ON INFINITELY.                                  ////
//// The thread structure has a Priority field but today is used only   ////
////   as boolean to enable/disable thread, but in the CDThread.h       ////
////   there is a function  cdthread_ThreadToRun(....) that can be      ////
////   rewrited to manage priority or change the rule that run threads  ////
////                                                                    ////
//// TO INDICATES THE NUMBER OF MAXIMUN THREAD ALLOWED BY SYSTEM, and   ////
////   so the memory usage allocated for thread (approx. 32bytes on     ////
////   32bits system for every thread), there is a define               ////
////    CDTHREAD_MAX_NUM_THREADS that must be changed by necessity      ////
////                                                                    ////
//// THE THREADS MUST BE ASSIGNED TO THREAD ENGINE OR COULD BE USER     ////
////   THREAD, AND IN THIS CASE THE THREAD ENGINE SKIPS THEIR ANALYSYS  ////
////   AND EXECUTION.                                                   ////
//// To create and register a new thread for engine use...              ////
////   cdThdID_t ThreadID= CDTHREADID_NOTHREAD;                         ////
////   int allOk; //to know if callfunction was called                  ////
////   allOk = cdthread_new(&ThreadID, functionForThread);              ////
//// To create and register a new thread for user (not used in engine)  ////
////   allOk = cdthread_newUserManaged(&ThreadID, FunctionForThread);   ////
////                                                                    //// 
//// To give information (eight chars) about thread, or connected       ////
////    function, is available the following function                   //// 
////     allOk = cdthread_setDescription(ThreadID, "descript", 8)       ////
//// In this example "descript" is the eight char of description and    ////
////  number 8 is the quantity of char to be stored for description     ////     
////                                                                    ////
//// This functions could return a invalid cdThdID_t to indicates error////
////   use the cdthread_isErrorID(ThreadID) macro to check this invalid ////
////   id, it returns true if id is invalid                             ////
////                                                                    ////
//// To changes only called function by indicated thread without change ////
////   any other thread fields use...                                   ////
////   int allOk; //to know if callfunction was called                  ////
////   allOk = cdthread_changeFunction(ThreadID, FunctionForThread);    ////
//// To call by user code a user managed thread (or a engined thread)   ////
////   use...                                                           ////
////   int fctExitCode;	//used to know exit code of callback function   ////
////   int allOk; //to know if callfunction was called                  ////
////   allOk = cdthread_UserCallThreadFunction(ThreadID, &fctExitCode); ////
////                                                                    ////
//// To know if a thread is user managed or engined use...              ////
////   cdthread_isEngineManagedID(ThreadID) ,returns true if engined or ////
////   cdthread_isUserManagedID(ThreadID) ,returns true if user managed ////
////                                                                    ////
//// To destroy/unregister thread, and release memory for another one.. ////
////   cdthread_DestroyThread( &ThreadID);                              ////
////                                                                    ////
//// To check if ThreadID refers to a destroyed thread use..            ////
////   int isKo; //to know if destroyed, true=destroyed, false=exist		////
////   isKo = cdthread_isDestroyed(ThreadID);                           ////
////                                                                    ////
//// To check if ThreadID exist (valid and thread not destroyed)        ////
////   int allOk; //to know if function exist; true=exist               ////
////   allOk = cdthread_Exist(ThreadID);                                ////
////                                                                    ////
//// HOW SAY BEFORE THERE IS THE POSSIBILITY TO ENABLE, OR DISABLE, THE ////
////   THREAD. WHEN THREAD IS DISABLED CAN'T BE EXECUTED BY ENGINE OR   ////
////   CALLED BY cdthread_UserCallThreadFunction(...) FUNCTION.         ////
//// To enable thread, or change its priority, use ...                  ////
////   int Priority = CDTHREADPRIORITY_ENABLED; //lower enabled priority////
////   allOk = cdthread_Enable(ThreadID, Priority);                     ////
//// To disable thread use ...                                          ////
////   allOk = cdthread_Disable(cdThdID_t pThId);                      ////
////                                                                    ////
//// To know if thread is enabled use the function....                  ////
////  cdthread_isEnabled(ThreadID) that return a true if enabled        ////
//// To read the thread priority use...                                 ////
////   Priority = cdthread_getPriority(ThreadID);                       ////
////                                                                    ////
////                                                                    ////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
//////////////////////////THREAD MESSAGES///////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
//// EVERY THREAD COULD RECEIVE MESSAGES, EVERY MESSAGE HAVE A UNIQUE ID////
////   AN INFO VALUE AND A MEMORY POINTER TO OPTIONAL DATA.             ////
//// THE REAL SIZE (in byte, word, etc) IS UNKNOWED, THE ONLY           ////
////   INFORMATION ABOUT SIZE IS THE NUMBER OF LOGICAL DATA STORED; THIS////
////   MEANS THAT REAL TYPE, SIZE, ORDER, ETC, OF DATA MUST BE KNOWED   ////
////   BY RECEIVER.                                                     ////
//// there is a define that indicates the maximun system message number ////
////    CDTHREAD_MAX_NUM_MESSAGES that must be changed by necessity     ////
////                                                                    ////
//// REMEMBER TO INITIALIZE MESSAGES SYSTEM ARRAY BEFORE TO CALL FIRST  ////
////   TIME A THREAD OR THE ENGINE MANAGER FUNCTION                     ////
////                                                                    ////
////   void main(void){                                                 ////
////     .......                                                        ////
////     cdmessage_initAll();                                           ////
////     cdthread_initAll();                                            ////
////     .......                                                        ////
////     while(true){                                                   ////
////       .......                                                      ////
////       cdthread_Engine();                                           ////
////       .......                                                      ////
////     }                                                              ////
////   }                                                                ////
////                                                                    ////
//// TO CREATE A NEW MESSAGE FOR A SPECIFIC THREAD USE...               ////
////   cdMsgID_t msgID;                                               ////
////   int info= 1;	//this is the code for message used inside      ////
////                    //the thread to determine which message is      ////
////   int NumData=10;	//this is the number (logical, not the size in  ////
////                    //byte) of data                                 ////
//// IS VERY IMPORTANT THAT DATA DON'T STAY IN STACK OR VOLATILE ZONE   ////
//// BECAUSE DATA AREN'T COPIED IN THE MESSAGE, BUT ONLY A POINTER IS   ////
//// STORED IN THE MESSAGE                                              ////
////   static int msgData[10]={1,2,3,4,5,6,7,8,9,10}; //the data must be////
////                    //primitive, structure or other, the important  ////
////                    //is that receiver know what is it              ////
////   msgID = cdmessage_new(Threadid,Info,NumData,(void*) msgData );   ////
//// THE MESSAGE ID COULD BE INVALID, THIS IS USED TO ADVISE THAT AN    ////
//// ERRROR WAS HAPPENED.                                               ////
//// To detect invalid msgID use macro cdmessage_isErrorID(msgID) that  ////
////   returns true if id is invalid/error                              ////
////                                                                    ////
//// TO DETECT IF THERE IS A MESSAGE INSIDE THE TREAD FUNCTION USE...   ////
////   cdthread_isThrereMessages(ThisThread) that returns true if there ////
////                                         is one or more messages    ////
//// TO READ THE MESSAGE ID, AND SO READS ITS DATA, INSIDE THE THREAD   ////
////   FUNCTION use...                                                  ////
////   cdMsgID_t msgID;                                                 ////
////   msgID = cdthread_getMessage(ThisThread);                         ////
//// IT SIGN ALSO THE MESSAGE AS READ, SO EXTERNALLY COULD BE DETECTED  ////
//// To detect externally to thread function the happened read message..////
////   cdmessage_isRead(msgID); //msgID returned when created the msg,  ////
////                              //this function return true if message////
////                              //was read or already deleted         ////
////                                                                    ////
//// TO DELETE A READ MESSAGE INSIDE THE THREAD FUNCTION USE....        ////
//// allOk = cdthread_removeMessage(ThisThread);                        ////
//// To detect externally to thread function the message deletion use   ////
//// cdmessage_isDeleted(msgID); //return true if message was deleted   ////
////                                                                    ////
//// IS POSSIBLE DELETE A MESSAGE ALSO OUTSIDE THE THREAD FUNCTION USING////
//// allOk = cdmessage_deleteMsg(msgID);  //this delete message from    ////
////                                      // message system array and   ////
////                                      // also remove message from   ////
////                                      // connected thread queue     ////
//// Following function delete all messages in the queue of a thread    ////
////   allOk = cdmessage_deleteAllMsgWithThreadID(ThreadID);            ////
//// or inside thread function....                                      ////
////   allOk = cdmessage_deleteAllMsgWithThreadID(ThisThread);          ////
////                                                                    ////
//// TO READ AND USE MESSAGE FIELD, INSIDE THREAD FUNCTION USE FOLLOWING////
//// FUNCTIONS.                                                         ////
//// Remember that msgID is get by cdthread_getMessage(ThisThread)      ////
//// To read info field, that is used to identify message, use....      ////
////   int info;                                                        ////
////   info = cdmessage_getInfo(msgID);                                 ////
//// To read number of logical data use....                             ////
////   int NumData;                                                     ////
////   NumData = cdmessage_getNumData(msgID);                           ////
//// To read the memory pointer to message data (remeber that receiver  ////
////   must know the type or structure of data, for this is used info   ////
////   and number of data fields) use....                               ////
////   int* array;	//suppose that message data is an int array         ////
////   array = (int*)cdmessage_getDataPointer(msgID);		                ////
////                                                                    ////
////                                                                    ////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
///////////////////THREAD CALLBACK FUNCTION/////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
//// THE THREAD_FUNCTION (CALLBACK FUNCTION CALLED BY THREAD) HAS TWO   ////
////   PARAMETERS, THE FIRST IS A BOOLEAN (INT), AND IF IT IS TRUE THAN ////
////   INDICATES THAT IS THE FIRST TIME THAT FUNCTION IS CALLED.        ////
//// IN THIS MODE THE THREAD FUNCTION COULD INITIALIZE THEIR VARIABLES, ////
////   MEMORY AND OBJECT.                                               ////
//// To force this variable to be true next time thread function will be////
////   called, so it reinitialize itself, use...                        ////
////   allok = cdthread_signAsFirtsTime(ThreadID);                      ////
////                                                                    ////
//// THE SECOND PARAMETER IS A cdThdID_t THAT IS THE ID OF THE THREAD   ////
////   CALLER, THIS IS USEFULL WHEN USE THREAD FUNCTIONS INSIDE CODE.   ////
////   is good practice call this parameter "ThisThread" as in example  ////
////                                                                    ////
//// int ThreadFunctionSample(int firstTime, cdThdID_t ThisThread, ...  ////
////                          int prevExitCode){                        ////
//// ....}                                                              ////
////                                                                    ////
//// THE THREAD FUNCTION MUST RETURN A VALUE THAT IS EXIT STATUS; THIS  ////
////   PREVIOUS STATUS MUST BE READ BY THREAD FUNCTION, SO IT CAN       ////
////   ITS PREVIOUS EXIT STATUS.                                        ////
//// To read previous exit status, or last exit status of a thread, use ////
////   int lastExitValue;                                               ////
////   lastExitValue = cdthread_getPrevExitStatus(ThreadID);            ////
//// or if is in the thread callback function is more right write...    ////
////   lastExitValue = cdthread_getPrevExitStatus(ThisThread);          ////
////                                                                    ////
//// PLEASE SEE THE CALLBACK FUNCTION EXAMPLE AT THE END OF THIS        ////
////  INTRODUCTION TO SEE HOW WRITE IT                                  ////
////////////////////////////////////////////////////////////////////////////
///////////////////TIMER FUNCTIONS AND MACRO////////////////////////////////
////////////////////////////////////////////////////////////////////////////
//// TO USE TIMER IS ENOUGH CREATE A cdTimerID VARIABLE AND ASK IN THE  ////
////   CODE WHEN THAT VARIABLE IS EXPIRED; SEE FOLLOWING EXAMPLE        ////
//// TO SET THAT VARIABLE WITH EXPIRATION VALUE YOU CAN USE             ////
////   cdtimers_setupTicks( num of ticks)                               ////
//// THAT CALCULATES NUM OF TICKS THAT MUST SCORE FROM CALL AT cdtimers_////
////   setupTicks(ticks) TO RETAIN TIMER ELAPSED.                       ////
////   cdtimers_setupus(us), cdtimers_setupms(ms), cdtimers_setup_s(s)  ////  
//// THAT INDICATES TIME IN MICROSECONDS, MILLISECONDS AND SECONDS      ////
//// TO KNOW HOW MANY TICKS THERE ARE IN A 1ms THERE IS A DEFINE:       ////
////   CDTIMER_1ms_TICKS(num of ms).                                    ////
//// ANOTHER DEFINE INDICATES HOW MANY MICROSECONDS THERE ARE IN A TICK ////
////   AND THIS DEFINE IS:                                              ////
////   CDTIMER_us_FOR_TICK(num of ticks)                                ////
//// TO CHECK IF TIME IS ELAPSED AND TIMER EXPIRED USE:                 ////
////   cdtimer_isExpired(cdTimerID) and cdtimer_isNotExpired(cdTimerID) ////
////                                                                    ////
//// GENERIC CDTIMER EXAMPLE:                                           ////
//// cdTimerID myTimer1, myTimer2, myTimer3, myTimer4;                  ////
//// myTimer1 = cdtimer_setupTicks(20);  //timer value for wait 20ticks////
//// myTimer2 = cdtimer_setup_s(1);	  //timer value for wait 1second////
//// myTimer3 = cdtimer_setupms(10);	  //timer value for wait 10ms   ////
//// myTimer4 = cdtimer_setupus(50);	  //timer value for wait 50us   ////
//// while( cdtimer_isNotExpired(myTimer1) ){                           ////
////   ....                                                             ////
////   if( cdtimer_isExpired(myTimer2){                                 ////
////     ....                                                           //// 
////   }                                                                ////
////   ....                                                             ////
//// }                                                                  ////
////                                                                    ////
//// WHEN CDTIMER IS INSIDE A THREAD REMEBER TO INDICATES CDTIMERID     ////
////   STATIC.                                                          ////
//// THREAD CDTIMER EXAMPLE:                                            ////
////   static cdTimerID myTimer1=0;                                     ////
////   if( myTimer1!=0 ) goto LABEL_timer_here;	                        //// 
////   //place here code to execute before start timer                  ////
////   myTimer1 = cdtimer_setupms(1); //timer value for wait 1ms        ////
//// LABEL_timer_here:                                                  //// 
////   if( cdtimer_isNotExpired(myTimer1) ) return 0;                   ////
////   //place here code to execute when timer expired                  ////
////     ....                                                           ////

 
////  ANOTHER WAY TO USE TIMER IS TO SUSPEND THREAD UNTIL THE ABOSLUTE  ////
////   TIMER BECAME UQUAL OR GOES UP THE INDICATED VALUE AT THE FUNCTION////
////   cdthread_waitUpTo(ThreadID, myTimer);                            ////
////  Sets myTimer = 0 if you want disable the suspend function         ////
////  IF YOU WANT A PERIODICAL CALL, REMEMBER TO CALL cdthread_waitUpTo ////
////    INSIDE THREAD FUNCTION.                                         ////
////  For Example if you want call thread every 5ms write following line////
////    in the head of thread function:                                 //// 
////  cdthread_waitUpTo(ThisThread, cdtimer_setupms(5) );               ////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////


/* FOLLOWIN THREAD FUNCTION ONLY FOR EXAMPLE 
int CDThreadFunction_Sample(int firstTime, cdThdID_t ThisThread, int prevExitCode){
	cdMsgID_t actMsg;
	int msgInfo;
	int thLastStatus;
	
	//-**********************************************
	//from here setup code 
	if(firstTime){
		//if this function is called first time then needs to set its variables;
		//setup code
	
		//HERE INSERT CODE FOR SETUP AND INITIALIZATION
		
	}//end else if(firstTime)
	
	//-**********************************************
	//from here messages manager 
	//first think check if there is messages
	if(cdthread_isThrereMessages(ThisThread)){
		//read message
		actMsg= cdthread_getMessage(ThisThread); 
		
		msgInfo = cdmessage_getInfo(actMsg);
		switch(){
			case 1:
				//HERE INSERT CODE FOR MESSAGE 1
				break;
			case 2:
				//HERE INSERT CODE FOR MESSAGE 2
				break;
				.
				.
			default:
				//HERE INSERT CODE FOR UNKNOW MESSAGES
		. }
		
		
		//when finisched to manage message destroy it
		cdthread_removeMessage(ThisThread);      
	}//end if(cdthread_isThrereMessages(ThisThread))
	
	//-**********************************************
	//from here jump table...
	switch(prevExitCode){
		case 0: goto LABEL_START;  break;
		case 1: goto LABEL_STATE1; break;
		case 2: goto LABEL_STATE2; break;
		case 3: goto LABEL_STATE3; break;
		.
		.
		.
		.
	}

	//-**********************************************
	//from here run time code (no setup code, no reads messages) 
LABEL_START:
	.... //code for prev exit state 0 (default)
	return 1;	
LABEL_STATE1:
	.... //code for prev exit state 1 (default)
	return 2;
LABEL_STATE2:
	.... //code for prev exit state 2
	return 3;	
LABEL_STATE3:
	.... //code for prev exit state 3
	....
	return n;
	.... //code for prev exit state n
	 	
	//HERE INSERT CODE FOR RUNTIME; TYPICALLY USES LAST STATUS TO DETERMINE OPERATION TO DO (PROVATE STATE MACHINE)
	return 0;
}
*/
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#ifndef _CDTHREAD_H_
#define _CDTHREAD_H_


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////



//following define is used to activate and fill cdMsgErrInf structure data; this structure counts errors in the cdMessage functions
//please remove define at the end of debug phases seen that has no usefulness and adds line codes and waste of time
#define ENABLE_CDMSGERRINF

//followind define is used to activate and fill cdThErrInf structure data; this structure counts errors in the cdthread functions
//please remove define at the end of debug phases seen that has no usefulness and adds line codes and waste of time
#define ENABLE_CDTHERRINF

//following define is used to enable cdmessage_deleteAllMsgWithThreadID that is a safe function called when
// a thread is destroyed, or when a message counter became zero inside e maessage_delte function.
//When avery module and permutation of all conditions test are checked (debug mode) and error counters show zero error then is
// possible, and suggested, to disable (not define the ENABLE_DELALLMSGWITHTHREADID) the cdmessage_deleteAllMsgWithThreadID 
#define ENABLE_DELALLMSGWITHTHREADID

//following define is used to enable the availablety of 8 chars in the structure to describle the thread or pointed function.
//This could be very usefull when you are in debug to identify the thread or function inside; in contrast this option increase
// memory usage and thread time creation to register the string
#define ENABLE_THREAD_DESCRIPTION
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////



#ifdef DOXYGEN
    #define section( YY )
#endif


#include "CD_types.h"
#include "CDEvents.h"

/*! \def CDTHREAD_MAX_NUM_THREADS
	used to indicates the absolute maximun number of thread that the system could have (thread array size)
*/
#define  CDTHREAD_MAX_NUM_THREADS	16

/*! \def CDTHREAD_MAX_NUM_MESSAGES
	used to indicates the absolute maximun number of messages that the system could have (thread array size)
*/
#define  CDTHREAD_MAX_NUM_MESSAGES	32


typedef int cdMsgID_t;	//!< cdmessage id type, used to store id message that is used by every function

typedef int cdThdID_t;	//!< cdthread id type, used to store id message that is used by every function

typedef uint64_t cdTimerID; //!< the cdTimerID is not a simple ID but contains the value when timer expires (expeted absolute timer value)

typedef uint32_t cdLoadTimer_t; //!< the is the number of tick that must be used to load at cdTimerID in autorelaod

//*********************************************************************
//*************messages************************************************
//*********************************************************************


typedef struct cdMessageStruct_tag{
	cdThdID_t cdthID ;	 //!< a number that identify unequivocal to which thread is associated this message
	int State;		 	 //!< 2=new message, 1=read but do not delete, 0=was managed and so is deletable 
	void* ptrData;	 	 //!< pointer to memory where is stored data for this message
	int NumData; 		 //!< number of elements (it isn't fixed dimension) intended as logical number of elements that compose message (ie: if is 32bit array of 16 elements, this value is 16, but is 16 also for an array of byte with 16 bytes)
	int Info; 		 	 //!< a information value, is optional and is a user value used to inform receiver
	cdMsgID_t NextMsgID;	 //!< Next message ID, if pointer is 0 or less (-1 and so on) means no previous message
	cdThdID_t cdthSenderID ;	 //!< a number that identify unequivocal to which thread is sender of this message
}cdmessageStruct;



/*!	\var cdmessageStruct cdmessagesSystemArray
    is the system array that stores the cdmessage structure used to manage messages
*/
extern cdmessageStruct cdmessagesSystemArray[CDTHREAD_MAX_NUM_MESSAGES];





/*! \def CDMESSAGEDATA_NODATA
	used in cdmessageStruct.ptrData to assign a no available pointer to data
*/
#define CDMESSAGEDATA_NODATA		(void*)0x00000000



/*! \def CDMESSAGESTATE_NEWMSG
	used in cdmessageStruct.State to indicates that message is in construction/loccked so also deletion is forbidden
*/
#define CDMESSAGESTATE_LOOCKED  255

/*! \def CDMESSAGESTATE_NEWMSG
	used in cdmessageStruct.State to indicates that message is a new message (never read)
*/
#define CDMESSAGESTATE_NEWMSG  2

/*! \def CDMESSAGESTATE_READ
	used in cdmessageStruct.State to indicates that message is read but not deleted
*/
#define CDMESSAGESTATE_READ  1

/*! \def CDMESSAGESTATE_DELETED
	used in cdmessageStruct.State to indicates that message is read and destroyed, so is reusable and overvritable
*/
#define CDMESSAGESTATE_DELETED 0



/*! \def CDMESSAGESTATE_DELETED
	used in cdmessageStruct.NextMsgID to indicates that there isn't other messages after this, and also is used...
	\n used in every function that return cdMsgID_t to indicates an invalid ID and at the same time an error
*/
#define CDMESSAGEID_ERROR 	0



/*! \def cdmessage_isErrorID(x)
	macro used to check if cdMsgID_t 'x' is an error id (==CDMESSAGEID_ERROR)
*/
#define cdmessage_isErrorID(x)		( x == CDMESSAGEID_ERROR)


/*! \def cdmessage_checkValidID(x)
	checks if cdMsgID_t 'x' is a valid ID, this means that isn't a CDMESSAGEID_ERROR and is inside from 0 to CDTHREAD_MAX_NUM_MESSAGES.
	\n return true if is valid otherwisw return false
*/
#define cdmessage_checkValidID( YY )	(( YY != CDMESSAGEID_ERROR)&&( YY >=0)&&( YY <= CDTHREAD_MAX_NUM_MESSAGES))



//******************************************************************************************************


void cdmessage_initAll(void); //!< must be called by initialization main to reset and init system messages array

cdMsgID_t cdmessage_getFreeID(); //!< searh free/available position in system message array
int cdmessage_getArrayIdxFromID(cdMsgID_t pMsgId); //!< returns the real index for cdmessagesSystemArray extracted from indicated cdMsgID_t (pMsgId)

cdMsgID_t cdmessage_new(cdThdID_t pThreadIDdest, int pInfoVal, int pNumData, void* ptrData, cdThdID_t pThreadIDorg ); //!< creates new message, and adds it to message queue of indicated thread 
int cdmessage_deleteMsg( cdMsgID_t pMsgId );  			//!< delete  this message from messages system array and remove it from message queue of connected thread (only if is the first msg of queue)

int cdmessage_deleteAllMsgWithThreadID( cdThdID_t pThId ); //!< delete  all messages in the message system array that have cdthID equal pThId

int cdmessage_sigMsgAsRead( cdMsgID_t pMsgId );     //!< sign this message as read, if action has success then return true

int cdmessage_isRead( cdMsgID_t pMsgId );           //!< checks if this message was read and returns true, this means that could be only read or yet deleted; returns false also an error happen
int cdmessage_isDeleted( cdMsgID_t pMsgId );        //!< check if this message was deleted and return true otherwise return false (also an error happen)

int cdmessage_getNumData( cdMsgID_t pMsgId );       //!< return the number of logical data attached to message; if there isn't, or an error happens, return 0 
void* cdmessage_getDataPointer( cdMsgID_t pMsgId ); //!< return pointer to memory where is the data attached to indicated message; if there isn't, or an error happens, return CDMESSAGEDATA_NODATA 
int cdmessage_getInfo( cdMsgID_t pMsgId );          //!< return the info value for indicated message
cdThdID_t cdmessage_getSenderThread( cdMsgID_t pMsgId ); //!< return the cdThdID_t of Thread that has sent the message; if there isn't, or an error happens, return CDTHREADID_ERROR 


//*********************************************************************
//*************thread *************************************************
//*********************************************************************

//! function pointer to the thread code, return the exit state (int) and has two param, first is 'first time' boolean (int), second is a cdThdID_t of calling thread
typedef int (*cdtreadFunctionType)(int, cdThdID_t, int); //


//this is struct type to manages thread
typedef struct cdthreadStruct_tag{
	cdThdID_t ID;		 			//!< a number that identify unequivocal this thread structure; used in all cdthread function. 0=unused/free, positive ID managed Thread Engine, negative not managed by Engine and used only by user
	int Priority;		 			//!< 0 means disabled, 1 or more indicates that is enabled and the priority level (higher value-> higher priority) 
	cdtreadFunctionType cdtreadFunction;//!< function pointer to the thread code, return the exit state (int) and has two param, first is 'first time' boolean (int), second is a cdThdID_t of calling thread
	int LastExitState;				//!< indicates the returned value when exiting from *cdtreadFunction; usefull when reenter to know last state.
	int isTheFirstTime;	 			//!< boolean, if true indicates that is the first time that the cdtreadFunction is called.
	int MessagesCounter; 	 		//!< indicates the number of messages available for this thread; 0 means no msg. The queue is LIFO type
	cdMsgID_t FirstMsgID;			//!< msgid of first message; if -1 means no messages 
	cdMsgID_t LastMsgID;			//!< msgid of last available message; if -1 means no messages 
	cdTimerID WaitUpTo;        		//!< indicates to wait until aboslute timer is over indicated value; 0 means doesn't wait       
	int waitModeMask;				//!< indicates  what the thread must waits to run 
	cdLoadTimer_t LoadTimVal;		//!< indicates the quantity of ticks to add at WaitUpTo
	cdevent_t* eventPtr;			//!< the pointer to the event that must waits; used with 
	int CountDown;					//!< the decrementing counter for times that this thread should be called (but not ran) 
	int LoadCountDown;				//!< 0 disabled, otherwise the value to reload in countdown when countdown elaps (on if wait counter mode enabled)
#ifdef ENABLE_THREAD_DESCRIPTION
   char Description[9];       //!< eight chars (last used for termination char) used to store a short description of thread or pointed function 
#endif //ENABLE_THREAD_DESCRIPTION
}cdthreadStruct;

//this value means nothing to wait 
#define cdTh_NOwaitMode    			0x00
//if it is set in the waitModeMask indicates to skip to run thread until almost a message (MessagesCounter > 0) is ready
#define cdTh_waitModeMessage			0x01
//if it is set in the waitModeMask indicates to skip to run thread until the specific ( eventPtr ) will be active/generated
#define cdTh_waitModeEvent				0x02
//if it is set in the waitModeMask indicates to skip to run thread until WaitUpTo timer is elapsed
#define cdTh_waitModeTimer				0x04
//valid only if cdTh_waitModeTimer is set, in this case indcates to reload WaitUpTo timer with Wait
#define cdTh_waitModeCounts			0x08



/*!	\var cdthreadStruct cdthreadsSystemArray
    is the system array that stores the cdthread structure used to manage threads
*/
extern cdthreadStruct cdthreadsSystemArray[CDTHREAD_MAX_NUM_THREADS];


/*!	\var cdThdID_t LASTCALLEDTHREADIDBYENGINE
    used to inform extern code which is the last executed thread function called by thread_engine
*/
extern cdThdID_t LASTCALLEDTHREADIDBYENGINE; 

/*!	\var cdThdID_t LASTCALLEDTHREADIDBYUSER
    advise external code which is the last called thread by user
*/
extern cdThdID_t LASTCALLEDTHREADIDBYUSER;	





/*! \def CDTHREADID_ERROR
	used to indicates that cdThdID_t that have this value are unusable ID, so this value is also used an error in functions that return cdThdID_t
*/
#define CDTHREADID_ERROR	0
#define CDTHREADID_NOTHREAD	0

/*! \def cdthread_isEngineManagedID(x)
	is a macro that check if cdThdID_t 'x' is Engine managed cdthread; return true if is Engine managed
*/
#define cdthread_isEngineManagedID(x)	( x > CDTHREADID_ERROR)

/*! \def cdthread_isUserManagedID(x)
	is a macro that check if cdThdID_t 'x' is user managed cdthread; return true if is user managed
*/
#define cdthread_isUserManagedID(x)		( x < CDTHREADID_ERROR)

/*! \def cdthread_isErrorID(x)
	is a macro that check if cdThdID_t 'x' is no usable cdThdID_t (errorID) ; return true if is unusable 
*/
#define cdthread_isErrorID(x)		( x == CDTHREADID_ERROR)

/*! \def cdthread_checkValidID(x)
	checks if cdThredID 'x' is a valid ID, this means that isn't a CDTHREADID_ERROR and is inside from -CDTHREAD_MAX_NUM_THREADS to CDTHREAD_MAX_NUM_THREADS.
	\n return true if is valid otherwis return false
*/
#define cdthread_checkValidID( YY)	(( YY !=CDTHREADID_ERROR)&&( YY <=CDTHREAD_MAX_NUM_THREADS)&&( YY >=(CDTHREAD_MAX_NUM_THREADS*(-1))))




/*! \def CDTHREADFUNCTION_NOFUNCTION
	used to indicates that there is associated function to pointer; it's a 0 address 
	used also as parameter in function like cdthread_new() and cdthread_newUserManaged to pass a CDTHREADFUNCTION_NOFUNCTION address for tread function
*/
#define CDTHREADFUNCTION_NOFUNCTION		(cdtreadFunctionType)0x00000000




/*! \def CDTHREADPRIORITY_DISABLED
	indicates the value for cdthreadStruct.Priority to indicates at the system that tread is disabled
*/
#define CDTHREADPRIORITY_DISABLED		0
#define CDTHREADPRIORITY_ENABLED		1




//******************************************************************************************************

void cdthread_initAll(void); //!< must be called by initialization main to reset and init system threads array

cdThdID_t cdthread_getFreeID(void);                           //!< searchs free/available position in system thread array
cdthreadStruct* cdthread_getPointerToStruct(cdThdID_t pThId); //!< returns a cdthreadStruct type pointer directly to inndicated element of internal thread struct array; in case of error return a pointer to 0x0000000
int cdthread_getArrayIdxFromID(cdThdID_t pThId);	   //!< return the index for cdthreadsSystemArray extracted from indicated cdThdID_t (pThId)

int cdthread_new(cdThdID_t* pThId, cdtreadFunctionType ptrFunction);	 //!< creates a new thread for Thread_Engine; after creation the thread is disabled, doesn't have messages and the firsttime is set to true
int cdthread_newUserManaged(cdThdID_t* pThId, cdtreadFunctionType ptrFunction); //!<creates a new USER managed thread (not managed by Thread Engine); after creation the thread is disabled, doesn't have messages and the first time is set to true

int cdthread_DestroyThread(cdThdID_t* pThId);	                     //!< destroy the indicated thread (memory are free for another new thread), and every message associated at that thread
int cdthread_isDestroyed(cdThdID_t pThId);			//!<checks if passed id is destroyed (invalid pThId or pThId signed as destroyed) and if it is then return true
#define cdthread_Exist( pThId )				(!cdthread_isDestroyed( pThId ))	//!<checks if passed id is valid and indicated thread is not destroyed 

int cdthread_changeFunction(cdThdID_t pThId, cdtreadFunctionType ptrFunction); //!< changes only called function by indicated thread without change any other thread fields
int cdthread_isFunction(cdThdID_t pThId, cdtreadFunctionType ptrFunction);	//!<checks if called function by indicated thread (pThId) is the same passed (ptrFunction)

int cdthread_signAsFirtsTime(cdThdID_t pThId);       //!<set firsttime flag as true for indicated thread

int cdthread_waitUpTo(cdThdID_t pThId, cdTimerID pWaitUpTo);   //!<sets when this thread will be recall indicating the absolute timer value (wait until)
int cdthread_RunEveryTicks(cdThdID_t pThId, cdLoadTimer_t pTicks); //!<sets when this thread will be recall after a number of ticks, continuosly
int cdthread_WaitCycles(cdThdID_t pThId, int pCycles, int pContinuosly); //!<sets after many thread cycles (thread index rounds inside thread manager) will be call one time, or continuosly
int cdthread_WaitMessage(cdThdID_t pThId);               //!<sets thread to wait almost a message before to be called 
int cdthread_WaitEvent(cdThdID_t pThId, cdevent_t* pEventPtr); //!<sets that this thread will be called when the indicated event is generated (active and enabled) 
int cdthread_WaitDisable(cdThdID_t pThId, int pDisableCode); //!< it disables wait function for thread, alls (if cdTh_NOwaitMode, so run always) or one specific (if cdTh_waitMode...)

int cdthread_Enable(cdThdID_t pThId, int pPriority); //!< enable indicated Thread with indicated priority

int cdthread_Disable(cdThdID_t pThId);	              //!< disable indicated Thread 

int cdthread_isEnabled(cdThdID_t pThId);          //!< checks if indicated Thread is enabled (priority over 0) and return true if enabled 

int cdthread_getPriority(cdThdID_t pThId);	       //!< return the actual pripority of thread (0=disable), or -1 for error
int cdthread_getPrevExitStatus(cdThdID_t pThId);  //!< return the last exit state; if hasn't a previous exit state (first time executed) return 0. if an error happen return -1

int cdthread_isThrereMessages(cdThdID_t pThId);   //!< check if indicated thread has almost a message on the thread message queue

cdMsgID_t cdthread_getMessage(cdThdID_t pThId);   //!<return the first message (ID) on msg queue of indicated thread (and sign it as read); if there isn't or an error occours then return CDMESSAGEID_ERROR

int cdthread_removeMessage(cdThdID_t pThId);      //!<remove first message from thread msg queue and return true if action has success, otherwise return false

int cdthread_setDescription(cdThdID_t pThId , const char* desc_ptr, int num_ch); //!<add description at thread and return true if action has success

//******************************************************************************************************

//following define used inside a thread to manage functionality
#define NEW_CDTHFUNCTION( XXX ) 	int XXX (int firstTime, cdThdID_t ThisThread, int prevExitState)
//for example NEW_CDTHFUNCTION( BuzzerCDThFunction ) creates:
//int BuzzerCDThFunction(int firstTime, cdThdID_t ThisThread, int prevExitState)

#define cdTHISth_getThreadID() 			ThisThread
#define cdTHISth_getPrevExitStatus() 	prevExitState
#define cdTHISth_IsFirstTimeThatRun() 	firstTime

#define cdTHISth_Destroy() 			   cdthread_DestroyThread( &ThisThread )
#define cdTHISth_signAsFirtsTime() 		cdthread_signAsFirtsTime( ThisThread )

#define cdTHISth_waitUpTo( TTT ) 		   cdthread_waitUpTo( ThisThread , TTT )
#define cdTHISth_RunEveryTicks( TTT ) 	   cdthread_RunEveryTicks( ThisThread , TTT )
#define cdTHISth_WaitCycles( TTT )        cdthread_WaitCycles(ThisThread, TTT, false)
#define cdTHISth_RecallEveryCycles( TTT ) cdthread_WaitCycles(ThisThread, TTT, true)
#define cdTHISth_WaitMessage()            cdthread_WaitMessage(ThisThread)
#define cdTHISth_WaitEvent( TTT )         cdthread_WaitEvent(ThisThread, TTT )

#define cdTHISth_WaitDisable( TTT )          cdthread_WaitDisable(ThisThread, TTT)
#define cdTHISth_DisableWaitUpTo()           cdthread_WaitDisable(ThisThread, cdTh_waitModeTimer)
#define cdTHISth_DisableRunEveryTicks()      cdthread_WaitDisable(ThisThread, cdTh_waitModeTimer)
#define cdTHISth_DisableWaitCycles()         cdthread_WaitDisable(ThisThread, cdTh_waitModeCounts)
#define cdTHISth_DisableRecallEveryCycles()  cdthread_WaitDisable(ThisThread, cdTh_waitModeCounts)
#define cdTHISth_DisableWaitMessage()        cdthread_WaitDisable(ThisThread, cdTh_waitModeMessage)
#define cdTHISth_DisableWaitEvent()          cdthread_WaitDisable(ThisThread, cdTh_waitModeEvent)
#define cdTHISth_WaitDisableAll()            cdthread_WaitDisable(ThisThread, cdTh_NOwaitMode)


#define cdTHISth_Enable() 			      cdthread_Enable( ThisThread , CDTHREADPRIORITY_ENABLED )
#define cdTHISth_Disable() 			   cdthread_Disable( ThisThread )
#define cdTHISth_isThrereMessages() 	cdthread_isThrereMessages( ThisThread )
#define cdTHISth_getMessage() 			cdthread_getMessage( ThisThread )
#define cdTHISth_removeMessage() 		cdthread_removeMessage( ThisThread )


#define cdTHISth_AnswerSenderNoData( XXXInfoVal )                                cdmessage_new( msgSender , XXXInfoVal , 0 , CDMESSAGEDATA_NODATA , ThisThread )
#define cdTHISth_AnswerSenderWithData( XXXInfoVal , XXXNumData , XXXPtrData )    cdmessage_new( msgSender , XXXInfoVal , XXXNumData , XXXPtrData , ThisThread )

#define cdTHISth_NewMsgNoData( XXXThreadDest , XXXInfoVal )                               cdmessage_new( XXXThreadDest , XXXInfoVal , 0 , CDMESSAGEDATA_NODATA , ThisThread )
#define cdTHISth_NewMsgWithData( XXXThreadDest , XXXInfoVal , XXXNumData , XXXPtrData )   cdmessage_new( XXXThreadDest , XXXInfoVal , XXXNumData , XXXPtrData , ThisThread )


//remember to place ALLOCATE_CDMESSAGE_STD_VARIABLES	to allocate the variables cdMsgID_t actMsg ; int msgInfo ; cdThdID_t msgSender ; int msgNumData; void* msgDataPtr
//this define simply call and assign value for the cdthread_getMessage and cdmessage_getInfo
#define ALLOCATE_ACTMSG_MSGINFO_VARIABLES		      cdMsgID_t actMsg ; int msgInfo 
#define ALLOCATE_STATIC_ACTMSG_MSGINFO_VARIABLES	static cdMsgID_t actMsg ; static int msgInfo 

#define cdTHISth_getActMsgAndMsgInfo() 		   actMsg = cdthread_getMessage( ThisThread ) ; msgInfo = cdmessage_getInfo( actMsg )

#define ALLOCATE_MSG_SENDER_NUMDATA_DATAPTR_VARIABLES		cdThdID_t msgSender ; int msgNumData; void* msgDataPtr

#define ALLOCATE_STATIC_MSG_SENDER_VARIABLE		   static cdThdID_t msgSender 
#define ALLOCATE_MSG_SENDER_VARIABLE		         cdThdID_t msgSender 

//following define used inside a thread to manage messages
#define cdTHISth_getMsgSender()		      msgSender = cdmessage_getSenderThread( actMsg )
#define cdTHISth_getMsgNumData()			   msgNumData = cdmessage_getNumData( actMsg )
#define cdTHISth_getMsgDataPointer()		msgDataPtr = cdmessage_getDataPointer( actMsg )

#define ALLOCATE_MSG_ALL_VARIABLES		ALLOCATE_ACTMSG_MSGINFO_VARIABLES ; ALLOCATE_MSG_SENDER_NUMDATA_DATAPTR_VARIABLES 

#define cdTHISth_useActMsg() 			   actMsg
#define cdTHISth_useMsgInfo() 			msgInfo
#define cdTHISth_useMsgNumData() 		msgNumData
#define cdTHISth_useMsgSender() 		   msgSender
#define cdTHISth_useMsgDataPtr()		   msgDataPtr


//******************************************************************************************************

int cdthread_Engine(void);	//!< is the engine for thread and must be called continuosly

//cdThdID_t cdthread_ThreadToRun(cdThdID_t LastRunnedThread);	//!< is called by cdthread_Engine() to determine which is next thread to run

int cdthread_UserCallThreadFunction(cdThdID_t pThId, int* exitCode); //!< call thread_function, if thread is enabled and id is good (no errors); return true if called, otherwise false





//*********************************************************************
//*************timers *************************************************
//*********************************************************************

//DEFINED AT HEAD OF THIS FILE
//typedef uint64_t cdTimerID; //!< the cdTimerID is not a simple ID but contains the value when timer expires (expeted absolute timer value)

//defined also in CBAsystem.h but allocated into memory_alloc.c; used in nmi isr and indicates that every tick=10us
extern  cdTimerID	   cdth_Absolute_timer;
//use a 64bit means that if ticks is 1us we have 580000 years before it has a round 

//indicates that 1 tik is equal 10us so...
//CLOCKS_PER_SEC is defined within time.h
#define CDTIMER_us_FOR_TICK	   10

//indicates that 1ms is equal to 100 ticks of Absolute_timer
#define CDTIMER_1ms_TICKS	( 1000 / CDTIMER_us_FOR_TICK )
#define CDTIMER_1s_TICKS	( 1000000 / CDTIMER_us_FOR_TICK )

//moved on the top of file for compilation requirements
//the cdTimerID is not a simple ID but contains the value when timer expires (expeted absolute timer value)
//typedef uint64_t cdTimerID;

//this function must be defined externally and must update also the cdth_Absolute_timer global variable
extern cdTimerID   cdtimer_updateAbsoluteTimer( void );

//#define cdtimer_getNOW() 	            (cdth_Absolute_timer)	
#define cdtimer_getNOW() 	               cdtimer_updateAbsoluteTimer()
//#define cdtimer_setupTicks(ticksVal) 	(cdth_Absolute_timer + ticksVal)
//#define cdtimer_setup_s(msVal) 			(cdth_Absolute_timer + (msVal * CDTIMER_1s_TICKS))
//#define cdtimer_setu_pms(msVal) 			(cdth_Absolute_timer + (msVal * CDTIMER_1ms_TICKS))
//#define cdtimer_setupus(usVal) 			(cdth_Absolute_timer + (usVal / CDTIMER_us_FOR_TICK))
#define cdtimer_setupTicks(ticksVal) 	   ( cdtimer_getNOW() + ticksVal )
#define cdtimer_setup_s( sVal ) 			   ( cdtimer_getNOW() + ( sVal * CDTIMER_1s_TICKS))
#define cdtimer_setup_ms( msVal ) 			( cdtimer_getNOW() + ( msVal * CDTIMER_1ms_TICKS))
#define cdtimer_setup_us( usVal ) 			( cdtimer_getNOW() + ( usVal / CDTIMER_us_FOR_TICK))
//#define cdtimer_isExpired( cdTID )		   (cdTID <= cdth_Absolute_timer)
#define cdtimer_isExpired( cdTID )		   (cdTID <= cdtimer_getNOW() )
//#define cdtimer_isNotExpired( cdTID )		(cdTID > cdth_Absolute_timer)
#define cdtimer_isNotExpired( cdTID )		(cdTID > cdtimer_getNOW() )

#define cdTHISth_Sleep_s( TTT ) 		   cdthread_waitUpTo( ThisThread , cdtimer_setup_s( TTT ) )
#define cdTHISth_Sleep_ms( TTT ) 		cdthread_waitUpTo( ThisThread , cdtimer_setup_ms( TTT ) )
#define cdTHISth_Sleep_us( TTT ) 		cdthread_waitUpTo( ThisThread , cdtimer_setup_us( TTT ) )

//use example:
//cdTimerID myTimer1, myTimer2;
//myTimer1 = cdtimers_setupTicks(20);	//assign the timer value for 200us
//myTimer2 = cdtimers_setupms(1);		//assign the timer value for 1ms

//while( cdtimer_isNotExpired(myTimer1) ){
//  ....
//  if( cdtimer_isExpired(myTimer2){
//    ....
//  }
//  ....
//} 


//FOLLOWING SECTION REMOVED IN DATE 
//*********************************************************************
//*************jump table inside cdth_functions************************
//*********************************************************************
////if varaible equl to n then jump to CDTH_EXITandRETURN with same n value
//#define CDTH_JUMPTABLE( variable , nnn )   if( variable == nnn ) goto LABEL_JUMP_WAIT##nnn  
#define cdTHISth_Jump_IfPrevStateIs( XYN )      if( prevExitState == XYN ) goto LABEL_JUMP_WAIT##XYN  
////exit with n value if condition is true, also return here from a CDTH_JUMPTABLE with same n value; return and test condition
//#define CDTH_RETURN_HERE(  nnn  )   LABEL_JUMP_WAIT##nnn: 
#define cdTHISth_ComeHere_IfPrevStateIs(  XYN  )   LABEL_JUMP_WAIT##XYN: 
////exit with n value if condition is true, also return here from a CDTH_JUMPTABLE with same n value; return and test condition
//#define CDTH_EXITandRETURN(  nnn , condition )   LABEL_JUMP_WAIT##nnn:  if( condition ) return nnn 
#define cdTHISth_ExitWithState(  XYN  )   return XYN 

#define cdTHISth_Exit()   return prevExitState 

#define cdTHISth_setState(  XYN )   prevExitState = XYN 


//////////////////////////////////////////////////////////////////////////
/////////information message system system////////////////////////////////
/////////information message system system////////////////////////////////
/////////information message system system////////////////////////////////
/////////information message system system////////////////////////////////
//////////////////////////////////////////////////////////////////////////


#ifdef  ENABLE_CDMSGERRINF
struct struct_cdmsgErrInf{
      uint16_t getFreeID_err;
      uint16_t getArrayIdxFromID_err;
      struct {
       uint16_t thRealIdxLessZero;
		 uint16_t threadid_err;
		 uint16_t freeid_err;
		 uint16_t idNotDeleted;
		 uint16_t idNotLocked;
		 uint16_t nextMsgID_Err;
     }cdmessage_new;   
	  struct {
		 uint16_t idIvalid;
		 uint16_t deletedOtherMsg;
		 uint16_t MsgLocked_err;
	  }deleteAllMsgWithThreadID;
	  struct {
		 uint16_t invalidIdThread;	
		 uint16_t deletedThread;
		 uint16_t IdxLessZero;
		 uint16_t MsgLocked_err;
		 uint16_t idNotLocked;
		 uint16_t msgCounterLessOne;
	  }deleteMsg;
	  struct {
		 uint16_t IdxLessZero;
		 uint16_t MsgLocked_err;
       uint16_t MsgAlreadyDel_warning;
	  }sigMsgAsRead;
	  uint16_t isRead_IdxLessZero;
	  uint16_t isDeleted_IdxLessZero;
	  uint16_t getSenderThread_IdxLessZero;	
	  uint16_t getNumData_IdxLessZero;
	  uint16_t getDataPointer_IdxLessZero;
	  uint16_t getInfo_IdxLessZero;
 };

extern struct struct_cdmsgErrInf cdmsgErrInf;
#endif //ENABLE_CDMSGERRINF
 
 
#ifdef  ENABLE_CDTHERRINF
struct struct_cdThErrInf{
	uint16_t getFreeID_err;
	//uint16_t getArrayIdxFromID_err;
	uint16_t getPointerToStruct_err;
	struct{
		uint16_t pThIdIsNotDestroyed;
		uint16_t freeid_err;
	}cdthread_new;
	struct{
		uint16_t new_err;
		uint16_t id_err;
	}newUserManaged;
	struct{
		uint16_t realIdx_err;
		uint16_t id_err;
	}changeFunction;
	struct{
		uint16_t realIdx_err;
		uint16_t id_err;
	}isFunction;
	uint16_t DestroyThread_realIdx_warning;
	uint16_t isDestroyed_realIdx_warning;
	struct{
		uint16_t realIdx_err;
		uint16_t id_err;
	}signAsFirtsTime;
	struct{
		uint16_t realIdx_err;
		uint16_t id_err;
	}waitUpTo;
	uint16_t Enable_realIdx_err;
	uint16_t Disable_realIdx_err;
	uint16_t isEnabled_realIdx_err;
	uint16_t getPriority_realIdx_err;
	uint16_t getPrevExitStatus_realIdx_err;
};
extern struct struct_cdThErrInf cdThErrInf;
#endif	//ENABLE_CDTHERRINF

//extern int cdmessageOVERFLOW;
//extern int cdmessagePENDING;
struct struct_cdMsgCOUNTERS{
   //used (if different from zero) to advice messages system that a serious error happened (no available messages)
   int OVERFLOW;
   //used to know how many messages are new or read but not deleted now (at this moment)
   int PENDING;
   //used to know how many thread was created from the start
   uint32_t CREATED;
   //used to know how many thread was destroyed from the start
   uint32_t DESTROYED; 
};
extern struct struct_cdMsgCOUNTERS cdMsgCOUNTERS;

//extern int cdthreadOVERFLOW;
//extern unsigned int cdthreadCREATED;
//extern unsigned int cdthreadDESTROYED;
//extern int cdthreadLOADED;
//extern int cdthreadENABLED;
struct struct_cdThCOUNTERS{
   //used (if different from zero) to advice threads system that a serious error happened (no available threads)
   int OVERFLOW;
   //used to know how many thread was created from the start
   uint16_t CREATED;
   //used to know how many thread was destroyed from the start
   uint16_t DESTROYED; 
   //used to know how many threads created and not destroyed now (at this moment)
   int LOADED;
   //used to know how many threads, of those loaded, are enabled now (at this moment)
   int ENABLED; 
};
extern struct struct_cdThCOUNTERS cdThCOUNTERS;

 
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////





#endif  //_CDTHREAD_H_

