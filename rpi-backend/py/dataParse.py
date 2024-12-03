import mido
from collections import deque


"""def getMessages(midi):

  currNotes_deque = deque()
  time = -1
  formattedNotes = []

  # Open the MIDI file
  mid = mido.MidiFile(midi)
  
  # Iterate over the messages in the file
  for msg in mid:
    if msg.type == 'note_on' and (msg.velocity > 0 or time == -1):
      if time == -1:
          time = 0
      else:
          #nothing should happen if immediate note.
          time += msg.time
      currNotes_deque.append([msg.note, time, -1])
    elif msg.type=='pitchwheel':
        #pitchwheel noise, so just add time
        #pitchwheel could be first.
        if time == -1:
          time = 0
        else:
          time += msg.time
    elif msg.type == 'note_off' or (msg.type == 'note_on' and msg.velocity == 0):
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


    
  #return formattedNotes
  

  unique_list = []
  for item in formattedNotes:
    if item not in unique_list:
        unique_list.append(item)

  return unique_list"""



"""def getMessages(midi):
    currNotes_deque = deque()
    time = -1
    formattedNotes = []
    time_offset = 0  # Cumulative time offset for removed notes

    # Open the MIDI file
    mid = mido.MidiFile(midi)

    # Iterate over the messages in the file
    for msg in mid:
        if msg.type == 'note_on' and (msg.velocity > 0 or time == -1):
            if msg.note < 52:
                # Add time to offset for ignored notes
                time_offset += msg.time
            else:
                # Process valid notes
                if time == -1:
                    time = 0
                else:
                    time += msg.time - time_offset
                    time_offset = 0  # Reset the offset since time has been applied

                currNotes_deque.append([msg.note, time, -1])

        elif msg.type == 'pitchwheel':
            # Treat pitchwheel as noise and add time
            if time == -1:
                time = 0
            else:
                time += msg.time - time_offset
                time_offset = 0

        elif msg.type == 'note_off' or (msg.type == 'note_on' and msg.velocity == 0):
            if msg.note < 52:
                # Add time to offset for ignored notes
                time_offset += msg.time
            else:
                # Process note-off events for valid notes
                time += msg.time - time_offset
                time_offset = 0  # Reset the offset

                if currNotes_deque and currNotes_deque[0][0] == msg.note:
                    currNotes_deque[0][2] = time
                    formattedNotes.append(currNotes_deque.popleft())
                elif currNotes_deque and currNotes_deque[-1][0] == msg.note:
                    currNotes_deque[-1][2] = time
                    formattedNotes.append(currNotes_deque.pop())
                else:
                    # Handle middle notes
                    for _ in range(len(currNotes_deque)):
                        if currNotes_deque[0][0] == msg.note:
                            currNotes_deque[0][2] = time
                            formattedNotes.append(currNotes_deque.popleft())
                            break
                        else:
                            currNotes_deque.append(currNotes_deque.popleft())
        else:
            # Other messages
            continue

    # Sort notes by start time
    formattedNotes.sort(key=lambda x: x[1])

    # Convert times to milliseconds and remove duplicates
    unique_list = []
    for note in formattedNotes:
        note[1] = int(note[1] * 1000)
        note[2] = int(note[2] * 1000)
        if note not in unique_list:
            unique_list.append(note)

    return unique_list"""


def getMessages(midi):
    currNotes_deque = deque()
    time = -1
    formattedNotes = []
    time_offset = 0  # Cumulative time offset for removed notes

    # Open the MIDI file
    mid = mido.MidiFile(midi)

    # Iterate over the messages in the file
    for msg in mid:
        if msg.type == 'note_on' and (msg.velocity > 0 or time == -1):
            if msg.note < 52:
                # Add time to offset for ignored notes
                time_offset += msg.time
            else:
                # Process valid notes
                if time == -1:
                    time = 0
                else:
                    time += msg.time - time_offset
                    time_offset = 0  # Reset the offset since time has been applied

                currNotes_deque.append([msg.note, time, -1])

        elif msg.type == 'pitchwheel':
            # Treat pitchwheel as noise and add time
            if time == -1:
                time = 0
            else:
                time += msg.time - time_offset
                time_offset = 0

        elif msg.type == 'note_off' or (msg.type == 'note_on' and msg.velocity == 0):
            if msg.note < 52:
                # Add time to offset for ignored notes
                time_offset += msg.time
            else:
                # Process note-off events for valid notes
                time += msg.time - time_offset
                time_offset = 0  # Reset the offset

                if currNotes_deque and currNotes_deque[0][0] == msg.note:
                    currNotes_deque[0][2] = time
                    formattedNotes.append(currNotes_deque.popleft())
                elif currNotes_deque and currNotes_deque[-1][0] == msg.note:
                    currNotes_deque[-1][2] = time
                    formattedNotes.append(currNotes_deque.pop())
                else:
                    # Handle middle notes
                    for _ in range(len(currNotes_deque)):
                        if currNotes_deque[0][0] == msg.note:
                            currNotes_deque[0][2] = time
                            formattedNotes.append(currNotes_deque.popleft())
                            break
                        else:
                            currNotes_deque.append(currNotes_deque.popleft())
        else:
            # Other messages
            continue

    # Sort notes by start time
    formattedNotes.sort(key=lambda x: x[1])

    # Convert times to milliseconds and remove duplicates
    unique_list = []
    for note in formattedNotes:
        note[1] = int(note[1] * 1000)
        note[2] = int(note[2] * 1000)
        if note not in unique_list:
            unique_list.append(note)

    # Adjust times so the first note starts at 0
    if unique_list:
        first_start = unique_list[0][1]
        for note in unique_list:
            note[1] -= first_start
            note[2] -= first_start

    return unique_list




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
