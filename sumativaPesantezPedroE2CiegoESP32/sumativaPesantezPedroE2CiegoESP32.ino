//Inclusión de librerias
#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>

//Credenciales para la conexión WiFi
const char* WIFI_SSID = "celu";
const char* WIFI_PASSWORD = "123456789";

//Credenciales del Firebase
const char* API_KEY = "AIzaSyDjUswbkSr20sl-pesG9uuTAiv2WOTx2-s";
const char* DATABASE_URL = "https://termometro-sumativapesantez-default-rtdb.firebaseio.com";
const char* USER_EMAIL = "calor@gmail.com";
const char* USER_PASSWORD = "123456";

//Objetos necesarios para la comunicación con el Firebase
FirebaseData fbdo; //Objeto para manejar la comunicación con Firebase
FirebaseAuth auth; //Objeto para la autenticación en Firebase
FirebaseConfig config; //Objeto de configuración de Firebase

const int INTENSIDAD_PIN = A0;// Pin de conexión del LDR

//Variables Auxiliares
int intensidad = 0;
int intensidadMap=0;

//Configura y establece la conexión Wi-Fi
void setup_WIFI() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
}

//Configura Firebase con las credenciales y la URL de la base de datos
void setupFirebase() {
  config.api_key = API_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.database_url = DATABASE_URL;
  config.token_status_callback = tokenStatusCallback;
  config.signer.tokens.legacy_token = "<database secret>";
  Firebase.reconnectNetwork(true);
  fbdo.setBSSLBufferSize(4096, 1024);
  fbdo.setResponseSize(2048);
  Firebase.begin(&config, &auth);
  Firebase.setDoubleDigits(5);
  config.timeout.serverResponse = 10 * 1000;
  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);
}

void setup() {
  Serial.begin(115200); //Comunicación seria
    setup_WIFI(); //Conecta el dispositivo a la red WiFi especificada con las credenciales 
    setupFirebase(); //Conexión con la plataforma Firebase con las credenciales
    //Configuración el pines como una entrada.
    pinMode(INTENSIDAD_PIN, INPUT);
}

void loop() {
  int intensidad = analogRead(INTENSIDAD_PIN); //Lee el valor del LDR
  intensidadMap = map(intensidad, 0, 3800, 0, 100); //Se realiza un mapeo

  if (Firebase.ready()) { //Verifica si la conexión a Firebase está lista 
      Firebase.RTDB.setInt(&fbdo, "/temperatura/intensidad", intensidadMap); //Envía el valor de la intensidad a la base de datos

      Serial.println(intensidadMap); //Imprime el valor en el monitor serial
      delay(2000);// Delay para el envio de datos
  }
}