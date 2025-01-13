from fastapi import FastAPI
import uvicorn


app = FastAPI()


@app.get("/")
async def root():
    return {"message": "hello"}


if __name__ == "__main__":
    uvicorn.run("api:app",
                host="127.0.0.1",
                port=55000,
                ssl_certfile="/etc/ca-certificates/ca.crt",
                ssl_keyfile="/etc/ca-certificates/ca.key"
    )
