import tkinter as tk
import paho.mqtt.client as mqtt
import json
import os

EFFECT_1 = '4546464543545f31'
EFFECT_2 = '4546464543545f32'

BUTTONS = {
    14: {'name': 'leds', 'addr': 8, 'effect': EFFECT_1},
    15: {'name': 'leds', 'addr': 8, 'effect': EFFECT_2},
    18: {'name': 'clock', 'addr': 8, 'effect': EFFECT_1}
}

last_id = 0

def led1():
    send_command(14)

def led2():
    send_command(15)

def clock():
    send_command(18)

def log(message):
    print(message)

def on_connect(client, userdata, flags, rc):
    log("Connected with result code " + str(rc))
    client.subscribe("out-topic/#")

def on_disconnect(client, userdata, rc):
    log("Client disconnected")

def on_message(client, userdata, msg):
    log(msg.topic + " " + str(msg.payload))

def topic_for_channel(info):
    return info['name'] + "/rpc"

def payload_for_channel(info):
    global last_id
    last_id += 1
    data = {
        "src": "out-topic",
        "id": last_id,
        "method": "I2C.Write",
        "args": {"addr": info['addr'], "data_hex": info['effect']}
    }
    return json.dumps(data)

def send_command(channel):
    log("Button %i has been pressed" % channel)
    info = BUTTONS[channel]
    client.publish(topic_for_channel(info), payload_for_channel(info))

def setup_mqtt_client():
    client = mqtt.Client()
    username = os.environ['MQTT_USER']
    password = os.environ['MQTT_PASSWORD']
    client.username_pw_set(username, password)
    client.on_connect = on_connect
    client.on_message = on_message
    client.on_disconnect = on_disconnect
    client.connect("192.168.100.1", 1883, 60)
    return client

root = tk.Tk()
frame = tk.Frame(root)
frame.pack()

clock = tk.Button(frame,
                   text="clock",
                   fg="red",
                   command=clock)
clock.pack(side=tk.LEFT)

led1 = tk.Button(frame,
                   text="led 1",
                   command=led1)
led1.pack(side=tk.LEFT)

led2 = tk.Button(frame,
                   text="led 2",
                   command=led2)
led2.pack(side=tk.LEFT)

client = setup_mqtt_client()
client.loop_start()

root.mainloop()
