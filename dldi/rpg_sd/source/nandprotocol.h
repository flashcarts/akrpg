#ifndef _NANDPROTOCOL_H_
#define _NANDPROTOCOL_H_

#include <nds.h>

#ifdef __cplusplus
extern "C" {
#endif

void npReadPage512( u32 addr, void * buffer, void * redudant );

void npReadPage528( u32 addr, void * buffer );

void npReadPage2112( u32 addr, void * buffer );

void npRead512Redundant( u32 addr, void * buffer );

void npRead2KRedundant( u32 addr, void * buffer );

void npWritePage512( u32 addr, const void * data, u16 logicBlk, u8 * subPageCache );

void npWritePage528( u32 addr, const void * data, u16 logicBlk );

void npEraseBlock( u32 addr );

void npPageCopySafe( u32 src, u32 dest );

void npPageCopyFast( u32 src, u32 dest );

u32 npMakeECC256( u16 * data_buf );

bool npCheckEcc( u8 * nandEcc, u8 * readEcc );

bool npEccCorrectData( u8 * data, u8 * nandEcc, u8 * readEcc );

#ifdef __cplusplus
}
#endif

#endif//_NANDPROTOCOL_H_
