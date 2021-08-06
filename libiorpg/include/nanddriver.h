/*---------------------------------------------------------------------------------


Copyright (C) 2007 Acekard, www.acekard.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.


---------------------------------------------------------------------------------*/









#ifndef _NANDDRIVER_H_
#define _NANDDRIVER_H_

#include <nds.h>

#ifdef __cplusplus
extern "C" {
#endif

bool ndInitNAND();

// 建立逻辑/物理块映射表
void ndBuildLUT();

// 参数是逻辑地址，返回值是物理地址
// oldPhyAddress 若不为 NULL，则表示是写操作，需要返回 oldPhyAddress 给调用者
// 使调用者可以擦除 被标记为 FREE 的 oldPhyAddress 所在的 block
// logicBlock [out] 参数，在 oldPhyAddress 所在块其实是freeblock的时候，存放了
// 转换得到的 logicBlock 编号，以便稍候写入的时候可以给该freeblock分配一个编号

u32 ndLog2Phy( u32 logicAddress, u32 * oldPhyAddress );

// 这个函数在指定的 zone 内寻找 freeblock，返回值范围 0 - 1023
u16 ndSearchFreeBlock( u8 zone, bool markFreeBlkToCfg );

// 读数据，addr 的值 末 9 位都是0, addr 是 LBA
void ndReadPages( u32 addr, u32 pageCount, u8 * buffer );

// 写数据，addr 的值 末 9 位都是0, addr 是 LBA
void ndWritePages( u32 addr, u32 pageCount, const u8 * data );

// 写数据，addr 的值 末 9 位都是0, addr 是 LBA，写完立即拷贝未完成的block
void ndWritePagesSafe( u32 addr, u32 pageCount, const u8 * data );

// 执行 nand 的内部 Page Copy 操作，一般是用于写入的时候，把old block的内容拷贝到free block
// pageCount 是指有多少个512 page
// startSubPage 是指从第一个2k page的哪个512k page开始
// 由于都是在 block 之间拷贝，所以最多 64 个 page
// srcAddr 和 destAddr 应该对齐
void ndNandMove( u32 srcAddr, u32 destAddr, u8 pageCount );

// 在 内部 PageCopy 无法使用的时候，手动拷贝src地址开始 528 为单位的 page 到dest地址
// pageCount 是指有多少个512 page
// startSubPage 是指从第一个2k page的哪个512k page开始
// 由于都是在 block 之间拷贝，所以最多 64 个2kpage，换算成512也刚好256个
void ndCopyPages( u32 srcAddr, u32 destAddr, u8 pageCount );

// 在 内部 PageCopy 无法使用的时候，手动拷贝src地址开始 528 为单位的 page 到dest地址
// pageCount 是指有多少个512 page
// startSubPage 是指从第一个2k page的哪个512k page开始
// 由于都是在 block 之间拷贝，所以最多 64 个2kpage，换算成512也刚好256个
void ndCopyPages2K( u32 srcAddr, u32 destAddr, u8 pageCount );

// 如果这次操作不是紧跟在上次的扇区后面的Write操作，或者这次操作不是Write操作(比如Read)，
// 那么就执行ndFinishPartialCopy，来把刚才搬移的那个block的后半部分内容拷贝到新的block里
void ndFinishPartialCopy();

/*
错误处理
*/
void ndReplaceBlock( u32 oldBlockAddr );

// 检查读写过程中是否有 ecc 错误，
// 如果出现了 ecc 错误，说明旧块可能不稳定了
// 那么用ndReplaceBlock()把旧块拷贝到新块，旧块标记为坏块，如果拷贝过程中存在有 2 bit ecc error
// 那么重新计算那个page的ecc，把带着错误的数据写入新块，避免丢失更多数据
// 返回值： 0  no error
//          1  1 bit ecc error
//          2  2 bit ecc error
bool ndCheckError();
/*
错误处理结束
*/

u8 getNandDriverVer();

#ifdef __cplusplus
}
#endif

#endif//_NANDDRIVER_H_
