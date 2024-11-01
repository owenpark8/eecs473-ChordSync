from mido import MidiFile
from difflib import SequenceMatcher

def midi_to_note_name(midi_note_number):
	note_names = ["C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"]
	
	octave = (midi_note_number // 12) - 1
	note_index = midi_note_number % 12
	
	#print(f"{note_names[note_index]}{octave}\n")
	return f"{note_names[note_index]}{octave}"
	

def midi_to_event_string(midi_path):
	midi = MidiFile(midi_path)
	events = []
	for track in midi.tracks:
		for msg in track:
			if not msg.is_meta and msg.type in ['note_on']:
				events.append(midi_to_note_name(msg.note))
	return events


content = midi_to_event_string("interstellar_basic_pitch.mid")
content2 = midi_to_event_string("INTERSTELLARA.mid")

#print(content)
m = SequenceMatcher(None, content, content2)
print(m.ratio())

print(len(content))
print(len(content2))

def find_first_match_with_indices(list1, list2):
	for index1, element in enumerate(list1):
		index2 = list2.index(element)
		return element, index1, index2
	return None, None, None
	
element, index1, index2 = find_first_match_with_indices(content[0:1383], content2)
print(f"First matching element: {element}")
print(f"Index in list1: {index1}")
print(f"Index in list2: {index2}")


print(content[0:30])
print(content2[0:30])
