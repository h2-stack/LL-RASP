//
// Created by zhuonan li on 8/23/21.
//

#include <malloc.h>
#include <string.h>
#include <ruby.h>
#include "../rs_header.h"


extern void *__libc_dlopen_mode(const char *, int);

extern void *__libc_dlsym(void *, const char *);

extern void *__libc_dlclose(void *);

extern struct elf_info get_elf_info(int pid, const char *elf_name);

extern void hook_module(char *elf_path, const char *func_name, void *new_func, int inline_hook, int is_save_symbol);

void (*analyze_event)(const char *type, char *param, char *bt, void *param_addr);


__attribute__((unused)) void dump_back_trace(char *bt) {
    VALUE rb_bt = rb_make_backtrace();
    VALUE a = rb_ary_join(rb_bt, rb_str_new_cstr("\n"));
    strncat(bt, rb_string_value_cstr(&a), SIZE_4K);
}


// ruby 2.7
void rs_rb_execarg_new(int argc, const VALUE *argv, int accept_shell, int allow_exc_opt) {

    unsigned long j1 = 0;
    unsigned long j2 = 0;
    // ruby memory
    memcpy(&j1, argv, LONG_SIZE);

    memcpy(&j2, (void *) j1 + LONG_SIZE * 2, LONG_SIZE);

    char *cmd = calloc(1, SIZE_1K);

    void *param_addr;

    if (j2 < 0xFFFFFFFF) {
        unsigned long j3 = 0;
        memcpy(&j3, (void *) j1 + LONG_SIZE * 3, LONG_SIZE);
        param_addr = (void *) j3;
    } else {
        param_addr = (void *) j1 + LONG_SIZE * 2;
    }

    memcpy(cmd, param_addr, SIZE_1K);

    printf("[*] [UDS] [RS] Detected [rb_execarg_new] with cmd: [%s]\n", cmd);

    char *bt = calloc(1, SIZE_4K);
    dump_back_trace(bt);
    analyze_event("rb_execarg_new", cmd, bt, param_addr);
    free(bt);
    bt = NULL;
}


void *engine_module = NULL;

__attribute__((unused)) void install() {

    // init analyze_event
    engine_module = __libc_dlopen_mode(RS_ENGINE_PATH, 2);

    if (engine_module == NULL) {
        printf("[!] [UDS] [RS] Failed to load engine module.\n");
        return;
    }

    analyze_event = __libc_dlsym(engine_module, "analyze_event");
    if (analyze_event == NULL) {
        printf("[!] [UDS] [RS] Failed to locate analyze_event.\n");
        return;
    }

    struct elf_info elfInfo = get_elf_info(0, "libruby");
    if (elfInfo.base != 0) {
        hook_module(elfInfo.path, "rb_execarg_new", rs_rb_execarg_new, 1, 0);
    } else {
        printf("[!] [UDS] [RS] Unsupported Ruby Version.\n");
    }

}

extern void uninstall_hook();

__attribute__((unused)) void uninstall() {
    DEBUG_PRINT(("[*] [UDS] [RS] Uninstalling UDS-Ruby extension.\n"));
    uninstall_hook();
    __libc_dlclose(engine_module);
    DEBUG_PRINT(("[*] [UDS] [RS] Ruby module has been successfully uninstalled.\n"));
}
