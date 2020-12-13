let possibleSongs = [];
let possibleIndex = 0;

let metadataManager = {
    tryNext: function() {
        console.log("trying next song");
        if(possibleSongs.length - 1 == possibleIndex) {
            console.log("no more songs to try!");
            return;
        }
        // try next track
        ++possibleIndex;
        let song = possibleSongs[possibleIndex];
        document.getElementById("artist").innerHTML = song.artists[0].name;
        document.getElementById("songName").innerHTML = song.currentSong;
        document.getElementById("albumArt").setAttribute("src", song.images[0].uri)   
    },
    refresh: function(recognizedSongs) {
        possibleSongs = [];
        possibleIndex = -1;

        searchDiscogs(recognizedSongs)
        .then(filteredSongs => {
            // console.log(JSON.stringify(filteredSongs, null, 2));
            return getUniqueMasters(filteredSongs);
        })
        .then(arrayOfMasters => {
            console.log(JSON.stringify(arrayOfMasters, null, 2));
            possibleSongs = arrayOfMasters;
            console.log(JSON.stringify(possibleSongs, null, 2));

            // assume first song and update html
            this.tryNext();
        });
    }
};

async function searchDiscogs(recognizedSongs) {
    // refresh from new recognition data
    let discogsPromises = [];
    for(const [key, value] of Object.entries(recognizedSongs)) {
        discogsPromises.push(discogs.search(value.artists[0].name, value.song, false));
    };
    return Promise.all(discogsPromises);
};

async function getUniqueMasters(filteredSongs) {
    let masterIds = [];
    filteredSongs.forEach(songArray => {
        songArray.forEach(song => {
            if("master_id" in song) {
                if(!(song.master_id in song) && song.master_id != 0) { // some entries don't have masters
                    masterIds.push({id: song.master_id, song: song.currentSong}); // the master record won't know what track we're on so remember it
                }
            }
        });
    });
    console.log(JSON.stringify(masterIds, null, 2));
    return getDiscogsMasters(masterIds);
};

async function getDiscogsMasters(masters) {
    let discogsMasterPromises = [];
    masters.forEach(masterObject => {
        discogsMasterPromises.push(discogs.getMaster(masterObject.id));
    });
    return Promise.all(discogsMasterPromises)
            .then(arrayOfMasters => {
                console.log(JSON.stringify(arrayOfMasters[0], null, 2));
                for(let i = 0; i < arrayOfMasters.length; ++i) {
                    arrayOfMasters[i].currentSong = masters[i].song;
                };
                console.log(JSON.stringify(arrayOfMasters[0], null, 2));
                return arrayOfMasters;
            });
};