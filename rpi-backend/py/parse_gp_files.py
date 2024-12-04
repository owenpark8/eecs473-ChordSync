import sys
import guitarpro

DEBUG = False

def debug_print(message):
    if DEBUG:
        print(message)
# array to convert string number to midi note value
# High E (64) to low E (40)
string_dict = [64, 59, 55, 50, 45, 40]

# parses gpx file and then outputs vector of ints [[start_timestamp_ms, length_ms, note value, int fret, int string]]
# fret == 0 means open string
def parse_gp_file(gp_file_path):
    song = guitarpro.parse(gp_file_path)

    debug_print(f"Available tracks:")
    debug_print(f"Song tempo: {song.tempo}")
    for track in song.tracks:
        debug_print(f"Track {track.number}: {track.name}")
    debug_print("----------------------------------------------") # empty line
    song_info_out = []
    track = song.tracks[0] # the first track is usually the correct one / the main part
    timestamp = 0.0
    for measure in track.measures:
        for voice in measure.voices:
            for beat in voice.beats:
                # Iterate over notes in the beat
                for note in beat.notes:
                    midi_note_val = string_dict[note.string-1] + note.value # string is 1-indexed
                    fret_number = note.value
                    string_number = note.string - 1 # string is 1-indexed
                    length = beat.duration.time
                    _, _, note_type = str(note.type).partition('.')

                    debug_print(f"Start timestamp: {timestamp:.2f} ms, Length: {length}, MidiNote val: {midi_note_val}, Fret: {fret_number}, String: {string_number}, NoteType: {note_type}")
                    song_info_out.append([timestamp, length, midi_note_val, fret_number, string_number])

                # Increment timestamp
                timestamp += beat.duration.time
    return song_info_out

def get_song_tempo(gp_file_path):
    song = guitarpro.parse(gp_file_path)
    return song.tempo

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print("Usage: python read_notes.py <path_to_gp_file>")
    else:
        gp_file_path = sys.argv[1]
        parse_gp_file(gp_file_path)
