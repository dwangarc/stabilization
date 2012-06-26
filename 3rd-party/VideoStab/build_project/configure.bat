cd %1
set BUILD_DIR="build"
set BUILD_TYPE="Debug"

if  "%2" == "" ( 
	echo Build Type: Debug
)
else ( 
	set BUILD_TYPE = %2 
	echo Build Type: %BUILD_TYPE%
)

if exist "%BUILD_DIR%" (
	if exist "%BUILD_DIR%\CMakeCache.txt" ( 
		del %BUILD_DIR%\CMakeCache.txt
	)
)
else ( 
	mkdir %BUILD_DIR% 
)


cd %BUILD_DIR%

cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=%BUILD_TYPE% -DCMAKE_INSTALL_PREFIX=..\ 	..\sources