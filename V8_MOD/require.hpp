#pragma once

void V8Require(const v8::FunctionCallbackInfo<v8::Value>& args) {

    v8::HandleScope handle_scope(isolate);
    // v8::Context::Scope context_scope(context);

    v8::Local<v8::Context> contextre = v8::Context::New(isolate, NULL, global);

    v8::String::Utf8Value str(isolate, args[0]);
    const char* cstr = ToCString(str);

    string raw_re_js = get_file(cstr);
    v8::Local<v8::String> source_re = v8::String::NewFromUtf8(isolate, raw_re_js.c_str(), v8::NewStringType::kNormal).ToLocalChecked();

    v8::Local<v8::Script> script_re = v8::Script::Compile(contextre, source_re).ToLocalChecked();

    // Run the script to get the result.
    v8::Local<v8::Value> resultz_re = script_re->Run(contextre).ToLocalChecked();

    v8::Local<v8::Value> Export_varuables = contextre->Global()->Get(contextre, v8::String::NewFromUtf8(isolate, "exports").ToLocalChecked()).ToLocalChecked();

    args.GetReturnValue().Set(Export_varuables);
}