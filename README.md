# Where's My Water? 🌱💧  
**Sistem Penyiraman Tanaman Cerdas**

## Deskripsi Proyek  
Proyek ini merupakan sistem otomatis untuk menyiram tanaman berdasarkan tingkat kelembaban tanah yang terdeteksi menggunakan sensor YL-69/FC-28. Sistem ini menggunakan mikrokontroler ESP32 dan terintegrasi dengan platform **Blynk**, sehingga pengguna dapat memantau kondisi tanah secara real-time dan mengontrol pompa air secara manual melalui aplikasi.

---

## Fitur  
- 🌡️ **Penyiraman Otomatis**  
  Sistem akan menyalakan pompa air secara otomatis jika kelembaban tanah berada di bawah ambang batas tertentu.  

- 📊 **Pemantauan Real-Time**  
  Pengguna dapat memantau tingkat kelembaban tanah dan status pompa air (ON/OFF) melalui aplikasi Blynk.  

- 🛠️ **Kontrol Manual**  
  Pengguna dapat menyalakan atau mematikan pompa air secara manual melalui aplikasi Blynk sesuai kebutuhan.  

---

## Komponen Perangkat Keras  
- **ESP32**: Mikrokontroler untuk pemrosesan data dan komunikasi WiFi.  
- **Sensor Kelembaban Tanah YL-69/FC-28**: Mengukur tingkat kelembaban tanah.  
- **Pompa Air**: Untuk menyiram tanaman.  
- **Modul Relay**: Mengontrol daya pompa air.  
- **Catu Daya**: Untuk ESP32 dan pompa.  
- **WiFi**: Untuk integrasi dengan Blynk.  

---

## Cara Kerja  
1. **Pengumpulan Data Sensor**  
   Sensor YL-69 membaca tingkat kelembaban tanah setiap 5 detik menggunakan timer pada ESP32.  
2. **Pemrosesan Data**  
   Jika tingkat kelembaban tanah berada di bawah ambang batas, ESP32 akan menyalakan pompa air melalui modul relay.  
3. **Integrasi Aplikasi**  
   - Data dari sensor dan status pompa dikirim ke aplikasi Blynk untuk pemantauan.  
   - Pengguna dapat mengontrol pompa air secara manual melalui antarmuka Blynk.  

---

## Perangkat Lunak dan Library  
- **Arduino IDE**: Untuk memprogram ESP32.  
- **Library Blynk**: Untuk integrasi dengan platform Blynk.  
- **Library Timer**: Untuk pembacaan sensor secara berkala.  

---

## Langkah Instalasi  
1. Clone repositori ini:  
   ```bash
   git clone https://github.com/username-kamu/where-s-my-water.git
