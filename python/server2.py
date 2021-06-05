import socket
import json
import numpy as np
import keyboard as kb
import time
import mariokart as mk

HOST = '192.168.1.102'  # Standard loopback interface address (localhost)
PORT = 3000        # Port to listen on (non-privileged ports are > 1023)

#_mod_
keyDict = {"Left": "left", "Right": "right", "Item_front":"up", "Item_back":"down", "Drift":"c", "Acc": "x"}

# data format example
# "{"Acc_x":%.2f,"Acc_y":%.2f,"Acc_z":%.2f,"Gyro_x":%.2f,"Gyro_y":%.2f,"Gyro_z":%.2f,"Item_front":%d,"Item_back":%d,"Acc":%d}"
# {'Acc_x':10000.25,"Acc_y":10000.25,"Acc_z":10000.25,"Gyro_x":10000.25,"Gyro_y":10000.25,"Gyro_z":10000.25,"Item_front":1,"Item_back":1,"Acc":1}

def main():
    size = 200
    past_j_data = None
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind((HOST, PORT))
        print('listen...')
        s.listen()
        conn, addr = s.accept()
        with conn:
            print('Connected by', addr)

            while True:
                data = conn.recv(200).decode('utf-8')
                if not data:
                    print("no data!!!")
                    break
                print("Receive data: "+data)
                #check data size!! if size is reasonable then parse json
                if len(data) > size:
                    j_data=json.loads(data)
                    for control in j_data:
                        if j_data[control] == 1 and past_j_data[control] ==0:
                            kb.press(keyDict[control])
                        elif j_data[control] == 0 and past_j_data[control] == 1:
                            kb.release(keyDict[control])
                    past_j_data = j_data
                else:
                    print("error in transmission length: {:d}".format(len(data)))
                    
            s.close()
            return
    return
"""
def strikeKey(key):
    kb.press(key)
    time.sleep(0.1)
    kb.release(key)
    return
"""
if __name__ == '__main__':
    main()