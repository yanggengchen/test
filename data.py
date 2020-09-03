import smbus
import time
import struct

bus = smbus.SMBus(1)
address = 0x04


def writeNumber(value):
    bus.write_byte(address, value)
    return -1


def readNumber(command):
    data = bus.read_i2c_block_data(address, int(command), 32)
    return [chr(i) for i in data]


"""
while True:
    var = input("Enter1–9:")
    if not var:
        continue
    # writeNumber(int(var))
    print("RPI: Hi Arduino, I sent you ", var)
    # sleep one second
    time.sleep(1)
    number = readNumber(var)
    print("Arduino: Hey RPI,I received a digit", number)
    print("======================================\n")
"""


def get_data():
    var = 1
    time.sleep(1)
    number1 = readNumber(var)
    print(number1)
    result1 = b""
    for i in number1:
        hex_num = hex(ord(i))[2:]
        result1 += bytes.fromhex(hex_num if len(hex_num) == 2 else ("0" + hex_num))

    var = 2
    time.sleep(1)
    number2 = readNumber(var)
    print(number2)
    result2 = b""
    for i in number2:
        hex_num = hex(ord(i))[2:]
        result2 += bytes.fromhex(hex_num if len(hex_num) == 2 else ("0" + hex_num))

    var = 3
    time.sleep(1)
    number3 = readNumber(var)
    print(number3)
    result3 = b""
    for i in number3:
        hex_num = hex(ord(i))[2:]
        result3 += bytes.fromhex(hex_num if len(hex_num) == 2 else ("0" + hex_num))

    head_gas,gas_1,gas_2,gas_3,alcohol_left,alcohol_right,ammonia_left,ammonia_right,gas_4,gas_5,valid_gas,tail_gas = struct.unpack('<BBBBLLLLBBBB',result1[0:24])
    alcohol = alcohol_left + alcohol_right / 65536
    ammonia = ammonia_left + ammonia_right / 65536
    head_weight_l,weightl_1,weightl_2,weightl_3,weight_1_left,weight_1_right,weight_2_left,weight_2_right,weightl_4,weightl_5,valid_weight_l,tail_weight_l = struct.unpack('<BBBBLLLLBBBB',result2[0:24])
    weight_1 = weight_1_left + weight_1_right / 65536
    weight_2 = weight_2_left + weight_2_right / 65536
    head_weight_r,weightr_1,weightr_2,weightr_3,weight_3_left,weight_3_right,weight_4_left,weight_4_right,weightr_4,weightr_5,valid_weight_r,tail_weight_r = struct.unpack('<BBBBLLLLBBBB',result3[0:24])
    weight_3 = weight_3_left + weight_3_right / 65536
    weight_4 = weight_4_left + weight_4_right / 65536
    print(alcohol,ammonia,weight_1,weight_2,weight_3,weight_4)
    return {
        "alcohol": alcohol,
        "ammonia": ammonia,
        "weight": [weight_1, weight_2, weight_3, weight_4],
        "time": time.time()
    }

get_data()