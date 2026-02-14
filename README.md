# 🤖 NodeMCU AI Chatbot Web Server

<div align="center">

![ESP8266](https://img.shields.io/badge/ESP8266-NodeMCU-blue?style=for-the-badge&logo=espressif)
![Gemini](https://img.shields.io/badge/Google-Gemini_AI-4285F4?style=for-the-badge&logo=google)
![License](https://img.shields.io/badge/License-MIT-green?style=for-the-badge)
![Cost](https://img.shields.io/badge/Maliyet-0₺-brightgreen?style=for-the-badge)

**NodeMCU ESP8266 üzerinde çalışan, Google Gemini API ile güçlendirilmiş ücretsiz AI chatbot.**

Tarayıcınızdan bağlanın, yapay zeka ile sohbet edin. Sunucu yok, ücret yok.

[🎬 Demo Video](#demo) · [📦 Kurulum](#kurulum) · [⚙️ Ayarlar](#ayarlar) · [❓ SSS](#sss)

</div>

---


## 🎬 Demo

> (https://www.linkedin.com/feed/update/urn:li:groupPost:10050108-7428126316568219648?utm_source=share&utm_medium=member_desktop&rcm=ACoAADq0uBEB37Eb8-GsG0Tn99dvBYCo-vGxDYs)

---

## ✨ Özellikler

- 🔌 **Tek kart** — Sadece NodeMCU ESP8266 yeterli
- 💰 **Tamamen ücretsiz** — Gemini API ücretsiz katmanı (kredi kartı gerekmez)
- 🌐 **Web tabanlı arayüz** — Herhangi bir cihazdan tarayıcı ile erişin
- 📱 **Responsive tasarım** — Mobil ve masaüstü uyumlu
- ⚡ **Kolay kurulum** — 3 değişken doldurup yükleyin, bitti
- 🔒 **Güvenli** — Yerel ağda çalışır, dışarıdan erişim yok

---

## 🏗️ Mimari

```
┌─────────────────┐      HTTP       ┌──────────────────┐
│   Tarayıcı      │ ◄────WiFi────► │  NodeMCU ESP8266  │
│ (Telefon / PC)  │                 │  - Web Server     │
└─────────────────┘                 │  - HTML Arayüz    │
                                    └────────┬─────────┘
                                             │ HTTPS
                                             ▼
                                    ┌──────────────────┐
                                    │  Google Gemini    │
                                    │  API (Ücretsiz)   │
                                    └──────────────────┘
```

---

## 📦 Gereksinimler

### Donanım

| Malzeme | Açıklama |
|---------|----------|
| **NodeMCU ESP8266** | Ana kart (ESP-12E modülü) |
| **Mikro USB Kablo** | Bilgisayara bağlantı ve güç |

### Yazılım

| Yazılım | Versiyon | Link |
|---------|----------|------|
| Arduino IDE | 2.x | [İndir](https://www.arduino.cc/en/software) |
| ESP8266 Board Paketi | 3.1.x+ | Boards Manager'dan |
| ArduinoJson | 7.x | Library Manager'dan |

### Hesap

- **Google hesabı** (Gmail yeterli)
- **Gemini API Key** — [aistudio.google.com](https://aistudio.google.com) (ücretsiz)

---

## 🚀 Kurulum

### 1️⃣ Arduino IDE Hazırlığı

**ESP8266 board paketi:**

1. Arduino IDE → `File` → `Preferences`
2. "Additional Board Manager URLs" kutusuna ekleyin:
   ```
   http://arduino.esp8266.com/stable/package_esp8266com_index.json
   ```
3. `Tools` → `Board` → `Boards Manager` → `esp8266` arayın → **Install**

**ArduinoJson kütüphanesi:**

1. `Sketch` → `Include Library` → `Manage Libraries`
2. `ArduinoJson` arayın → **Install** (v7.x)

### 2️⃣ Gemini API Anahtarı (Ücretsiz)

1. [aistudio.google.com](https://aistudio.google.com) adresine gidin
2. Google hesabınızla giriş yapın
3. Sol menü → **Get API Key** → **Create API Key**
4. Oluşan anahtarı kopyalayın

> 💡 Kredi kartı gerekmez. Ücretsiz katman limitleri:
> | Model | İstek/Gün | İstek/Dakika |
> |-------|-----------|--------------|
> | `gemini-2.5-flash-lite` | ~1000 | 15 |
> | `gemini-2.5-flash` | ~250 | 10 |

### 3️⃣ Kodu Yükleme

1. Bu repoyu klonlayın:
   ```bash
   git clone https://github.com/[KULLANICI_ADINIZ]/NodeMCU-AI-Chatbot.git
   ```

2. Arduino IDE'de `src/NodeMCU_AI_Chatbot.ino` dosyasını açın

3. **Üstteki 3 değeri doldurun:**
   ```cpp
   const char* ssid     = "WIFI_ADINIZ";           // WiFi ağ adınız
   const char* password = "WIFI_SIFRENIZ";          // WiFi şifreniz
   const char* apiKey   = "GEMINI_API_ANAHTARINIZ"; // API anahtarınız
   ```

4. Board ayarları:
   - `Tools` → `Board` → **NodeMCU 1.0 (ESP-12E Module)**
   - `Tools` → `Port` → Doğru COM portunu seçin

5. **Upload** butonuna basın (→)

### 4️⃣ Çalıştırma

1. `Tools` → `Serial Monitor` açın (baud: **115200**)
2. IP adresini not alın (örn: `192.168.1.105`)
3. Aynı WiFi'deki herhangi bir cihazda tarayıcıyı açın
4. Adres çubuğuna IP'yi yazın: `http://192.168.1.105`
5. Sohbete başlayın! 🎉

---

## ⚙️ Ayarlar

### Model Değiştirme

```cpp
// En yüksek günlük limit (önerilen):
const char* modelName = "gemini-2.5-flash-lite";

// Daha akıllı, daha az günlük hak:
const char* modelName = "gemini-2.5-flash";
```

### Yanıt Uzunluğu

```cpp
const int maxTokens = 500;   // Kısa yanıtlar (önerilen, RAM dostu)
const int maxTokens = 1000;  // Daha uzun yanıtlar
```

### Yaratıcılık Seviyesi

```cpp
const float temperature = 0.3;  // Tutarlı, resmi
const float temperature = 0.7;  // Dengeli (varsayılan)
const float temperature = 1.0;  // Yaratıcı, sürprizli
```

---

## 🔌 API Endpoints

| Endpoint | Method | Açıklama |
|----------|--------|----------|
| `/` | GET | Web arayüzü (chatbot sayfası) |
| `/chat` | POST | AI'ya mesaj gönder (`msg` parametresi) |
| `/status` | GET | Sistem durumu (uptime, RAM, sinyal gücü) |

### Örnek API Kullanımı

```bash
# Chatbot'a mesaj gönder
curl -X POST http://192.168.1.105/chat \
  -d "msg=Merhaba, nasılsın?"

# Sistem durumu
curl http://192.168.1.105/status
```

---

## ❓ SSS

<details>
<summary><b>5GHz WiFi çalışır mı?</b></summary>
Hayır. ESP8266 sadece 2.4GHz WiFi destekler. Modem ayarlarınızdan 2.4GHz ağın açık olduğundan emin olun.
</details>

<details>
<summary><b>Dışarıdan (internetten) erişilebilir mi?</b></summary>
Varsayılan olarak hayır. NodeMCU yerel IP alır, sadece aynı WiFi ağındaki cihazlar erişebilir. Dışarıdan erişim için router'da port forwarding gerekir (önerilmez).
</details>

<details>
<summary><b>"API Hatası 429" alıyorum</b></summary>
Günlük ücretsiz istek limitini aştınız. Ertesi gün sıfırlanır. Daha yüksek limit için <code>gemini-2.5-flash-lite</code> modelini kullanın.
</details>

<details>
<summary><b>API anahtarım çalışmıyor</b></summary>

- Anahtarı doğru kopyaladığınızdan emin olun (başında/sonunda boşluk olmasın)
- [aistudio.google.com](https://aistudio.google.com) → API Keys → anahtarın aktif olduğunu kontrol edin
- Türkiye'den erişimde sorun varsa VPN deneyin
</details>

<details>
<summary><b>Serial Monitor'de IP adresi görünmüyor</b></summary>

- Baud rate'i **115200** olarak ayarlayın
- WiFi adı ve şifresini kontrol edin
- NodeMCU'yu resetleyin (RST butonu)
</details>

<details>
<summary><b>Birden fazla kişi aynı anda kullanabilir mi?</b></summary>
Evet, aynı ağdaki herkes tarayıcıdan bağlanabilir. Ancak ESP8266 tek çekirdekli olduğundan eş zamanlı istekler sırayla işlenir.
</details>

---

## 🛠️ Geliştirme Fikirleri

- [ ] 💾 Sohbet geçmişini SPIFFS'e kaydetme
- [ ] 🏠 GPIO pin kontrolü (akıllı ev komutları)
- [ ] 🌡️ Sensör verisi okuma ve AI'ya aktarma
- [ ] 🔐 Web arayüzüne şifre koruması
- [ ] 📡 mDNS desteği (`http://chatbot.local`)
- [ ] 🌍 Ngrok ile dışarıdan erişim

---

## 📁 Klasör Yapısı

```
NodeMCU-AI-Chatbot/
├── src/
│   └── NodeMCU_AI_Chatbot.ino    # Ana Arduino kodu
├── LICENSE
└── README.md
```

---

## 📄 Lisans

Bu proje [MIT Lisansı](LICENSE) ile lisanslanmıştır. Özgürce kullanın, değiştirin, dağıtın.

---

## 🤝 Katkıda Bulunma

1. Fork edin
2. Feature branch oluşturun (`git checkout -b feature/yeni-ozellik`)
3. Commit atın (`git commit -m 'Yeni özellik eklendi'`)
4. Push edin (`git push origin feature/yeni-ozellik`)
5. Pull Request açın

---

## ⭐ Destek

Projeyi beğendiyseniz ⭐ vermeyi unutmayın!

Sorularınız için www.linkedin.com/in/emrehan-şephanelioğlu-101a22235 üzeriden iletişime geçebilirsiniz.
