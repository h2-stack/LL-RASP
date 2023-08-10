cmake_minimum_required(VERSION 3.20.2)
project(rosetta_s C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS_DEBUG "-DRS_DEBUG")

# Module info
add_definitions(-D__KERNEL__ -DMODULE)

# Find kernel release
execute_process(
        COMMAND uname -r
        OUTPUT_VARIABLE KERNEL_RELEASE
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Find Path of kernel headers
find_path(
        KERNEL_HEADERS_DIR
        include/linux/user.h
        PATHS /usr/src/kernels/${KERNEL_RELEASE} /usr/src/linux-headers-${KERNEL_RELEASE}
)

message(STATUS "Kernel Release: ${KERNEL_RELEASE}")
message(STATUS "Kernel Headers: ${KERNEL_HEADERS_DIR}")

# Add header files
include_directories(
        #        /usr/include
        #        ${KERNEL_HEADERS_DIR}/include
        #        ${KERNEL_HEADERS_DIR}/arch/x86/include
)

add_library(ext_module src/playground/ext_module.c)
add_executable(rs_debug src/playground/rs_debug.c src/rs_factory.c)

add_executable(nflog src/playground/nf-log.c)
target_include_directories(nflog PRIVATE /usr/include)

# link section.
target_link_libraries(rs_debug ${CMAKE_DL_LIBS} pthread ext_module)
#target_compile_options(rs_debug PRIVATE "-fomit-frame-pointer")

add_library(rs_engine SHARED src/rs_engine.c)

add_library(rs_jni SHARED src/VM/rs_jni.c)
target_include_directories(rs_jni PRIVATE /home/ubuntu/.sdkman/candidates/java/8.0.302-open/include /home/ubuntu/.sdkman/candidates/java/8.0.302-open/include/linux)

add_library(rs_c SHARED src/VM/rs_c.c src/rs_factory.c)
add_library(rs_js SHARED src/VM/rs_js.c src/rs_factory.c)
add_library(rs_go SHARED src/VM/rs_go.c src/rs_factory.c)
add_library(rs_jdk6 SHARED src/VM/rs_jvm.c src/rs_factory.c)
add_library(rs_jdk7 SHARED src/VM/rs_jvm.c src/rs_factory.c)
add_library(rs_jdk8 SHARED src/VM/rs_jvm.c src/rs_factory.c)
add_library(rs_jdk17 SHARED src/VM/rs_jvm.c src/rs_factory.c)

add_library(rs_py24 SHARED src/VM/rs_py.c src/rs_factory.c)
add_library(rs_py25 SHARED src/VM/rs_py.c src/rs_factory.c)
add_library(rs_py26 SHARED src/VM/rs_py.c src/rs_factory.c)
add_library(rs_py27 SHARED src/VM/rs_py.c src/rs_factory.c)
add_library(rs_py30 SHARED src/VM/rs_py.c src/rs_factory.c)
add_library(rs_py31 SHARED src/VM/rs_py.c src/rs_factory.c)
add_library(rs_py32 SHARED src/VM/rs_py.c src/rs_factory.c)
add_library(rs_py33 SHARED src/VM/rs_py.c src/rs_factory.c)
add_library(rs_py34 SHARED src/VM/rs_py.c src/rs_factory.c)
add_library(rs_py35 SHARED src/VM/rs_py.c src/rs_factory.c)
add_library(rs_py36 SHARED src/VM/rs_py.c src/rs_factory.c)
add_library(rs_py37 SHARED src/VM/rs_py.c src/rs_factory.c)
add_library(rs_py38 SHARED src/VM/rs_py.c src/rs_factory.c)
add_library(rs_py39 SHARED src/VM/rs_py.c src/rs_factory.c)
add_library(rs_py310 SHARED src/VM/rs_py.c src/rs_factory.c)
add_library(rs_py311 SHARED src/VM/rs_py.c src/rs_factory.c)

add_library(rs_php54 SHARED src/VM/rs_php.c src/rs_factory.c)
add_library(rs_php55 SHARED src/VM/rs_php.c src/rs_factory.c)
add_library(rs_php56 SHARED src/VM/rs_php.c src/rs_factory.c)
add_library(rs_php70 SHARED src/VM/rs_php.c src/rs_factory.c)
add_library(rs_php71 SHARED src/VM/rs_php.c src/rs_factory.c)
add_library(rs_php72 SHARED src/VM/rs_php.c src/rs_factory.c)
add_library(rs_php73 SHARED src/VM/rs_php.c src/rs_factory.c)
add_library(rs_php74 SHARED src/VM/rs_php.c src/rs_factory.c)
add_library(rs_php80 SHARED src/VM/rs_php.c src/rs_factory.c)
add_library(rs_php81 SHARED src/VM/rs_php.c src/rs_factory.c)

add_library(rs_rb27 SHARED src/VM/rs_rb.c src/rs_factory.c)
add_library(rs_rb30 SHARED src/VM/rs_rb.c src/rs_factory.c)
add_library(rs_rb31 SHARED src/VM/rs_rb.c src/rs_factory.c)

# include section.
target_include_directories(rs_jdk6 PRIVATE /home/ubuntu/.sdkman/candidates/java/6.0.119-zulu/include /home/ubuntu/.sdkman/candidates/java/6.0.119-zulu/include/linux)
target_include_directories(rs_jdk7 PRIVATE /home/ubuntu/.sdkman/candidates/java/7.0.342-zulu/include /home/ubuntu/.sdkman/candidates/java/7.0.342-zulu/include/linux)
target_include_directories(rs_jdk8 PRIVATE /home/ubuntu/.sdkman/candidates/java/8.0.302-open/include /home/ubuntu/.sdkman/candidates/java/8.0.302-open/include/linux)
target_include_directories(rs_jdk17 PRIVATE /home/ubuntu/.sdkman/candidates/java/17.0.6-amzn/include /home/ubuntu/.sdkman/candidates/java/17.0.6-amzn/include/linux)

target_include_directories(rs_py24 PRIVATE /home/ubuntu/python/Python-2.4.6 /home/ubuntu/python/Python-2.4.6/Include)
target_include_directories(rs_py25 PRIVATE /home/ubuntu/python/Python-2.5.6 /home/ubuntu/python/Python-2.5.6/Include)
target_include_directories(rs_py26 PRIVATE /home/ubuntu/python/Python-2.6.9 /home/ubuntu/python/Python-2.6.9/Include)
target_include_directories(rs_py27 PRIVATE /home/ubuntu/python/Python-2.7.18 /home/ubuntu/python/Python-2.7.18/Include)
target_include_directories(rs_py30 PRIVATE /home/ubuntu/python/Python-3.0.1 /home/ubuntu/python/Python-3.0.1/Include)
target_include_directories(rs_py31 PRIVATE /home/ubuntu/python/Python-3.1.5 /home/ubuntu/python/Python-3.1.5/Include)
target_include_directories(rs_py32 PRIVATE /home/ubuntu/python/Python-3.2.6 /home/ubuntu/python/Python-3.2.6/Include)
target_include_directories(rs_py33 PRIVATE /home/ubuntu/python/Python-3.3.7 /home/ubuntu/python/Python-3.3.7/Include)
target_include_directories(rs_py34 PRIVATE /home/ubuntu/python/Python-3.4.10 /home/ubuntu/python/Python-3.4.10/Include)
target_include_directories(rs_py35 PRIVATE /home/ubuntu/python/Python-3.5.10 /home/ubuntu/python/Python-3.5.10/Include)
target_include_directories(rs_py36 PRIVATE /home/ubuntu/python/Python-3.6.15 /home/ubuntu/python/Python-3.6.15/Include)
target_include_directories(rs_py37 PRIVATE /home/ubuntu/python/Python-3.7.12 /home/ubuntu/python/Python-3.7.12/Include)
target_include_directories(rs_py38 PRIVATE /home/ubuntu/python/Python-3.8.13 /home/ubuntu/python/Python-3.8.13/Include)
target_include_directories(rs_py39 PRIVATE /home/ubuntu/python/Python-3.9.13 /home/ubuntu/python/Python-3.9.13/Include)
target_include_directories(rs_py310 PRIVATE /home/ubuntu/python/Python-3.10.5 /home/ubuntu/python/Python-3.10.5/Include)
target_include_directories(rs_py311 PRIVATE /home/ubuntu/python/Python-3.11.2 /home/ubuntu/python/Python-3.11.2/Include)

target_include_directories(rs_php54 PRIVATE /home/ubuntu/php/php-5.4.0/ /home/ubuntu/php/php-5.4.0/main /home/ubuntu/php/php-5.4.0/Zend /home/ubuntu/php/php-5.4.0/TSRM /home/ubuntu/php/php-5.4.0/include)
target_include_directories(rs_php55 PRIVATE /home/ubuntu/php/php-5.5.0/ /home/ubuntu/php/php-5.5.0/main /home/ubuntu/php/php-5.5.0/Zend /home/ubuntu/php/php-5.5.0/TSRM /home/ubuntu/php/php-5.5.0/include)
target_include_directories(rs_php56 PRIVATE /home/ubuntu/php/php-5.6.0/ /home/ubuntu/php/php-5.6.0/main /home/ubuntu/php/php-5.6.0/Zend /home/ubuntu/php/php-5.6.0/TSRM /home/ubuntu/php/php-5.6.0/include)
target_include_directories(rs_php70 PRIVATE /home/ubuntu/php/php-7.0.0/ /home/ubuntu/php/php-7.0.0/main /home/ubuntu/php/php-7.0.0/Zend /home/ubuntu/php/php-7.0.0/TSRM /home/ubuntu/php/php-7.0.0/include)
target_include_directories(rs_php71 PRIVATE /home/ubuntu/php/php-7.1.0/ /home/ubuntu/php/php-7.1.0/main /home/ubuntu/php/php-7.1.0/Zend /home/ubuntu/php/php-7.1.0/TSRM /home/ubuntu/php/php-7.1.0/include)
target_include_directories(rs_php72 PRIVATE /home/ubuntu/php/php-7.2.0/ /home/ubuntu/php/php-7.2.0/main /home/ubuntu/php/php-7.2.0/Zend /home/ubuntu/php/php-7.2.0/TSRM /home/ubuntu/php/php-7.2.0/include)
target_include_directories(rs_php73 PRIVATE /home/ubuntu/php/php-7.3.0/ /home/ubuntu/php/php-7.3.0/main /home/ubuntu/php/php-7.3.0/Zend /home/ubuntu/php/php-7.3.0/TSRM /home/ubuntu/php/php-7.3.0/include)
target_include_directories(rs_php74 PRIVATE /home/ubuntu/php/php-7.4.0/ /home/ubuntu/php/php-7.4.0/main /home/ubuntu/php/php-7.4.0/Zend /home/ubuntu/php/php-7.4.0/TSRM /home/ubuntu/php/php-7.4.0/include)
target_include_directories(rs_php80 PRIVATE /home/ubuntu/php/php-8.0.0/ /home/ubuntu/php/php-8.0.0/main /home/ubuntu/php/php-8.0.0/Zend /home/ubuntu/php/php-8.0.0/TSRM /home/ubuntu/php/php-8.0.0/include)
target_include_directories(rs_php81 PRIVATE /home/ubuntu/php/php-8.1.0/ /home/ubuntu/php/php-8.1.0/main /home/ubuntu/php/php-8.1.0/Zend /home/ubuntu/php/php-8.1.0/TSRM /home/ubuntu/php/php-8.1.0/include)

target_include_directories(rs_rb27 PRIVATE /home/ubuntu/ruby/ruby-2.7.6/.ext/include/x86_64-linux /home/ubuntu/ruby/ruby-2.7.6/include)
target_include_directories(rs_rb30 PRIVATE /home/ubuntu/ruby/ruby-3.0.4/.ext/include/x86_64-linux /home/ubuntu/ruby/ruby-3.0.4/include)
target_include_directories(rs_rb31 PRIVATE /home/ubuntu/ruby/ruby-3.1.2/.ext/include/x86_64-linux /home/ubuntu/ruby/ruby-3.1.2/include)


# cxx section.
enable_language(CXX)

add_library(rs_node6 SHARED src/VM/rs_js.cc)
add_library(rs_node8 SHARED src/VM/rs_js.cc)
add_library(rs_node9 SHARED src/VM/rs_js.cc)
add_library(rs_node10 SHARED src/VM/rs_js.cc)
add_library(rs_node11 SHARED src/VM/rs_js.cc)
add_library(rs_node12 SHARED src/VM/rs_js.cc)
add_library(rs_node13 SHARED src/VM/rs_js.cc)
add_library(rs_node14 SHARED src/VM/rs_js.cc)
add_library(rs_node15 SHARED src/VM/rs_js.cc)
add_library(rs_node16 SHARED src/VM/rs_js.cc)
add_library(rs_node17 SHARED src/VM/rs_js.cc)
add_library(rs_node18 SHARED src/VM/rs_js.cc)


target_include_directories(rs_node6 PRIVATE /home/ubuntu/.nvm/versions/node/v6.17.1/include/node) # NAPI 3
target_include_directories(rs_node8 PRIVATE /home/ubuntu/.nvm/versions/node/v8.12.0/include/node) # NAPI 3
target_include_directories(rs_node9 PRIVATE /home/ubuntu/.nvm/versions/node/v9.11.2/include/node) # NAPI 3
target_include_directories(rs_node10 PRIVATE /home/ubuntu/.nvm/versions/node/v10.24.1/include/node) # NAPI 3
target_include_directories(rs_node11 PRIVATE /home/ubuntu/.nvm/versions/node/v11.15.0/include/node) # NAPI 3
target_include_directories(rs_node12 PRIVATE /home/ubuntu/.nvm/versions/node/v12.22.12/include/node) # NAPI 4
target_include_directories(rs_node13 PRIVATE /home/ubuntu/.nvm/versions/node/v13.14.0/include/node) # NAPI 5
target_include_directories(rs_node14 PRIVATE /home/ubuntu/.nvm/versions/node/v14.19.3/include/node) # NAPI 6
target_include_directories(rs_node15 PRIVATE /home/ubuntu/.nvm/versions/node/v15.14.0/include/node) # NAPI 7
target_include_directories(rs_node16 PRIVATE /home/ubuntu/.nvm/versions/node/v16.15.1/include/node) # NAPI 8
target_include_directories(rs_node17 PRIVATE /home/ubuntu/.nvm/versions/node/v17.9.1/include/node) # NAPI 8
target_include_directories(rs_node18 PRIVATE /home/ubuntu/.nvm/versions/node/v18.4.0/include/node) # NAPI 8


# ----- private study -----
add_executable(test_cpp src/playground/test_cpp.cpp)

# --- Custom MAKE Section
set(LIBS
        c
        go
        jdk6 jdk7 jdk8 jdk17
        node6 node8 node9 node10 node11 node12 node13 node14 node15 node16 node17 node18
        py24 py25 py26 py27 py30 py31 py32 py33 py34 py35 py36 py37 py38 py39 py310 py311
        php54 php55 php56 php70 php71 php72 php73 php74 php80 php81
        rb27 rb30 rb31
        )

function(do_archive)
    foreach (lang ${LIBS})
        # compile rs_engine rs_js first.
        add_dependencies(rs_${lang} rs_engine rs_js)
        add_custom_command(TARGET rs_${lang}
                POST_BUILD
                COMMENT "[*] Running Script after build ${lang} ..."
                COMMAND ${CMAKE_COMMAND} -DLANG=${lang} -P ${CMAKE_SOURCE_DIR}/CMakeArchive.cmake
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                )
        set_property(TARGET rs_${lang}
                APPEND
                PROPERTY ADDITIONAL_CLEAN_FILES archive_${lang}.tgz
                )
    endforeach ()
endfunction()

do_archive()
