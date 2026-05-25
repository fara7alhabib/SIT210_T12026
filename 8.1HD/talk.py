import asyncio
from bleak import BleakScanner, BleakClient
import speech_recognition as sr

CHARACTERISTIC_UUID = "19B10001-E8F2-537E-4F6C-D104768A1214"

async def send_command(command):

    print("Searching for Arduino...")

    device = await BleakScanner.find_device_by_name("VoiceLightSystem")

    if device is None:
        print("Arduino not found")
        return

    print("Connecting to Arduino...")

    async with BleakClient(device) as client:

        print("Connected")

        await client.write_gatt_char(
            CHARACTERISTIC_UUID,
            command.encode()
        )

        print("Command sent:", command)

async def main():

    r = sr.Recognizer()

    while True:

        with sr.Microphone() as source:

            print("\nSpeak now...")
            audio = r.listen(source)

        try:

            text = r.recognize_google(audio).lower()

            print("You said:", text)

            if "turn on all" in text or "all on" in text:
                await send_command("all_on")

            elif "turn on bathroom" in text:
                await send_command("bathroom_on")

            elif "turn on hallway" in text:
                await send_command("hallway_on")

            elif "turn on room" in text:
                await send_command("room_on")

            elif "turn on fan" in text:
                await send_command("fan_on")

            elif "turn off all" in text or "all off" in text:
                await send_command("all_off")

            elif "turn off bathroom" in text:
                await send_command("bathroom_off")

            elif "turn off hallway" in text:
                await send_command("hallway_off")

            elif "turn off room" in text:
                await send_command("room_off")

            elif "turn off fan" in text:
                await send_command("fan_off")

            elif "stop program" in text:
                print("Stopping program...")
                break

            else:
                print("Command not recognized")

        except Exception as e:
            print("Error:", e)

asyncio.run(main())
