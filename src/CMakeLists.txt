file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true *.h *.cpp)


############### Declaration of all targets: ###############

add_executable(${PROJECT_NAME} ${SOURCES}) # the executable binary

add_library(${PROJECT_NAME}_lib STATIC ${SOURCES}) # compiled as static library (e.g. for using in tests)


############### Libraries for target ${PROJECT_NAME} ###############

if(LINUX)
	target_link_libraries(${PROJECT_NAME} PUBLIC
		pthread
	)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE cpr::cpr)


############### Libraries for target ${PROJECT_NAME}_lib ###############

target_link_libraries(${PROJECT_NAME}_lib PRIVATE cpr::cpr)
