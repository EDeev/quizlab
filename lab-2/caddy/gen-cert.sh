#!/bin/sh

set -e
mkdir -p certs

# 1. Генерация корневого сертификата root.crt и его rsa-ключа root.key 
openssl req -x509 -new -nodes -newkey rsa:2048 \
  -keyout certs/root.key -days 3650 \
  -out certs/root.crt -subj "//CN=Root CA"

# 2.  Генерация  rsa-ключа  для  сертификата  сервера  и  вспомогательного  CSR-файла (Certificate Signing Request) на основе этого ключа
openssl req -new -nodes -newkey rsa:2048 \
  -keyout certs/lab2.key -out certs/lab2.csr \
  -subj "//CN=lab2 server cert"

# 3.  Собственно,  генерация  сертификата  сервера *.crt  из  *.csr-файла, сгенерированного выше
echo "subjectAltName=DNS:localhost,IP:127.0.0.1" > certs/extfile.txt
openssl x509 -req -in certs/lab2.csr -days 3650 \
  -CA certs/root.crt -CAkey certs/root.key \
  -CAcreateserial -out certs/lab2.crt \
  -sha256 -extfile certs/extfile.txt
rm certs/extfile.txt

# 4.  Удаление  вспомогательного  CSR-файла
rm certs/lab2.csr
echo "Done. Certificates written to ./certs/"
