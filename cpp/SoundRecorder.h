#include <napi.h>

#include "fstream"

#define BUF_SIZE 1000

#define DEFAULT_CLIP_LENGTH 10
#define SAMPLE_RATE 44100
#define NUM_CHANNELS 1
#define BITS_PER_SAMPLE 16

class SoundRecorder : public Napi::AsyncWorker {

    public:

        std::string recordWavFile(int fileLengthInSeconds);

        SoundRecorder(Napi::Function& callback, int lengthOfSoundClipInSeconds);
        // virtual ~SoundRecorder() {};



        // Napi::String RecordWavFileNapi(const Napi::CallbackInfo& info);

        // Napi::Object Init(Napi::Env env, Napi::Object exports);

        void Execute(); //override
        void OnOK(); //override
    
    private:
        int lengthOfSoundClipInSeconds;
        int sampleRate = SAMPLE_RATE;
        int numChannels = NUM_CHANNELS;
        int bitsPerSample = BITS_PER_SAMPLE;

        std::ofstream *outfile;

        void openFile();
        void writeFile(char *buffer, int bufferSize);
        void closeFile();
        void writeFileHeader();
};
