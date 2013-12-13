/**
 * @file DateTime.h
 * Define absoute_time, DateTime and time utility functions.
 * @copyright Copyright (c) 2012 GenieDB Inc
 *
 * @date Created on: 18 Sep 2012
 * @author jmclaughlin
 */

#ifndef DATETIME_H_
#define DATETIME_H_

#include <string>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/throw_exception.hpp>

namespace Genie { namespace DateTime {

	typedef boost::posix_time::ptime	absolute_time;

//	class DateTime {
//	public:
		extern const absolute_time		EPOCH;
		extern const uint32_t			tics_per_sec;
//	};

	absolute_time           GetAbsoluteTime( void );
	absolute_time           DeserializeTime( const std::string & secs_dot_microsecs_since_epoch );
	std::string             SerializeTime( absolute_time thetime );

	std::string             GetDateTimeStringUTC( const absolute_time & thetime = GetAbsoluteTime() );
	std::string             GetTimeStringUTC( const absolute_time & thetime = GetAbsoluteTime() );
	std::string             GetDateTimeAsFilenameString( const absolute_time & thetime = GetAbsoluteTime() );

	int64_t                 GetTimeDurationMicrosecs( const absolute_time & start, const absolute_time & stop );
	int64_t                 GetTimeDurationMillisecs( const absolute_time & start, const absolute_time & stop );
	absolute_time           ComputeTimeDeltaMicrosecs( int64_t uSecs, const absolute_time & thetime );
	int64_t                 MicroSecondsSinceUnixEpoch( const absolute_time & thetime );
	absolute_time           TimeFromMicroSecondsSinceUnixEpoch( int64_t uSecs );
	int32_t                 SecondsSinceUnixEpoch( const absolute_time & thetime );
	absolute_time           TimeFromSecondsSinceUnixEpoch( int32_t secs );

	inline int64_t          uSecs2Secs( int64_t uSecs ) { return uSecs / 1000000; }
	inline int64_t          uSecs2SecsRounded( int64_t uSecs ) { return (uSecs + 500000) / 1000000; }
	inline int64_t          uSecs2mSecs( int64_t uSecs ) { return uSecs / 1000; }
	inline int64_t          uSecs2mSecsRounded( int64_t uSecs ) { return (uSecs + 500) / 1000; }

	inline int64_t          mSecs2uSecs( int64_t mSecs ) { return mSecs * 1000; }
	inline int64_t          mSecs2Secs( int64_t mSecs ) { return mSecs / 1000; }
	inline int64_t          mSecs2SecsRounded( int64_t mSecs ) { return (mSecs + 500) / 1000; }

	inline int64_t          Secs2uSecs( int64_t Secs ) { return Secs * 1000000; }
	inline int64_t          Secs2mSecs( int64_t Secs ) { return Secs * 1000; }

	inline absolute_time    Secs2Time( int32_t secs ) { return TimeFromSecondsSinceUnixEpoch(secs); }
	inline int32_t          Time2Secs( const absolute_time & thetime = GetAbsoluteTime() ) { return SecondsSinceUnixEpoch(thetime); }
	inline absolute_time    TimeFromSecs( int32_t secs ) { return TimeFromSecondsSinceUnixEpoch(secs); }
	inline int32_t          SecsFromTime( const absolute_time & thetime = GetAbsoluteTime() ) { return SecondsSinceUnixEpoch(thetime); }

	//! Specialisation for timespec to string conversion.
	inline std::string ToStr(const absolute_time & timespec) {
		return Genie::DateTime::GetTimeStringUTC( timespec );
	}

}} //namespace Genie { namespace DateTime

#endif //DATETIME_H_

