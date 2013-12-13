/**
 * @file DateTime.cpp
 * Implement absoute_time, DateTime and time utility functions.
 * @copyright Copyright (c) 2012 GenieDB Inc
 *
 * @date Created on: 18 Sep 2012
 * @author Author: jmclaughlin
 */

#include "CF_Platform.h"	// Support for Win32 Port
#include "DateTime.h"

//! Function that converts a time_t into a ptime.
static
boost::posix_time::ptime fixed_from_time_t(std::time_t t)
{
	boost::posix_time::ptime start(boost::gregorian::date(1970,1,1));
	return start + boost::posix_time::seconds(static_cast<boost::int32_t>(t));
}


const Genie::DateTime::absolute_time	Genie::DateTime::EPOCH(			fixed_from_time_t(0) );
const uint32_t							Genie::DateTime::tics_per_sec(	boost::posix_time::time_duration::ticks_per_second() );

Genie::DateTime::absolute_time Genie::DateTime::GetAbsoluteTime( void )
{
	boost::posix_time::ptime	absolute_time = boost::posix_time::microsec_clock::universal_time();
	return absolute_time;
}

Genie::DateTime::absolute_time Genie::DateTime::DeserializeTime( const std::string & secs_dot_microsecs_since_epoch )
{
	uint64_t secs;
	uint64_t uSecs;
	char decPt;

	std::istringstream iss( secs_dot_microsecs_since_epoch );
	iss >> secs >> decPt >> uSecs;
//
//The following lines:
	Genie::DateTime::absolute_time	secTime = Genie::DateTime::TimeFromSecondsSinceUnixEpoch( secs );
	return ComputeTimeDeltaMicrosecs( uSecs, secTime);
//But it really should be:
//	return Genie::DateTime::TimeFromMicroSecondsSinceUnixEpoch( Genie::DateTime::uSecsFromSecs(secs) + uSecs );
//But until we upgrade our Boost to v1.40.0 or later, there is a known overflow issue (Ticket #3471).
//The Ticket is described at: https://svn.boost.org/trac/boost/ticket/3471
//
}

std::string Genie::DateTime::SerializeTime( Genie::DateTime::absolute_time thetime )
{
	int64_t uSecsSinceEPOCH = Genie::DateTime::MicroSecondsSinceUnixEpoch( thetime );

	std::string timestr(
		stringf("%u.%06u",
			unsigned( uSecsSinceEPOCH / Genie::DateTime::Secs2uSecs(1)),
			unsigned( uSecsSinceEPOCH % Genie::DateTime::Secs2uSecs(1))
		));
	return timestr;
}

std::string Genie::DateTime::GetDateTimeStringUTC( const absolute_time & thetime )
{
	boost::gregorian::date				thedate = thetime.date();
	boost::posix_time::time_duration	thetimeofday = thetime.time_of_day();
	std::string timestr(
		stringf("%02u-%02u-%02u_%02u:%02u:%02u.%05u",
			unsigned(thedate.year() % 100),
			unsigned(thedate.month()),
			unsigned(thedate.day()),
			unsigned(thetimeofday.hours()),
			unsigned(thetimeofday.minutes()),
			unsigned(thetimeofday.seconds()),
			unsigned((100000 * thetimeofday.fractional_seconds()) / Genie::DateTime::tics_per_sec)
		));
	return timestr;
}

std::string Genie::DateTime::GetTimeStringUTC( const absolute_time & thetime )
{
	boost::posix_time::time_duration	thetimeofday = thetime.time_of_day();
	std::string timestr(
		stringf("%02u:%02u:%02u.%05u",
			unsigned(thetimeofday.hours()),
			unsigned(thetimeofday.minutes()),
			unsigned(thetimeofday.seconds()),
			unsigned((100000 * thetimeofday.fractional_seconds()) / Genie::DateTime::tics_per_sec)
		));
	return timestr;
}

std::string Genie::DateTime::GetDateTimeAsFilenameString( const absolute_time & thetime )
{
	boost::gregorian::date				thedate = thetime.date();
	boost::posix_time::time_duration	thetimeofday = thetime.time_of_day();
	std::string timestr(
		stringf("%02u-%02u-%02u_%02u-%02u-%02u",
			unsigned(thedate.year() % 100),
			unsigned(thedate.month()),
			unsigned(thedate.day()),
			unsigned(thetimeofday.hours()),
			unsigned(thetimeofday.minutes()),
			unsigned(thetimeofday.seconds())
		));
	return timestr;
}

int64_t Genie::DateTime::GetTimeDurationMicrosecs( const Genie::DateTime::absolute_time & start, const Genie::DateTime::absolute_time & stop )
{
	int64_t uSecs = (stop - start).total_microseconds();
	return uSecs;
}

int64_t Genie::DateTime::GetTimeDurationMillisecs( const Genie::DateTime::absolute_time & start, const Genie::DateTime::absolute_time & stop )
{
	int64_t mSecs = (stop - start).total_milliseconds();
	return mSecs;
}

Genie::DateTime::absolute_time Genie::DateTime::ComputeTimeDeltaMicrosecs( int64_t uSecs, const Genie::DateTime::absolute_time & abstime )
{
	boost::posix_time::ptime thetime = abstime + boost::posix_time::microseconds(uSecs);
	return thetime;
}

int64_t Genie::DateTime::MicroSecondsSinceUnixEpoch( const absolute_time & thetime )
{
	int64_t uSecs = (thetime - Genie::DateTime::EPOCH).total_microseconds();
	return uSecs;
}

Genie::DateTime::absolute_time Genie::DateTime::TimeFromMicroSecondsSinceUnixEpoch( int64_t uSecs )
{
	boost::posix_time::ptime	thetime = Genie::DateTime::EPOCH + boost::posix_time::time_duration(0,0,0,uSecs);
	return thetime;
}

int32_t Genie::DateTime::SecondsSinceUnixEpoch( const Genie::DateTime::absolute_time & thetime )
{
	int32_t secs = (thetime - Genie::DateTime::EPOCH).total_seconds();
	return secs;
}

Genie::DateTime::absolute_time Genie::DateTime::TimeFromSecondsSinceUnixEpoch( int32_t secs )
{
	boost::posix_time::ptime	thetime = Genie::DateTime::EPOCH + boost::posix_time::seconds(secs);
	return thetime;
}
