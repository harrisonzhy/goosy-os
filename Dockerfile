FROM gcc:latest

COPY . /code
WORKDIR /code

CMD ["tail", "-f", "/dev/null"]