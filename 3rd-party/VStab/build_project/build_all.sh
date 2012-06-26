#!/bin/bash
# TODO сделать удаление исходников после возникновения ошибки сборки
VERSION=""
SVN_SRV_ADDR='svn+ssh://192.168.0.240/svn_repository'
PRJ_LST=''
#gnomes/server
#'
#gnomes/articules2xml
#gnomes/testclient
#engine/multiplexor
#engine/multiplexor_admin
#gnomes/admin_tools
#gnomes/server
#'


while getopts “hv:p:” OPTION
do
     case $OPTION in
         h)
             echo "Usage"
             exit 1
             ;;
         v)
             VERSION=${OPTARG}
             ;;
         p)
             PRJ_LST=${OPTARG}
             ;;
         ?)
             usage
             exit
             ;;
     esac
done


if [[ ${VERSION} == "" ]]; then
	SVN_PATH=trunk
else
	SVN_PATH=tags/${VERSION}
fi

echo Building
echo version ${VERSION}
echo Projects: \"${PRJ_LST}\"


for PROJ in ${PRJ_LST}; do
	rm -rf ${PROJ}
        echo Checking out from ${SVN_SRV_ADDR}/assemblies/${PROJ}/${SVN_PATH}
	svn co ${SVN_SRV_ADDR}/assemblies/${PROJ}/${SVN_PATH} ${PROJ}
	if [ $? != 0 ]; then
		echo "Failed to get source of " ${PROJ}
		exit 1
	fi
done

for PROJ in ${PRJ_LST}; do
	./${PROJ}/build_project/build_distrib ${PROJ}
	if [ $? != 0 ]; then
		echo "Failed to build of " ${PROJ}
		exit 1
	fi
done

for PROJ in ${PRJ_LST}; do
	rm -rf ${PROJ}
done

