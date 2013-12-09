/*
 * CF_Platform.h
 *
 *  Created on: 13 Sep 2012
 *      Author: jmclaughlin
 */

#ifndef CF_PLATFORM_H_
#define CF_PLATFORM_H_

#define __STDC_LIMIT_MACROS

#define USE_ARG(x) (void)(x)
#define IS_TRUE(x) (x != 0)
#define IS_FALSE(x) (x != 0)
#define CF_NULL_OR_NOT(x) ((x)==NULL ? "-NULL-" : "not null")
#define CF_NULL_SAFE_STR(x) ((x)==NULL ? "-NULL-" : (x))
#define CF_SAFE_STR(x) ((x)==NULL ? "" : (x))
#define CF_BOOL_STR(x) ((x) ? "TRUE" : "FALSE")
#define CF_NON_ZERO_FAILURE(x) (x)!=0
#define CF_ZERO_SUCCESS(x) (x)==0
#define CF_MAX_STRINGF_SIZE 1048576

#define TRSTRM std::cerr
#define TRSN TRSTRM << Genie::DateTime::GetTimeStringUTC() << " @ "
#define TRSNL std::endl
#define TRS {TRSN << __FILE__ << "::" << (int)__LINE__ << " - "
#define TRS_END TRSNL; TRSTRM.flush();}
#define TRL TRS << TRS_END
#define TRACEL(__arg__) TRS << __arg__ << TRS_END

#define BREAKABLE_POINT {int dummy=1;(void)dummy;}

#if 0
#define CHECK_NONNULL_ARG(function,arg,check_non_blank) {\
	if (arg==NULL) {\
		throw std::invalid_argument( Genie::stringf( "%s : %s argument is NULL @ %s:%d" #function, #arg, __FILE__, __LINE__) );\
	} else if (check_non_blank && strlen((const char *)arg)==0) {\
		throw std::invalid_argument( Genie::stringf( "%s : %s argument is BLANK @ %s:%d" #function, #arg, __FILE__, __LINE__) );\
	}\
}
#endif

//**************************************
//std Exceptions
//std::exception("string")
//    std::logic_error("string")
//    	std::domain_error("string")
//    	std::invalid_argument("string")
//    	std::length_error("string")
//    	std::out_of_range("string")
//    std::runtime_error("string")
//    	std::overflow_error("string")
//    	std::underflow_error("string")
//    	std::range_error("string")

#if defined(_WIN32)

//#pragma message("CF_Platform.h -- Temporarily disabled 64bit -> 32bit possible data loss warnings")
#pragma warning(disable: 4127 4244 4267 4800 4996 )

	#if !defined(WIN32_LEAN_AND_MEAN)
		#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
	#endif
	#if !defined(NOMINMAX)
		#define NOMINMAX
	#endif
	#if !defined(NOGDI)
		#define NOGDI
	#endif
	#include <Windows.h>
	#include <io.h>
	#include <process.h>

	#if defined(DEFINE_FILE_ATTR_HEX)
		#define S_IRWXU 00700
		#define S_IRUSR 00400
		#define S_IWUSR 00200
		#define S_IXUSR 00100

		#define S_IRWXG 00070
		#define S_IRGRP 00040
		#define S_IWGRP 00020
		#define S_IXGRP 00010

		#define S_IRWXO 00007
		#define S_IROTH 00004
		#define S_IWOTH 00002
		#define S_IXOTH 00001
	#endif
#else
	#include <unistd.h>
	#include <sys/types.h>
	#include <dirent.h>
	#include <sys/statvfs.h>
	#include <sys/stat.h>
	#include <netdb.h>
	#include <arpa/inet.h>
	#include <ifaddrs.h>
	#include <errno.h>
	#include <pthread.h>
#endif	//_WIN32
#include <sys/stat.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdint.h>	// Included early to avoid MSVC weakness with Boost defs.
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <set>
#include <vector>

#if defined(_WIN32)
	#define DIR_SEP_CHAR_STR "\\"
	#define FOREIGN_DIR_SEP_CHAR_STR "/"
	#define DIR_SEP_CHAR '\\'
	#define FOREIGN_DIR_SEP_CHAR '/'
#else
	#define DIR_SEP_CHAR_STR "/"
	#define FOREIGN_DIR_SEP_CHAR_STR "\\"
	#define DIR_SEP_CHAR '\\'
	#define FOREIGN_DIR_SEP_CHAR '/'
#endif

#if defined(_WIN32)
	/**
	 * \brief Implementation of Linux __sync_val_compare_and_swap using Windows API - int
	 *
	 * \param[in]	__p Pointer to variable to be modified
	 * \param[in]	__v Value to be added
	 * \return Value of variable prior to modification
	 */
	inline int __sync_val_compare_and_swap (volatile int * __p, int __compare, int __v)
	{
		//* \todo - Verify this reinterpret_cast is safe */
		return InterlockedCompareExchange( reinterpret_cast<volatile LONG *>(__p), static_cast<LONG>(__v), static_cast<LONG>(__compare) );
	}
	inline uint32_t __sync_val_compare_and_swap (volatile uint32_t * __p, uint32_t __compare, uint32_t __v)
	{
		//* \todo - Verify this reinterpret_cast is safe */
		return InterlockedCompareExchange( reinterpret_cast<volatile LONG *>(__p), static_cast<LONG>(__v), static_cast<LONG>(__compare) );
	}
	inline LONG __sync_val_compare_and_swap (volatile LONG * __p, LONG __compare, LONG __v)
	{
		return InterlockedCompareExchange( __p, __v, __compare );
	}
	inline LONGLONG __sync_val_compare_and_swap (volatile LONGLONG * __p, LONGLONG __compare, LONGLONG __v)
	{
		return InterlockedCompareExchange64( __p, __v, __compare );
	}
	inline uint64_t __sync_val_compare_and_swap (volatile uint64_t * __p, uint64_t __compare, uint64_t __v)
	{
		//* \todo - Verify this reinterpret_cast is safe */
		return InterlockedCompareExchange64( reinterpret_cast<volatile LONGLONG *>(__p), static_cast<LONGLONG>(__v), static_cast<LONGLONG>(__compare) );
	}

	/**
	 * \brief Implementation of Linux __sync_fetch_and_add using Windows API - int
	 *
	 * \param[in]	__p Pointer to variable to be modified
	 * \param[in]	__v Value to be added
	 * \return Value of variable prior to modification
	 */
	inline int __sync_fetch_and_add (volatile int * __p, int __v)
	{
		//* \todo - Verify this reinterpret_cast is safe */
		return InterlockedCompareExchange( reinterpret_cast<volatile LONG *>(__p), static_cast<LONG>(*__p + __v), static_cast<LONG>(*__p) );
	}
	inline uint32_t __sync_fetch_and_add (volatile uint32_t * __p, uint32_t __v)
	{
		//* \todo - Verify this reinterpret_cast is safe */
		return InterlockedCompareExchange( reinterpret_cast<volatile LONG *>(__p), static_cast<LONG>(*__p + __v), static_cast<LONG>(*__p) );
	}
	inline LONG __sync_fetch_and_add (volatile LONG * __p, LONG __v)
	{
		return InterlockedCompareExchange( __p, *__p + __v, *__p );
	}
	inline LONGLONG __sync_fetch_and_add (volatile LONGLONG * __p, LONGLONG __v)
	{
		return InterlockedCompareExchange64( __p, *__p + __v, *__p );
	}
	inline uint64_t __sync_fetch_and_add (volatile uint64_t * __p, uint64_t __v)
	{
		//* \todo - Verify this reinterpret_cast is safe */
		return InterlockedCompareExchange64( reinterpret_cast<volatile LONGLONG *>(__p), static_cast<LONGLONG>(*__p + __v), static_cast<LONGLONG>(*__p) );
	}

	/**
	 * \brief Implementation of Linux __sync_fetch_and_sub using Windows API - int
	 *
	 * \param[in]	__p Pointer to variable to be modified
	 * \param[in]	__v Value to be subtracted
	 * \return Value of variable prior to modification
	 */
	inline int __sync_fetch_and_sub (volatile int * __p, int __v)
	{
		//* \todo - Verify this reinterpret_cast is safe */
		return InterlockedCompareExchange( reinterpret_cast<volatile LONG *>(__p), static_cast<LONG>(*__p - __v), static_cast<LONG>(*__p) );
	}
	inline uint32_t __sync_fetch_and_sub (volatile uint32_t * __p, uint32_t __v)
	{
		//* \todo - Verify this reinterpret_cast is safe */
		return InterlockedCompareExchange( reinterpret_cast<volatile LONG *>(__p), static_cast<LONG>(*__p - __v), static_cast<LONG>(*__p) );
	}
	inline LONG __sync_fetch_and_sub (volatile LONG * __p, LONG __v)
	{
		return InterlockedCompareExchange( __p, *__p - __v, *__p );
	}
	inline LONGLONG __sync_fetch_and_sub (volatile LONGLONG * __p, LONGLONG __v)
	{
		return InterlockedCompareExchange64( __p, *__p - __v, *__p );
	}
	inline uint64_t __sync_fetch_and_sub (volatile uint64_t * __p, uint64_t __v)
	{
		//* \todo - Verify this reinterpret_cast is safe */
		return InterlockedCompareExchange64( reinterpret_cast<volatile LONGLONG *>(__p), static_cast<LONGLONG>(*__p - __v), static_cast<LONGLONG>(*__p) );
	}
#endif

namespace Genie {

	std::string stringf(const std::string &fmt, ...);
	std::vector<std::string> splitString( const std::string & source, const char * delims );

	class FatalException : public std::runtime_error
	{
	public:
		FatalException( const std::string & message, const char * srcfile, int srcLine )
			:	std::runtime_error(toString(message, srcfile, srcLine))
		{}

		static std::string toString(const std::string & message, const char * srcfile, int srcLine)
		{
			return stringf( "FATAL ERROR: %s @ %s:%d", message.c_str(), srcfile, srcLine );
		}
	};

	inline void usleep(int sleepMicroSecs)
	{
		#if defined(_WIN32)
			Sleep( static_cast<DWORD>((sleepMicroSecs+500) / 1000) );
		#else
			::usleep(sleepMicroSecs);
		#endif
	}


	/**
	 * \brief Implementation of Linux chmod using Windows API - io.h/_chmod
	 *
	 * \param[in]	fileName	String with filepath
	 * \param[in]	mask		Linux/Unix filemask (e.g. 0666)
	 * \return Success == 0 else Failure
	 */
	inline int chmod(const char *fileName, int mask)
	{
		#if defined(_WIN32)
			int pmode = 0;
			if ((mask & 02) || (mask & 020) || (mask & 0200))
				pmode |= _S_IWRITE;
			if ((mask & 04) || (mask & 040) || (mask & 0400))
				pmode |= _S_IREAD;

			return ::_chmod(fileName, pmode);
		#else
			return ::chmod(fileName, mask);
		#endif
	}


	/**
	 * \brief Implementation of Linux mkstemp using Windows API - io.h/_chmod
	 *
	 * \param[in,out]	fileNameTemplate	String with filepath ending with XXXXXX
	 * \return fileDescriptor on success, -1 on failure
	 */
	inline int mkstemp(char *fileNameTemplate)
	{
		#if defined(_WIN32)
			//#pragma message("CF_Platform.h -- cf_mkstemp(fileNameTemplate) still needs to be implemented")
			return ::_open( fileNameTemplate, (_O_CREAT|_O_RDWR), (_S_IREAD|_S_IWRITE) );
		#else
			return ::mkstemp(fileNameTemplate);
		#endif
	}

	/**
	 * \brief Implementation of Linux fdopen using Windows API - io.h/_chmod
	 *
	 * \param[in]	fileDescriptor	Integer fileDescriptor
	 * \param[in]	mode			String with mode ("r","w","a", etc)
	 * \return FILE* handle on success, NULL on failure
	 */
	inline ::FILE* fdopen(int fileDescriptor, const char *mode)
	{
		#if defined(_WIN32)
			return ::_fdopen( fileDescriptor, mode );
		#else
			return ::fdopen(fileDescriptor, mode);
		#endif
	}

	/**
	 * \brief Makes a direcotry path (including parent dirs if necessary)
	 *
	 * \param[in]	dirPath		A directory path to create
	 * \param[in]	skipLast_IsFilename	If true, last portion assumed filename and not created
	 * \return true if successful
	 */
	inline bool makeDirs(const char *dirPath, bool skipLast_IsFilename=false)
	{
		bool rtn = false;
		std::string path;
		std::vector<std::string> pathParts = splitString( std::string(dirPath), "/\\" );
		int levels=pathParts.size();
		int level = 1;
		for (	std::vector<std::string>::const_iterator it=pathParts.begin();
				it != pathParts.end();
				++it)
		{
			if (it==pathParts.begin()) {
				if ( *it == "" ) {
					path = DIR_SEP_CHAR_STR;
				}
			} else if (!skipLast_IsFilename || levels!=level) {
				path.append(*it);
				#if defined(_WIN32)
					rtn = CreateDirectory(path.c_str(), NULL);
				#else
					rtn = (mkdir(path.c_str(), 0755)==0);
				#endif
				path.append(DIR_SEP_CHAR_STR);
			}
			level++;
		}
		return rtn;
	}

	inline bool rotateLogFile(const char *filePath, const char *suffix)
	{
		bool rtn = false;

		if (filePath == NULL)
			return false;

		std::string tmpPath(filePath);
		std::string::size_type pos = tmpPath.find(FOREIGN_DIR_SEP_CHAR_STR);
		while (pos != std::string::npos) 
		{
			tmpPath[pos] = DIR_SEP_CHAR;
			pos = tmpPath.find(FOREIGN_DIR_SEP_CHAR_STR);
		}

		FILE *fHan = fopen(tmpPath.c_str(),"r");
		if (fHan == NULL)
			return false;
		fclose( fHan );

		std::vector<std::string> pathParts = splitString( tmpPath, "." );
		std::string newFile = std::string(pathParts[0])
								.append("_")
								.append(suffix)
								.append(".")
								.append(pathParts[1]);
		#if defined(_WIN32)
			rtn = MoveFile( tmpPath.c_str(), newFile.c_str() );
			std::cerr << "MoveFile( tmpPath='" << tmpPath.c_str() << "', newFile='" << newFile.c_str() << "' ) = " << rtn << std::endl;
			std::cerr.flush();
		#else
			rtn = rename(filePath, newFile.c_str());
		#endif
		return rtn;
	}


	/**
	 * \brief Implementation of Linux unlink using Windows API - io.h/_chmod
	 *
	 * \param[in]	filename	String with filepath
	 */
	inline int unlink(const char *filename)
	{
		#if defined(_WIN32)
			return ::_unlink( filename );
		#else
			return ::unlink( filename );
		#endif
	}

	/**
	 * \brief Implementation of Linux getpid using Windows API -
	 *  	  <process.h>/_getpid
	 *
	 * \return Current ProcessID
	 */
	inline unsigned getpid(void)
	{
		#if defined(_WIN32)
			return ::_getpid();
		#else
			return ::getpid();
		#endif
	}

	/**
	 * \brief Implementation of Linux pthread_self() using Windows API -
	 *  	  GetCurrentThreadId()
	 *
	 * \return Current ThreadID
	 */
	inline unsigned gettid(void)
	{
		#if defined(_WIN32)
			return (unsigned) ::GetCurrentThreadId();
		#else
			return (unsigned) ::pthread_self();
		#endif
	}

	inline int breakpoint()
	{
		int rtn = 0;
		#if defined(_WIN32)
			__debugbreak();
		#else
			int dummy = 0;
			(void)dummy;
		#endif
		return rtn;
	}

#if 0
	// Requires exception support
	inline int countThreads()
	{
		#if defined(_WIN32)
			return 0;
		#else
			DIR *dp;
			int numThreads = 0;
			struct dirent *ep;

			dp = opendir ("/proc/self/task");
			if (dp != NULL) {
				while ((ep = readdir(dp)))
					++numThreads;
				(void) closedir (dp);
			} else
				throw std::runtime_error("Couldn't count threads");
			return numThreads;
		#endif
	}
#endif

	inline std::string stringf(const std::string &fmt, ...) {
		int size=100;  // size and n exclude the terminating NULL
		std::string str(size, ' ');
		va_list ap;

		va_start(ap, fmt);
		int n = vsnprintf((char *)str.c_str(), size+1, fmt.c_str(), ap);
		va_end(ap);
		// C standards says return number of chars that should have been
		// written.
		// http://msdn.microsoft.com/en-us/library/1kt27hek%28v=vs.110%29.aspx
		// says that MSVC returns -1 if it was truncated.
#ifdef _WIN32
		while ((n == -1 || n > size) && size < CF_MAX_STRINGF_SIZE) {
			size *= 2;
#else
		if (n > size && n < CF_MAX_STRINGF_SIZE) {
			size = n;
#endif
			str.resize(size);
			va_start(ap, fmt);
			n = vsnprintf((char *)str.c_str(), size+1, fmt.c_str(), ap);
			va_end(ap);
		}

		if (n >= 0 && n < CF_MAX_STRINGF_SIZE) {
			if (n < size) {
				str.resize(n);
			}
		} else {
			return std::string();
			//throw std::invalid_argument("Could not format " + fmt);
		}
		return str;
	}

	inline std::string stringSetToString( const std::set<std::string> & theSet) {
		std::string rtn;
		if (theSet.size()==0) {
			return rtn;
		}
		bool first=true;
		for (std::set<std::string>::const_iterator it=theSet.begin(); it!=theSet.end(); ++it) {
			if (first) {
				rtn.append( *it );
				first = false;
			} else {
				rtn.append( "," ).append( *it );
			}
		}
		return rtn;
	}

	inline std::string stripString( const std::string & source, bool leading = true, bool trailing = true )
	{
		if (source.length()==0)
			return std::string();

		std::string::size_type b = 0;
		if (leading) {
			b = source.find_first_not_of(" \t\n");
			if (b == std::string::npos)
				return std::string();
		}

		std::string::size_type e = source.length()-1;
		if (trailing)
			e = source.find_last_not_of(" \t\n");

		return source.substr(b, e-b+1);
	}

	inline std::vector<std::string> splitString( const std::string & source, const char * delims )
	{
		std::vector<std::string> rtn;
		std::string temp = Genie::stripString(source);
		if (temp.length()==0)
			return rtn;
		std::string::size_type b = 0;
		std::string::size_type e = 0;
		while ( (e=source.find_first_of(delims,b)) != std::string::npos) {
			rtn.push_back( stripString(source.substr(b,e-b)) );
			b = e + 1;
		}
		rtn.push_back( stripString(source.substr(b)) );
		return rtn;
	}

	inline bool isNumStr( const std::string & s, long * dest = NULL )
	{
		char *sp;
		if (s.length()==0)
			return false;

		long rtnVal = ::strtol( s.c_str(), &sp, 0);
		if (*sp != '\0')
			return false;

		if (dest != NULL)
			*dest = rtnVal;
		return true;
	}

	inline bool isIPAddr( const std::string & s )
	{
		std::vector<std::string> parts = splitString(s, ".");
		if (parts.size() != 4)
			return false;

		long val;
		for (int i=0; i<4; i++) {
			if (!Genie::isNumStr(parts[i], &val) || val < 0 || val > 255)
				return false;
		}
		return true;
	}

	inline std::string Uint642String( uint64_t val, int radix=10 )
	{
		switch (radix) {
		case 10:	return stringf("%llu", val);
		case 16:	return stringf("0x%llx", val);
		default:	return stringf("%llu", val);
		}
	}


}


#endif /* CF_PLATFORM_H_ */
