import tensorflow as tf
import os
import sys
from dataParse import get_tempo_and_time_signature
from dataParse import getMessages
#getMessages(midi)

record = sys.argv[1]
id = sys.argv[2]
duration = sys.argv[3]
bpm_value = sys.argv[4]

rec_command = f'arecord -D plughw:0,0 --duration={duration} --rate=88200 --format=S16_LE {id}_rec.wav'

#if we are recording, if we just want to parse data, we could just do it in other ways. 
if record == '-r':
    os.system(rec_command)

    from basic_pitch.inference import predict, Model
    from basic_pitch import ICASSP_2022_MODEL_PATH

    basic_pitch_model = Model(ICASSP_2022_MODEL_PATH)

    from basic_pitch.inference import predict_and_save


    wav_name = f'{id}_rec.wav'
    mid_name = f'{id}_rec.mid'

    predict_and_save(
        audio_path_list=[wav_name],
        output_directory=".",
        save_midi=True,
        sonify_midi=True,
        save_model_outputs=False,
        save_notes=False,
        model_or_model_path=basic_pitch_model,
        onset_threshold=0.70,
        frame_threshold=0.50,
        minimum_note_length=11,
        midi_tempo=int(bpm_value),
        multiple_pitch_bends=False
    )
    
    

#convert data then figure out how you will push delete the midi files.
#pybind stuff. 
mid_basic = f'{id}_rec_basic_pitch.mid'
mid_basic_wav = f'{id}_rec_basic_pitch.wav'


tempo, time_signature, ticks_per_beat = get_tempo_and_time_signature(mid_basic)
print(tempo)
print(time_signature)
print(ticks_per_beat)

notes = getMessages(mid_basic)
print(notes)
print(len(notes))

#remove
os.remove(wav_name)
os.remove(mid_basic)
os.remove(mid_basic_wav)

