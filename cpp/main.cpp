#include <napi.h>
#include "SoundRecorder.h"

void RecordWavFileNapi(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() < 1 || !info[0].IsNumber()) {
        Napi::TypeError::New(env, "Length of output WAV is required.").ThrowAsJavaScriptException();
    } 

    Napi::Number lengthOfSoundClipInSeconds = info[0].As<Napi::Number>();

    Napi::Function callback = info[1].As<Napi::Function>();
    SoundRecorder* soundRecorder = new SoundRecorder(callback, lengthOfSoundClipInSeconds.Int32Value());
    soundRecorder->Queue();

    // Napi::String returnValue = Napi::String::New(env, SoundRecorder::recordWavFile(lengthOfSoundClipInSeconds.Int32Value()));

}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set("recordWavFile", Napi::Function::New(env, RecordWavFileNapi));
    return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)
