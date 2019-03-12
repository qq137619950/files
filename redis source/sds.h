/* SDSLib, A C dynamic strings library
 *
 * Copyright (c) 2006-2010, Salvatore Sanfilippo <antirez at gmail dot com>
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

#ifndef __SDS_H
#define __SDS_H

#define SDS_MAX_PREALLOC (1024*1024)

#include <sys/types.h>
#include <stdarg.h>

typedef char *sds;

/**
 * 1.定义len，获取长度复杂度为O(1)
 * 2.防止缓冲区溢出，比如sdscat函数可以先检查SDS空间是否足够
 * 3.减少修改字符串时带来的内存重分配。redis是数据库，因此对于数据修改速度要求比较高
 *   方式：（1）空间预分配：SDS需要空间扩展的时候，程序会为SDS分配修改所必须的空间，另外还会分配额外未使用空间
 *         （2）惰性空间释放：SDS缩短时，不立即回收空间，而是记录在free字段，可通过API真正回收
**/
struct sdshdr {
	// 记录buf数组中已使用字节的数量
	// 等于SDS所保存字符串的长度
    unsigned int len;
	
	// 记录buf数组中未使用字节的数量
    unsigned int free;
	
	// 字节数组，用于保存字符串
	// 末尾保存一个空字符'\0'，好处是可以重用c字符串函数库里面的函数
    char buf[];
};

static inline size_t sdslen(const sds s) {
    struct sdshdr *sh = (void*)(s-(sizeof(struct sdshdr)));
    return sh->len;
}

static inline size_t sdsavail(const sds s) {
    struct sdshdr *sh = (void*)(s-(sizeof(struct sdshdr)));
    return sh->free;
}

sds sdsnewlen(const void *init, size_t initlen);

// 创建一个包含给定C字符串的SDS
sds sdsnew(const char *init);

// 创建一个不包含任何内容的空SDS
sds sdsempty(void);

// 返回SDS已使用的空间数，即存储字符串长度
size_t sdslen(const sds s);

// 创建一个给定SDS的副本
sds sdsdup(const sds s);

// 释放给定的SDS
void sdsfree(sds s);

// 返回SDS的未使用空间的字节数
size_t sdsavail(const sds s);

// 用空字符将SDS扩展至给定长度
sds sdsgrowzero(sds s, size_t len);

sds sdscatlen(sds s, const void *t, size_t len);

// 将C字符串拼接到SDS字符串的末尾
sds sdscat(sds s, const char *t);

// 将给定SDS字符串拼接到另一个SDS字符串的末尾
sds sdscatsds(sds s, const sds t);

sds sdscpylen(sds s, const char *t, size_t len);

// 将给定的C字符串复制到SDS里面，覆盖原有的字符串
sds sdscpy(sds s, const char *t);

sds sdscatvprintf(sds s, const char *fmt, va_list ap);
#ifdef __GNUC__
sds sdscatprintf(sds s, const char *fmt, ...)
    __attribute__((format(printf, 2, 3)));
#else
sds sdscatprintf(sds s, const char *fmt, ...);
#endif

sds sdscatfmt(sds s, char const *fmt, ...);

sds sdstrim(sds s, const char *cset);

// 保留SDS给定区间内的数据，不在区间内的数据将会被覆盖或者清除
void sdsrange(sds s, int start, int end);

void sdsupdatelen(sds s);

// 清空SDS保存的字符串内容
void sdsclear(sds s);

// 对比两个SDS是否相同
int sdscmp(const sds s1, const sds s2);

sds *sdssplitlen(const char *s, int len, const char *sep, int seplen, int *count);
void sdsfreesplitres(sds *tokens, int count);
void sdstolower(sds s);
void sdstoupper(sds s);
sds sdsfromlonglong(long long value);
sds sdscatrepr(sds s, const char *p, size_t len);
sds *sdssplitargs(const char *line, int *argc);
sds sdsmapchars(sds s, const char *from, const char *to, size_t setlen);
sds sdsjoin(char **argv, int argc, char *sep);

/* Low level functions exposed to the user API */
sds sdsMakeRoomFor(sds s, size_t addlen);
void sdsIncrLen(sds s, int incr);
sds sdsRemoveFreeSpace(sds s);
size_t sdsAllocSize(sds s);

#endif
