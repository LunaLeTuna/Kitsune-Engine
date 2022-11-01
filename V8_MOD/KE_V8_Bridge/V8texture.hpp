#pragma once

int away = 0; //size
vector<texture> ctextures;

void SetTextureFile(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handle_scope(isolate);

    int MID = args.Holder()->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "_id").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

    if(!args[0].IsEmpty() && args[0]->IsString()){
        v8::String::Utf8Value str(isolate, args[0]);
        const char* cstr = ToCString(str);
        ctextures[MID].load((char*)cstr);
    }
}

void TextureConstructor( const v8::FunctionCallbackInfo<v8::Value>& args ) {
    v8::HandleScope handle_scope(isolate);
    v8::Persistent<v8::ObjectTemplate> texture_templ;

    texture texturecl;

    texturecl.flipped = false;

    texturecl.filtering = 1;

    texturecl.name = "texture_"+to_string(away);

    if(!args[0].IsEmpty() && args[0]->IsString()) {
        v8::String::Utf8Value str(isolate, args[0]);
        const char* cstr = ToCString(str);
        texturecl.load((char*)cstr);
    }

    ctextures.push_back(texturecl);


    v8::EscapableHandleScope inner(isolate);
    v8::Local<v8::ObjectTemplate> local = v8::ObjectTemplate::New(isolate);
    local->Set(isolate, "_id", v8::Integer::New(isolate, away));
    local->Set(isolate, "GetFromFile", v8::FunctionTemplate::New(isolate, SetTextureFile));
    away++;
    texture_templ.Reset(isolate, inner.Escape(local));

        v8::Local<v8::Object> texture_obj =
      v8::Local<v8::ObjectTemplate>::New(isolate, texture_templ)
          ->NewInstance(isolate->GetCurrentContext())
          .ToLocalChecked();

        args.GetReturnValue().Set(texture_obj);
}