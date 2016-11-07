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
//// Created on 26/07/2012                                              ////
//// File: cdstream.c                                                   ////
//// Description:                                                       ////
////    This file has the functions to manage a generic byte stream.    ////
////    see .h for help                                                 ////  
////                                                                    ////
////                                                                    ////
////////////////////////////////////////////////////////////////////////////



//#ifndef _CDSTREAM_C_
//#define _CDSTREAM_C_

#include <stdlib.h>
#include "cdstream.h"

#ifdef DOXYGEN
    #define section(YY)
#endif


/*! \fn int cdstreamInit(CDStream_t* pStream, uint16_t* pPtrBuff,uint32_t pBuffSize)
	\author Dario Cortese
	\date 26-07-2012
	\version 1.0
	\brief initialize the streamer data (and structure) 
	\param pStream is the pointer to CDstream structure to initialize
	\param pPtrBuff is the pointer to external allocated buffer
	\param pBuffSize is the dimension, in bytes, of external allocated buffer
	\return 0 or positive number if all ok, otherwise return -1 or negative numbers
	\todo nothing
*/
int cdstreamInit(CDStream_t* pStream, uint16_t* pPtrBuff, uint32_t pBuffSize){
	if( pStream== NULL ) return -1;
    pStream->posWrite =0;
	pStream->posRead =0;
	pStream->errors= CDSTREAMERR_UNITIALIZED;
	
	if( pPtrBuff== NULL ) return -2;
	pStream->buffPtr = pPtrBuff;
	
	if( pBuffSize==0 ) return -3;
	if( pBuffSize > CDSTREAMER_MAX_BUFFER_SIZE ) return -4;
	pStream->buffSize = pBuffSize;	
	
	pStream->errors= CDSTREAMERR_NO_ERROR;
	
	pStream->autoFlush=true;
	pStream->blockRead=false;
	return 0;
} 


/*! \fn int cdstreamPutVal(CDStream_t* pStream, uint16_t pData)
	\author Dario Cortese
	\date 26-07-2012
	\version 1.0
	\brief write a byte inside the indicated streamer
	\param pStream is the pointer to CDstream (data structure)
	\param pData is the data (16bits unsigned) to store in the stream
	\return 0 or positive number if all ok, otherwise return -1 or negative numbers to indicates errors
	\note for error code see STREAMERR_WRITE_FULL or STREAMERR_UNITIALIZED
	\see cdstreamGetError  
	\todo test it
*/
section("L1_code")
int cdstreamPutVal(CDStream_t* pStream, uint16_t pData){
	uint32_t numB;
	if( pStream== NULL ) return -1;
	if(( pStream->errors & CDSTREAMERR_UNITIALIZED )!=0) return -2;
	
	if(	pStream->posWrite >= pStream->buffSize){
		//error, write will be over the buffer
		pStream->errors |= CDSTREAMERR_WRITE_FULL ;
		return -3;
	}
	pStream->buffPtr[ pStream->posWrite ]= pData;
	pStream->posWrite++;
	return 0;
}


/*! \fn int cdstreamPutArrayVal(CDStream_t* pStream, uint16_t *pData, int iNumData)
	\author Dario Cortese
	\date 30-01-2013
	\version 1.0
	\brief write a series of vals (uitn16 array) inside the indicated streamer
	\param pStream is the pointer to CDstream (data structure)
	\param pData is the data source pointer (16bits unsigned pointer) for data to store in the stream
	\param iNumData is the number of data to transfer in stream
	\return 0 or positive number if all ok, otherwise return -1 or negative numbers to indicates errors
	\note for error code see STREAMERR_WRITE_FULL or STREAMERR_UNITIALIZED
	\see cdstreamGetError  
	\todo test it
*/
section("L1_code")
int cdstreamPutArrayVal(CDStream_t* pStream, uint16_t *pData, int iNumData){
	uint16_t* u16ptr; 
	if( pStream== NULL ) return -1;
	if(( pStream->errors & CDSTREAMERR_UNITIALIZED )!=0) return -2;
	
	if(	(pStream->posWrite + iNumData) >= pStream->buffSize) {
		//error, write will be over the buffer
		pStream->errors |= CDSTREAMERR_WRITE_FULL;
		return -3;
	}
	u16ptr = &pStream->buffPtr[ pStream->posWrite ];
	pStream->posWrite += iNumData;
	for(;iNumData>0;iNumData--){
		*u16ptr = *pData;
		pData++;
		u16ptr++;
	}	
	return 0;
}





/*! \fn sint32_t cdstreamGetNumVals(CDStream_t* pStream)
	\author Dario Cortese
	\date 26-07-2012
	\version 1.0
	\brief return the number of bytes wainting to be read from indicated stream
	\param pStream is the pointer to CDstream (data structure)
	\return 0 or a positive number to indicates bytes in the stream, otherwise return -1 or negative numbers to indicates errors
	\note if BlockRead is activated then this function return always 0
	\note for error code see STREAMERR_UNITIALIZED
	\see cdstreamGetError  
	\todo nothing
*/
sint32_t cdstreamGetNumVals(CDStream_t* pStream){
	sint32_t numB;
	if( pStream== NULL )
	   return -1;
	if(( pStream->errors & CDSTREAMERR_UNITIALIZED )!=0) return -2;
	//if stream read is blocked then return always zero byte readable
	if( pStream->blockRead)
	   return 0;	//read blocked so always indicates 0 available val to be read
	
	numB = pStream->posWrite;
	//if there isn't data in the stream buffer to be more fast then.....
	if(numB == 0)
	   return 0; //no data in the stream
	numB -=  pStream->posRead;
	//if posread is equal or over the poswrite then its mean that there isn't bytes to read so....
	if(numB <= 0){
		if(pStream->autoFlush){
			//reset the logic counter (restart)
		   pStream->posWrite =0;
			pStream->posRead =0;
		}
		return 0; //no data in the stream
	}
	return numB;	//return the number of bytes (a positive number more than 0)
}



/*! \fn sint32_t  cdstreamPregetVal(CDStream_t* pStream)
	\author Dario Cortese
	\date 26-07-2012
	\version 1.0
	\brief return the next available byte from the stream, but doesn't remove it from stream, so next timer you read stream, it return the same actual value
	\param pStream is the pointer to CDstream (data structure)
	\return 0 or a positive number (up to 65535) for the value read in the stream, otherwise return -1 or negative numbers to indicates errors
	\note if BlockRead is activated then this function generate STREAMERR_READ_EMPTY error and return -4
	\note for error code see STREAMERR_READ_EMPTY or STREAMERR_UNITIALIZED
	\see cdstreamGetError  
	\todo test it
*/
sint32_t  cdstreamPregetVal(CDStream_t* pStream){
	uint16_t val;
	sint32_t numB;
	if( pStream== NULL ) return -1;
	if(( pStream->errors & CDSTREAMERR_UNITIALIZED )!=0) return -2;
	numB = cdstreamGetNumVals(pStream);
	//if happened an error into streamGetNumBytes 
	if(numB < 0) return -3;	//error in a sub call
	if(numB == 0){
		//generate in the stream the reading an empty stream error
		pStream->errors |= CDSTREAMERR_READ_EMPTY;
		return -4; //error for empty stream
	} 
	//if numB > 0 then
	val= pStream->buffPtr[ pStream->posRead ];
	//return (sint32_t)val & 0x0000FFFF;	
	return val; //compiler convert it	
}



/*! \fn sint32_t cdstreamGetVal(CDStream_t* pStream)
	\author Dario Cortese
	\date 26-07-2012
	\version 1.0
	\brief return the next available byte from the stream, and remove it from the stream
	\param pStream is the pointer to CDstream (data structure)
	\return 0 or a positive number (up to 65535) for the value read in the stream, otherwise return -1 or negative numbers to indicates errors
	\note if BlockRead is activated then this function generate STREAMERR_READ_EMPTY error and return -4
	\note for error code see STREAMERR_READ_EMPTY or STREAMERR_UNITIALIZED
	\see cdstreamGetError  
	\todo test it
*/
sint32_t  cdstreamGetVal(CDStream_t* pStream){
	sint32_t val;
	val = cdstreamPregetVal(pStream);
	//if no error happen then increment read logical pointer for the stream
	if( val >= 0){
		//pStream->posRead++;
		cdstreamDiscardGetVal(pStream);
	}
	return val;
}		



/*! \fn uint32_t cdstreamGetError(CDStream_t* pStream)
	\author Dario Cortese
	\date 26-07-2012
	\version 1.0
	\brief return the error value (is a bit map value) and reset it in the stream
	\param pStream is the pointer to CDstream (data structure)
	\return the error value (uint32) but is a bit map
	\note see STREAMERR_....
	\todo test it
*/
uint32_t cdstreamGetError(CDStream_t* pStream){
	uint32_t errval;
	//if the pointer is null then return error for uninitialized stream
	if( pStream== NULL ) return CDSTREAMERR_UNITIALIZED;
	
	errval= pStream->errors; 
	//never must be reset the unitialized stream signaling/error
	if((errval & CDSTREAMERR_UNITIALIZED) !=0){
		pStream->errors= CDSTREAMERR_UNITIALIZED;
	}else{
		pStream->errors= CDSTREAMERR_NO_ERROR;
	}
	return errval;
}



/*! \fn sint32_t cdstreamFlush(CDStream_t* pStream)
	\author Dario Cortese
	\date 26-07-2012
	\version 1.0
	\brief empty the stream buffer and reset errors
	\param pStream is the pointer to CDstream (data structure)
	\return 0 or positive number to indicate the number of flushed bytes, otherwise return -1 or negative numbers to indicates errors
	\note for error code see STREAMERR_UNITIALIZED
	\see cdstreamGetError  
	\todo test it
*/
sint32_t cdstreamFlush(CDStream_t* pStream){
	sint32_t numB;
	
	if( pStream== NULL ) return -1;
	numB = cdstreamGetNumVals(pStream);
	if( numB < 0) return numB;
	//else numB if 0 or more then mean that pStream is initializated and not null 
    pStream->posWrite =0;
    pStream->posRead =0;
	pStream->errors= CDSTREAMERR_NO_ERROR;	//reset errors
	return numB;
}



/*! \fn int cdstreamRemoveLastVal(CDStream_t* pStream)
	\author Dario Cortese
	\date 31-07-2012
	\version 1.0
	\brief remove last putted byte in stream buffer, and return its value
	\param pStream is the pointer to CDstream (data structure)
	\return 0 or positive number to indicate the value of removed byte, otherwise return -1 or negative numbers to indicates errors
	\todo test it
*/
int cdstreamRemoveLastVal(CDStream_t* pStream){
	int val;

	if( pStream== NULL ) return -1;
	if(( pStream->errors & CDSTREAMERR_UNITIALIZED )!=0) return -2;
	if(pStream->posWrite >= 1){
		pStream->posWrite--;
		val = pStream->buffPtr[ pStream->posWrite ];
		return val;	
	}
	return -3;	//error 
}

/*! \fn sint32_t cdstreamPutAvilableVals(CDStream_t* pStream)
	\author Dario Cortese
	\date 30-07-2012
	\version 1.0
	\brief return the number of available bytes that could be written into indicated stream
	\param pStream is the pointer to CDstream (data structure)
	\return 0 or a positive number to indicates bytes in the stream, otherwise return -1 or negative numbers to indicates errors
	\note for error code see STREAMERR_UNITIALIZED
	\see ccstreamGetError  
	\todo test it
*/
sint32_t cdstreamPutAvilableVals(CDStream_t* pStream){
	sint32_t numB;
	if( pStream== NULL ) return -1;
	if(( pStream->errors & CDSTREAMERR_UNITIALIZED )!=0) return -2;
	numB = pStream->buffSize;
	numB -= pStream->posWrite;
	//if poswrite is equal or over the buffsize then its mean that there isn't bytes to write (buffer full).
	if(numB <= 0){
		return 0; //no data in the stream
	}
	return numB;	//return the number of available bytes in the buffer
}

/*/! \fn void cdstreamRestartGetByte(CDStream_t* pStream)
	\author Dario Cortese
	\date 30-07-2012
	\version 1.0
	\brief reset the logical pointer for getByte so return at the start of stream buffer to reread all data in the stream buffer
	\param pStream is the pointer to CDstream (data structure)
	\warning this is trasformed in a macro in .h
void cdstreamRestartGetByte(CDStream_t* pStream){
    pStream->posRead =0;
}
*/

/*/! \fn void cdstreamBlockRead(CDStream_t* pStream, int pBlock)
	\author Dario Cortese
	\date 30-07-2012
	\version 1.0
	\brief block or re-enable the read for stream
	With reading block is active (pBlock = true) always return 0 byte available to be read from stream regardless
	  of the reality, and will be generated an error when try to read a byte from it.
	\n When reading block is removed (pBlock = false) then the available byte in stream to be read is the reality.
	\n When reading bloc is active, is possible to write data in the stream without any error, please use streamPutAvilableBytes(..)
	  to know real available bytes to be written.
	\param pStream is the pointer to CDstream (data structure)
	\param pBlock is a boolean that indicates if enable (pBlock=true) or disable (pBlock=false) the read block
	\note when attempt to read a byte from the stream with read blocket the will be generated an STREAMERR_READ_EMPTY
	\warning this is trasformed in a macro in .h
void cdstreamBlockRead(CDStream_t* pStream, int pBlock){
	pStream->blockRead=pBlock;
}
*/


/*/! \fn int cdstreamIsBlockedRead(CDStream_t* pStream)
	\author Dario Cortese
	\date 30-07-2012
	\version 1.0
	\brief indicates if stream reading is blocked (return true) or enabled (return false)
	\param pStream is the pointer to cdstream (data structure)
	\return the state of streamer reading block (true or false)
	\note when attempt to read a byte from the stream with read blocket the will be generated an STREAMERR_READ_EMPTY
	\warning this is trasformed in a macro in .h
int cdstreamIsBlockedRead(CDStream_t* pStream){
	return pStream->blockRead;
}
*/



//*********************************************************************************************************************
//*********************************************************************************************************************
//*********************************************************************************************************************
//*********************************************************************************************************************

/*!	\fn sint16_t cdstream8ToUINT8(CDStream_t* cds){
	\author Dario Cortese
	\date 31-07-2012
	\version 1.0
	\brief extract from stream a unsigned byte 
	extract val from stream and convert it in byte (range value from 0 to 255), after return it.
	\n if there is an error return a negative number, but if value from stream is positive but over 255, it will be masked with 0xFF and no error return
	\param cds is the cdstream pointer
	\return a signed 16bits that if negative indicates error, if 0 to 255 indicates extracted uint8 value
	\note if the wanted value is a signed int8 then cast the returned value to it after you have checked the negative value for errors
*/
sint16_t cdstream8ToUINT8(CDStream_t* cds){
	sint32_t val= cdstreamGetVal(cds);
	if(val<0) return val;
	return val&0xFF;
}

/*!	\fn sint32_t cdstream8ToUINT16(CDStream_t* cds){
	\author Dario Cortese
	\date 31-07-2012
	\version 1.0
	\brief extract from stream a unsigned 16bits value 
	extract two vals from stream and convert they in bytes, after use the first like MSB of 16bits and the second like LSB, after return the 16bits value  (range value from 0 to 65535).
	if there is an error return a negative number, but if single val extrated from stream is positive but over 255, it will be masked with 0xFF and no error return
	\param cds is the cdstream pointer
	\return a signed 32bits that if negative indicates error, if 0 to 65535 indicates extracted uint16 value
	\note if the wanted value is a signed int16 then cast the returned value to it after you have checked the negative value for errors
*/
sint32_t cdstream8ToUINT16(CDStream_t* cds){
	sint32_t val= cdstreamGetVal(cds);
	sint32_t val2= cdstreamGetVal(cds);
	if(val<0) return val;
	if(val2<0) return val2;
	//val2&=0xFF;
	//val<<=8;
	//val&=0xFF00;
	//val+=val2;
	//return val;
	return ((val2&0xFF)+((val<<8)&0xFF00));
}





/*!	\fn int cdstream8ToUINT32(CDStream_t* cds,uint32_t* ptrVal)
	\author Dario Cortese
	\date 31-07-2012
	\version 1.0
	\brief extract from stream a unsigned 32bits value 
	extract four vals from stream and convert they in bytes, after use the first like MSB of 32bits and the fourth like LSB, after return the 32bits value
	if there is an error return a negative number, but if single val extrated from stream is positive but over 255, it will be masked with 0xFF and no error return
	\param cds is the cdstream pointer
	\param ptrVal is a pointer to external uint32 that is used to store extracted uint32 from stream values 
	\return if negative indicates error, if 0 or more that all is ok
	\note if the wanted value is a signed int32 then cast the returned value to it after you have checked if the returned value is negative (to check errors)
*/
int cdstream8ToUINT32(CDStream_t* cds, uint32_t* ptrVal){
	sint32_t val= cdstreamGetVal(cds);
	sint32_t val2= cdstreamGetVal(cds);
	sint32_t val3= cdstreamGetVal(cds);
	sint32_t val4= cdstreamGetVal(cds);
	if(val<0) return val;
	if(val2<0) return val2;
	if(val3<0) return val3;
	if(val4<0) return val4;
	
	//*ptrVal =(((uint32_t)val <<24) & 0xFF000000);
	//*ptrVal +=(        (val2 <<16) & 0x00FF0000);
	//*ptrVal +=(        (val3 <<8)  & 0x0000FF00);
	//*ptrVal +=(         val4       & 0x000000FF);
	*ptrVal= ((((uint32_t)val<<24)&0xFF000000)+((val2<<16)&0xFF0000)+((val3<<8)&0xFF00)+(val4&0xFF));
	return 0; //all ok
}




/*! \fn int cdstream8ToString(CDStream_t* cds, uint8_t* ptrDest, uint32_t maxSize )
	\author Dario Cortese
	\date 03-12-2013
	\version 1.0
	\brief extract from stream a null terminated char string 
	extract a maxSize vals from stream and convert they in chars (uint8_t), or extract a number of vals until find a val equal '\0'.
	\n Extracted chars are copied in the ptrDest buffer and returns the number of copied chars.
	\n If an error occours then return a negative value and in the ptrDest buffer will be inserted a '\0'
	\param cds is the cdstream pointer
	\param ptrDest is a pointer to external uint8_t buffer that is used to store extracted string from stream values
	\param maxSize is the size of ptrDest buffer 
	\return if negative indicates error, if 0 or more that all is ok
*/
int cdstream8ToString(CDStream_t* cds, uint8_t* ptrDest, uint32_t maxSize ){
   sint32_t val;
   uint32_t initSize = maxSize;
   if( maxSize < 2 ) return -1; //size too small
   maxSize--;  //reduce size for the /0 terminator
   do{    
	   val= cdstreamGetVal(cds);
	   if(val < 0){
	      //stop to copy and trunk the string putting a null terminated char
	      *ptrDest = '\0';
	      return val; //iterate/report the error
	   }
      *ptrDest= (uint8_t)val;
      if(val== 0) break;   //exit from loop
	   ptrDest++;
	   maxSize--;
   }while(maxSize > 0);
   //if exit for over size then past a \0 in the dest buffer as last char
   if(maxSize ==0) *ptrDest = '\0'; 
   //return the number of copied chars
   return (initSize - maxSize);
}



/*! \fn int UINT8toCDStream8(CDStream_t* cds, uint8_t pByte)
	\author Dario Cortese
	\date 02-08-2012
	\version 1.0
	\brief add one val (formatted like 8bits) to stream by a unsigned 8bits
	\param cds is the cdstream pointer
	\param pByte is the 8bits to add as answer; must be signed or unsiged but must be casted to unsigned
	\return 0 or positive number if all ok, otherwise return -1 or negative numbers to indicates errors
*/
int UINT8toCDStream8(CDStream_t* cds, uint8_t pByte){
	return cdstreamPutVal(cds, pByte);
}



/*! \fn int UINT16toCDStream8(CDStream_t* cds, uint16_t pDiByte)
	\author Dario Cortese
	\date 02-08-2012
	\version 1.0
	\brief add two val (formatted like 8bits) to stream by a unsigned 16bits (first val is MSB last is LSB of 16bits)
	\param cds is the cdstream pointer
	\param pDiByte is the 16bits to add as answer; must be signed or unsiged but must be casted to unsigned
	\return 0 or positive number if all ok, otherwise return -1 or negative numbers to indicates errors
*/
int UINT16toCDStream8(CDStream_t* cds, uint16_t pDiByte){
	//check if in streamer there is almost two bytes free
	if(cdstreamPutAvilableVals(cds)<2) return -20;
	cdstreamPutVal(cds, (uint8_t)((pDiByte>>8) & 0xff));
	cdstreamPutVal(cds, (uint8_t)(pDiByte & 0xff));
	return 2;	//added 2 bytes
}






/*! \fn int UINT24toCDStream8(CDStream_t* cds, uint32_t pQuadByte)
	\author Dario Cortese
	\date 25-10-2012
	\version 1.0
	\brief add three val (formatted like 8bits) to stream by a unsigned 32bits (first val is MSB last is LSB of 32bits)
	\param cds is the cdstream pointer
	\param pQuadByte is the 32bits to add as answer; must be signed or unsiged but must be casted to unsigned
	\return 0 or positive number if all ok, otherwise return -1 or negative numbers to indicates errors
*/
int UINT24toCDStream8(CDStream_t* cds, uint32_t pQuadByte){
	//check if in streamer there is almost four bytes free
	if(cdstreamPutAvilableVals(cds)<3) return -20;
	cdstreamPutVal(cds, (uint8_t)((pQuadByte>>16) & 0xff));
	cdstreamPutVal(cds, (uint8_t)((pQuadByte>>8) & 0xff));
	cdstreamPutVal(cds, (uint8_t)(pQuadByte & 0xff));
	return 4; //added 4 bytes
}





/*! \fn int UINT32toCDStream8(CDStream_t* cds, uint32_t pQuadByte)
	\author Dario Cortese
	\date 02-08-2012
	\version 1.0
	\brief add four val (formatted like 8bits) to stream by a unsigned 32bits (first val is MSB last is LSB of 32bits)
	\param cds is the cdstream pointer
	\param pQuadByte is the 32bits to add as answer; must be signed or unsiged but must be casted to unsigned
	\return 0 or positive number if all ok, otherwise return -1 or negative numbers to indicates errors
*/
int UINT32toCDStream8(CDStream_t* cds, uint32_t pQuadByte){
	//check if in streamer there is almost four bytes free
	if(cdstreamPutAvilableVals(cds)<4) return -20;
	cdstreamPutVal(cds, (uint8_t)((pQuadByte>>24) & 0xff));
	cdstreamPutVal(cds, (uint8_t)((pQuadByte>>16) & 0xff));
	cdstreamPutVal(cds, (uint8_t)((pQuadByte>>8) & 0xff));
	cdstreamPutVal(cds, (uint8_t)(pQuadByte & 0xff));
	return 4; //added 4 bytes
}



/*! \fn int stringToCDstream8(CDStream_t* cds, uint8_t* ptrOrig, uint32_t maxSize )
	\author Dario Cortese
	\date 03-12-2013
	\version 1.0
	\brief inserts in a stream a null terminated char string 
	insert vals in the stream  until find a terminator ('\0') in the ptrOrig string, or goes over maxSize vals, or there isn't available vals in the stream.
	\n If finds a null terminated char in the origin string then return the number of copied chars included the terminator chars.
	\n If goes over maxsize then stops to copy chars and insert in the stream a terminator char, returning a value taht is maxSize+1  
	\n if buffer becomes full then force last char in the buffer to be a terminator char, and return an error (negatives value) 
	\param cds is the cdstream pointer
	\param ptrOrig is a pointer to external uint8_t buffer that is used to gets chars (string)
	\param maxSize is the size of ptrOrig buffer 
	\return a negative value in case of error or  positive number that is the amount fo cahrs copied in the stream
*/
int stringToCDstream8(CDStream_t* cds, uint8_t* ptrOrig, uint32_t maxSize ){
   uint32_t availableVals = cdstreamPutAvilableVals(cds);
   uint32_t copiedchars = 0;
   uint8_t actCh;
   if (maxSize < 1) return -1; //size too small
   do{    
      //verify if there is available chars in the stream
      if(availableVals < 2){
         //if there is only one available val in the stream then
         if(availableVals == 1){
            //put terminator in that value
            cdstreamPutVal(cds, (uint8_t)('\0'));  
            //but don't return an error if actual origin char is a terminator char
            if( *ptrOrig != '\0' ) return -1; //otherwise return an error
         }
         return -2;  //in every case if available vals are zero or less (error code), return an error
      }//end if(availableVals < 2)
      
      actCh = *ptrOrig;
      ptrOrig++;
      cdstreamPutVal(cds, (uint8_t)actCh);  
      availableVals--;
      copiedchars++;
      if ( actCh == '\0' ) break; //exit from loop
      maxSize--;
   }while(maxSize > 0);
   //if exit for over size then past a \0 in the dest buffer as last char
   if(maxSize ==0){
      //put terminator in that value
      cdstreamPutVal(cds, (uint8_t)('\0'));  
      copiedchars++;
   }
   //return the number of copied chars
   return copiedchars;
}



/*! \fn int cdstreamToCDStream(CDStream_t* org, CDStream_t* dest, uint32_t numOfVals){
	\author Dario Cortese
	\date 03-08-2012
	\version 1.0
	\brief copy, nad moves the internal pointers, numOfVals Vals from org stream to dest stream
	
	copy numOfVals value from cdstream org to cdstream dest; the copy start from actual read position of org cdstream and start to copy at actual
	 position of dest cdstream.
	\n At the end of copy the internal counter of org (get counter) and dest (put counter) was incremented of numOfVals.
	\n Before start the function check if are accessible the two cdstreams and if they have sufficient vals to complete copy.
	\n If when copying an error happen, while read org or write dest, a negative number was returned but the streams pointer are restored as entered
	 in this function, pratically no modification are made to stream if an error happen.
	\n To be more precise, when an error copy happen, the values in the dest cdstream from initial position (put position/counter when call this
	 function) are unpredictable changed but the initial position is restored as if you had never written anything. 
	\n The possible errors/negative returned values	 
	\n -1 problem to access org stream
	\n -2 number of readable val in org stream are less than numOfVals
	\n -3 problem to access dest stream
	\n -4 number of writable val in org stream are less than numOfVals
	\n -5 problem to read a val from stream org
	\n -6 problem to write a val to stream dest

	\param org is the cdstream pointer where val will be read
	\param dest is the cdstream pointer where val will be written
	\param numOfVals is the number of vals that will be copied from org to dest 
	\return 0 if all ok, otherwise a negative number indicating error type
	\note to test
*/
int cdstreamToCDStream(CDStream_t* org, CDStream_t* dest, uint32_t numOfVals){
	sint32_t sAppo;
	uint32_t posGet;
	uint32_t posPut;

	if( numOfVals==0) return 0;	//nothing to do
	//checks if the two streamer have space to move numOfVals vals
	sAppo = cdstreamGetNumVals(org);
	//check errors
	if(sAppo<0) return -1; //an error when attempt to access at org stream
	if(sAppo < numOfVals) return -2; //few vals in org stream;
	
	sAppo = cdstreamPutAvilableVals(dest);
	//check errors
	if(sAppo<0) return -3; //an error when attempt to access at dest stream
	if(sAppo < numOfVals) return -4; //few vals in dest stream;

	//store intial position, for rollback action if errors happen while copying
	posGet=cdstreamGetPosGet(org);
	posPut=cdstreamGetPosPut(dest);
	
	//copy cycle
	for( ; numOfVals != 0; numOfVals--){
		sAppo = cdstreamGetVal(org);
		//check error in read
		if(sAppo < 0){
			//restore the original state of streamer
			cdstreamSetPosGet(org,posGet);
			cdstreamSetPosPut(dest,posPut);
			return -5;
		} 
		
		sAppo = cdstreamPutVal(dest, sAppo);
		//check error in write
		if(sAppo < 0){
			//restore the original state of streamer
			cdstreamSetPosGet(org,posGet);
			cdstreamSetPosPut(dest,posPut);
			return -6;
		} 
	}//end for( ; numOfVals != 0; numOfVals--)
	return 0;	//all ok
}


/*! \fn int fastCDStreamToCDStream(CDStream_t* org, CDStream_t* dest, uint32_t numOfVals)
	\author Dario Cortese
	\date 03-08-2012
	\version 1.0
	\brief fast copy, but unsafe, numOfVals Vals from org stream to dest stream
	
	is as cdstreamToCDStream function but doesn't check if errors happen while copyng, moreover uses mem pointer that access directly to internal stream
	 buffers to avoid to spent time to calculate location and check bound limits.
	\n This is dangerous becaus if some change to counters or data inside buffer happen it can't detect and cause unpredictable problem and data corruption.
	\n But if you sure that nothing change streamers when execute this copy, this function allow to save time.
	\n At the end of copy the internal counter of org (get counter) and dest (put counter) was incremented of numOfVals.
	\n Before start the function check if are accessible the two cdstreams and if they have sufficient vals to complete copy.
	\n When copying doesn't detect errors (while read org or write dest)
	\n Only this errors are possible to return	 
	\n -2 problem to access org stream or number of readable val in org stream are less than numOfVals
	\n -4 problem to access dest stream or number of writable val in org stream are less than numOfVals

	\param org is the cdstream pointer where val will be read
	\param dest is the cdstream pointer where val will be written
	\param numOfVals is the number of vals that will be copied from org to dest 
	\return 0 if all ok, otherwise a negative number indicating error type
	\see cdsstreamToCDStream
	\note to test
*/
int fastCDStreamToCDStream(CDStream_t* org, CDStream_t* dest, uint32_t numOfVals){
	sint32_t sAppo;
	uint32_t iCount;
	uint16_t *buffOrgPtr;
	uint16_t *buffDestPtr;
	
	if( numOfVals==0) return 0;	//nothing to do
	//checks if the two streamer have space to move numOfVals vals
	sAppo = cdstreamGetNumVals(org);
	if(sAppo < numOfVals) return -2; //few vals in org stream or an error when attempt to access at org stream
	sAppo = cdstreamPutAvilableVals(dest);
	if(sAppo < numOfVals) return -4; //few vals in dest stream or an error when attempt to access at dest stream

	//copy cycle
	//buffOrgPtr= &org.buffPtr[ org->posRead ]);
	//buffDestPtr= &dest.buffPtr[ dest->posWrite ]);
	
	buffOrgPtr= &(org->buffPtr[ org->posRead ]);
	buffDestPtr= &(dest->buffPtr[ dest->posWrite ]);
	
	for(iCount = numOfVals; iCount != 0; iCount--){
		*buffDestPtr = *buffOrgPtr;
		buffDestPtr++;
		buffOrgPtr++;
	}//end for(iCount = numOfVals; iCount != 0; iCount--)
	org->posRead+=numOfVals;
	dest->posWrite+=numOfVals;
	return 0;	//all ok
}




//#endif //_CDSTREAM_C_
