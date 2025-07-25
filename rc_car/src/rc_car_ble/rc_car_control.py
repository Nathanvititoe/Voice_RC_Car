# rc_car_control.py

import asyncio
from bleak import BleakScanner, BleakClient

ARDUINO_NAME = "Voice_RC"
CHAR_UUID = "2A56"

COMMAND_MAP = {
    "go": 0x01, # forward
    "down": 0x05, # backward
    "stop": 0x02, # stop 
    "left": 0x03, # turn left
    "right": 0x04, # turn right
}

# convert string to byte before sending
def send_command(command: str):
    print(f"[TEST] Sending command string: {command}")
    # print(f"[BLE] Sending command string: {command}")
    # asyncio.run(send_ble(command))

# send command over BLE to arduino
async def send_ble(cmd_string):
    # connect to arduino over BLE
    device = await BleakScanner.find_device_by_filter(lambda d, _: d.name == ARDUINO_NAME)
    
    # throw err if connection fails
    if not device:
        print("BLE device not found")
        return
    
    # write command to client
    async with BleakClient(device) as client:
        await client.write_gatt_char(CHAR_UUID, cmd_string.encode())
        print(f"Sent BLE command: {cmd_string}")
