import tensorflow as tf
import os

os.system('arecord -D plughw:0,0 --duration=190 --rate=88200 --format=S16_LE interstellar.wav')

from basic_pitch.inference import predict, Model
from basic_pitch import ICASSP_2022_MODEL_PATH

basic_pitch_model = Model(ICASSP_2022_MODEL_PATH)

from basic_pitch.inference import predict_and_save

predict_and_save(
    audio_path_list=["interstellar.wav"],
    output_directory=".",
    save_midi=True,
    sonify_midi=True,
    save_model_outputs=False,
    save_notes=False,
    model_or_model_path=basic_pitch_model,
)


