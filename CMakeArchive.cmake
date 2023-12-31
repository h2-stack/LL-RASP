# Archive lightweight extensions.
if (${LANG} MATCHES "node")
    message("[*] Archiving NodeJs to archive_${LANG}.tgz to ${CMAKE_CURRENT_BINARY_DIR}")
    execute_process(COMMAND ln -s librs_${LANG}.so librs_js_ext.so WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    execute_process(COMMAND ln -s librs_js.so librs_ext.so WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    execute_process(COMMAND tar -zcf archive_${LANG}.tgz librs_engine.so librs_${LANG}.so librs_ext.so librs_js_ext.so librs_js.so WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    execute_process(COMMAND rm -rf librs_ext.so librs_js_ext.so WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
else ()
    message("[*] Archiving to archive_${LANG}.tgz")
    execute_process(COMMAND ln -s librs_${LANG}.so librs_ext.so WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    execute_process(COMMAND tar -zcf archive_${LANG}.tgz librs_engine.so librs_${LANG}.so librs_ext.so WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    execute_process(COMMAND rm -rf librs_ext.so WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
endif ()
