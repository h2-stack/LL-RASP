//
// Created by zhuonan li on 1/30/22.
//

// Thus, for an addon to remain ABI-compatible across Node.js major versions,
// it must use Node-API exclusively by restricting itself to using
#include <node_api.h>
#include <v8.h>
#include <cstring>
#include <iostream>
#include "../rs_header.h"


extern "C" void dump_back_trace(char *bt) {
    v8::Isolate *isolate = v8::Isolate::GetCurrent();
    v8::Local<v8::StackTrace> st = v8::StackTrace::CurrentStackTrace(isolate, SIZE_256, v8::StackTrace::kOverview);
    char tmp_line[SIZE_256];
    for (uint32_t i = 0; i < st->GetFrameCount(); i++) {

#if NAPI_VERSION >= 4
        v8::Local<v8::StackFrame> frame = st->GetFrame(isolate, i);
        v8::String::Utf8Value scriptName(isolate, frame->GetScriptName());
        v8::String::Utf8Value functionName(isolate, frame->GetFunctionName());
#else
        v8::Local<v8::StackFrame> frame = st->GetFrame(i);
        v8::String::Utf8Value scriptName(frame->GetScriptName());
        v8::String::Utf8Value functionName(frame->GetFunctionName());
#endif

        if (*functionName != nullptr) {
            memset(tmp_line, 0, SIZE_256);
            snprintf(tmp_line, SIZE_256, "at %s (%s:%d:%d)\n", *functionName, *scriptName, frame->GetLineNumber(),
                     frame->GetColumn());
            strncat(bt, tmp_line, SIZE_256);
        }

    }
}
