cmake_minimum_required(VERSION 2.6)

# дефолтовые настройки
# "-fprofile-arcs -ftest-coverage" -- может понадобится при проверке покрытия тестов
SET(CMAKE_CXX_FLAGS_DEBUG "-Wall -O0 -g")
SET(CMAKE_CXX_FLAGS_RELEASE "-O2 -g -DNDEBUG")
SET(TESTING_ENABLED False)
SET(DISTIBUTIVE_GENERATING_ENABLED False)
SET(DOXYGEN_GENERATING_ENABLED False)
SET(TESTING_PROJECT_SOURCE_DIR "testing")
SET(TESTS_SOURCE_DIR "tests")
SET(BINARY_DIR bin)
SET(CONF_DIR conf)
SET(DOCS_DIR docs)
SET(LOGS_DIR logs)
SET(BUILD_DIR build_dir)
SET(BUILD_PROJECT_DIR build_project)
SET(DOXYGEN_CONFIG_FILE_TEMPLATE doxy.config.in)
SET(DOXYGEN_TARGET docs)
SET(VERSION_MAJOR 0)
SET(VERSION_MINOR 0)
SET(VERSION_RELEASE 0)
SET(CONFIG_FILE_NAME "Config.h")

if(WIN32)
    set(MINGW_PREFIX "i686-pc-mingw32-")
ENDIF()

EXECUTE_PROCESS(COMMAND "date" "+%Y.%m.%d %H:%m:%S" OUTPUT_VARIABLE BUILD_DATE OUTPUT_STRIP_TRAILING_WHITESPACE)

FIND_PACKAGE(Subversion)
IF(Subversion_FOUND)
    IF(EXISTS ${PROJECT_SOURCE_DIR}/.svn)
        Subversion_WC_INFO(${PROJECT_SOURCE_DIR} SUBVERSION)
    ENDIF()
ENDIF()

INCLUDE("AssemblyOptions.cmake")

SET(VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_RELEASE})

# TODO сделать дифинишиня для всего, что определяется выше
# чтобы в коде можено было использовать пути ко всем директориям
#SET(ASSEMBLY_DEFINITIONS )

# если необходимо включаются тесты
IF(TESTING_ENABLED)
    SET(DROP_METHOD "http")
    SET(DROP_SITE "localhost")
    SET(DROP_LOCATION "/submit.php?project=${PROJECT_NAME}")
    SET(VALGRIND_COMMAND_OPTIONS "-q --tool=memcheck --leak-check=full")
    INCLUDE(CTest)
    ENABLE_TESTING()
#    Отключено за ненадобностью
#    SET(SUBDIRS ${SUBDIRS} ${TESTING_PROJECT_SOURCE_DIR})
ENDIF()

# корневая директория с исходниками, отtesting неё беруться пути к инклюдам модулей в cmake модулях
SET(ROOT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# Config.h
IF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${CONFIG_FILE_NAME}.in)
    MESSAGE(STATUS "Generating config file '"${CONFIG_FILE_NAME}')
    CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/${CONFIG_FILE_NAME}.in ${CMAKE_CURRENT_BINARY_DIR}/${CONFIG_FILE_NAME})
    SET(BUILD_CONFIG_DIR ${CMAKE_CURRENT_BINARY_DIR})
    INCLUDE_DIRECTORIES(${BUILD_CONFIG_DIR})
    ADD_DEFINITIONS(-DHAVE_CONFIG_H)
ENDIF()

# подключения файлов с опциями проектов
FOREACH(SUBDIR ${SUBDIRS})
    IF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${SUBDIR}/ProjectOptions.cmake")
        INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/${SUBDIR}/ProjectOptions.cmake)
    ENDIF()
ENDFOREACH()

# Чтобы не допускать перекрёстные зависимоси между модулями,
# и добавление пути к cmake модулю, и добавление поддиректории
# находятся в одном цикле.
# В результате модуль может подключать только модули, которые находятся выше по списку.
FOREACH(SUBDIR ${SUBDIRS})
    SET(CMAKE_MODULE_PATH
        ${CMAKE_CURRENT_SOURCE_DIR}/${SUBDIR}/cmake_modules
        ${CMAKE_MODULE_PATH}
    )
    ADD_SUBDIRECTORY(${SUBDIR})
ENDFOREACH()

# если влключены тесты, то нужно добавить директорию с ними
IF(TESTING_ENABLED)
    ADD_SUBDIRECTORY(${TESTS_SOURCE_DIR})
ENDIF()


# если включена генерация документации, то нужно настроить doxygen и добавить цель для сборки
IF(DOXYGEN_GENERATING_ENABLED)
    SET(DOXYGEN_GEN_DIR "${CMAKE_CURRENT_BINARY_DIR}/doxygen")
    SET(DOXYGEN_CONFIG_FILE doxy.config)
    FIND_PACKAGE(Doxygen REQUIRED)

    IF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${DOXYGEN_CONFIG_FILE_TEMPLATE}")
        MESSAGE(STATUS "configured ${CMAKE_CURRENT_SOURCE_DIR}/${DOXYGEN_CONFIG_FILE_TEMPLATE} --> ${CMAKE_CURRENT_BINARY_DIR}/${DOXYGEN_CONFIG_FILE}")
        CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/${DOXYGEN_CONFIG_FILE_TEMPLATE} ${CMAKE_CURRENT_BINARY_DIR}/${DOXYGEN_CONFIG_FILE} @ONLY)
        SET(DOXY_CONFIG "${CMAKE_CURRENT_BINARY_DIR}/${DOXYGEN_CONFIG_FILE}")
    ELSE()
         MESSAGE(FATAL "${CMAKE_CURRENT_SOURCE_DIR}/${DOXYGEN_CONFIG_FILE_TEMPLATE} not found")
    ENDIF()

    ADD_CUSTOM_TARGET(${DOXYGEN_TARGET} ${DOXYGEN_EXECUTABLE} ${DOXYGEN_CONFIG_FILE})
    file(MAKE_DIRECTORY ${DOXYGEN_GEN_DIR})

    INSTALL(DIRECTORY ${DOXYGEN_GEN_DIR} DESTINATION ${DOCS_DIR})
ENDIF()

IF(DISTIBUTIVE_GENERATING_ENABLED)
    # что-то он не делает то, что хочется =((
    #SET(CPACK_INSTALL_PREFIX "/opt/drimmi/${PROJECT_NAME}")
    SET(CPACK_PACKAGE_VERSION ${VERSION})
    SET(CPACK_PACKAGE_FILE_NAME ${PROJECT_NAME}-${VERSION})
    SET(CPACK_GENERATOR TGZ)
    INCLUDE(CPack)
ENDIF()
