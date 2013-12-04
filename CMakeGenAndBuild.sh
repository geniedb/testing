#!/bin/sh

buildcfg2subdir ()
{
	case $1 in
	[Dd]ebug)	echo Debug;;
	[Rr]elease)	echo Release;;
	[Rr]el[Ww]ith[Dd]eb[Ii]nfo)	echo RelWithDebInfo;;
	*)		echo "$1";;
	esac
}

incremental=
nobuild=
make_opts=
cmake_opts=
configurations="Debug Release"
target=all
usage="Usage: %s [-i] [-c] [-C CMAKE_OPTS] [M -MAKE_OPTS] [-m TARGET] [CONFIGURATION ...]

Run CMake and build the given configurations (default is
Debug and Release.

Options:
  -i               Incremental build (don't clean).
  -c               Configure only (don't build).
  -C CMAKE_OPTS    Extra options to pass to CMake.
  -M MAKE_OPTS     Extra options to pass to Make.
  -m TARGET        Make target to build. Defaults to all.
"

while getopts icC:M:m:h name
do
  case $name in
  i)  incremental=1;;
  c)  nobuild=1;;
  C)  cmake_opts="$OPTARG";;
  M)  make_opts="$OPTARG";;
  m)  target="$OPTARG";;
  h)  printf "$usage" $0
      exit 0;;
  ?)  printf "$usage" $0
      exit 2;
  esac
done

shift $(($OPTIND - 1))
[ $# -gt 0 ] && configurations=$@
[ -n "$nobuild" ] && configurations=

#If REPO_ROOT is not defined, then the default behavior is to
#set REPO_ROOT to be the directory containing this script.
if [ "${REPO_ROOT}" = "" ]
then
	REPO_ROOT=`readlink -f $0`
	REPO_ROOT=`dirname ${REPO_ROOT}`
	REPO_ROOT="${REPO_ROOT}/.."
fi
export PROJ_ROOT="$REPO_ROOT/testing"

cd "${PROJ_ROOT}"
echo Project directory is ${PROJ_ROOT}

# Find Cmake, preferring env-specified or cmake28
for CMAKE in $CMAKE /usr/bin/cmake28 /usr/bin/cmake
do
	[ -x "$CMAKE" ] && break
done

[ -n "$incremental" ] || rm -rf CMakeGenerated

retVal=0

for configuration in $configurations
do
	subdir=`buildcfg2subdir $configuration`
	mkdir -p CMakeGenerated/$subdir
	(cd CMakeGenerated/$subdir
		$CMAKE $cmake_opts -DCMAKE_BUILD_TYPE="$subdir" "${PROJ_ROOT}" || exit $?
		[ -n "$incremental" ] || make $make_opts clean || exit $?
		[ -n "$nobuild" ] || make $make_opts $target || exit $?
	) || retVal=$?
done

exit $retVal
