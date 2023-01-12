#pragma once

void file_dialog( const v8::FunctionCallbackInfo<v8::Value>& args ) {
    v8::HandleScope handle_scope(isolate);

    string lefile = ask_user_file();
    lefile.pop_back();

    args.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, lefile.c_str()).ToLocalChecked());
}