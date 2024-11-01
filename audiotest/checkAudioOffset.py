import pretty_midi

# Load MIDI files
midi1 = pretty_midi.PrettyMIDI('interstellar_basic_pitch.mid')
midi2 = pretty_midi.PrettyMIDI('INTERSTELLARA.mid')
midi3 = pretty_midi.PrettyMIDI('interstellar_basic_pitch_aligned.mid')

print(midi1.instruments)
print(midi2.instruments)
print(midi3.instruments)

# Get the start time of the first note in each file
first_note_time_midi1 = min(note.start for instrument in midi1.instruments for note in instrument.notes)
first_note_time_midi2 = min(note.start for instrument in midi2.instruments for note in instrument.notes)
first_note_time_midi3 = min(note.start for instrument in midi3.instruments for note in instrument.notes)


print(first_note_time_midi1)
print(first_note_time_midi2)
print(first_note_time_midi3)

# Calculate the offset
timing_offset = first_note_time_midi2 - first_note_time_midi1

print(f"\nDetected timing offset: {timing_offset} seconds")


"""#midi 1 starts at a later time
if(timing_offset < 0):
	for note in midi2.instruments[0].notes:
		note.start += timing_offset
		note.end += timing_offset
	midi2.write('INTERSTELLARA_aligned.mid')
elif(timing_offset > 0):
	for note in midi1.instruments[0].notes:
		note.start += timing_offset
		note.end += timing_offset
	midi1.write('interstellar_basic_pitch_aligned.mid')
else:
	print("no changes")"""
