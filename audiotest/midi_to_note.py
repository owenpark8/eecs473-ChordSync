from mido import MidiFile
from difflib import SequenceMatcher
import librosa

def midi_to_note_name(midi_note_number):
	note_names = ["C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"]
	
	octave = (midi_note_number // 12) - 1
	note_index = midi_note_number % 12
	
	#print(f"{note_names[note_index]}{octave}\n")
	return f"{note_names[note_index]}{octave}"""
	
def midi_to_event_string(midi_path, option):
	midi = MidiFile(midi_path)
	events = []
	for track in midi.tracks:
		for msg in track:
			if not msg.is_meta and msg.type in ['note_on']:
			#if not msg.is_meta and msg.type in ['note_on', 'note_off']:
				if(option == 1):
					events.append(midi_to_note_name(msg.note))
				else:
					events.append(librosa.midi_to_note(msg.note))
	return events


content = midi_to_event_string("interstellar_basic_pitch_aligned.mid", 1)
content2 = midi_to_event_string("INTERSTELLARA.mid", 1)
content3 = midi_to_event_string("interstellar_basic_pitch.mid", 1)

#content_librosa = midi_to_event_string("interstellar_basic_pitch_aligned.mid", 0)
#content2_librosa = midi_to_event_string("INTERSTELLARA.mid", 0)

#print(content)
m = SequenceMatcher(None, content[0:1383], content2)
print("The ratio of midi recording and actual midi changed audio\n")
print(m.ratio())

m_1 = SequenceMatcher(None, content2, content3)
print("The ratio of midi recording and actual midi\n")
print(m_1.ratio())

print(len(content))
print(len(content2))
print(len(content3))

"""m_librosa = SequenceMatcher(None, content, content_librosa)
print("\nThe ratio of manual and librosa of recording\n")
print(m_librosa.ratio())

m_librosa_midi = SequenceMatcher(None, content2, content2_librosa)
print("\nThe ratio of manual and librosa of recording\n")
print(m_librosa_midi.ratio())

print(len(content))
print(len(content2))"""

def find_first_match_with_indices(list1, list2):
	for index1, element in enumerate(list1):
		index2 = list2.index(element)
		return element, index1, index2
	return None, None, None
	
"""element, index1, index2 = find_first_match_with_indices(content[0:1383], content2)
print(f"First matching element: {element}")
print(f"Index in list1: {index1}")
print(f"Index in list2: {index2}")
"""
element, index1, index2 = find_first_match_with_indices(content2, content3[0:1383])
print(f"First matching element: {element}")
print(f"Index in list1: {index1}")
print(f"Index in list2: {index2}")


print(content[0:30])
print(content2[0:30])
