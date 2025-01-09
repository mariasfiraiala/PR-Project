import paho.mqtt.client as mqtt
import requests
import json
import time
import os
import ssl

MQTT_BROKER = os.getenv('MQTT_BROKER', 'localhost')
MQTT_PORT = int(os.getenv('MQTT_PORT', 8883))
MQTT_TOPIC = os.getenv('MQTT_TOPIC', 'sensor/data')
FASTAPI_URL = os.getenv('FASTAPI_URL', 'http://localhost:55000/data/')

def on_connect(client, userdata, flags, rc, properties):
    client.subscribe(MQTT_TOPIC)

def on_message(client, userdata, msg):
    try:
        print(msg)
        payload = json.loads(msg.payload)
        required_fields = ['humidity', 'temperature', 'pressure']
        if not all(field in payload for field in required_fields):
            print(f"Bad data: {payload}")
            return
        
        response = requests.post(FASTAPI_URL, json=payload)
        
        if response.status_code == 200:
            print(f"Success: {payload}")
        else:
            print(f"Fail: {response.status_code}")
    
    except json.JSONDecodeError:
        print(f"Failed to decode JSON: {msg.payload}")
    except requests.RequestException as e:
        print(f"Request error: {e}")

def main():
    client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
    
    client.on_connect = on_connect
    client.on_message = on_message
    
    try:
        client.tls_set("/etc/ca-certificates/ca.crt",
                       "/etc/ca-certificates/client.crt",
                       "/etc/ca-certificates/client.key"
        )
        client.connect(MQTT_BROKER, MQTT_PORT, 60)
        
        print(f"MQTT Bridge started. Listening on topic: {MQTT_TOPIC}")
        client.loop_forever()
    
    except Exception as e:
        print(f"Error connecting to MQTT broker: {e}")

if __name__ == "__main__":
    main()
