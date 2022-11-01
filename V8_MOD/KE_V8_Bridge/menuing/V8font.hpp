#pragma once

int fontpogs = 0; //size
vector<font> cfonts;

void GetFontFile(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handle_scope(isolate);

    int MID = args.Holder()->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "_id").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

    // cout << "setted was the font " << MID << endl;

    if(!args[0].IsEmpty() && args[0]->IsString()){
        v8::String::Utf8Value str(isolate, args[0]);
        const char* cstr = ToCString(str);
        cfonts[MID].craft(cstr);
    }
}

void FontConstructor( const v8::FunctionCallbackInfo<v8::Value>& args ) {
    v8::HandleScope handle_scope(isolate);
    v8::Persistent<v8::ObjectTemplate> font_templ;

    font fontcl;

    if(!args[0].IsEmpty() && args[0]->IsString()) {
        v8::String::Utf8Value str(isolate, args[0]);
        const char* cstr = ToCString(str);
        fontcl.craft(get_file(cstr));
        //cout << "wa " << cstr << endl;
        //cout << "setted was the model " << modecl.VBO << endl;
    }

    cfonts.push_back(fontcl);


    v8::EscapableHandleScope inner(isolate);
    v8::Local<v8::ObjectTemplate> local = v8::ObjectTemplate::New(isolate);
    local->Set(isolate, "_id", v8::Integer::New(isolate, fontpogs));
    local->Set(isolate, "GetFontFile", v8::FunctionTemplate::New(isolate, GetFontFile));
    fontpogs++;
    font_templ.Reset(isolate, inner.Escape(local));

        v8::Local<v8::Object> font_obj =
      v8::Local<v8::ObjectTemplate>::New(isolate, font_templ)
          ->NewInstance(isolate->GetCurrentContext())
          .ToLocalChecked();

        args.GetReturnValue().Set(font_obj);
}