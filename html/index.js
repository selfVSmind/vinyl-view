window.onload = function() {
    document.body.onkeydown = keyDownEventHandler;

    AudioManager.recordWavFile(10, callback);
    // callback();
}

function keyDownEventHandler(e) {
    if(e.key == 'n') metadataManager.tryNext();
}

function callback() {
    AudioManager.identifySong('recorded_in_cpp_using_pulse.wav', secondCallback);
}

function secondCallback(err, httpResponse, body) {
    if (err) console.log(err);

    body = JSON.parse(body);
    console.log(JSON.stringify(body, null, 2));

    if(body.status.code == 1001) {
        AudioManager.recordWavFile(10, callback);
        return;
    }

    let recognizedSongs = body.metadata.music;
    let cleanRecognizedSongs = {};
    recognizedSongs.forEach(song => {
        let key = song.artists[0].name + song.title;
        if(!(key in recognizedSongs)) {
            cleanRecognizedSongs[key] = {
                artists: song.artists,
                song: song.title,
                offset: song.play_offset_ms,
                length: song.duration_ms
            }
        }
                
    });

    console.log(JSON.stringify(cleanRecognizedSongs, null, 2));

    metadataManager.refresh(cleanRecognizedSongs);
}
