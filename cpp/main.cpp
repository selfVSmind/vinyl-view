#include <napi.h>
#include "audio_manager.h"

Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
  return audio_manager::Init(env, exports);
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, InitAll)
