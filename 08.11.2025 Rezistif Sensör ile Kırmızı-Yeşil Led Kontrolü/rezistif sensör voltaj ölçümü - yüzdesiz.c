// ESP32 ADC ölçümü – GPIO34
// Not: GPIO18 ADC değildir. Lütfen sensör/ölçüm hattını GPIO34'e takın.

const int LED_1 = 22;
const int LED_2 = 23;
const int ADC_PIN = 34;  // Ölçüm pini (ADC1_CH6)
const int NUM_SAMPLES = 16; // Gürültü azaltmak için ortalama
// Eğer girişte voltaj bölücü varsa (R1 üstte, R2 altta), çarpanı buraya girin.
// Bölücü yoksa 1.0 bırakın.
const float DIVIDER_GAIN = 1.0; 

void setup() {
  Serial.begin(115200);
  delay(200);
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);

  // ADC çözünürlüğü ve attenuasyon ayarı
  analogReadResolution(12);                 // 0..4095
  analogSetPinAttenuation(ADC_PIN, ADC_11db); // ~0..3.3V aralığı

  // (İsteğe bağlı) ADC’yi bu pine bağla – yeni çekirdekte gerekmiyor ama zararı yok:
  // adcAttachPin(ADC_PIN);

  Serial.println("ESP32 ADC voltaj olcumu basladi (GPIO34)...");
}

void loop() {
  // Çoklu örnek al ve ortala
  uint32_t sum = 0;
  for (int i = 0; i < NUM_SAMPLES; i++) {
    sum += analogRead(ADC_PIN);
    delay(2);
  }
  uint16_t raw = sum / NUM_SAMPLES;

  // mV cinsinden oku (ESP32 cekirdegi kalibrasyonu kullanir)
  // Not: analogReadMilliVolts, attenuation ayariyla uyumludur.
  uint32_t mv_sum = 0;
  for (int i = 0; i < NUM_SAMPLES; i++) {
    mv_sum += analogReadMilliVolts(ADC_PIN);
    delay(2);
  }
  float mv = (float)mv_sum / NUM_SAMPLES;

  // Volt hesabı (bölücü varsa çarpanı uygula)
  float vin = (mv / 1000.0f) * DIVIDER_GAIN;
  
  if (vin > 1.9){
	   digitalWrite(LED_1, HIGH); 
	   digitalWrite(LED_2, LOW);	   // yan
  }
  else {
	   digitalWrite(LED_2, HIGH); 
	   digitalWrite(LED_1, LOW);
  }

  Serial.print("RAW: ");
  Serial.print(raw);
  Serial.print("  |  ADC(mV): ");
  Serial.print(mv, 1);
  Serial.print("  |  V_in: ");
  Serial.print(vin, 3);
  Serial.println(" V");

  delay(500);
}
