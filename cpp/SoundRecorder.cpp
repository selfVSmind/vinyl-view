/***
 * This file has been completely reworked but I started with the following example code
 * http://0pointer.de/lennart/projects/pulseaudio/doxygen/parec-simple_8c-example.html
 * 
 * 
 * All changes from that original file have been done by Jason Lambert
***/

#include "SoundRecorder.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <pulse/simple.h>
#include <pulse/error.h>

#include "iostream"

SoundRecorder::SoundRecorder(Napi::Function& callback, int duration)
    : AsyncWorker(callback), lengthOfSoundClipInSeconds(duration), sampleRate(SAMPLE_RATE), numChannels(NUM_CHANNELS), bitsPerSample(BITS_PER_SAMPLE) {};

void SoundRecorder::OnOK() {
    std::string msg = "SimpleAsyncWorker returning after 'working' seconds.";
    Callback().Call({Env().Null(), Napi::String::New(Env(), msg)});
}

void SoundRecorder::openFile() {
   	outfile = new std::ofstream("recorded_in_cpp_using_pulse.wav", std::ios::out | std::ios::binary);
}

void SoundRecorder::closeFile() {
   	outfile->close();
}

void SoundRecorder::writeFile(char *buffer, int bufferSize) {
	outfile->write(buffer,bufferSize);
}

// The Raspberry Pi 4 is a little endian machine
// This code does not even attempt to work on big endian cpus
// Hopefully it's not an issue later
void SoundRecorder::writeFileHeader() {
    int numSamples = sampleRate * lengthOfSoundClipInSeconds;
    int subChunk2Size = numSamples * numChannels * bitsPerSample / 8;

    outfile->write("RIFF", sizeof(char) * 4);
    // 4 bytes for chunk size
    int chunkSize = 36 + subChunk2Size;
    outfile->write(reinterpret_cast<const char *>(&chunkSize), sizeof(chunkSize));
    outfile->write("WAVE", sizeof(char) * 4);
    outfile->write("fmt ", sizeof(char) * 4);
    const char subChunkSize[] = {0x10, 0x00, 0x00, 0x00};
    outfile->write(subChunkSize, sizeof(char) * 4);
    // 2 bytes for audio format (PCM = "0x01 0x00")
    const char audioFormat[] = {0x01, 0x00};
    outfile->write(audioFormat, sizeof(char) * 2);
    // 2 bytes for number of channels
    outfile->write(reinterpret_cast<const char *>(&numChannels), 2);
    // Four bytes for sample rate
    outfile->write(reinterpret_cast<const char *>(&sampleRate), sizeof(sampleRate));
    // Four bytes for byte rate
    int byteRate = sampleRate * numChannels * bitsPerSample / 8;
    outfile->write(reinterpret_cast<const char *>(&byteRate), sizeof(byteRate));
    // 2 bytes for block align
    int blockAlign = numChannels * bitsPerSample / 8;
    outfile->write(reinterpret_cast<const char *>(&blockAlign), 2);
    // 2 bytes for bits per sample
    outfile->write(reinterpret_cast<const char *>(&bitsPerSample), 2);
    outfile->write("data", sizeof(char) * 4);
    // 4 bytes for sub chunk 2 size
    outfile->write(reinterpret_cast<const char *>(&subChunk2Size), sizeof(subChunk2Size));
}

// int main(int argc, char*argv[]) {
// std::string SoundRecorder::recordWavFile(int lengthOfSoundClipInSeconds) {
void SoundRecorder::Execute() {

    // How many times we have to call writeFile() to satisfy the file length
    int numberOfWrites = sampleRate * lengthOfSoundClipInSeconds * numChannels * bitsPerSample / 8 / BUF_SIZE;

    /* The sample type to use */
    static const pa_sample_spec ss = {
        .format = PA_SAMPLE_S16LE,
        .rate = (uint32_t)sampleRate,
        .channels = (uint8_t)numChannels
    };
    pa_simple *s = NULL;

    int error;
    /* Create the recording stream */
    // if (!(s = pa_simple_new(NULL, argv[0], PA_STREAM_RECORD, NULL, "record", &ss, NULL, NULL, &error))) {
    if (!(s = pa_simple_new(NULL, "stream_name", PA_STREAM_RECORD, NULL, "record", &ss, NULL, NULL, &error))) {
        fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
        goto finish;
    }

    openFile();
    writeFileHeader();

    for (int i = 0; i < numberOfWrites; ++i) {
        uint8_t buf[BUF_SIZE];
        /* Record some data ... */
        if (pa_simple_read(s, buf, sizeof(buf), &error) < 0) {
            fprintf(stderr, __FILE__": pa_simple_read() failed: %s\n", pa_strerror(error));
            goto finish;
        }

        // and write it to file
        writeFile((char*)buf, sizeof(buf));
    }

    closeFile();

finish:
    if (s)
        pa_simple_free(s);
    // return "We've got ourselves a wav file!!";
}
