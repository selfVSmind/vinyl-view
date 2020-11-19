/***
  This file is part of PulseAudio.
  PulseAudio is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published
  by the Free Software Foundation; either version 2.1 of the License,
  or (at your option) any later version.
  PulseAudio is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  General Public License for more details.
  You should have received a copy of the GNU Lesser General Public License
  along with PulseAudio; if not, see <http://www.gnu.org/licenses/>.
***/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <pulse/simple.h>
#include <pulse/error.h>

#include "iostream"
#include "fstream"

#define BUFSIZE 1000

using namespace std;

ofstream *outfile;

int sampleRate = 44100;
int fileLengthInSeconds = 3;
int numChannels = 1;
int bitsPerSample = 16;

int numberOfWrites = sampleRate * fileLengthInSeconds * numChannels * bitsPerSample / 8 / BUFSIZE;

void openFile() {
   	outfile = new ofstream("recorded_in_cpp_using_pulse.wav", ios::out | ios::binary);
}

void closeFile() {
   	outfile->close();
}

// The Raspberry Pi 4 is a little endian machine
// This code does not even attempt to work on big endian cpus
// Hopefully it's not an issue later
void writeFileHeader() {
    int numSamples = sampleRate * fileLengthInSeconds;
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

void writeFile(char *buffer, int bufferSize) {
	outfile->write(buffer,bufferSize);
}


/* A simple routine calling UNIX write() in a loop */
static ssize_t loop_write(int fd, const void*data, size_t size) {
    ssize_t ret = 0;
    while (size > 0) {
        ssize_t r;
        if ((r = write(fd, data, size)) < 0)
            return r;
        if (r == 0)
            break;
        ret += r;
        data = (const uint8_t*) data + r;
        size -= (size_t) r;
    }
    return ret;
}
int main(int argc, char*argv[]) {
    /* The sample type to use */
    static const pa_sample_spec ss = {
        .format = PA_SAMPLE_S16LE,
        .rate = (uint32_t)sampleRate,
        .channels = (uint8_t)numChannels
    };
    pa_simple *s = NULL;
    int ret = 1;
    int error;
    /* Create the recording stream */
    if (!(s = pa_simple_new(NULL, argv[0], PA_STREAM_RECORD, NULL, "record", &ss, NULL, NULL, &error))) {
        fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
        goto finish;
    }

    openFile();
    writeFileHeader();

    for (int i = 0; i < numberOfWrites; ++i) {
        uint8_t buf[BUFSIZE];
        /* Record some data ... */
        if (pa_simple_read(s, buf, sizeof(buf), &error) < 0) {
            fprintf(stderr, __FILE__": pa_simple_read() failed: %s\n", pa_strerror(error));
            goto finish;
        }

        // and write it to file
        writeFile((char*)buf, sizeof(buf));

        /* And write it to STDOUT */
        // if (loop_write(STDOUT_FILENO, buf, sizeof(buf)) != sizeof(buf)) {
        //     fprintf(stderr, __FILE__": write() failed: %s\n", strerror(errno));
        //     goto finish;
        // }
    }

    closeFile();

    ret = 0;
finish:
    if (s)
        pa_simple_free(s);
    return ret;
}
