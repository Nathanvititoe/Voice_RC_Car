# Voice-Controlled RC Car

This project enables voice-based control of an Arduino RC car using a TCP Connection and the Vosk speech recognition model running on your PC.

### Arduino -  
- [Arduino Uno R4 Wifi](https://docs.arduino.cc/hardware/uno-r4-wifi/)
- any arduino with Wifi capability and the ability to control an RC car - [Browse Hardware](https://docs.arduino.cc/hardware/)
---

##  Project Structure

```
rc_car/
├── pc_voice_control/
│   ├── src/
│   │   └── voice_recognition.py         # logic for vosk model 
│   │   └── commands.py                  # map of commands for the rc car
│   │   └── rc_car_control.py            # connection to tcp server
│   ├── vosk_voice_rec_model/            # Unzipped Vosk Voice Recognition model directory
│   ├── main.py                          # Entry point for voice recognition + BLE communication
│   ├── test_mic.py                      # tests if sounddevice library is finding input devices
│   └── requirements.txt                 # Python packages
├── secrets/                             # will need to be created 
│   └── arduino_secrets.h                    # will need to be created
├── rc_car.ino                           # main logic for arduino (setup/loop/handleCommands) 
├── WebConnection.cpp                    # Web Connection logic (creates TCP Connection)    
├── WebConnection.h                      # Web Connection Header     
```

---

## PC Setup (Voice Recognition + BLE)

1. **Create and activate a virtual environment in Powershell**  
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
   From the `pc_voice_control` directory:
   ```bash
   python main.py
   ```
5. **It is now listening for voice commands from your input device**

---

## Arduino Setup (RC Car Code)

1. Open `arduino/rc_car_voice.ino` in the Arduino IDE.
2. Make sure the board is set to your wifi capable board.
3. Ensure the WiFiS3 library is installed through the library manager.
4. Change pin numbering to match your RC Car/L293D Drivers
5. Create an `arduino_secrets.h` at the same level as the `rc_car.ino`,
   add these variables to the secrets file for your configuration
   - #define SECRET_SSID "" (your wifi network name)
   - #define SECRET_PASS "" (your wifi password)
   - #define CONNECTION_PORT 9000 (this can be any port)
   - #define READ_TIMEOUT_MS 3000 (how long before reading message times out)
   - #define RETRY_MS 2000 (how quick to retry connection)
5. Upload the sketch to your Arduino.

---

## Voice Commands

You can say any of the following phrases:

| Action          | Trigger Phrases                           | Sent Command|
|-----------------|-------------------------------------------|-------------|
| Forward         | "go", "forward", "move forward", "start"  | `forward`   |
| Stop            | "stop", "halt", "brake", "woah            | `stop`      |
| Turn Left       | "left", "turn left"                       | `left`      |
| Turn Right      | "right", "turn right"                     | `right`     |
| Reverse         | "reverse", "backward", "go back"          | `reverse`   |
| Speed Up        | "speed up", "faster", "full speed"        | `fast`      |
| Slow Down       | "slow down", "slower", "half speed"       | `slow`      |

The recognized command string (like `"forward"`, `"stop"`) is sent to the Arduino via TCP Connection and will trigger a movement function. 

---
## Notes

- The Vosk model must be unzipped; don’t just place the `.zip` file.
- You can customize the voice phrases and their output messages in `commands.py`.
- The pins that are set in the arduino file must match the pins you use for your H bridges and the RC car, set this to whatever you use. 
- This system runs speech recognition off of your PC and Home Wifi Network.

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

