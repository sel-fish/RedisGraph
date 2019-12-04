INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/deps/xxHash)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/deps/rax)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/deps/libcypher-parser/lib/src)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/deps/RediSearch/src)

ADD_DEFINITIONS(-DREDISMODULE_EXPERIMENTAL_API)
ADD_DEFINITIONS(-D_GNU_SOURCE)

FILE(GLOB SOURCE_FILES
        "algorithms/*.c"
        "arithmetic/*.c"
        "arithmetic/list_funcs/*.c"
        "arithmetic/time_funcs/*.c"
        "arithmetic/entity_funcs/*.c"
        "arithmetic/string_funcs/*.c"
        "arithmetic/boolean_funcs/*.c"
        "arithmetic/numeric_funcs/*.c"
        "arithmetic/conditional_funcs/*.c"
        "arithmetic/path_funcs/*.c"
        "bulk_insert/*.c"
        "commands/*.c"
        "datatypes/*.c"
        "datatypes/path/*.c"
        "execution_plan/*.c"
        "execution_plan/ops/*.c"
        "execution_plan/ops/shared/*.c"
        "execution_plan/optimizations/*.c"
        "filter_tree/*.c"
        "graph/*.c"
        "graph/entities/*.c"
        "graph/serializers/*.c"
        "graph/serializers/encoder/*.c"
        "graph/serializers/decoders/*.c"
        "graph/serializers/decoders/prev/*.c"
        "graph/serializers/decoders/prev/*/*.c"
        "GraphBLASExt/*.c"
        "grouping/*.c"
        "index/*.c"
        "ast/*.c"
        "ast/enrichment/*.c"
        "resultset/*.c"
        "resultset/formatters/*.c"
        "schema/*.c"
        "procedures/*.c"
        "util/*.c"
        "util/datablock/*.c"
        "util/thpool/*.c"
        "util/range/*.c"
        "*.c"
        )

ADD_LIBRARY(redisgraph SHARED ${SOURCE_FILES})
TARGET_LINK_LIBRARIES(redisgraph
        ${PROJECT_SOURCE_DIR}/deps/rax/rax.o
        ${PROJECT_SOURCE_DIR}/deps/xxHash/libxxhash.a
        ${PROJECT_SOURCE_DIR}/deps/GraphBLAS/build/libgraphblas.a
        ${PROJECT_SOURCE_DIR}/deps/RediSearch/build/libredisearch.a
        ${PROJECT_SOURCE_DIR}/deps/libcypher-parser/lib/src/.libs/libcypher-parser.a
        )
SET_TARGET_PROPERTIES(redisgraph PROPERTIES PREFIX "")
SET_TARGET_PROPERTIES(redisgraph PROPERTIES SUFFIX ".so")
IF (APPLE)
    SET_TARGET_PROPERTIES(redisgraph
            PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
ENDIF()