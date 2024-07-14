cmake_minimum_required(VERSION 3.24)

project(
	PersonalizedMannequins
	VERSION 1.2.0
	DESCRIPTION "An SKSE plugin built with CommonLibSSE to make mannequins look like the player"
	LANGUAGES CXX
)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
include(XSEPlugin)

find_package(EASTL CONFIG REQUIRED)

target_link_libraries(
	${PROJECT_NAME}
	PRIVATE
	EASTL
)