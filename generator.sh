#################################################################################################################################################################################
#Certificate generator for TLS encryption																	#
#################################################################################################################################################################################
openssl req -new -x509 -days 365 -extensions v3_ca -keyout ca.key -out ca.crt -passout pass:1234 -subj '/CN=TrustedCA.net'
#If you generating self-signed certificates the CN can be anything

openssl genrsa -out mosquitto1.key 2048
openssl req -out mosquitto1.csr -key mosquitto1.key -new -subj '/CN=localhost'
openssl x509 -req -in mosquitto1.csr -CA ca.crt -CAkey ca.key -CAcreateserial -out mosquitto1.crt -days 365 -passin pass:1234 
#Mostly the client verifies the adress of the mosquitto server, so its necessary to set the CN to the correct adress (eg. yourserver.com)!!!

openssl genrsa -out mosquitto2.key 2048
openssl req -out mosquitto2.csr -key mosquitto2.key -new -subj '/CN=192.168.27.36'
openssl x509 -req -in mosquitto2.csr -CA ca.crt -CAkey ca.key -CAcreateserial -out mosquitto2.crt -days 365 -passin pass:1234 
#Mostly the client verifies the adress of the mosquitto server, so its necessary to set the CN to the correct adress (eg. yourserver.com)!!!


#################################################################################################################################################################################
#These certificates are only needed if the mosquitto broker requires a certificate for client autentithication (require_certificate is set to true in mosquitto config).      	#
#################################################################################################################################################################################
openssl genrsa -out client.key 2048
openssl req -out client.csr -key client.key -new -subj '/CN='
openssl x509 -req -in client.csr -CA ca.crt -CAkey ca.key -CAcreateserial -out client.crt -days 365 -passin pass:1234
#If the server (mosquitto) identifies the clients based on CN key, its necessary to set it to the correct value, else it can be blank.

#################################################################################################################################################################################
#For ESP32. Formatting for source code. The output is the esp_certificates.c                                                                                                    #
#################################################################################################################################################################################
echo -en "#include \"esp_certificates.h\"\n\nconst char CA_cert[] = \\\\\n" > esp_certificates.c;
sed -z '0,/-/s//"-/;s/\n/\\n" \\\n"/g;s/\\n" \\\n"$/";\n\n/g' ca.crt >> esp_certificates.c     #replace first occurance of - with "-  ;  all newlnies with \n " \ \n", except last newline where just add ;"
echo "const char ESP_CA_cert[] = \\" >> esp_certificates.c;
sed -z '0,/-/s//"-/;s/\n/\\n" \\\n"/g;s/\\n" \\\n"$/";\n\n/g' client.crt >> esp_certificates.c     #replace first occurance of - with "-  ;  all newlnies with \n " \ \n", except last newline where just add ;"
echo "const char ESP_RSA_key[] = \\" >> esp_certificates.c;
sed -z '0,/-/s//"-/;s/\n/\\n" \\\n"/g;s/\\n" \\\n"$/";/g' client.key >> esp_certificates.c     #replace first occurance of - with "-  ;  all newlnies with \n " \ \n", except last newline where just add ;"

cp ca.crt mosquitto/certificates/ca.crt
mv mosquitto*.{crt,key} mosquitto/certificates

cp ca.crt mqtt/certificates/ca.crt
mv client.{crt,key} mqtt/certificates 

chmod a+r mosquitto/certificates/mosquitto*.key mqtt/certificates/client.key
rm ca.crt ca.key ca.srl client.csr mosquitto*.csr
