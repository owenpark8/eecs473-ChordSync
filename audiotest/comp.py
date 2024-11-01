from mido import MidiFile
from difflib import SequenceMatcher

def midi_to_event_string(midi_path):
	midi = MidiFile(midi_path)
	events = []
	
	for track in midi.tracks:
		for msg in track:
			if not msg.is_meta and msg.type in ['note_on']:
				events.append(f"{msg.type}_{msg.note}")
	return ' '.join(events)
	
"""for track in midi.tracks:
		for msg in track:
			#if not msg.is_meta and msg.type in ['note_on', 'note_off']:
			if not msg.is_meta and msg.type in ['note_on']:
				#events.append(f"{msg.type}_{msg.note}_{msg.time}")
			events.append(f"{msg.type}_{msg.note}")
	return ' '.join(events)"""

content = midi_to_event_string("turkish_basic_pitch.mid")
content2 = midi_to_event_string("turkish_march_variations.mid")

"""with open("turkish_basic_pitch.mid", 'r', encoding="utf-8") as f:
	content=f.read()
with open("turkish_march_variations.mid", 'r', encoding="utf-8") as f2:
	content2=f2.read()"""

m = SequenceMatcher(None, content, content2)
print(m.ratio())
