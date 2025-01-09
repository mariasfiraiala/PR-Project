FROM python:3.12-slim

WORKDIR /api
COPY . .

ENV PYTHONDONTWRITEBYTECODE 1
ENV PYTHONUNBUFFERED 1

RUN pip install -r requirements.txt

CMD ["fastapi", "dev", "api.py", "--port", "55000"]
