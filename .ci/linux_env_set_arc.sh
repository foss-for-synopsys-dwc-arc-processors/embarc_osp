#!/bin/bash
# this script is to generate a env_set for user to source. #
#

TOOLCHAIN=mw
ARC_DEV_TOOL_ROOT="~/ARC"
ENVIRONMENT_FILE="arctool.env"
help_flag=false

# Get Command Options #
while getopts "t:r:e:hH?" opt ; do
	case $opt in
	t)
		TOOLCHAIN=$OPTARG
		;;
	r)
		ARC_DEV_TOOL_ROOT=$(echo $OPTARG|sed 's/\/\/*$/\//')
		;;
	e)
		ENVIRONMENT_FILE=$OPTARG
		;;
	h | ? | H)
		help_flag=true
		;;
	esac
done


if $help_flag ; then
	echo "$0 -t <mw/gnu> -r <toolset root>"
	exit -1
fi

if [ -f "${ENVIRONMENT_FILE}" ] ; then
	echo "Environment file exists, now backup it into ${ENVIRONMENT_FILE}.bak"
	mv -f ${ENVIRONMENT_FILE} ${ENVIRONMENT_FILE}.bak
fi

if [ "${TOOLCHAIN}" == "mw" ] ; then
	## For Metaware Development Kit ##
	if [ -d "${ARC_DEV_TOOL_ROOT}/MetaWare/arc/bin" ] ; then
		## Echo Message
		echo "Generate ${ENVIRONMENT_FILE} environment file for Metaware Development Toolset"
		## Generate File
		echo "# source <this file> to set up the run env for MWDT #" >> ${ENVIRONMENT_FILE}
		REAL_ARC_DEV_ROOT=$(readlink -f ${ARC_DEV_TOOL_ROOT})
		echo "METAWARE_ROOT=${REAL_ARC_DEV_ROOT}/MetaWare" >> ${ENVIRONMENT_FILE}
		echo "export METAWARE_ROOT" >> ${ENVIRONMENT_FILE}
		echo "NSIM=${REAL_ARC_DEV_ROOT}/nSIM" >> ${ENVIRONMENT_FILE}
		echo "export NSIM" >> ${ENVIRONMENT_FILE}
		echo "PATH=${REAL_ARC_DEV_ROOT}/MetaWare/ide:${REAL_ARC_DEV_ROOT}/MetaWare/arc/bin:${REAL_ARC_DEV_ROOT}/nSIM/nSIM/bin:\${PATH}" >> ${ENVIRONMENT_FILE}
		echo "export PATH" >> ${ENVIRONMENT_FILE}
		echo "LD_LIBRARY_PATH=\${LD_LIBRARY_PATH}:${REAL_ARC_DEV_ROOT}/MetaWare/arc/bin" >> ${ENVIRONMENT_FILE}
		echo "export LD_LIBRARY_PATH" >> ${ENVIRONMENT_FILE}
	else
		echo "This is not a Metaware Development Toolset Directory Root"
	fi
elif [ "${TOOLCHAIN}" == "gnu" ] ; then
	## For GNU Development Kit ##
	if [ -d "${ARC_DEV_TOOL_ROOT}/bin" ] ; then
		## Echo Message
		echo "Generate ${ENVIRONMENT_FILE} environment file for ARC GNU Development Toolset"
		## Generate File
		echo "# source <this file> to set up the run env for ARC GNU #" >> ${ENVIRONMENT_FILE}
		REAL_ARC_DEV_ROOT=$(readlink -f ${ARC_DEV_TOOL_ROOT})
		echo "ARC_GNU_ROOT=${REAL_ARC_DEV_ROOT}" >> ${ENVIRONMENT_FILE}
		echo "export ARC_GNU_ROOT" >> ${ENVIRONMENT_FILE}
		echo "PATH=${REAL_ARC_DEV_ROOT}/bin:\${PATH}" >> ${ENVIRONMENT_FILE}
		echo "export PATH" >> ${ENVIRONMENT_FILE}
	else
		echo "This is not an ARC GNU Development Toolset Directory Root"
	fi
else
	echo "Toolchain $TOOLCHAIN not supported(mw,gnu)"
fi