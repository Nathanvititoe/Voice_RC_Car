# voice_recognition.py

import queue
import sounddevice as sd
import vosk
import json
from .commands import COMMAND_MAP

q = queue.Queue()

def _audio_callback(indata, frames, time, status):
    if status:
        print("Audio status:", status)
    q.put(bytes(indata))

def listen_for_commands(callback_fn):
    model = vosk.Model("vosk_voice_rec_model")
    recognizer = vosk.KaldiRecognizer(model, 16000)

    with sd.RawInputStream(samplerate=16000, blocksize=8000, dtype='int16',
                           channels=1, callback=_audio_callback):
        print("Listening for commands...")

        # listen for commands on pc mic input
        while True:
            data = q.get()
            if recognizer.AcceptWaveform(data):
                result = json.loads(recognizer.Result())
                text = result.get("text", "").lower().strip()
                print(f"[Model] Heard: {text}")

                # find matching phrase in availabe commands map
                for phrase, command in COMMAND_MAP.items():
                    if phrase in text:
                        print(f"[Match] '{phrase}' → {command}")
                        callback_fn(command)
                        break  

