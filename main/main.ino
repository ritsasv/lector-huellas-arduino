#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>

// Configuración del puerto serie para el lector de huellas
SoftwareSerial mySerial(2, 3); // RX, TX
Adafruit_Fingerprint finger(&mySerial);

void setup() {
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Iniciando Lector de Huellas...");

  finger.begin(57600);
  if (finger.verifyPassword()) {
    Serial.println("Lector de huellas encontrado.");
  } else {
    Serial.println("Error al conectar con el lector de huellas.");
    while (1);
  }
  Serial.println("\nESTAMOS EN VIVO CON RITSA");
  Serial.println("Para registrar una nueva huella, presiona 'n' en el monitor serial.");
}

void loop() {
  if (Serial.available()) {
    char command = Serial.read();
    if (command == 'n') {
      enrollFinger();
    }
  }

  int result = getFingerprintID();
  if (result == FINGERPRINT_OK) {
    Serial.print("Huella identificada con ID: ");
    Serial.println(finger.fingerID);
  } else if (result == FINGERPRINT_NOTFOUND) {
    Serial.println("No se encontró ninguna huella coincidente.");
  }
  delay(50); // Reducir el uso de la CPU y evitar sobrecargas
}

// Función para capturar e identificar huellas
int getFingerprintID() {
  int result = finger.getImage();
  if (result == FINGERPRINT_NOFINGER) return FINGERPRINT_NOFINGER;
  if (result != FINGERPRINT_OK) return FINGERPRINT_NOTFOUND;

  result = finger.image2Tz();
  if (result != FINGERPRINT_OK) return FINGERPRINT_NOTFOUND;

  result = finger.fingerFastSearch();
  if (result == FINGERPRINT_OK) return FINGERPRINT_OK;
  return FINGERPRINT_NOTFOUND;
}

// Función para registrar una nueva huella
void enrollFinger() {
  int id = 0;
  Serial.println("Introduce un ID para la huella (0-127):");
  while (!Serial.available());
  id = Serial.parseInt();
  if (id < 0 || id > 127) {
    Serial.println("ID fuera de rango.");
    return;
  }

  Serial.println("Coloca el dedo en el lector...");
  while (finger.getImage() != FINGERPRINT_OK);

  if (finger.image2Tz(1) != FINGERPRINT_OK) {
    Serial.println("Error al procesar imagen.");
    return;
  }

  Serial.println("Retira el dedo.");
  delay(2000);
  while (finger.getImage() != FINGERPRINT_NOFINGER);
  Serial.println("Coloca el mismo dedo nuevamente...");
  
  while (finger.getImage() != FINGERPRINT_OK);
  if (finger.image2Tz(2) != FINGERPRINT_OK) {
    Serial.println("Error al procesar imagen.");
    return;
  }

  if (finger.createModel() != FINGERPRINT_OK) {
    Serial.println("Error al crear modelo.");
    return;
  }

  if (finger.storeModel(id) != FINGERPRINT_OK) {
    Serial.println("Error al guardar modelo.");
    return;
  }

  Serial.println("Huella registrada exitosamente.");
  Serial.println("\nEl lector está activo nuevamente y esperando huellas.");
}