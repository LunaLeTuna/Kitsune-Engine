#pragma once

void readFileSync( const v8::FunctionCallbackInfo<v8::Value>& args ) {
    v8::HandleScope handle_scope(isolate);

    if (args[0]->IsString()) {

        v8::String::Utf8Value str(isolate, args[0]);
        const char* cstr = ToCString(str);

        string rawline = get_file(cstr);

        args.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, rawline.c_str(), v8::NewStringType::kNormal).ToLocalChecked());
    }

}
