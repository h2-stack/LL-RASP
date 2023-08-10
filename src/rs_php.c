//
// Created by zhuonan li on 8/18/21.
//

#include "php.h"
#include <zend_builtin_functions.h>
#include "../rs_header.h"


extern void *__libc_dlopen_mode(const char *, int);

extern void *__libc_dlsym(void *, const char *);

extern void *__libc_dlclose(void *);

extern char *get_binary_path(pid_t pid);

extern void hook_module(char *elf_path, const char *func_name, void *new_func, int inline_hook, int is_save_symbol);

void (*analyze_event)(const char *type, char *param, char *bt, void *param_addr);


__attribute__((unused)) void dump_back_trace(char *bt) {
    zval backtrace;
    zend_fetch_debug_backtrace(&backtrace, 0, 0, 0);
#if PHP_MAJOR_VERSION >= 7
    zend_array *ht = Z_ARRVAL(backtrace);
    Bucket *p = ht->arData;
    Bucket *end = p + ht->nNumUsed;

    for (; p != end; p++) {
        zval *z = &p->val;
        // stack trace array
        if (Z_TYPE_P(z) == IS_ARRAY) {
            zend_array *ht1 = Z_ARRVAL_P(z);
            Bucket *p1 = ht1->arData;
            Bucket *end1 = p1 + ht1->nNumUsed;
            zend_string *string_key;

            for (; p1 != end1; p1++) {
                zval *z1 = &p1->val;
                string_key = p1->key;

                if (string_key) {
                    char *t = ZSTR_VAL(string_key);
                    if (Z_TYPE_P(z1) == IS_STRING) {
                        if (strncmp(t, "file", 4) == 0 || strncmp(t, "function", 8) == 0) {
                            zend_string *str = zval_get_string(z1);
                            char *c = ZSTR_VAL(str);
                            strncat(bt, c, SIZE_256);
                            if (strncmp(t, "function", 8) != 0) {
                                strncat(bt, ".", SIZE_2);
                            }
                            zend_string_release(str);
                        }
                    }
                }
            }
            strncat(bt, "\n", SIZE_2);
        }
    }
    zend_array_destroy(ht);
#else
#if PHP_MINOR_VERSION >= 4
    zval **tmp;
    zval **file, **function;
    HashPosition hashPosition;

    zend_hash_internal_pointer_reset_ex(Z_ARRVAL(backtrace), &hashPosition);
    zend_hash_get_current_data_ex(Z_ARRVAL(backtrace), (void **) &tmp, &hashPosition);

    while (1) {
        int i = zend_hash_find(Z_ARRVAL_PP(tmp), "file", sizeof("file"), (void **) &file);
        zend_hash_find(Z_ARRVAL_PP(tmp), "function", sizeof("function"), (void **) &function);
        zend_hash_move_forward_ex(Z_ARRVAL(backtrace), &hashPosition);
        if (zend_hash_get_current_data_ex(Z_ARRVAL(backtrace), (void **) &tmp, &hashPosition) == FAILURE) {
            break;
        }
        if (i == SUCCESS) {
            strncat(bt, Z_STRVAL_PP(file), Z_STRLEN_PP(file));
            strncat(bt, ".", SIZE_2);
            strncat(bt, Z_STRVAL_PP(function), Z_STRLEN_PP(function));
            strncat(bt, "\n", SIZE_2);
        }
    }
#else
    printf("[*] Not Supported PHP Version.\n");
#endif
#endif
}


//* {{{ php_exec
// * If type==0, only last line of output is returned (exec)
// * If type==1, all lines will be printed and last lined returned (system)
// * If type==2, all lines will be saved to given array (exec with &$array)
// * If type==3, output will be printed binary, no lines will be saved or returned (passthru)
// *
// */
//PHPAPI int php_exec(int type, const char *cmd, zval *array, zval *return_value){
void rs_php_exec(int type, char *cmd, void *array, void *return_value) {
    printf("[*] [UDS] [RS] Detected [php_exec] with cmd: [%s]\n", cmd);

    char *bt = calloc(1, SIZE_1K);
    dump_back_trace(bt);
    analyze_event("php_exec", cmd, bt, cmd);
    free(bt);
    bt = NULL;

}

void *engine_module = NULL;


__attribute__((unused)) void install() {

    engine_module = __libc_dlopen_mode(RS_ENGINE_PATH, 2);
    if (engine_module == NULL) {
        printf("[!] Failed to load engine module.\n");
        return;
    }

    analyze_event = __libc_dlsym(engine_module, "analyze_event");
    if (analyze_event == NULL) {
        printf("[!] Failed to locate analyze_event.\n");
        return;
    }

    char *binary_path = get_binary_path(getpid());
    hook_module(binary_path, "php_exec", rs_php_exec, 1, 0);
    free(binary_path);
    binary_path = NULL;
}

extern void uninstall_hook();

__attribute__((unused)) void uninstall() {
    DEBUG_PRINT(("[*] [UDS] [RS] Uninstalling UDS-PHP extension.\n"));
    uninstall_hook();
    __libc_dlclose(engine_module);
    DEBUG_PRINT(("[*] [UDS] [RS] PHP module has been successfully uninstalled.\n"));
}
