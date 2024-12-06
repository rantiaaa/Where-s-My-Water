# Where's My Water? 🌱💧  
**Sistem Penyiraman Tanaman Cerdas**

## Deskripsi Proyek  
Proyek ini merupakan sistem penyiraman tanaman otomatis berbasis IoT menggunakan **ESP32** dan platform **Blynk**. Sistem bekerja dengan membaca tingkat kelembaban tanah setiap 5 detik menggunakan sensor YL-69, mengontrol pompa air secara otomatis berdasarkan ambang batas kelembaban, serta memberikan opsi kontrol manual melalui aplikasi. Sistem ini diimplementasikan menggunakan multitasking FreeRTOS untuk efisiensi.

---

## Fitur  
- 🌡️ **Penyiraman Otomatis**  
  Pompa air menyala otomatis jika kelembaban tanah di bawah ambang batas yang ditentukan melalui slider di aplikasi Blynk.  

- 📊 **Pemantauan Real-Time**  
  Aplikasi Blynk menampilkan tingkat kelembaban tanah dan status pompa air (ON/OFF).  

- 🛠️ **Kontrol Manual**  
  Pengguna dapat menyalakan/mematikan pompa air secara manual melalui tombol di aplikasi Blynk.  

- ⏳ **Multitasking dengan FreeRTOS**  
  Sistem menggunakan dua task paralel:  
  - **TaskSensor**: Membaca sensor kelembaban setiap 5 detik.  
  - **TaskPump**: Mengatur logika pompa air setiap 0,5 detik.  

---

## Komponen Perangkat Keras  
- **ESP32**: Mikrokontroler untuk pemrosesan data dan komunikasi WiFi.  
- **Sensor Kelembaban Tanah YL-69**: Mengukur tingkat kelembaban tanah.  
- **Pompa Air & Modul Relay**: Untuk penyiraman tanaman.  
- **Catu Daya**: Untuk ESP32 dan pompa.  

---

## Diagram Virtual Pin di Blynk  
- **V0 (PUMP_BUTTON)**: Tombol untuk mengontrol pompa air secara manual.  
- **V1 (TRESHOLD_SLIDER)**: Slider untuk mengatur ambang batas kelembaban.  
- **V2 (MOISTURE_LABEL)**: Label untuk menampilkan tingkat kelembaban tanah.  
- **V3 (LED_INDICATOR)**: Indikator LED status pompa air (ON/OFF).  

---

## Cara Kerja  
1. **Pengumpulan Data Sensor**  
   Task `vTaskSensor` membaca kelembaban tanah setiap 5 detik dan mengirim data ke aplikasi Blynk.  

2. **Logika Pompa**  
   Task `vTaskPump` menentukan status pompa:  
   - **Otomatis ON** jika kelembaban < ambang batas (diatur dengan slider).  
   - **Manual ON/OFF** berdasarkan tombol di aplikasi Blynk.  

3. **Integrasi dengan Blynk**  
   - Data dari sensor ditampilkan di aplikasi Blynk.  
   - Kontrol pompa dapat dilakukan secara manual atau otomatis.  

---

## Instalasi dan Penggunaan  
1. Clone repositori ini:  
   ```bash
   git clone https://github.com/username-kamu/where-s-my-water.git
