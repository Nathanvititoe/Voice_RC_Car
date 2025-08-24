from src.voice_recognition.voice_recognition import listen_for_commands
from src.rc_web_connect.rc_car_control import send_command

if __name__ == "__main__":
    listen_for_commands(send_command)
