#pragma once

#ifdef Include_console_interaction
void run_cmd(const v8::FunctionCallbackInfo<v8::Value>& args){
    v8::HandleScope handle_scope(isolate);

    v8::String::Utf8Value str(isolate, args[0]);

    string c = ToCString(str);

    FILE *fp,*outputfile;
    char var[40];

    string last = "";

    fp = popen(ToCString(str), "r");
    while (fgets(var, sizeof(var), fp) != NULL)
        {
        last+=var;
        }
    pclose(fp);

    const char *cv = last.c_str();
    args.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, cv).ToLocalChecked());

    fflush(stdout);
}
#endif