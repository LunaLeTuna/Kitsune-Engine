use std::time::{Instant, Duration};

use boa_engine::{Context, JsResult, JsValue, JsArgs, Source, property::Attribute, NativeFunction, value::TryFromJs, JsNativeError};
use boa_runtime::Console;
use futures_util::Future;

use crate::fs_system::grab;

pub struct ScriptSpace<'a> {
    pub world: i16,
    context: boa_engine::Context<'a>,
    scripts: Vec<(String,String)> //actuall code, then it's file location
}

#[derive(Debug, TryFromJs)]
#[allow(dead_code)]
struct TestStruct {
    inner: bool,
    hello: String,
    // You can override the conversion of an attribute.
    #[boa(from_js_with = "lossy_conversion")]
    my_float: i16,
}

fn awawawa(_: &JsValue, args: &[JsValue], ctx: &mut Context<'_>) -> JsResult<JsValue> {
    let arg = args.get(0).unwrap();

    // BUG: Dev branch seems to be passing string arguments along with quotes
    let libfile = arg
        .to_string(ctx)
        .expect("Failed to convert to string")
        .to_std_string_escaped();

        println!(
            "{}",
            libfile
        );

        Ok(JsValue::String("meow :3".into()))
}

fn delay(
    _this: &JsValue,
    args: &[JsValue],
    context: &mut Context<'_>,
) -> impl Future<Output = JsResult<JsValue>> {
    let millis = args.get_or_undefined(0).to_u32(context);

    async move {
        let millis = millis?;
        println!("Delaying for {millis} milliseconds ...");
        let now = Instant::now();
        smol::Timer::after(Duration::from_millis(millis as u64)).await;
        let elapsed = now.elapsed().as_secs_f64();
        Ok(elapsed.into())
    }
}


/// Converts the value lossly
fn lossy_conversion(value: &JsValue, _context: &mut Context) -> JsResult<i16> {
    match value {
        JsValue::Rational(r) => Ok(r.round() as i16),
        JsValue::Integer(i) => Ok(*i as i16),
        _ => Err(JsNativeError::typ()
            .with_message("cannot convert value to an i16")
            .into()),
    }
}

impl ScriptSpace<'_> {

    pub fn new() -> ScriptSpace<'static>{

        // Instantiate the execution context
        let mut context: Context<'_> = Context::default();

        // Initialize the Console object.
        let console = Console::init(&mut context);

        // Register the console as a global property to the context.
        context
            .register_global_property(Console::NAME, console, Attribute::all())
            .expect("the console object shouldn't exist yet");

        context.register_global_callable("awawawa", 0, NativeFunction::from_fn_ptr(awawawa))
            .unwrap();

        match context.eval(Source::from_bytes(include_str!("KE/ke_api.js"))) {
            Ok(res) => {
                
            }
            Err(e) => {
                // Pretty print the error
                eprintln!("Uncaught {e}");
            }
        };

        ScriptSpace {
            world: 0,
            context,
            scripts: Vec::new(),
        }
    }

    pub fn add_script(&mut self, location: String){
        let js_code: String = grab(&location);

        self.scripts.push((js_code, location))
    }

    pub fn run(&mut self){

        for code in &self.scripts {
            // Parse the source code
            match self.context.eval(Source::from_bytes(&code.0)) {
                Ok(res) => {
                    
                }
                Err(e) => {
                    // Pretty print the error
                    eprintln!("Uncaught {e}");
                }
            };
        }
    }

    pub fn job_runs(&mut self){
        match self.context.eval(Source::from_bytes("loop()")) {
            Ok(res) => {
                // println!(
                //     "{}",
                //     res.to_string(&mut self.context).unwrap().to_std_string_escaped()
                // );
            }
            Err(e) => {
                // Pretty print the error
                eprintln!("Uncaught {e}");
            }
        };
    }
}