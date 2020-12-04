window.onload = function() {
   AudioManager.recordWavFile(10, callback);
}

function callback() {
    AudioManager.identifySong('recorded_in_cpp_using_pulse.wav', secondCallback);
}

function secondCallback(err, httpResponse, body) {
    if (err) console.log(err);
    body = JSON.parse(body);
    let selection = body.metadata.music[0];
    let artist = selection.artists[0].name;
    let album = selection.album.name;

    document.getElementById("artist").innerHTML = artist;
    document.getElementById("album").innerHTML = album;

    const discogsSearch = "https://api.discogs.com/database/search?q="
                            + artist.replace(" ", "%20")
                            + "%20"
                            + album.replace(" ", "%20")
                            + "&token="
                            + PrivateKeys.discogs.token;

    console.log(discogsSearch);
    fetch(discogsSearch)
    .then(data => {return data.json()})
    .then(data => {
        document.getElementById("albumArt").setAttribute("src", data.results[0].cover_image)});
    // .then(data=>{console.log(JSON.stringify(data))});
}
