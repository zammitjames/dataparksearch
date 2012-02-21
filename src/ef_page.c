/* Copyright (C) 2003-2007 Datapark corp. All rights reserved.
   Based on Electric Fence 2.2 by Bruce Perens

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA 
*/

#include "dps_common.h"

#ifdef EFENCE

#include "dps_efence.h"
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

/*
 * Lots of systems are missing the definition of PROT_NONE.
 */
#ifndef	PROT_NONE
#define	PROT_NONE	0
#endif

/*
 * 386 BSD has MAP_ANON instead of MAP_ANONYMOUS.
 */
#if ( !defined(MAP_ANONYMOUS) && defined(MAP_ANON) )
#define	MAP_ANONYMOUS	MAP_ANON
#endif

/*
 * For some reason, I can't find mprotect() in any of the headers on
 * IRIX or SunOS 4.1.2
 */
/* extern C_LINKAGE int mprotect(void * addr, size_t len, int prot); */

static caddr_t	startAddr = (caddr_t) 0;

#if ( !defined(sgi) && !defined(_AIX) && !(defined(BSD) && (BSD >= 199103)) && !defined(__linux__) )
extern int	sys_nerr;
extern char *	sys_errlist[];
#endif

static const char *
stringErrorReport(void)
{
#if ( defined(sgi) )
	return strerror(oserror());
#elif ( defined(_AIX) )
	return strerror(errno);
#else
	if ( errno > 0 && errno < sys_nerr )
		return sys_errlist[errno];
	else
		return "Unknown error.\n";
#endif
}

/*
 * Create memory.
 */
#if defined(MAP_ANONYMOUS)
void *
Page_Create(size_t size)
{
	caddr_t		allocation;

	/*
	 * In this version, "startAddr" is a _hint_, not a demand.
	 * When the memory I map here is contiguous with other
	 * mappings, the allocator can coalesce the memory from two
	 * or more mappings into one large contiguous chunk, and thus
	 * might be able to find a fit that would not otherwise have
	 * been possible. I could _force_ it to be contiguous by using
	 * the MMAP_FIXED flag, but I don't want to stomp on memory mappings
	 * generated by other software, etc.
	 */
	allocation = (caddr_t) mmap(
	 startAddr
	,(int)size
	,PROT_READ|PROT_WRITE
	,MAP_PRIVATE|MAP_ANONYMOUS
	,-1
	,0);

#ifndef	__hpux
	/*
	 * Set the "address hint" for the next mmap() so that it will abut
	 * the mapping we just created.
	 *
	 * HP/UX 9.01 has a kernel bug that makes mmap() fail sometimes
	 * when given a non-zero address hint, so we'll leave the hint set
	 * to zero on that system. HP recently told me this is now fixed.
	 * Someone please tell me when it is probable to assume that most
	 * of those systems that were running 9.01 have been upgraded.
	 */
	startAddr = allocation + size;
#endif

	if ( allocation == (caddr_t)-1 )
	  EF_Abort("mmap() failed at %s:%d: %s [startAddr:0x%x, size:%d]", __FILE__, __LINE__, stringErrorReport(), startAddr, size);

	return (void *)allocation;
}
#else
void *
Page_Create(size_t size)
{
	static int	devZeroFd = -1;
	caddr_t		allocation;

	if ( devZeroFd == -1 ) {
		devZeroFd = open("/dev/zero", O_RDWR);
		if ( devZeroFd < 0 )
			EF_Exit(
			 "open() on /dev/zero failed: %s"
			,stringErrorReport());
	}

	/*
	 * In this version, "startAddr" is a _hint_, not a demand.
	 * When the memory I map here is contiguous with other
	 * mappings, the allocator can coalesce the memory from two
	 * or more mappings into one large contiguous chunk, and thus
	 * might be able to find a fit that would not otherwise have
	 * been possible. I could _force_ it to be contiguous by using
	 * the MMAP_FIXED flag, but I don't want to stomp on memory mappings
	 * generated by other software, etc.
	 */
	allocation = (caddr_t) mmap(
	 startAddr
	,(int)size
	,PROT_READ|PROT_WRITE
	,MAP_PRIVATE
	,devZeroFd
	,0);

	startAddr = allocation + size;

	if ( allocation == (caddr_t)-1 )
	  EF_Exit("mmap() failed %s:%d: %s", __FILE__, __LINE__, stringErrorReport());

	return (void *)allocation;
}
#endif

static void
mprotectFailed(void)
{
	EF_Exit("mprotect() failed: %s", stringErrorReport());
}

void
Page_AllowAccess(void * address, size_t size)
{
	if ( mprotect((caddr_t)address, size, PROT_READ|PROT_WRITE) < 0 )
		mprotectFailed();
}

void
Page_DenyAccess(void * address, size_t size)
{
	if ( mprotect((caddr_t)address, size, PROT_NONE) < 0 )
		mprotectFailed();
}

void
Page_Delete(void * address, size_t size)
{
	if ( munmap((caddr_t)address, size) < 0 )
		Page_DenyAccess(address, size);
}

#if defined(_SC_PAGESIZE)
size_t
Page_Size(void)
{
	return (size_t)sysconf(_SC_PAGESIZE);
}
#elif defined(_SC_PAGE_SIZE)
size_t
Page_Size(void)
{
	return (size_t)sysconf(_SC_PAGE_SIZE);
}
#else
/* extern int	getpagesize(); */
size_t
Page_Size(void)
{
	return getpagesize();
}
#endif


#endif /* EFENCE */
