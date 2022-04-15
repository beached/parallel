# Copyright (c) Darrell Wright
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
#
# Official repository: https://github.com/beached/parallel
#

set(TEST_SOURCES 
	daw_parallel_copy_mutex_test.cpp 
	daw_parallel_counter_test.cpp 
	daw_parallel_latch_test.cpp 
	daw_parallel_scoped_multilock_test.cpp 
	daw_parallel_semaphore_test.cpp)

#not included in CI as they are not ready
set(DEV_TEST_SOURCES 
	daw_parallel_locked_value_test.cpp 
	daw_parallel_observable_ptr_test.cpp 
	daw_parallel_observable_ptr_pair_test.cpp 
	daw_parallel_spin_lock_test.cpp
)

#Allows building all in some IDE's
add_custom_target(full)

add_library(daw_test INTERFACE)
target_link_libraries(daw_test INTERFACE daw::daw-parallel ${COMPILER_SPECIFIC_LIBS} ${CMAKE_THREAD_LIBS_INIT})
target_compile_options(daw_test INTERFACE $<$<CXX_COMPILER_ID:MSVC>:/permissive->)

set(all_tests ${TEST_SOURCES})

if (NOT CMAKE_CXX_COMPILER_ID MATCHES "MSVC" )
	list(APPEND all_tests ${NOT_MSVC_TEST_SOURCES})
endif ()

foreach (CUR_TEST IN LISTS all_tests)
	string(REPLACE ".cpp" "" CUR_TEST_NAME ${CUR_TEST})
	add_executable(${CUR_TEST_NAME} src/${CUR_TEST})
	target_link_libraries(${CUR_TEST_NAME} PRIVATE daw_test)
	add_test(${CUR_TEST_NAME}_test ${CUR_TEST_NAME})
	add_dependencies(full ${CUR_TEST_NAME})
endforeach ()

option(DAW_ENABLE_DEV_TESTING "Build unit tests still in development" OFF)
if (DAW_ENABLE_DEV_TESTING)
	foreach (CUR_TEST IN LISTS DEV_TEST_SOURCES)
		string(REPLACE ".cpp" "" CUR_TEST_NAME ${CUR_TEST})
		add_executable(${CUR_TEST_NAME} ${CUR_TEST})
		target_link_libraries(${CUR_TEST_NAME} PRIVATE daw_test)
		add_dependencies(full ${CUR_TEST_NAME})
	endforeach ()
endif ()

