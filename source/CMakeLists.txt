if(NOT HAVE_C11_THREADS)
	add_subdirectory(tinycthread)
endif()

set(SEQF_SRCS
    seqflib.c
    seqferrno.c
    seqfstrerror.c
    readfasta.c
    readfastq.c
    readreads.c
    seqf_read.c
    seqfread.c)

set(SEQF_PRIVATE_HEADERS
    seqf_core.h
    seqf_read.h)

# Create shared and static libraries from object files
add_library(seqf_shared SHARED ${SEQF_SRCS})
add_library(seqf_static STATIC ${SEQF_SRCS})

target_include_directories(seqf_shared PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR} ${SEQF_INCLUDE_DIR} ${COMPRESSION_INC})
target_include_directories(seqf_static PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR} ${SEQF_INCLUDE_DIR} ${COMPRESSION_INC})

target_link_libraries(seqf_shared PUBLIC
    ${THREAD_LIB} ${COMPRESSION_LIB})
target_link_libraries(seqf_static PUBLIC
    ${THREAD_LIB} ${COMPRESSION_LIB})

# Set optimization for release builds
if(ipo_is_supported)
    set_property(TARGET seqf_shared seqf_static
                 PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
endif()

set_target_properties(seqf_shared PROPERTIES
    POSITION_INDEPENDENT_CODE ON
	VERSION ${PROJECT_VERSION}
	OUTPUT_NAME seqf)
set_target_properties(seqf_static PROPERTIES
	VERSION ${PROJECT_VERSION}
	OUTPUT_NAME seqf)
