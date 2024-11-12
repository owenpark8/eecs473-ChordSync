import tensorflow as tf
import os
import sys

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
        output_filename= mid_name,
        save_midi=True,
        sonify_midi=True,
        save_model_outputs=False,
        save_notes=False,
        model_or_model_path=basic_pitch_model,
        bpm=bpm_value
    )

#convert data then figure out how you will push delete the midi files.
#pybind stuff. 

