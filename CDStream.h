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
//// File: cdstream.h                                                   ////
//// Description:                                                       ////
////    This file has the variable, structure and function definition   ////
////     to manage a generic stream (actually for 16bits values).       ////
////                                                                    ////
//// THIS TYPE OF STREAM IS AS ARRAY BUT WITH THE POWER TO BE WRITTEN   ////
////   AND READ SERIALLY; ACTUALLY IS IMPLEMENTED ONLY FIFO TYPE.       ////
//// THIS STREAM IS MORE SLOW AND CODE CONSUMING RESPECT ARRAY BUT HAVE ////
////   MORE CAPABILITY AND VERATILITY VS ARRAY, FOR THIS REASON REDUCE  ////
////   USER CODE AND SAVE TIME                                          ////

//// TO CREATE AND INTIALIZE THIS STREAM YOU MUST ALLOCATE A            ////
////   stream_struct TYPE AND A UINT16 ARRAY AND PASS THEY TO INITIALI- ////
////   ZATION FUNCTION                                                  ////   
//// cdstreamInit(&stream, Buffer,BufSize) intialize stream             ////
////     The stream is a allocated stream_struct and the buffer is a    ////
////     allocated array pointer; BufSize is the number of elements of  ////
////                                allocated array (int16)             ////

//// TO READ AND WRITE VALUES IN THE STREAM ....
//// cdstreamPutVal(&stream, Val)   write a val (int16) in the stream   ////
//// cdstreamPutArrayVal(&stream, &pData[0], iNumData) write a series   ////
////                              of vals (ptr uitn16 array) inside the ////
////                              indicated streamer
//// cdstreamRemoveLastVal(&stream) remove last written val in stream   ////
//// cdstreamGetVal(&stream)	  returns the actual readable val in the////
////                                stream, and update internal counters//// 
//// cdstreamPregetVal(&stream)   returns the actual readable val in the////
////                                stream, but doesn't update counters ////
////                              Next time you read re-read this val   ////
//// cdstreamDiscardGetVal(&stream) Discard actual getval, usefull with ////
////                                   cdstreamPregetVal                ////
//// cdstreamRestartGetVal(&stream)  reset the logical pointer for      ////
////        cdstreamGetVal so return at the start of stream buffer to   ////
////        reread all data in                                          ////

//// IS POSSIBLE TO ASK HOW MANY VALS ARE STORED IN BUT NOT READ, HOW   ////
//// BIG IS THE STREAM AND HOW MANY VALS COULD BE STORED YET.           ////
//// cdstreamGetNumVals(&stream)      returns the number of vals (int16)////
////                                    wainting to be read in stream   ////
//// cdstreamPutAvilableVals(&stream) returns the number of vals (int16)////
////                                    that could be written in stream ////
//// cdstreamGetSize(&stream)         returns the total number of vals  ////
////                                    (int16) storable in the stream  ////
////                                    (buffer)                        ////

//// IS POSSIBLE TO BLOCK READING (APPEAR AS THERE ISN'T VALS TO BE     ////
////   READ)BUT COULD WRITE DATA IN.                                    ////
//// cdstreamBlockRead(&stream, true)  block the reading                ////
//// cdstreamBlockRead(&stream, false) re-enable the reading            ////
//// cdstreamIsBlockedRead(&stream)    return true if read is blocked   ////

//// NORMALLY WHEN THERE ISN'T VALS TO BE READ THEN READ AND WRITE      ////
////   POINTERS ARE REINITIALIZED (AUTOFLUSH), BUT THIS FUNCTIONALITY   ////
////   COULB BE DISABLED.                                               ////
//// cdstreamAutoFlush(&stream,false) disable autoflush                 ////
//// cdstreamAutoFlush(&stream,true)  re-enable autoflush               ////
//// cdstreamIsAutoFlushed(&stream)   return true if uatoflush enabled  ////
//// cdstreamFlush(&stream)           empty the stream buffer; reset    ////
////                                    internal counters               ////

//// THERE IS THE POSSIBILITY TO COPY VALS FROM ONE CDSTREAM TO ANOTHER ////
////   CDSTREAM; TWO METHOD EXIST, ONE MORE SLOW BUT SAFE AND ONTHER    //// 
////   MORE FAST BUT UNSAFE BECAUSE DOESN'T DETECT SOME ERRORS          ////
//// cdstreamToCDStream(&stream_org, &stream_dest, numOfVals) copy      ////
////                                      numOfVals Vals from stream_org////
////                                      stream to stream_dest stream  ////
//// fastCDStreamToCDStream(&stream_org, &stream_dest, numOfVals) copy  ////
////                                      numOfVals Vals from stream_org////
////                                      stream to stream_dest stream  ////
////                                      in fast mode (unsafe, some    ////
////                                      errors is not detected)       ////

//// THERE ARE THE CAPABILITY TO GETS THE VALUE (OFFSET FROM START OF   ////
////    ARRAY/STREAM) OF INTERNAL COUNTER ALSO FOR READ_COUNTER THAT    ////
////    FOR WRITE COUNTER; IS ALSO POSSIBLE TO MODIFY THEY              ////
//// cdstreamGetPosPut(x)     returns the actual position of Put (write)////
////                            position counter                        //// 
//// cdstreamGetPosGet(x)     returns the actual position of Get (read) ////
////                            position counter                        ////
//// cdstreamSetPosPut(x,y)   Set the internal position of Put(write)   ////
////                            position counter, that must be new      ////
////                            position where you want to write        ////
//// cdstreamSetPosGet(x,y)   Set the internal position of Get(read)    ////
////                            position counter, that must be new      ////
////                            position that next getval operation     ////
////                            must read                               ////

//// SOMETIME COULD HAPPEN ERRORS AND THE STREAM FUNCTIONS RETURNS      ////
//// NEGATIVE NUMBERS; TO KNOW THE ERROR CODE HAPPENED THERE ARE        ////
//// FOLLOWING FUNCTIONS:                                               ////
//// cdstreamGetError(&Stream) return the error value (is a bit map     ////
////                             value) and reset errors in the stream  ////


//// IS POSSIBLE EXTRACT DIRECTLY FROM STREAM A 8BIT,16BIT,32BIT VALUE  ////
//// TREATING THE STREAM AS IS A 8BIT STREAM (EVERY VAL IS 8BITS),      //// 
//// REMEMBER THAT, IN TRUTH, STREAM STORES 16BITS VALUES.              ////
//// THE EXTRACTED VALUE ARE BUILDED AS BIGENDIAN NUMBER (MSB FIRST)    ////
//// cdstream8ToUINT8(&stream)    extract from stream a unsigned 8bits  ////
//// cdstream8ToUINT16(&stream)   extract from stream a unsigned 16bits ////
//// cdstream8ToUINT32(&stream,&Val32bit) extract from stream a unsigned////
////                                        32bits value                ////
//// cdstream8ToString(&stream, &Dest, dest_Size ) estract null terminat////
////                                               -ed string           ////

//// IS POSSIBLE STORE DIORECTLY IN THE STREAM A 8,16,32 BITS VALUES    //// 
//// TREATING THE STREAM AS IS A 8BIT STREAM (EVERY VAL IS 8BITS),      //// 
//// REMEMBER THAT, IN TRUTH, STREAM STORES 16BITS VALUES.              ////
//// THE INSERTED VALUES ARE BUILDED AS BIGENDIAN NUMBER (MSB FIRST)    ////
//// UINT8toCDStream8(&stream,Val32bit)  add one val (formatted as 8    ////
////                                        bits) to stream             ////
//// UINT16toCDStream8(&stream,Val16bit) add two val (formatted as 8    ////
////                                        bits MSBfirst) to stream    ////
//// UINT32toCDStream8(&stream,Val32bit) add four val (formatted as 8   ////
////                                        bits MSBfirst) to stream    ////
//// stringToCDstream8(&stream, &Orig, Orig_size ) add many vals (format////
////                                               ted as 8bits MSBfirst)///
////                                               to stream as string  //// 

////////////////////////////////////////////////////////////////////////////


#ifndef __CDSTREAM_H_
#define __CDSTREAM_H_

#ifdef DOXYGEN
    #define section( YY )
#endif

#include "CD_types.h"

/*! \def CDSTREAMER_MAX_BUFFER_SIZE
	used to indicate maximun possible size for streamBuffer; it isn't the default size, but the size beyound which will generate an error
	For this reasono this is the systema maximun allocable memory for single streamer intended like number of vals (int16 for this dsp) 
    Now is aprox 4MB
*/
#define CDSTREAMER_MAX_BUFFER_SIZE	0x3fffff


typedef struct CDStream_struct_tag{
	//uint8_t *buffPtr;	//!< pointer to external allocated buffer
	uint16_t *buffPtr;	//!< pointer to external allocated buffer (used 16bits to avoid memory align problems );will be casted to 8bits
	uint32_t  buffSize;	//!< indicates the maximun size for buffer; is sets at the init phase
	uint32_t  posWrite;	//!< indicates the position, from 0, of next write in the buffer
	uint32_t  posRead;	//!< indicates the position, from 0, of next read in the buffer
	uint32_t  errors;	//!< the bitmap for errors, every bit set to 1 indicates a different type of error
	int blockRead;		//!< boolean that indicates if read is blocked, always return 0 bytes available to be read
	int autoFlush;		//!< boolean that indicates if reset counter when there isn't vals to be read (all vals read)
 }CDStream_t;

#define CDSTREAMERR_NO_ERROR	0x0000
#define CDSTREAMERR_UNITIALIZED	0x0001
#define CDSTREAMERR_READ_EMPTY	0x0002
#define CDSTREAMERR_WRITE_FULL	0x0004
 
 

//int cdinitStream(CDStream_t* pStream, uint8_t*pPtrBuff,uint32_t pBuffSize);	//!< initialize the streamer data (and structure)
int cdstreamInit(CDStream_t* pStream, uint16_t* pPtrBuff,uint32_t pBuffSize);	//!< initialize the streamer data (and structure)
//int cdstreamPutVal(CDStream_t* pStream, uint8_t pData); //!< write a byte inside the indicated streamer
int cdstreamPutVal(CDStream_t* pStream, uint16_t pData); //!< write a byte inside the indicated streamer
int cdstreamPutArrayVal(CDStream_t* pStream, uint16_t *pData, int iNumData);	//!< write a series of vals (uitn16 array) inside the indicated streamer

int cdstreamRemoveLastVal(CDStream_t* pStream);	//!<remove last putted byte in stream buffer, and return its value

sint32_t cdstreamGetNumVals(CDStream_t* pStream);	//!< returns the number of vals wainting to be read from indicated stream
sint32_t cdstreamGetVal(CDStream_t* pStream);		//!< returns the next available byte from the stream, and remove it from the stream
sint32_t cdstreamPregetVal(CDStream_t* pStream);	//!< returns the next available byte from the stream, but doesn't remove it from stream, so next timer you read the same value

uint32_t cdstreamGetError(CDStream_t* pStream);	//!< return the error value (is a bit map value) and reset it in the stream
sint32_t cdstreamFlush(CDStream_t* pStream);		//!< empty the stream buffer
sint32_t cdstreamPutAvilableVals(CDStream_t* pStream);	//!< return the number of available bytes that could be written into indicated stream

sint16_t cdstream8ToUINT8(CDStream_t* cds); 				//!< extract from stream a unsigned byte 
sint32_t cdstream8ToUINT16(CDStream_t* cds); 			//!< extract from stream a unsigned 16bits value 
int cdstream8ToUINT32(CDStream_t* cds,uint32_t* ptrVal);  //!< extract from stream a unsigned 32bits value 
int cdstream8ToString(CDStream_t* cds, uint8_t* ptrDest, uint32_t maxSize );  //!< extract from stream a string (nell terminated array of uint8_t)
int UINT32toCDStream8(CDStream_t* cds, uint32_t pQuadByte); //!< add four val (formatted like 8bits) to stream by a unsigned 32bits (first val is MSB last is LSB of 32bits)
int UINT24toCDStream8(CDStream_t* cds, uint32_t pQuadByte); //!< add four val (formatted like 8bits) to stream by a unsigned 24bits (low part of 32bits)(first val is MSB last is LSB of 24bits)	
int UINT16toCDStream8(CDStream_t* cds, uint16_t pDiByte);   //!< add two val (formatted like 8bits) to stream by a unsigned 16bits (first val is MSB last is LSB of 16bits)
int UINT8toCDStream8(CDStream_t* cds, uint8_t pByte);       //!< add one val (formatted like 8bits) to stream by a unsigned 8bits
int stringToCDstream8(CDStream_t* cds, uint8_t* ptrOrig, uint32_t maxSize ); //!< add many vals (formatted as 8bits) to stream by a string (null terminated uint8 array)

int cdstreamToCDStream(CDStream_t* org, CDStream_t* dest, uint32_t numOfVals);	//!< copy numOfVals Vals from org stream to dest stream
int fastCDStreamToCDStream(CDStream_t* org, CDStream_t* dest, uint32_t numOfVals);//!< copy numOfVals Vals from org to dest stream in fast mode (unsafe, some errors is not detected)       ////




/*! \def cdstreamDiscardGetVal(xx)
	discard actual getval, usefull with cdstreamPregetVal
	\n Param xx is pStream pointer to CDstream (data structure)
*/
#define cdstreamDiscardGetVal(xx) (((CDStream_t*)(xx))->posRead++)


/*! \def cdstreamGetSize(xx)
	return the size of stream buffer in number of storable Vals (int16)
	\n Param xx is pStream pointer to CDstream (data structure)
*/
#define cdstreamGetSize(xx) (((CDStream_t*)(xx))->buffSize)


//void cdstreamRestartGetVal(CDStream_t* pStream);	//!< reset the logical pointer for getByte so return at the start of stream buffer to reread all data in the stream buffer
/*!	\def cdstreamRestartGetVal(xx)
	reset the logical pointer for getByte so return at the start of stream buffer to reread all data in the stream buffer
	\n Param xx is pStream pointer to CDstream (data structure)
*/
#define cdstreamRestartGetVal(xx) ((CDStream_t*)(xx))->posRead=0


//void cdstreamBlockRead(CDStream_t* pStream, int pBlock);	//!< block or re-enable the read for stream
/*!	\def cdstreamBlockRead(xx,yy)
	If set block = true then when ask number of readable vals it always return 0 regardless of the reality, and
	 if you attempt to read a value from stream it will generate an error.
	\n When block is removed (yy =false) then the available byte in stream to be read is the reality.
	\n When block is active (yy=true), is possible to write data in the stream without any error, but is impossible to read
	   values or ask number of readable vals in stream.
	\n Param xx is pStream pointer to CDstream (data structure)
	\n Param yy is a boolean (int) with value true or false
*/
#define cdstreamBlockRead(xx,yy) ((CDStream_t*)(xx))->blockRead=(yy)


//int  cdstreamIsBlockedRead(CDStream_t* pStream);	//!< indicates if stream reading is blocked (return true) or enabled (return false)
/*! \def cdstreamIsBlockedRead(xx)
	indicates if stream reading is blocked (return true) or enabled (return false)
	\n Param xx is pStream pointer to CDstream (data structure)
*/
#define cdstreamIsBlockedRead(xx) (((CDStream_t*)(xx))->blockRead)


/*!	\def cdstreamAutoFlush(xx,yy)
	With reading block is active (pBlock = true) always return 0 byte available to be read from stream regardless
	  of the reality, and will be generated an error when try to read a byte from it.
	\n When reading block is removed (pBlock = false) then the available byte in stream to be read is the reality.
	\n When reading bloc is active, is possible to write data in the stream without any error, please use streamPutAvilableBytes(..)
	  to know real available bytes to be written.
	\n Param xx is pStream pointer to CDstream (data structure)
	\n Param yy is a boolean (int) with value true or false
*/
#define cdstreamAutoFlush(xx,yy) ((CDStream_t*)(xx))->autoFlush=(yy)

/*! \def cdstreamIsAutoFlushed(xx)
	if return true indicates that the stream have the capability to autoflush (resets internal counter when there aren't vals to be read),
	\n Param xx is pStream pointer to CDstream (data structure)
*/
#define cdstreamIsAutoFlushed(xx) (((CDStream_t*)(xx))->autoFlush)


/*!	\def cdstreamGetPosPut(xx)
	returns the actual position of Put (write) position counter, that is the equal to stored vals +1 (+1 because point to new writable location).
	\n Remember that internal counters starts from position 0 so if pos is zero than there isn't previous val
	\n Param xx is pStream pointer to CDstream (data structure)
*/
#define cdstreamGetPosPut(xx) (((CDStream_t*)(xx))->posWrite)

/*!	\def cdstreamSetPosPut(xx,yy)
	Set the internal position of Put(write) position counter, that must be new position where you want to write;
	 the new position must be less than stream size [streamGetSize(&stream)] .
	\n Remember that internal counters starts from position 0.
	\n Param xx is pStream pointer to CDstream (data structure)
	\n Param yy is a unsigned integer that must be less than streamGetSize(&stream) 
*/
#define cdstreamSetPosPut(xx,yy) ((CDStream_t*)(xx))->posWrite=(yy)



/*!	\def cdstreamGetPosGet(xx)
	returns the actual position of Get (read) position counter, that is the equal last read val +1 (+1 because point to next readable location)..
	\n Remember that internal counters starts from position 0 so if pos is zero than there isn't previous val
	\n Param xx is pStream pointer to CDstream (data structure)
*/
#define cdstreamGetPosGet(xx) (((CDStream_t*)(xx))->posRead)

/*!	\def cdstreamSetPosGet(xx,yy)
	Set the internal position of Get(read) position counter, that must be actual position that next getval operation must read;
	 the new position must be less than stream size [streamGetSize(&stream)] .
	\n Remember that internal counters starts from position 0.
	\n Param xx is pStream pointer to CDstream (data structure)
	\n Param yy is a unsigned integer that must be less than streamGetSize(&stream) 
*/
#define cdstreamSetPosGet(xx,yy) ((CDStream_t*)(xx))->posWrite=(yy)



#endif //__CDSTREAM_H_
