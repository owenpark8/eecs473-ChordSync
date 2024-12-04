import tensorflow as tf
import os
import sys
from dataParse import get_tempo_and_time_signature
from dataParse import getMessages

from basic_pitch.inference import predict, Model
from basic_pitch import ICASSP_2022_MODEL_PATH

basic_pitch_model = Model(ICASSP_2022_MODEL_PATH)

from basic_pitch.inference import predict_and_save


#this just runs once in the beginning
def prediction(id, bpm_value):
    wav_name = f'{id}_rec.wav'

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

def record_convert_no_offset(id, bpm_value):
    print("Running")
        
        

    #convert data then figure out how you will push delete the midi files.
    #pybind stuff. 
    mid_basic = f'{id}_rec_basic_pitch.mid'
    #mid_basic_wav = f'{id}_rec_basic_pitch.wav'


    tempo, time_signature, ticks_per_beat = get_tempo_and_time_signature(mid_basic)
    print(tempo)
    print(time_signature)
    print(ticks_per_beat)

    notes = getMessages(mid_basic)
    print(notes)
    print(len(notes))

    #remove
    return notes


def record_convert_offset(id):
    print("Running")
        
        

    #convert data then figure out how you will push delete the midi files.
    #pybind stuff. 
    mid_basic = f'{id}_rec_basic_pitch.mid'
    #mid_basic_wav = f'{id}_rec_basic_pitch.wav'


    tempo, time_signature, ticks_per_beat = get_tempo_and_time_signature(mid_basic)
    print(tempo)
    print(time_signature)
    print(ticks_per_beat)

    notes = getMessages(mid_basic)
    print(notes)
    print(len(notes))

    #remove
    return notes



def record_convert(id, duration, bpm_value):
    #convert data then figure out how you will push delete the midi files.
    #pybind stuff. 
    mid_basic = f'{id}_rec_basic_pitch.mid'
    #mid_basic_wav = f'{id}_rec_basic_pitch.wav'


    tempo, time_signature, ticks_per_beat = get_tempo_and_time_signature(mid_basic)
    print(tempo)
    print(time_signature)
    print(ticks_per_beat)

    notes = getMessages(mid_basic)
    print(notes)
    print(len(notes))

    return notes


def remove_files(id):
    os.remove(f'{id}_rec_basic_pitch.mid')
    os.remove(f'{id}_rec_basic_pitch.wav')
    os.remove(f'{id}_rec.wav')

