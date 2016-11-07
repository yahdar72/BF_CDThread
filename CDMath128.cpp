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
//// File: cdMath128.c                                                   ////
//// Description:                                                       ////
////    This file has the functions prototypes and data definitions     ////
////    to manage software math 128bit types                                 ////  
////////////////////////////////////////////////////////////////////////////
#include "CD_types.h"
#include "CDMath128.h"

#ifdef DOXYGEN
    #define section(YY)
#endif


//init ot 0 a 128bit struct
#pragma inline
void CDinit128 (CDnum128_t *Res)
{
	Res->hi=0;
	Res->lo=0;
	Res->neg=false;
}


//convert CDnum128_t to signed inte 64bit; remember that if num128 is higher as value to the possible storable in a 63 bit (with sign) then
// 64 destination will set to maximun positive or maximun negative, depends on sign of 128bit
void CDconvert128ToS64 (CDnum128_t *Org, sint64_t *Dest)
{
	if(( Org->hi != 0) || (Org->lo > 0x7fffffffffffffff)){
		//if 128bit can not contained in the 63bit + sign then
		//checks sign of 128bit and set 64bit (with sing) at maximun positive or negative
		if(Org->neg){
		  *Dest = 0x8000000000000000;	
		}
		else
		{
			*Dest = 0x7fffffffffffffff;
		}
		return;
	}
	else
	{
		//if 128bit may be contained in the 63bit + sign	
		//checks sign of 128bit and set 64bit (with sing) at right value
		if(Org->neg){
		  *Dest = ~(Org->lo --);
		}
		else
		{
			*Dest = Org->lo;
		}
		return;
		
	}
}


//convert CDnum128_t to signed int 64bit; remember that if num128 higher part (64bits) is a value bigger than 63 bit + sign
// then 64 destination will set to maximun positive or maximun negative
void CDconvert128HighToS64 (CDnum128_t *Org, sint64_t *Dest)
{
	if(Org->hi > 0x7fffffffffffffff){
		//if 128bit can not contained in the 63bit + sign then
		//checks sign of 128bit and set 64bit (with sing) at maximun positive or negative
		if(Org->neg){
		  *Dest = 0x8000000000000000;	
		}
		else
		{
			*Dest = 0x7fffffffffffffff;
		}
		return;
	}
	else
	{
		//if 128bit may be contained in the 63bit + sign	
		//checks sign of 128bit and set 64bit (with sing) at right value
		if(Org->neg){
		  *Dest = ~(Org->hi --);
		}
		else
		{
			*Dest = Org->hi;
		}
		return;
	}
}



//convert sint64 to CDnum128_t type
void CDconvert64to128 (sint64_t *A, CDnum128_t *Dest)
{
	Dest->hi=0;
	if(*A<0)
	{
		//converts the negative value in positive but 
		Dest->lo= (*A ^ 0xFFFFFFFFFFFFFFFF);
		Dest->lo++;
		Dest->neg=true;	
	}
	else
	{
		Dest->lo= *A;
		Dest->neg=false;
	}
}



//shift right entire 128bit for a number of indicated bit (max 127)
//this operation as is a div 2^numBits
void CDshiftRight128(CDnum128_t *val, uint8_t numBits)
{
	TuniInt64_t app64;
	
	if(numBits==0) return;
	if(numBits>127)
	{
		val->lo = 0;
		val->hi = 0;
		return;
	}
	
	if(numBits>=64)
	{
		val->lo = val->hi;
		val->hi = 0;
		numBits-=64;
	}
	if(numBits>=32)
	{
		val->lo>>=32;
		app64.i64= val->hi;
		val->hi>>=32;
		((TuniInt64_t*)&val->lo)->i32log.i32high = app64.i32log.i32low;
		numBits-=32;
	}
	if(numBits==0) return;
	if(numBits<32)
	{
		val->lo>>=numBits;
		app64.i64= val->hi;
		val->hi>>=numBits;
		app64.i32log.i32low<<=(32-numBits);
		((TuniInt64_t*)&val->lo)->i32log.i32high |= app64.i32log.i32low;
	}
} //void CDshiftRight128(CDnum128_t *val, uint8 numBits)



//shift left entire 128bit for a number of indicated bit (max 127)
//this operation as is a mul 2^numBits
void CDshiftLeft128(CDnum128_t *val, uint8_t numBits)
{
	TuniInt64_t app64;

	if(numBits==0) return;
	if(numBits>127)
	{
		val->lo = 0;
		val->hi = 0;
		return;
	}

	if(numBits>=64)
	{
		val->hi = val->lo;
		val->lo = 0;
		numBits-=64;
	}
	if(numBits>=32)
	{
		val->hi<<=32;
		app64.i64= val->lo;
		val->lo<<=32;
		((TuniInt64_t*)&val->hi)->i32log.i32low = app64.i32log.i32high;
		numBits-=32;
	}
	if(numBits==0) return;
	if(numBits<32)
	{
		val->hi<<=numBits;
		app64.i64= val->lo;
		val->lo<<=numBits;
		app64.i32log.i32high>>=(32-numBits);
		((TuniInt64_t*)&val->hi)->i32log.i32low|=app64.i32log.i32high;
	}
} //void CDshiftLeft128(CDnum128_t *val, uint8 numBits)



//add 128bit A value to the B 128bit value nad store the result in B
void CDadd128_AtoB(CDnum128_t *A, CDnum128_t *B)
{
	if(A->neg == B->neg)
	{
		//if A and B are positive, or A or B are both negatives, in short if result is a sum
		//B.neg = A.neg;
		B->hi += A->hi;
		B->lo += A->lo;
		if(B->lo < A->lo) B->hi ++;
	}
	else
	{
		CDnum128_t *dest=B;
		//if only one of a or b is negative, that is a subtraction then...
		if ((B->hi > A->hi) || ((B->hi == A->hi) && (B->lo > A->lo)))
		{
			//exchange operators
			B = A;
			A = dest;
		}
		dest->neg = A->neg;
		dest->hi = A->hi - B->hi;
		dest->lo = A->lo - B->lo;
    if (dest->lo > A->lo) dest->hi --;
	}
}


//add a 64bit signed with the 128bit B; before convert the signed 64bit to a 128bit and after add it to 128
void CDadd_A64toB128(sint64_t *A, CDnum128_t *B)
{
	CDnum128_t Abig;
	CDconvert64to128( A, &Abig);
	CDadd128_AtoB( &Abig, B);
}
