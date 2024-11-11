import mido
from collections import deque


def getMessages(midi):

  currNotes_deque = deque()
  time = -1
  formattedNotes = []

  # Open the MIDI file
  mid = mido.MidiFile(midi)

  # Iterate over the messages in the file
  for msg in mid:
    #check first note
    if msg.type == 'note_on':
      if time == -1:
          time = 0
      else:
          #nothing should happen if immediate note.
          time += msg.time
      currNotes_deque.append([msg.note, time, -1])
    elif msg.type == 'note_off':
        #update time, nothing should happen if it is the same time as the previous.
        time += msg.time

        #first note in scope has now ended.
        if currNotes_deque[0][0] == msg.note:
          currNotes_deque[0][2] = time
          formattedNotes.append(currNotes_deque[0])
          currNotes_deque.popleft()

        #now check if the last not in scope has ended; this means that the latest note is the shortest note.
        elif currNotes_deque[-1][0] == msg.note:
          currNotes_deque[-1][2] = time
          formattedNotes.append(currNotes_deque[-1])
          currNotes_deque.pop()

        #if the note in scope is in the middle, we need to iterate through deque to find the one.
        else:
          idxLen = len(currNotes_deque)
          for i in range(idxLen):
            if currNotes_deque[0][0] == msg.note:
              currNotes_deque[0][2] = time
              formattedNotes.append(currNotes_deque[0])
              currNotes_deque.popleft()
            else:
              #append then pop front.
              currNotes_deque.append(currNotes_deque[0])
              currNotes_deque.popleft()
    else:
      continue

  formattedNotes.sort(key=lambda x: x[1])

  for i in range(len(formattedNotes)):

    start = formattedNotes[i][1]
    end = formattedNotes[i][2]

    start *= 1000
    end *= 1000


    formattedNotes[i][1] = int(start)
    formattedNotes[i][2] = int(end)
  return formattedNotes


def get_tempo_and_time_signature(midi_file):
    """Extracts tempo and time signature from a MIDI file."""

    mid = mido.MidiFile(midi_file)
    tempo = 500000  # Default tempo (microseconds per beat)
    time_signature = (4, 4)  # Default time signature
    ticks_per_beat = mid.ticks_per_beat

    for track in mid.tracks:
        for msg in track:
            if msg.type == 'set_tempo':
                tempo = msg.tempo
            if msg.type == 'time_signature':
                time_signature = (msg.numerator, msg.denominator)

    return tempo, time_signature, ticks_per_beat

#do we need this?
def clean_extraneous_notes(id_rec, id):
   return [x for x in id_rec if any(x[0] == y[0] for y in id)]