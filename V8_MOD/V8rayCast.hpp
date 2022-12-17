#ifdef Include_physics
void RayConstruct( const v8::FunctionCallbackInfo<v8::Value>& args ) {

    v8::HandleScope handle_scope(isolate);

    float xp1 = args[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "x").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

    float yp1 = args[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "y").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

    float zp1 = args[0]->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "z").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

    float xp2 = args[1]->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "x").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

    float yp2 = args[1]->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "y").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

    float zp2 = args[1]->ToObject(isolate->GetCurrentContext()).ToLocalChecked()->Get(context, v8::String::NewFromUtf8(isolate, "z").ToLocalChecked()).ToLocalChecked()->NumberValue(isolate->GetCurrentContext()).FromJust();

    btCollisionWorld::ClosestRayResultCallback res(btVector3(xp1,yp1,zp1), btVector3(xp2,yp2,zp2));

    dynamicWorld->rayTest(btVector3(xp1,yp1,zp1), btVector3(xp2,yp2,zp2), res);
    v8::Local<v8::Object> Raycast_obj = v8::Object::New(isolate);

    if(res.hasHit()){
//         printf("Collision at: <%.2f, %.2f, %.2f>\n", res.m_hitPointWorld.getX(), res.m_hitPointWorld.getY(), res.m_hitPointWorld.getZ());

        Raycast_obj->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, "hasHit").ToLocalChecked(), v8::Boolean::New(isolate, 1));

        Raycast_obj->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, "position").ToLocalChecked(), quick_create_vector3(glm::vec3(res.m_hitPointWorld.getX(), res.m_hitPointWorld.getY(), res.m_hitPointWorld.getZ())));
    
        Raycast_obj->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, "object").ToLocalChecked(), v8::Number::New(isolate, res.m_collisionObject->getUserIndex()));
    }else{
        Raycast_obj->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, "hasHit").ToLocalChecked(), v8::Boolean::New(isolate, 0));
    }

    args.GetReturnValue().Set(Raycast_obj);
}
#endif