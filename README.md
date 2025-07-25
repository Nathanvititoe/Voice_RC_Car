# Voice-Controlled RC Car

This project enables voice-based control of an Arduino RC car using BLE and a the Vosk speech recognition model running on your PC.

Arduino -  
   Arduino Nano 33 BLE Rev2 - https://docs.arduino.cc/hardware/nano-33-ble-rev2/
   Arduino Nano 33 BLE Sense Rev2 - https://docs.arduino.cc/hardware/nano-33-ble-sense-rev2/

   any arduino with BLE capability and the ability to control an RC car
---

##  Project Structure

```
rc_car/
├── arduino/
│   └── rc_car_voice.ino             # Arduino sketches for BLE-controlled RC car
├── pc_voice_control/
│   ├── test_mic.py                      # tests if sounddevice library is finding input devices
│   ├── main.py                      # Entry point for voice recognition + BLE communication
│   ├── src/
│   │   ├── voice_recognition/
│   │   │   └── voice_recognition.py
│   │   ├── rc_car_control/
│   │   │   └── rc_car_control.py
│   │   └── commands.py              # Phrase-to-command mapping
│   ├── vosk_voice_rec_model/        # Unzipped Vosk Voice Recognition model directory
│   └── requirements.txt             # Python package dependencies
```

---

## PC Setup (Voice Recognition + BLE)

1. **Create and activate a virtual environment in Powershell**  
   *(Windows example)*
   ```powershell
   python -m venv venv
   .\venv\Scripts\activate
   ```

2. **Install Python dependencies**
   ```bash
   pip install -r requirements.txt
   ```

3. **Download and extract the Vosk model**
   - Go to [https://alphacephei.com/vosk/models](https://alphacephei.com/vosk/models)
   - Download `vosk-model-small-en-us-0.15.zip`
   - Unzip it into:  
     `pc_voice_control/vosk_voice_rec_model/`  
     (The folder should contain files like `conf`, `am`, `graph`, etc.)

4. **Run the program**
   From the `pc_side_voice_control` directory:
   ```bash
   python main.py
   ```
5. **It is now listening for voice commands from your input device**

---

## Arduino Setup (RC Car Code)

1. Open `arduino/rc_car_voice.ino` in the Arduino IDE.
2. Make sure the board is set to **Arduino Nano 33 BLE** or **Arduino Nano 33 BLE sense**.
3. Ensure the ArduinoBLE library is installed through the library manager.
3. Upload the sketch to your Arduino.
4. The BLE peripheral should advertise with the name `Voice_RC`.

---

## Voice Commands

You can say any of the following phrases:

| Action          | Trigger Phrases                           | BLE Command |
|-----------------|-------------------------------------------|-------------|
| Forward         | "go", "forward", "move forward", "start"  | `FWD`       |
| Stop            | "stop", "halt", "brake"                   | `STOP`      |
| Turn Left       | "left", "turn left"                       | `LEFT`      |
| Turn Right      | "right", "turn right"                     | `RIGHT`     |
| Reverse         | "reverse", "backward", "go back"          | `REV`       |
| Speed Up        | "speed up", "faster"                      | `FAST`      |
| Slow Down       | "slow down", "slower"                     | `SLOW`      |

The recognized command string (like `"FWD"`, `"STOP"`) is sent to the Arduino via BLE and will trigger a movement function. 

---
## Notes

- The Vosk model must be unzipped; don’t just place the `.zip` file.
- You can customize the voice phrases and BLE messages in `commands.py`.
- The pins that are set in the arduino file must match the pins you use for your H bridges and the RC car, set this to whatever you use. 
- This system runs speech recognition entirely offline on your PC.

---

##  Requirements

See `requirements.txt`, but core packages include:

- `vosk`
- `sounddevice`
- `bleak`

---

##  Future Improvements 
- Integrate camera for remote viewing while controlling the car  

---

