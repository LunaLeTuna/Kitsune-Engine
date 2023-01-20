mkdir v8_workspace
cd v8_workspace
git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git
export PATH=`pwd`/depot_tools:$PATH
gclient
mkdir v8
cd v8
fetch v8
cd v8
git pull && gclient sync
tools/dev/gm.py x64.release

cd out/x64.release

echo dcheck_always_on = false &> args.gn
echo is_component_build = false >> args.gn
echo is_debug = false >> args.gn
echo target_cpu = \"x64\" >> args.gn
echo use_custom_libcxx = false >> args.gn
echo v8_monolithic = true >> args.gn
echo v8_use_external_startup_data = false >> args.gn
echo v8_enable_pointer_compression = false >> args.gn

cd ../..

ninja -C out/x64.release