from fastapi import FastAPI
from pydantic import BaseModel
from sqlalchemy import create_engine, Column, Float, Integer, DateTime, func
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy.orm import sessionmaker


SQLALCHEMY_DATABASE_URL = "postgresql://user:user@localhost/sensors"
engine = create_engine(SQLALCHEMY_DATABASE_URL)
SessionLocal = sessionmaker(autocommit=False, autoflush=False, bind=engine)
Base = declarative_base()

class SensorData(BaseModel):
    humidity: float
    temperature: float
    pressure: float

class SensorDataDB(Base):
    __tablename__ = "sensor_data"
    id = Column(Integer, primary_key=True, index=True)
    humidity = Column(Float)
    temperature = Column(Float)
    pressure = Column(Float)
    time = Column(DateTime, server_default=func.now())

Base.metadata.create_all(bind=engine)

app = FastAPI()

@app.post("/data/")
async def post_data(data: SensorData):
    db = SessionLocal()
    try:
        db_data = SensorDataDB(**data.dict())
        db.add(db_data)
        db.commit()
        return {"message": "Data added successfully", "data": data.dict()}
    finally:
        db.close()

@app.get("/")
async def root():
    return {"message": "hello"}

