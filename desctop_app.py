import asyncio
import psutil
from bleak import BleakScanner, BleakClient
MY_CHAR_UUID = "0000abcd-0000-1000-8000-00805f9b34fb"  # Замените на UUID вашей характеристики

async def scan_and_connect():
    devices = await BleakScanner.discover()
    for d in devices:
        print(d)
    device = await BleakScanner.find_device_by_name("xxxxx") # Замените на имя вашего устройства

    if not device:
        print("Device not found")
        return None

    client = BleakClient(device)
    await client.connect()
    return client

async def main():
    client = await scan_and_connect()
    if not client:
        return

    try:
        while True:
            # Чтение характеристики
            #data = await client.read_gatt_char(MY_CHAR_UUID)
           # print("received:", data)
            cpu_percentage=int(psutil.cpu_percent())
            # Запись в характеристику
            print(cpu_percentage)

            byte_array = cpu_percentage.to_bytes(2, byteorder='big') # данные
            # Создаем новый массив байт с добавлением '\x24' в начало
            new_byte_array = bytes([0x24]) + byte_array #данные с командой
            print(new_byte_array)




            await client.write_gatt_char(MY_CHAR_UUID, new_byte_array)  # Замените b"your-data" на данные, которые вы хотите записать
            print("written to characteristic 22")
            
            # Ожидание 1 секунды
            await asyncio.sleep(0.1)
            #await client.write_gatt_char(MY_CHAR_UUID, b'\x23')  # Замените b"your-data" на данные, которые вы хотите записать
            #print("written to characteristic 23")


    finally:
        await client.disconnect()

asyncio.run(main())

