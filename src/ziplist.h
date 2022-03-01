/*
 * Copyright (c) 2009-2012, Pieter Noordhuis <pcnoordhuis at gmail dot com>
 * Copyright (c) 2009-2012, Salvatore Sanfilippo <antirez at gmail dot com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Redis nor the names of its contributors may be used
 *     to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _ZIPLIST_H
#define _ZIPLIST_H

#define ZIPLIST_HEAD 0
#define ZIPLIST_TAIL 1

/* Each entry in the ziplist is either a string or an integer. */
typedef struct {
    /* When string is used, it is provided with the length (slen). */
    unsigned char *sval;
    unsigned int slen;
    /* When integer is used, 'sval' is NULL, and lval holds the value. */
    long long lval;
} ziplistEntry;
//ziplist的数据类型，没有用自定义的struct之类的来表达，
//而就是简单的unsigned char *。
//这是因为ziplist本质上就是一块连续内存，
//内部组成结构又是一个高度动态的设计（变长编码），也没法用一个固定的数据结构来表达。

unsigned char *ziplistNew(void);//创建新的ziplist,只包含<zlbytes><zltail><zllen><zlend>
unsigned char *ziplistMerge(unsigned char **first, unsigned char **second);//将两个ziplist合并成一个新的ziplist
//在ziplist头部或者尾部插入数据,
//返回值是新的ziplist
//调用方必须用这里返回的新的ziplist，替换之前传进来的旧的ziplist变量，而经过这个函数处理之后，原来旧的ziplist变量就失效了
//因为ziplist是一块连续空间，对它的追加操作，会引发内存的realloc，因此ziplist的内存位置可能会发生变化
unsigned char *ziplistPush(unsigned char *zl, unsigned char *s, unsigned int slen, int where);
//index参数指定的数据项的内存位置。index可以是负数，表示从尾端向前进行索引
unsigned char *ziplistIndex(unsigned char *zl, int index);
//以下两个函数返回指定数据项p的后一项和前一项
unsigned char *ziplistNext(unsigned char *zl, unsigned char *p);
unsigned char *ziplistPrev(unsigned char *zl, unsigned char *p);
unsigned int ziplistGet(unsigned char *p, unsigned char **sval, unsigned int *slen, long long *lval);
//在指定数据项前插入
unsigned char *ziplistInsert(unsigned char *zl, unsigned char *p, unsigned char *s, unsigned int slen);
unsigned char *ziplistDelete(unsigned char *zl, unsigned char **p);
unsigned char *ziplistDeleteRange(unsigned char *zl, int index, unsigned int num);
unsigned char *ziplistReplace(unsigned char *zl, unsigned char *p, unsigned char *s, unsigned int slen);
unsigned int ziplistCompare(unsigned char *p, unsigned char *s, unsigned int slen);
//查找给定的数据,参数skip用于hash查找,奇数索引表示value,偶数索引表示key,需要跳过
unsigned char *ziplistFind(unsigned char *zl, unsigned char *p, unsigned char *vstr, unsigned int vlen, unsigned int skip);
//包含数据项个数
unsigned int ziplistLen(unsigned char *zl);
size_t ziplistBlobLen(unsigned char *zl);
void ziplistRepr(unsigned char *zl);
typedef int (*ziplistValidateEntryCB)(unsigned char* p, unsigned int head_count, void* userdata);
int ziplistValidateIntegrity(unsigned char *zl, size_t size, int deep,
                             ziplistValidateEntryCB entry_cb, void *cb_userdata);
void ziplistRandomPair(unsigned char *zl, unsigned long total_count, ziplistEntry *key, ziplistEntry *val);
void ziplistRandomPairs(unsigned char *zl, unsigned int count, ziplistEntry *keys, ziplistEntry *vals);
unsigned int ziplistRandomPairsUnique(unsigned char *zl, unsigned int count, ziplistEntry *keys, ziplistEntry *vals);

#ifdef REDIS_TEST
int ziplistTest(int argc, char *argv[], int accurate);
#endif

#endif /* _ZIPLIST_H */
