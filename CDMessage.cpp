////////////////////////////////////////////////////////////////////////////
////        (C) Copyright Dario Cortese                                 ////
//// This source code may only be used by Dario Cortese licensed users  ////
//// This source code may only be distributed to Dario Cortese licensed ////
//// users.  No other use, reproduction  or distribution is permitted   ////
//// without written permission.                                        ////
//// Author: Dario Cortese                                              ////
//// Client: I.S.A. srl                                                 ////
//// User: Dario Cortese                                                ////
//// Created on 10/08/2012                                              ////
//// File: cdmessage.c                                                  ////
//// Description:                                                       ////
//// THIS FILE HAVE ALL IMPLEMENTATION OF FUNCTION USED FOR CDMESSAGE   ////
////   MANAGING; MESSAGES ARE USED ALSO IN CDTHREAD, AND VICEVERSA      ////        
////////////////////////////////////////////////////////////////////////////

#ifndef _CDMESSAGE_C_
#define _CDMESSAGE_C_

#include "CDThread.h"
 
#ifdef DOXYGEN
    #define section(YY)  
#endif 

//is the system array that stores the cdmessage structure used to manage messages
 section("sdram0") cdmessageStruct cdmessagesSystemArray[ CDTHREAD_MAX_NUM_MESSAGES ];

 //used (if different from zero) to advice message system that a serious error happened (no available messages)
 //int cdmessageOVERFLOW;
 //int cdmessagePENDING;
 struct struct_cdMsgCOUNTERS cdMsgCOUNTERS;
 
 
#ifdef  ENABLE_CDMSGERRINF
   struct struct_cdmsgErrInf cdmsgErrInf;
#endif //ENABLE_CDMSGERRINF
 
/*! \fn void cdmessage_initAll(void)
	\author Dario Cortese
	\date 08-08-2012
	\brief must be called by initialization main to reset and init system messages array
	\version 1.00
*/
void cdmessage_initAll(void)
{
	int idx;
	for(idx=0; idx < CDTHREAD_MAX_NUM_MESSAGES; idx++){
		cdmessagesSystemArray[idx].cdthID=CDTHREADID_ERROR;		//free/available position, so not associated
		cdmessagesSystemArray[idx].State=CDMESSAGESTATE_DELETED;//free/available position
		cdmessagesSystemArray[idx].ptrData = CDMESSAGEDATA_NODATA;	//no data associated
		cdmessagesSystemArray[idx].NumData = 0;	//no data available
		cdmessagesSystemArray[idx].Info = 0;	//default value
		cdmessagesSystemArray[idx].NextMsgID = CDMESSAGEID_ERROR;	//no next message	
	}
	
	cdMsgCOUNTERS.OVERFLOW=0;
	cdMsgCOUNTERS.PENDING=0;
	cdMsgCOUNTERS.CREATED=0;
   cdMsgCOUNTERS.DESTROYED=0;
		
#ifdef  ENABLE_CDMSGERRINF	
	cdmsgErrInf.getFreeID_err=0;
	cdmsgErrInf.getArrayIdxFromID_err=0;
	cdmsgErrInf.cdmessage_new.thRealIdxLessZero=0;
	cdmsgErrInf.cdmessage_new.threadid_err=0;
	cdmsgErrInf.cdmessage_new.freeid_err=0;
	cdmsgErrInf.cdmessage_new.idNotDeleted=0;
	cdmsgErrInf.cdmessage_new.idNotLocked=0;
	cdmsgErrInf.cdmessage_new.nextMsgID_Err=0;
	cdmsgErrInf.deleteAllMsgWithThreadID.idIvalid=0;
	cdmsgErrInf.deleteAllMsgWithThreadID.deletedOtherMsg=0;
	cdmsgErrInf.deleteAllMsgWithThreadID.MsgLocked_err=0;
	cdmsgErrInf.deleteMsg.invalidIdThread=0;
	cdmsgErrInf.deleteMsg.deletedThread=0;
	cdmsgErrInf.deleteMsg.IdxLessZero=0;
	cdmsgErrInf.deleteMsg.MsgLocked_err=0;
	cdmsgErrInf.deleteMsg.idNotLocked=0;
	cdmsgErrInf.deleteMsg.msgCounterLessOne=0;
	cdmsgErrInf.sigMsgAsRead.IdxLessZero=0;
	cdmsgErrInf.sigMsgAsRead.MsgLocked_err=0;
   cdmsgErrInf.sigMsgAsRead.MsgAlreadyDel_warning=0;
	cdmsgErrInf.isRead_IdxLessZero=0;
	cdmsgErrInf.isDeleted_IdxLessZero=0;
	cdmsgErrInf.getSenderThread_IdxLessZero=0;
	cdmsgErrInf.getNumData_IdxLessZero=0;
	cdmsgErrInf.getDataPointer_IdxLessZero=0;
	cdmsgErrInf.getInfo_IdxLessZero=0;	
#endif //ENABLE_CDMSGERRINF
}



/*! \fn cdMsgID_t cdmessage_getFreeID(void)
	\author Dario Cortese
	\date 08-08-2012
	\brief searh free/available position in system message array
	Searchs inside cdmessagesSystemArray if there is a available/free position (searhs for every position if there is a State==CDMESSAGESTATE_DELETED, 
	 this means that that position is free to use)
	\return CDMESSAGEID_ERROR if doesn't find any available position, or positive number that indicate ID (array position + 1) 
	\note ONLY FOR INTERNAL USE
	\version 1.00
*/
cdMsgID_t cdmessage_getFreeID(void) {
	//uses static id so next time that a new message position is requested, start from a new position and avoid to 
	//reuse position until array end is reached  
	static cdMsgID_t idx=0;
	
	if(idx >=CDTHREAD_MAX_NUM_MESSAGES) idx=0;
	
	for(; idx < CDTHREAD_MAX_NUM_MESSAGES; idx++){
		if (cdmessagesSystemArray[idx].State==CDMESSAGESTATE_DELETED){
			idx++;	//so next time that reenter use next new position
			//return (idx+1); ////ID is always the real array index +1, because id=0 means unused/deleted
			return idx;
		}
	}
	//if first search, searching from last idx has no positive result (otherwire exit with return and doesn't arrives here)
	// then execute a second search starting from 0
	for(idx=0; idx < CDTHREAD_MAX_NUM_MESSAGES; idx++){
		if (cdmessagesSystemArray[idx].State==CDMESSAGESTATE_DELETED){
			idx++;	//so next time that reenter use next new position
			//return (idx+1); ////ID is always the real array index +1, because id=0 means unused/deleted
			return idx;
		}
	}
	//if also second search has no positive result then this means that there isn't available message id free, at compiling time
	// increments CDTHREAD_MAX_NUM_MESSAGES define, or check if some message is many time allocated without remove	
#ifdef  ENABLE_CDMSGERRINF
	cdmsgErrInf.getFreeID_err++;
#endif //ENABLE_CDMSGERRINF
   cdMsgCOUNTERS.OVERFLOW=1; //advices my  message system that a serious error happened (no available messages)
	return CDMESSAGEID_ERROR; //means no available messages
} 



/*! \fn int cdmessage_getArrayIdxFromID(cdMsgID_t pMsgId)
	\author Dario Cortese
	\date 09-08-2012
	\brief return the real index for cdmessagesSystemArray extracted from indicated cdMsgID_t (pMsgId)
	\param pMsgId is cdmessage id 
	\return -1 if thre is an error or positive number (0 or more) if all ok
	\warning DANGEROUS, because return a index that will be used to access directly an element of system message array without any checks or value casting; possibility of data corruption
	\note ONLY FOR INTERNAL USE
	\version 1.00
*/
int cdmessage_getArrayIdxFromID(cdMsgID_t pMsgId) {
	int realidx;
	if(! cdmessage_checkValidID( pMsgId)){
#ifdef  ENABLE_CDMSGERRINF
	    cdmsgErrInf.getArrayIdxFromID_err++;
#endif //ENABLE_CDMSGERRINF
	    return -1;
	}
	//convert pMsgId in real index for cdmessagesSystemArray
	realidx = (int)pMsgId;
	realidx--;
	return realidx;
}




/*! \fn cdMsgID_t cdmessage_new(cdThdID_t pThreadIDdest, int pInfoVal, int pNumData, void* ptrData, cdThdID_t pThreadIDorg )
	\author Dario Cortese
	\date 09-08-2012 revised 21-03-2013
	\brief creates new message, and adds it to message queue of indicated thread 
	Creates new message and add it to indicated thread, if operation has success then return a valid cdMsgID_t, otherwise CDMESSAGEID_ERROR.
	This function search a available message in the system message array, after checks if thread id is a valid ID.
	If all ok proced to add created message to thread message queue.
	\n If no data and thread to add at this message please use cdmessage_new( pThreadID , pInfoVal, 0 , CDMESSAGEDATA_NODATA, CDTHREADID_NOTHREAD )
	\param pThreadIDdest is the thread where message will be asent (added to msg thread queue)
	\param pInfoVal is an integer that inform receiver what is the message (is a number and values must be user defined)
	\param pNumData is the logical number (NOT real number of byte or other) of data stored in memory and pointet by ptrData
	\param ptrData is the memory address where message data are stored; if no data pointer please set this param to CDMESSAGEDATA_NODATA
	\param pThreadIDorg is the thread that send the message, if you don't have this information, then set it to CDTHREADID_NOTHREAD
	\return CDMESSAGEID_ERROR if doesn't find any available position or happen an error, or positive number that indicates new cdMsgID_t  
	\version 1.01
*/
cdMsgID_t cdmessage_new(cdThdID_t pThreadIDdest, int pInfoVal, int pNumData, void* ptrData, cdThdID_t pThreadIDorg ) {
	cdMsgID_t idx;
	cdMsgID_t othIdx;
	cdmessageStruct* MsgPtr;
	int thRealIdx;
	int appoInt;

	//check if cdThdID_t is valid and get the real index of system thread array
	thRealIdx = cdthread_getArrayIdxFromID(pThreadIDdest);
	if (thRealIdx < 0){
#ifdef  ENABLE_CDMSGERRINF	
	   cdmsgErrInf.cdmessage_new.thRealIdxLessZero++ ;
#endif  //ENABLE_CDMSGERRINF
	   return CDMESSAGEID_ERROR;
	}
	//check if thread is destroyed, so is impossible to sent a message and return an error
	if (cdthreadsSystemArray[thRealIdx].ID==CDTHREADID_ERROR){
#ifdef  ENABLE_CDMSGERRINF
  	    cdmsgErrInf.cdmessage_new.threadid_err++ ;
#endif  //ENABLE_CDMSGERRINF
	    return CDMESSAGEID_ERROR;
	}	
	//get the new message struct	
	idx= cdmessage_getFreeID();
	//check if not finds a available message struct
	if (idx==CDMESSAGEID_ERROR){
#ifdef  ENABLE_CDMSGERRINF
	    cdmsgErrInf.cdmessage_new.freeid_err++ ;
#endif //ENABLE_CDMSGERRINF
	    return CDMESSAGEID_ERROR;
	}
	//get a pointer to nmessage struct
	MsgPtr =  &cdmessagesSystemArray[(int)idx - 1];
	// initialize values	
	//to avoid isr problema a fraction of time befor lock verify that is free
	if(MsgPtr->State != CDMESSAGESTATE_DELETED){
#ifdef  ENABLE_CDMSGERRINF
	   cdmsgErrInf.cdmessage_new.idNotDeleted++;
#endif	//ENABLE_CDMSGERRINF
	   return CDMESSAGEID_ERROR;
	}
	//to avoid isr problema a fraction of time after lock verify that is locked    
	MsgPtr->State=CDMESSAGESTATE_LOOCKED;
	if(MsgPtr->State != CDMESSAGESTATE_LOOCKED){
#ifdef  ENABLE_CDMSGERRINF
	   cdmsgErrInf.cdmessage_new.idNotLocked++;
#endif //ENABLE_CDMSGERRINF
	   return CDMESSAGEID_ERROR;
	}	
	MsgPtr->cdthID=pThreadIDdest;
	//MsgPtr->State=CDMESSAGESTATE_NEWMSG;
	MsgPtr->ptrData = ptrData;
	MsgPtr->NumData = pNumData;
	MsgPtr->Info = pInfoVal;
	MsgPtr->NextMsgID = CDMESSAGEID_ERROR;	//no other messages after this
	MsgPtr->cdthSenderID = pThreadIDorg;

	//now add to the thread message queue this message
	//check if there isn't other messages on the queue, in this case....
	if(cdthreadsSystemArray[thRealIdx].MessagesCounter <= 0){
		//if there isn't others messages on the queue then...
		cdthreadsSystemArray[thRealIdx].FirstMsgID = idx;
		cdthreadsSystemArray[thRealIdx].LastMsgID = idx;
		cdthreadsSystemArray[thRealIdx].MessagesCounter=1;
	}else{ //if(cdthreadsSystemArray[thRealIdx].MessagesCounter <= 0)
		//if there is others messages on the queue then...
		othIdx = cdthreadsSystemArray[thRealIdx].LastMsgID;
		cdthreadsSystemArray[thRealIdx].LastMsgID = idx;
		//now change the NextMsgID of previouse "lastMsgID" to this message id
		//remember that
		appoInt = cdmessage_getArrayIdxFromID(othIdx); 
		//if conversion happen without errors then...
		if(appoInt>=0){
 			cdmessagesSystemArray[appoInt].NextMsgID = idx;
		}else{
#ifdef  ENABLE_CDMSGERRINF
	   cdmsgErrInf.cdmessage_new.nextMsgID_Err++;
#endif //ENABLE_CDMSGERRINF
		}
		appoInt = cdthreadsSystemArray[thRealIdx].MessagesCounter++;			
	} //end else if(cdthreadsSystemArray[thRealIdx].MessagesCounter <= 0)
	MsgPtr->State=CDMESSAGESTATE_NEWMSG;
	cdMsgCOUNTERS.PENDING++;
	cdMsgCOUNTERS.CREATED++;
	return idx;	//return the new cdMsgID_t added to thread
}




/*! \fn int cdmessage_deleteAllMsgWithThreadID( cdThdID_t pThId )
	\author Dario Cortese
	\date 09-08-2012
	\brief delete  all messages in the message system array that have cdthID equal pThId (as is delete all messages associated at indicated thread)
	\return true if all ok otherwise return false if an error happen
	\version 1.00
*/
int cdmessage_deleteAllMsgWithThreadID( cdThdID_t pThId ) {
	int idx;
	if(!cdthread_checkValidID( pThId)){
#ifdef  ENABLE_CDMSGERRINF	
	   cdmsgErrInf.deleteAllMsgWithThreadID.idIvalid++;
#endif  //ENABLE_CDMSGERRINF
	   return false;
	}
	for(idx=0; idx < CDTHREAD_MAX_NUM_MESSAGES; idx++){
		if(cdmessagesSystemArray[idx].cdthID== pThId){
		   //delete message only if is unlocked
		   if( cdmessagesSystemArray[idx].State != CDMESSAGESTATE_DELETED){
            if( cdmessagesSystemArray[idx].State != CDMESSAGESTATE_LOOCKED){
#ifdef  ENABLE_CDMSGERRINF
		         cdmsgErrInf.deleteAllMsgWithThreadID.deletedOtherMsg++;
#endif //ENABLE_CDMSGERRINF
   			   cdmessagesSystemArray[idx].State=CDMESSAGESTATE_DELETED;//free/available position
#ifdef  ENABLE_CDMSGERRINF		   
               cdMsgCOUNTERS.PENDING--;
               cdMsgCOUNTERS.DESTROYED++;
   		   }else{
	   	       cdmsgErrInf.deleteAllMsgWithThreadID.MsgLocked_err++;
#endif //ENABLE_CDMSGERRINF
		      }
		   } //end if( cdmessagesSystemArray[idx].State != CDMESSAGESTATE_DELETED)
		} //end if(cdmessagesSystemArray[idx].cdthID== pThId)
	} //end for(idx=0; idx < CDTHREAD_MAX_NUM_MESSAGES; idx++)
	return true;
}

/*! \fn int cdmessage_deleteMsg( cdMsgID_t pMsgId )
	\author Dario Cortese
	\date 09-08-2012
	\brief deletes this message from messages system array and remove it from message queue of connected thread (only if is the first msg of queue)
	\param pMsgId is the cdMsgID_t of message that shall be deleted and removed from thread msg queue
	\return true if all ok otherwise return false if an error happen
	\version 1.00
*/
int cdmessage_deleteMsg( cdMsgID_t pMsgId ) {
	int idx;
	cdThdID_t thID;
	int thIdx;
	cdthreadStruct* ptr_cdthrS; 
	
	idx = cdmessage_getArrayIdxFromID(pMsgId);
	if(idx < 0){
#ifdef  ENABLE_CDMSGERRINF	
	   cdmsgErrInf.deleteMsg.IdxLessZero++;
#endif  //ENABLE_CDMSGERRINF
	   return false;	//indicate an error
	}   
	
	//if already deleted then return immediatly
	if(cdmessagesSystemArray[idx].State == CDMESSAGESTATE_DELETED){
	   return true;
	}
	
	if(cdmessagesSystemArray[idx].State == CDMESSAGESTATE_LOOCKED){
#ifdef  ENABLE_CDMSGERRINF
	   cdmsgErrInf.deleteMsg.MsgLocked_err++;
#endif //ENABLE_CDMSGERRINF
	   return false;	//indicate an error
	}
	//indicates that it is working on this message
	//cdmessagesSystemArray[idx].State=CDMESSAGESTATE_DELETED;//free/available position
	cdmessagesSystemArray[idx].State=CDMESSAGESTATE_LOOCKED;//locked
	//to verify that a NMI or ISR remove my lock than re-verify it
	if(cdmessagesSystemArray[idx].State != CDMESSAGESTATE_LOOCKED){
#ifdef  ENABLE_CDMSGERRINF
	   cdmsgErrInf.deleteMsg.idNotLocked++;
#endif //ENABLE_CDMSGERRINF
	   return false;	//indicate an error
	}
	   
	//remove this message from message thread queue
	//take thread id of this message
	thID = cdmessagesSystemArray[idx].cdthID;
	//check if this thread is valid
	thIdx = cdthread_getArrayIdxFromID(thID);
#ifdef  ENABLE_CDMSGERRINF
	if( thIdx < 0 ) 
			cdmsgErrInf.deleteMsg.invalidIdThread++;
#endif  //ENABLE_CDMSGERRINF	
	if( thIdx >= 0){
	   //to be more fast
	   ptr_cdthrS = &cdthreadsSystemArray[thIdx];
		//proceed to remove message from msg thread queue
		//check if this thread is deleted, in this case delete every message is used for it
		//if( cdthreadsSystemArray[thIdx].ID==CDTHREADID_ERROR ){
		if( ptr_cdthrS->ID==CDTHREADID_ERROR ){
#ifdef  ENABLE_CDMSGERRINF
			cdmsgErrInf.deleteMsg.deletedThread++;
#endif  //ENABLE_CDMSGERRINF	
      	cdmessagesSystemArray[idx].State=CDMESSAGESTATE_DELETED;//free/available position
      	ptr_cdthrS->MessagesCounter = 0;
      	cdMsgCOUNTERS.PENDING--;
         cdMsgCOUNTERS.DESTROYED++;
#ifdef ENABLE_DELALLMSGWITHTHREADID      	
			cdmessage_deleteAllMsgWithThreadID( thID );
#endif //ENABLE_DELALLMSGWITHTHREADID
		}else{ //if( ptr_cdthrS->ID==CDTHREADID_ERROR )
			//if thread isn' delted then check if there is message on queue
			//if(cdthreadsSystemArray[thIdx].MessagesCounter<=0){
			if(ptr_cdthrS->MessagesCounter <= 0){
#ifdef  ENABLE_CDMSGERRINF
				cdmsgErrInf.deleteMsg.msgCounterLessOne++;
#endif  //ENABLE_CDMSGERRINF	
				//this is a abnormal case because this message isnt in queue so is an error, 
				// in this case sign FirstMsgID and LastMsgID as CDMESSAGEID_ERROR and search to delete every message with this threadID
				//cdthreadsSystemArray[thIdx].FirstMsgID=CDMESSAGEID_ERROR;
				ptr_cdthrS->FirstMsgID=CDMESSAGEID_ERROR;
				//cdthreadsSystemArray[thIdx].LastMsgID=CDMESSAGEID_ERROR;
				ptr_cdthrS->LastMsgID=CDMESSAGEID_ERROR;
      	   ptr_cdthrS->MessagesCounter = 0;
         	cdmessagesSystemArray[idx].State=CDMESSAGESTATE_DELETED;//free/available position
         	cdMsgCOUNTERS.PENDING--;
            cdMsgCOUNTERS.DESTROYED++;
#ifdef ENABLE_DELALLMSGWITHTHREADID
				cdmessage_deleteAllMsgWithThreadID( thID );
#endif //ENABLE_DELALLMSGWITHTHREADID
			}else{ //if(ptr_cdthrS->MessagesCounter<=0)
				//if there is a message then check if this is the first, in this case procede to remove it from queue;
				//if this message isn't the first of thread msg queue than do nothing because is unmanageable error
				//if( cdthreadsSystemArray[thIdx].FirstMsgID== pMsgId){
				if( ptr_cdthrS->FirstMsgID == pMsgId){
					//assign this NextMsgID to thread msg queue start point 
					//cdthreadsSystemArray[thIdx].FirstMsgID=cdmessagesSystemArray[idx].NextMsgID;
					ptr_cdthrS->FirstMsgID = cdmessagesSystemArray[idx].NextMsgID;
					//decrement msg counter
					//cdthreadsSystemArray[thIdx].MessagesCounter--;
					ptr_cdthrS->MessagesCounter--;
				} 
				//if no other messages threre is in the queue then .....
				//if(cdthreadsSystemArray[thIdx].MessagesCounter<=0){
				if(ptr_cdthrS->MessagesCounter > 0){
            	cdmessagesSystemArray[idx].State=CDMESSAGESTATE_DELETED;//free/available position
            	cdMsgCOUNTERS.PENDING--;
               cdMsgCOUNTERS.DESTROYED++;
				}else{
				   //cdthreadsSystemArray[thIdx].FirstMsgID=CDMESSAGEID_ERROR;
					ptr_cdthrS->FirstMsgID = CDMESSAGEID_ERROR;
					//cdthreadsSystemArray[thIdx].LastMsgID=CDMESSAGEID_ERROR;
					ptr_cdthrS->LastMsgID = CDMESSAGEID_ERROR;
					//only for safe check if there is other messages with this thread id and delte they
            	cdmessagesSystemArray[idx].State=CDMESSAGESTATE_DELETED;//free/available position
            	cdMsgCOUNTERS.PENDING--;
               cdMsgCOUNTERS.DESTROYED++;
#ifdef ENABLE_DELALLMSGWITHTHREADID
					cdmessage_deleteAllMsgWithThreadID( thID );
#endif //ENABLE_DELALLMSGWITHTHREADID
				}
			}//end else if(ptr_cdthrS->MessagesCounter<=0)
		} //end else if( ptr_cdthrS->ID==CDTHREADID_ERROR )
	} //end if( thIdx >= 0)	
	//last action free the msg
	return true;
}



/*! \fn int cdmessage_sigMsgAsRead( cdMsgID_t pMsgId )
	\author Dario Cortese
	\date 09-08-2012
	\brief sign this message as read, if action has success then return true 
	\param pMsgId is the cdMsgID_t of message that will be signed as read
	\return true if all ok otherwise return flase if an error happen
	\version 1.00
*/
int cdmessage_sigMsgAsRead( cdMsgID_t pMsgId ) {
	int idx;
	idx = cdmessage_getArrayIdxFromID(pMsgId);
	if(idx < 0)
	{
#ifdef  ENABLE_CDMSGERRINF	
	   cdmsgErrInf.sigMsgAsRead.IdxLessZero++;
#endif //ENABLE_CDMSGERRINF
	   return false;	//indicate an error
	}

   if(cdmessagesSystemArray[idx].State == CDMESSAGESTATE_DELETED){
#ifdef  ENABLE_CDMSGERRINF
	   cdmsgErrInf.sigMsgAsRead.MsgAlreadyDel_warning++;
#endif //ENABLE_CDMSGERRINF
      return false;
   }	
	if (cdmessagesSystemArray[idx].State == CDMESSAGESTATE_LOOCKED)
	{
#ifdef  ENABLE_CDMSGERRINF
	   cdmsgErrInf.sigMsgAsRead.MsgLocked_err++;
#endif //ENABLE_CDMSGERRINF
      return false;
	}
	
   //if(cdmessagesSystemArray[idx].State != CDMESSAGESTATE_READ)
   //{   
	cdmessagesSystemArray[idx].State=CDMESSAGESTATE_READ;
   //}
   return true;
}



/*! \fn int cdmessage_isRead( cdMsgID_t pMsgId )
	\author Dario Cortese
	\date 09-08-2012
	\brief checks if this message was read and returns true, this means that could be only read or yet deleted; returns false also an error happen
	\param pMsgId is the cdMsgID_t to check
	\return true if message was read or deleted (read and after deleted), otherwise false, also an error happen (ie: id ID is invalid)
	\version 1.00
*/
int cdmessage_isRead( cdMsgID_t pMsgId ) {
	int idx;
	idx = cdmessage_getArrayIdxFromID(pMsgId);
	if(idx < 0){
#ifdef  ENABLE_CDMSGERRINF	
	   cdmsgErrInf.isRead_IdxLessZero++;
#endif	//ENABLE_CDMSGERRINF
	   return false;	//indicate an error
	}
	if(cdmessagesSystemArray[idx].State==CDMESSAGESTATE_READ)
	   return true;
	if(cdmessagesSystemArray[idx].State==CDMESSAGESTATE_DELETED)
	   return true;
	return false;
}



/*! \fn int cdmessage_isDeleted( cdMsgID_t pMsgId )
	\author Dario Cortese
	\date 09-08-2012
	\brief checks if this message was deleted and returns true otherwise returns false (also an error happen)
	\param pMsgId is the cdMsgID_t to check
	\return true if message was deleted (read and after deleted), otherwise false, also if an error happen (ie: id ID is invalid)
	\version 1.00
*/
int cdmessage_isDeleted( cdMsgID_t pMsgId ) {
	int idx;
	idx = cdmessage_getArrayIdxFromID(pMsgId);
	if(idx < 0){
#ifdef  ENABLE_CDMSGERRINF
	   cdmsgErrInf.isDeleted_IdxLessZero++;
#endif	//ENABLE_CDMSGERRINF
	   return false;	//indicate an error
	}
	if(cdmessagesSystemArray[idx].State==CDMESSAGESTATE_DELETED)
	   return true;
	return false;
}


/*! \fn cdThdID_t cdmessage_getSenderThread( cdMsgID_t pMsgId ){
	\author Dario Cortese
	\date 21-03-2013
	\brief return the cdThdID_t of Thread that has sent the message; if there isn't, or an error happens, return CDTHREADID_ERROR 
	\param pMsgId is the cdMsgID_t to check
	\return CDTHREADID_ERROR for error or no data, otherwise return the cdThdID_t of sender thread
	\version 1.00
*/
cdThdID_t cdmessage_getSenderThread( cdMsgID_t pMsgId ) {
	int idx;
	idx = cdmessage_getArrayIdxFromID(pMsgId);
	if(idx < 0){
#ifdef  ENABLE_CDMSGERRINF	
	   cdmsgErrInf.getSenderThread_IdxLessZero++;
#endif	//ENABLE_CDMSGERRINF
	   return CDTHREADID_ERROR;	//indicate an error
	}
	return cdmessagesSystemArray[idx].cdthSenderID;
}



/*! \fn int cdmessage_getNumData( cdMsgID_t pMsgId )
	\author Dario Cortese
	\date 09-08-2012
	\brief return the number of logical data attached to message; if there isn't, or an error happens, return 0 
	\param pMsgId is the cdMsgID_t to check
	\return 0 for error or no data, otherwise return the number of logical data attached to message
	\version 1.00
*/
int cdmessage_getNumData( cdMsgID_t pMsgId ) {
	int idx;
	idx = cdmessage_getArrayIdxFromID(pMsgId);
	if(idx < 0){
#ifdef  ENABLE_CDMSGERRINF	
	   cdmsgErrInf.getNumData_IdxLessZero++;
#endif	//ENABLE_CDMSGERRINF
	   return 0;	//indicate an error
	}
	if(cdmessagesSystemArray[idx].NumData <= 0)
	   return 0;
	return cdmessagesSystemArray[idx].NumData;
}


/*! \fn void* cdmessage_getDataPointer( cdMsgID_t pMsgId )
	\author Dario Cortese
	\date 09-08-2012
	\brief return pointer to memory where is the data attached to indicated message; if there isn't, or an error happens, return CDMESSAGEDATA_NODATA 
	\param pMsgId is the cdMsgID_t to check
	\return CDMESSAGEDATA_NODATA for error or no data, otherwise return the pointer to memory where is the data
	\version 1.00
*/
void* cdmessage_getDataPointer( cdMsgID_t pMsgId ) {
	int idx;
	idx = cdmessage_getArrayIdxFromID(pMsgId);
	if(idx < 0){
#ifdef  ENABLE_CDMSGERRINF	
	   cdmsgErrInf.getDataPointer_IdxLessZero++;
#endif	//ENABLE_CDMSGERRINF
	   return CDMESSAGEDATA_NODATA;	//indicate an error
	}
	if(cdmessagesSystemArray[idx].NumData <= 0)
	   return CDMESSAGEDATA_NODATA;
	return cdmessagesSystemArray[idx].ptrData;
}



/*! \fn int cdmessage_getInfo( cdMsgID_t pMsgId )
	\author Dario Cortese
	\date 10-08-2012
	\brief return the info value for indicated message
	\param pMsgId is the cdMsgID_t to check
	\return the info value stored int indicated message; if an error happen return 0 (but 0 is a default value, not a error value)
	\version 1.00
*/
int cdmessage_getInfo( cdMsgID_t pMsgId ) {
	int idx;
	idx = cdmessage_getArrayIdxFromID(pMsgId);
	if(idx < 0){
#ifdef  ENABLE_CDMSGERRINF
	   cdmsgErrInf.getInfo_IdxLessZero++;
#endif	//ENABLE_CDMSGERRINF
	   return 0;	//indicate an error
	}
	return cdmessagesSystemArray[idx].Info;
}




#endif //_CDMESSAGE_C_

