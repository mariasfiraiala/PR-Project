import paho.mqtt.client as mqtt
import json
import os
from sqlalchemy import create_engine, Column, Float, Integer, DateTime, func
from sqlalchemy.orm import declarative_base, sessionmaker

MQTT_BROKER = os.getenv('MQTT_BROKER', 'localhost')
MQTT_PORT = int(os.getenv('MQTT_PORT', 8883))
MQTT_TOPIC = os.getenv('MQTT_TOPIC', 'sensor/data')
SQLALCHEMY_DATABASE_URL = "postgresql://user:user@localhost/sensors"
engine = create_engine(SQLALCHEMY_DATABASE_URL)
SessionLocal = sessionmaker(autocommit=False, autoflush=False, bind=engine)
Base = declarative_base()


class SensorDataDB(Base):
    __tablename__ = "sensor_data"
    id = Column(Integer, primary_key=True, index=True)
    humidity = Column(Float)
    temperature = Column(Float)
    pressure = Column(Float)
    time = Column(DateTime, server_default=func.now())

Base.metadata.create_all(bind=engine)

def on_connect(client, userdata, flags, rc, properties):
    client.subscribe(MQTT_TOPIC)

def on_message(client, userdata, msg):
    try:
        print(msg.payload)
        payload = json.loads(msg.payload)
        required_fields = ['humidity', 'temperature', 'pressure']
        if not all(field in payload for field in required_fields):
            print(f"Bad data: {payload}")
            return
        

        db = SessionLocal()
        try:
            db_data = SensorDataDB(**payload)
            db.add(db_data)
            db.commit()
            return {"message": "Data added successfully", "data": payload}
        finally:
            db.close()
    
    except json.JSONDecodeError:
        print(f"Failed to decode JSON: {msg.payload}")

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
