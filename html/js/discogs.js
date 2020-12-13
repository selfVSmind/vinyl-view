let discogs = {
    search: async function(artist, track, vinylOnly) {
        return discogsFetch("https://api.discogs.com/database/search?"
                                + "q=" + track
                                + " " + artist
                                // don't search album, what if user is listening to a compilation?
                                // + "&album=" + album
                                // + vinylOnly == true ? "&format=vinyl" : ""
                                + "&token=" + PrivateKeys.discogs.token)
                .then(data => {
                    let results = data.results;
                    results.forEach(result => {
                        result.currentSong = track;
                    });
                    return results;
                });
    },
    getMaster: async function(masterId) {
        return discogsFetch("https://api.discogs.com/masters/"
                                + masterId + "?"
                                + "&token=" + PrivateKeys.discogs.token);
    }
};

async function discogsFetch(discogsUri) {
    let discogsSearch = encodeURI(discogsUri);
    console.log(discogsSearch);

    return fetch(discogsSearch)
    .then(data => {return data.json()})
    .then(data => {
        console.log(JSON.stringify(data, null, 2));
        return data;
    });
};
