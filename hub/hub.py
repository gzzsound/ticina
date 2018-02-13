import time
import os
import datetime
import signal
import sys
import json
import syslog
import paho.mqtt.client as mqtt
import RPi.GPIO as GPIO

EFFECT_1 = '4546464543545f31'
EFFECT_2 = '4546464543545f32'

LED_PIN = 23
OPERATIONAL_LED_PIN = 24
SHUTDOWN_PIN = 15
WAITING_TIME_FOR_SHUTDOWN = 20

BUTTONS = {
    14: {'name': 'leds', 'addr': 8, 'effect': EFFECT_1},
    15: {'name': 'leds', 'addr': 8, 'effect': EFFECT_2},
    18: {'name': 'clock', 'addr': 8, 'effect': EFFECT_1}
}

last_id = 0
client = None
operational_led = None
timestamp_for_shutdown = None

def log(message):
    if(os.environ.get('DEBUG')):
        print(message)
    else:
        syslog.syslog(message)

def on_connect(client, userdata, flags, rc):
    log("Connected with result code " + str(rc))
    client.subscribe("out-topic/#")

def on_disconnect(client, userdata, rc):
    log("Client disconnected")

def on_message(client, userdata, msg):
    log(msg.topic + " " + str(msg.payload))
    turn_led_off()

def signal_handler(signal, frame):
    log('You pressed Ctrl+C!')
    sys.exit(0)

def button_pressed(channel):
    gpio_state = GPIO.input(channel)

    if(gpio_state == 0):
        send_command(channel)

    process_for_shutdown(channel, gpio_state)

def process_for_shutdown(channel, gpio_state):
    if(channel != SHUTDOWN_PIN):
        return True

    global timestamp_for_shutdown
    if(gpio_state == 0):
        timestamp_for_shutdown = time.time()
    else:
        if((time.time()-timestamp_for_shutdown) - WAITING_TIME_FOR_SHUTDOWN):
            shutdown()

def shutdown():
    log("BYE BYE")
    clean_up()
    os.system('halt')

def turn_led_off():
    GPIO.output(LED_PIN,GPIO.LOW)

def blink_led():
    GPIO.output(LED_PIN,GPIO.HIGH)

def send_command(channel):
    log("Button %i has been pressed" % channel)
    blink_led()
    info = BUTTONS[channel]
    client.publish(topic_for_channel(info), payload_for_channel(info))

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

def setup_mqtt_client():
    client = mqtt.Client()
    username = os.environ['MQTT_USER']
    password = os.environ['MQTT_PASSWORD']
    client.username_pw_set(username, password)
    client.on_connect = on_connect
    client.on_message = on_message
    client.on_disconnect = on_disconnect
    client.connect("localhost", 1883, 60)
    return client

def setup_operational_led():
    global operational_led
    GPIO.setup(OPERATIONAL_LED_PIN, GPIO.OUT)
    operational_led = GPIO.PWM(OPERATIONAL_LED_PIN, 0.5)
    operational_led.start(50)

def setup_output_led():
    GPIO.setup(LED_PIN, GPIO.OUT)
    turn_led_off()

def setup_gpio_inputs():
    for button in BUTTONS.keys():
        GPIO.setup(button, GPIO.IN, pull_up_down=GPIO.PUD_UP)
        GPIO.add_event_detect(button, GPIO.BOTH, callback=button_pressed, bouncetime=400)

def setup_gpio():
    GPIO.setwarnings(False)
    GPIO.setmode(GPIO.BCM)

def clean_up():
    if client != None:
        client.loop_stop()
        client.disconnect()

    GPIO.cleanup()

try:
    setup_gpio()
    setup_gpio_inputs()
    setup_output_led()
    setup_operational_led()

    client = setup_mqtt_client()
    client.loop_start()

    signal.signal(signal.SIGINT, signal_handler)
    signal.signal(signal.SIGTERM, signal_handler)

    print('Press Ctrl+C to end')
    signal.pause()

finally:
    clean_up()
