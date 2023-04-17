cmake_minimum_required(VERSION 3.20)

project(
	PersonalizedMannequins
	VERSION 1.0.0
	DESCRIPTION "An SKSE plugin built with CommonLibSSE to make mannequins look like the player"
	LANGUAGES CXX
)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
include(XSEPlugin)
