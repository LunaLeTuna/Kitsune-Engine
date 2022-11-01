#pragma once

void Print(const v8::FunctionCallbackInfo<v8::Value>& args) {
    bool first = true;
    for (int i = 0; i < args.Length(); i++) {
        v8::HandleScope handle_scope(isolate);
        if (first) {
        first = false;
        } else {
        printf(" ");
        }
        v8::String::Utf8Value str(isolate, args[i]);
        const char* cstr = ToCString(str);
        printf("%s", cstr);
    }
    printf("\n");
    fflush(stdout);
}