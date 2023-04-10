use std::rc::Rc;

use deno_core::{
    anyhow, resolve_path,
    v8::{self, Local, Value},
    FsModuleLoader, JsRuntime, RuntimeOptions,
};

async fn run_js(file_path: &str) -> anyhow::Result<()> {
    let mut js_runtime = JsRuntime::new(RuntimeOptions {
        module_loader: Some(Rc::new(FsModuleLoader)),
        ..Default::default()
    });

    let main_module = resolve_path(file_path)?;

    let module_id = js_runtime.load_main_module(&main_module, None).await?;
    let result = js_runtime.mod_evaluate(module_id);

    js_runtime.run_event_loop(false).await?;

    println!("loaded result {:?}", result.await);

    let namespace = js_runtime.get_module_namespace(module_id)?;

    let scope = &mut js_runtime.handle_scope();

    let module_object = v8::Local::<v8::Object>::new(scope, namespace);

    let module_properties = module_object
        .get_property_names(scope, Default::default())
        .unwrap();

    println!(
        "module properties: {}",
        module_properties.to_rust_string_lossy(scope)
    );

    let export_fn_name = v8::String::new(scope, "tick").unwrap();

    dbg!(export_fn_name.to_rust_string_lossy(scope));

    let export_fn = module_object
        .get(scope, export_fn_name.into())
        .expect("couldnt find fn");

    let function = v8::Local::<v8::Function>::try_from(export_fn)?;

    let empty: &[Local<Value>] = &[];
    let recv: Local<Value> = module_object.into();

    loop {
        let scope = &mut v8::HandleScope::new(scope);

        function.call(scope, recv, empty);
    }
}

#[tokio::main]
async fn main() {
    run_js("./scripts/index.js").await.expect("run failed");
}