//
// Created by zhuonan li on 5/1/22.
//

#include <string.h>
#include <malloc.h>
#include <dlfcn.h>
#include "../rs_header.h"

#define JS_EXT_PATH RS_HOME"/librs_js_ext.so"


extern struct elf_info get_elf_info(int pid, const char *elf_name);

extern void hook_module(char *elf_path, const char *func_name, void *new_func, int inline_hook, int is_save_symbol);

void (*dump_back_trace)(char *) = NULL;

void (*analyze_event)(const char *type, char *param, char *bt, void *param_addr);

int (*orig_uv_spawn)(void *, void *, void *) =NULL;


void rs_uv_spawn_inline(void *loop, void *handle, void *options) {

    long file_address = 0;
    memcpy(&file_address, options + LONG_SIZE, LONG_SIZE);

    // http://iks.cs.ovgu.de/~elkner/tmp/ph/process.html
    // 0 exit_cb
    // 1 char* file
    // 2 char** args
    char *cmd = calloc(1, SIZE_1K);

    long args_address;
    long arg_address;
    memcpy(&args_address, options + LONG_SIZE * 2, LONG_SIZE);

    int arg_index = 0;
    long next_arg_address = 1;

    while (next_arg_address != 0) {
        memcpy(&arg_address, (void *) args_address + LONG_SIZE * arg_index, LONG_SIZE);
        memcpy(&next_arg_address, (void *) args_address + LONG_SIZE * (arg_index + 1), LONG_SIZE);

        strncat(cmd, (char *) arg_address, SIZE_256);

        if (next_arg_address != 0) {
            strncat(cmd, " ", SIZE_2);
        }

        arg_index++;
    }

    printf("[*] [UDS] [RS] Detected [uv_spawn] with cmd: [%s]\n", cmd);
    char *bt = calloc(1, SIZE_4K);
    dump_back_trace(bt);

    analyze_event("uv_spawn", cmd, bt, (void *) file_address);
    free(bt);
    bt = NULL;
    free(cmd);
    cmd = NULL;
}

int rs_uv_spawn(void *loop, void *handle, void *options) {
    rs_uv_spawn_inline(loop, handle, options);
    return orig_uv_spawn(loop, handle, options);
}


void *engine_module = NULL;
void *ext_module = NULL;

void install() {
    // init dump_back_trace;
    ext_module = dlopen(JS_EXT_PATH, RTLD_LAZY);
    if (ext_module == NULL) {
        printf("[!] [UDS] [RS] Failed to load ext module.\n");
        return;
    }

    // init analyze_event
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

    dump_back_trace = dlsym(ext_module, "dump_back_trace");
    if (dump_back_trace == NULL) {
        printf("[!] [UDS] [RS] Failed to find symbol dump_back_trace.\n");
        return;
    }

    struct elf_info elfInfo = get_elf_info(0, "libnode.so"); // dynamic link nodejs

    if (elfInfo.base != 0) {
        DEBUG_PRINT(("[*] Dynamic Linked Node.Js\n"));
        void *module_handle = dlopen(elfInfo.path, RTLD_LAZY);
        orig_uv_spawn = dlsym(module_handle, "uv_spawn");
        hook_module(elfInfo.path, "uv_spawn", (void *) rs_uv_spawn, 0, 0);
    } else {
        DEBUG_PRINT(("[*] Static Linked Node.Js\n"));
        elfInfo = get_elf_info(0, "/bin/node"); // static link nodejs
        if (elfInfo.base != 0) {
            hook_module(elfInfo.path, "uv_spawn", (void *) rs_uv_spawn_inline, 1, 0);
        } else {
            printf("[!] [UDS] [RS] Unsupported Node Version.\n");
        }
    }

}

extern void uninstall_hook();

void uninstall() {
    DEBUG_PRINT(("[*] [UDS] [RS] Uninstalling UDS-Node extension.\n"));
    uninstall_hook();
    dlclose(ext_module);
    DEBUG_PRINT(("[*] [UDS] [RS] Node extension module has been successfully uninstalled.\n"));
    dlclose(engine_module);
    DEBUG_PRINT(("[*] [UDS] [RS] Node module has been successfully uninstalled.\n"));
}

__attribute__((constructor)) void init() {
    printf("[*] [RS] [Node.js] install address: %p\n", install);
    printf("[*] [RS] [Node.js] uninstall address: %p\n", uninstall);
}
