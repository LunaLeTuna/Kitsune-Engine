#pragma once

int awaz = 0; //size
vector<model> cmodels;

void SetModelOBJ(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handle_scope(isolate);

    int MID = args.Holder()->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "_id").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

    cout << "setted was the model " << MID << endl;

    if(!args[0].IsEmpty() && args[0]->IsString()){
        v8::String::Utf8Value str(isolate, args[0]);
        const char* cstr = ToCString(str);
        cmodels[MID].load_obj(get_file(cstr));
        cout << "fom model " << cmodels[MID].VBO << "\ndn "<< cstr << endl;
    }
}

/*
 * this is the models
 * TODO:
 * multi vertex poly to 3 vertex poly inator
 * giv'm bones
 * allow models to be generated and modified through Script
 */

void ModelConstructor( const v8::FunctionCallbackInfo<v8::Value>& args ) {
    v8::HandleScope handle_scope(isolate);
    v8::Persistent<v8::ObjectTemplate> model_templ;

    model modecl;

    modecl.name = "moodel "+to_string(awaz);

    if(!args[0].IsEmpty() && args[0]->IsString()) {
        v8::String::Utf8Value str(isolate, args[0]);
        const char* cstr = ToCString(str);
        modecl.load_obj(get_file(cstr));
        //cout << "wa " << cstr << endl;
        //cout << "setted was the model " << modecl.VBO << endl;
    }

    cmodels.push_back(modecl);


    v8::EscapableHandleScope inner(isolate);
    v8::Local<v8::ObjectTemplate> local = v8::ObjectTemplate::New(isolate);
    local->Set(isolate, "_id", v8::Integer::New(isolate, awaz));
    local->Set(isolate, "GetFromOBJ", v8::FunctionTemplate::New(isolate, SetModelOBJ));
    awaz++;
    model_templ.Reset(isolate, inner.Escape(local));

        v8::Local<v8::Object> model_obj =
      v8::Local<v8::ObjectTemplate>::New(isolate, model_templ)
          ->NewInstance(isolate->GetCurrentContext())
          .ToLocalChecked();

        args.GetReturnValue().Set(model_obj);
}