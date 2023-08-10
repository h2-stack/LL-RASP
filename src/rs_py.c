//
// Created by zhuonan li on 8/18/21.
//

#include <stdio.h>
#include <unistd.h>
#include <dlfcn.h>
#include <Python.h>
#include<frameobject.h>
#include "../rs_header.h"

extern char *get_binary_path(pid_t pid);

extern struct elf_info get_elf_info(int pid, const char *elf_name);

extern void hook_module(char *elf_path, const char *func_name, void *new_func, int inline_hook, int is_save_symbol);

void (*analyze_event)(const char *type, char *param, char *bt, void *param_addr);

#if PY_MAJOR_VERSION >= 3 && PY_MINOR_VERSION >= 3
#define to_cstring PyUnicode_AsUTF8String
#else
#define to_cstring PyString_AsString
#endif

__attribute__((unused)) void dump_back_trace(char *bt) {
    char tmp_line[SIZE_256];
    PyGILState_STATE state = PyGILState_Ensure();
    PyThreadState *t_state = PyThreadState_Get();
#if PY_MAJOR_VERSION >= 3 && PY_MINOR_VERSION >= 11
    PyFrameObject *frame = PyThreadState_GetFrame(t_state);
#else
    PyFrameObject *frame = t_state->frame;
#endif

    while (frame != NULL) {
#if PY_MAJOR_VERSION >= 3 && PY_MINOR_VERSION >= 11
        int line = PyCode_Addr2Line(PyFrame_GetCode(frame), PyFrame_GetLasti(frame));
        const char *file_name = PyUnicode_AsUTF8(PyFrame_GetCode(frame)->co_filename);
        const char *func_name = PyUnicode_AsUTF8(PyFrame_GetCode(frame)->co_name);
#else
        int line = PyCode_Addr2Line(frame->f_code, frame->f_lasti);
        const char *file_name = (const char *) to_cstring(frame->f_code->co_filename);
        const char *func_name = (const char *) to_cstring(frame->f_code->co_name);
#endif
        memset(tmp_line, 0, SIZE_256);
        snprintf(tmp_line, SIZE_256, "%s (%s:%d)\n", func_name, file_name, line);
        strncat(bt, tmp_line, SIZE_256);
#if PY_MAJOR_VERSION >= 3 && PY_MINOR_VERSION >= 11
        frame = PyFrame_GetBack(frame);
#else
        frame = frame->f_back;
#endif
    }

    PyGILState_Release(state);
}


int rs_system(char *line) {
    printf("[*] [UDS] [RS] Detected [system] with cmd: [%s]\n", line);
    char *bt = calloc(1, SIZE_4K);
    dump_back_trace(bt);
    analyze_event("system", line, bt, line);
    free(bt);
    bt = NULL;
    return system(line);
}

void *engine_module = NULL;

void install() {

    engine_module = dlopen(RS_ENGINE_PATH, RTLD_LAZY);
    if (engine_module == NULL) {
        printf("[!] [UDS] [RS] Failed to load engine module.\n");
        return;
    }

    analyze_event = dlsym(engine_module, "analyze_event");
    if (analyze_event == NULL) {
        printf("[!] [UDS] [RS] Failed to locate analyze_event.\n");
        return;
    }

    struct elf_info elfInfo = get_elf_info(0, "libpython");

    if (elfInfo.base == 0) {
        char *binary_path = get_binary_path(getpid());
        hook_module(binary_path, "system", rs_system, 0, 0);
        free(binary_path);
        binary_path = NULL;
    } else {
        hook_module(elfInfo.path, "system", rs_system, 0, 0);
    }

}

extern void uninstall_hook();

void uninstall() {
    DEBUG_PRINT(("[*] [UDS] [RS] Uninstalling UDS-Python extension.\n"));
    uninstall_hook();
    dlclose(engine_module);
    DEBUG_PRINT(("[*] [UDS] [RS] Python module has been successfully uninstalled.\n"));
}

__attribute__((constructor)) void init() {
    printf("[*] [RS] [Python] install address: %p\n", install);
    printf("[*] [RS] [Python] uninstall address: %p\n", uninstall);
}
