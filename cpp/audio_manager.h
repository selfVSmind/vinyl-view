#include <napi.h>

namespace audio_manager {

    std::string recordWavFile(int fileLengthInSeconds);
    Napi::String RecordWavFileNapi(const Napi::CallbackInfo& info);

    Napi::Object Init(Napi::Env env, Napi::Object exports);
}
