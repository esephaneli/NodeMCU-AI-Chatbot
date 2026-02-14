/*
 * ============================================
 *  NodeMCU AI Chatbot Web Server
 *  Gemini API (Ücretsiz Katman) ile Çalışır
 * ============================================
 * 
 * Gereksinimler:
 *   - Arduino IDE (2.x veya 1.8.x)
 *   - ESP8266 Board Paketi (Boards Manager'dan yükleyin)
 *   - ArduinoJson kütüphanesi (Library Manager'dan yükleyin)
 * 
 * Kurulum:
 *   1. Arduino IDE → File → Preferences → Additional Board Manager URLs:
 *      http://arduino.esp8266.com/stable/package_esp8266com_index.json
 *   2. Tools → Board → Boards Manager → "esp8266" arayın ve yükleyin
 *   3. Sketch → Include Library → Manage Libraries → "ArduinoJson" yükleyin
 *   4. Tools → Board → NodeMCU 1.0 (ESP-12E Module) seçin
 *   5. Aşağıdaki WiFi ve API bilgilerini doldurun
 *   6. Upload butonuna basın
 *   7. Serial Monitor'ü açın (115200 baud) → IP adresini görün
 *   8. Tarayıcınızda o IP adresine gidin
 * 
 * Gemini API Key Alma:
 *   1. https://aistudio.google.com adresine gidin
 *   2. Google hesabınızla giriş yapın
 *   3. Sol menüden "Get API Key" → "Create API Key" tıklayın
 *   4. Oluşan anahtarı aşağıya yapıştırın
 */

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

// ========== AYARLAR ==========
const char* ssid     = "WIFI_ADINIZ";        // WiFi adınızı girin
const char* password = "WIFI_SIFRENIZ";       // WiFi şifrenizi girin
const char* apiKey   = "GEMINI_API_KEY";      // Gemini API anahtarınızı girin
const char* modelName = "gemini-2.5-flash-lite"; // Ücretsiz: gemini-2.5-flash-lite (en yüksek limit)
// =============================

ESP8266WebServer server(80);

// ==================== WEB ARAYÜZÜ (HTML) ====================
const char htmlPage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="tr">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>NodeMCU AI Chatbot</title>
  <style>
    * { margin: 0; padding: 0; box-sizing: border-box; }
    
    body {
      font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
      background: #0f0f23;
      color: #e2e8f0;
      height: 100vh;
      display: flex;
      flex-direction: column;
    }

    /* Header */
    .header {
      background: linear-gradient(135deg, #1a1a3e 0%, #2d1b69 100%);
      padding: 16px 20px;
      display: flex;
      align-items: center;
      gap: 12px;
      border-bottom: 1px solid #2d2d5e;
      flex-shrink: 0;
    }
    .header .logo {
      width: 40px; height: 40px;
      background: linear-gradient(135deg, #4f46e5, #7c3aed);
      border-radius: 12px;
      display: flex; align-items: center; justify-content: center;
      font-size: 20px;
    }
    .header h1 { font-size: 18px; font-weight: 600; }
    .header .status {
      margin-left: auto;
      font-size: 12px;
      color: #22c55e;
      display: flex; align-items: center; gap: 6px;
    }
    .header .status::before {
      content: '';
      width: 8px; height: 8px;
      background: #22c55e;
      border-radius: 50%;
      display: inline-block;
    }

    /* Chat Area */
    .chat-area {
      flex: 1;
      overflow-y: auto;
      padding: 20px;
      display: flex;
      flex-direction: column;
      gap: 16px;
    }

    .message {
      max-width: 85%;
      padding: 12px 16px;
      border-radius: 16px;
      line-height: 1.6;
      font-size: 14px;
      animation: fadeIn 0.3s ease;
      white-space: pre-wrap;
      word-wrap: break-word;
    }

    @keyframes fadeIn {
      from { opacity: 0; transform: translateY(8px); }
      to { opacity: 1; transform: translateY(0); }
    }

    .message.user {
      align-self: flex-end;
      background: linear-gradient(135deg, #4f46e5, #6366f1);
      color: white;
      border-bottom-right-radius: 4px;
    }

    .message.bot {
      align-self: flex-start;
      background: #1e1e3f;
      border: 1px solid #2d2d5e;
      border-bottom-left-radius: 4px;
    }

    .message.bot .label {
      font-size: 11px;
      color: #818cf8;
      margin-bottom: 4px;
      font-weight: 600;
    }

    .message.system {
      align-self: center;
      background: #1a1a2e;
      color: #64748b;
      font-size: 13px;
      padding: 8px 16px;
      border-radius: 20px;
    }

    /* Typing indicator */
    .typing {
      display: none;
      align-self: flex-start;
      padding: 12px 16px;
      background: #1e1e3f;
      border: 1px solid #2d2d5e;
      border-radius: 16px;
      border-bottom-left-radius: 4px;
    }
    .typing.active { display: flex; gap: 4px; align-items: center; }
    .typing span {
      width: 8px; height: 8px;
      background: #6366f1;
      border-radius: 50%;
      animation: bounce 1.4s infinite ease-in-out;
    }
    .typing span:nth-child(2) { animation-delay: 0.2s; }
    .typing span:nth-child(3) { animation-delay: 0.4s; }
    @keyframes bounce {
      0%, 80%, 100% { transform: scale(0.6); opacity: 0.4; }
      40% { transform: scale(1); opacity: 1; }
    }

    /* Input Area */
    .input-area {
      padding: 16px 20px;
      background: #12122b;
      border-top: 1px solid #2d2d5e;
      display: flex;
      gap: 10px;
      flex-shrink: 0;
    }

    .input-area input {
      flex: 1;
      padding: 12px 16px;
      background: #1e1e3f;
      border: 1px solid #2d2d5e;
      border-radius: 12px;
      color: #e2e8f0;
      font-size: 14px;
      outline: none;
      transition: border-color 0.2s;
    }
    .input-area input:focus { border-color: #6366f1; }
    .input-area input::placeholder { color: #475569; }

    .input-area button {
      padding: 12px 20px;
      background: linear-gradient(135deg, #4f46e5, #7c3aed);
      border: none;
      border-radius: 12px;
      color: white;
      font-size: 14px;
      font-weight: 600;
      cursor: pointer;
      transition: opacity 0.2s;
      white-space: nowrap;
    }
    .input-area button:hover { opacity: 0.9; }
    .input-area button:disabled { opacity: 0.5; cursor: not-allowed; }

    /* Info bar */
    .info-bar {
      text-align: center;
      padding: 8px;
      font-size: 11px;
      color: #475569;
      background: #0a0a1a;
      flex-shrink: 0;
    }
  </style>
</head>
<body>

  <div class="header">
    <div class="logo">🤖</div>
    <div>
      <h1>NodeMCU AI Chatbot</h1>
    </div>
    <div class="status">Çevrimiçi</div>
  </div>

  <div class="chat-area" id="chatArea">
    <div class="message system">Gemini AI ile güçlendirilmiş chatbot. Bir şey sorun!</div>
  </div>

  <div class="typing" id="typing">
    <span></span><span></span><span></span>
  </div>

  <div class="input-area">
    <input type="text" id="msgInput" placeholder="Mesajınızı yazın..." autocomplete="off" />
    <button id="sendBtn" onclick="sendMessage()">Gönder</button>
  </div>

  <div class="info-bar">
    NodeMCU ESP8266 üzerinde çalışıyor &bull; Gemini AI &bull; Ücretsiz Katman
  </div>

<script>
  const chatArea = document.getElementById('chatArea');
  const msgInput = document.getElementById('msgInput');
  const sendBtn  = document.getElementById('sendBtn');
  const typing   = document.getElementById('typing');

  msgInput.addEventListener('keypress', function(e) {
    if (e.key === 'Enter' && !sendBtn.disabled) sendMessage();
  });

  function addMessage(text, type) {
    const div = document.createElement('div');
    div.className = 'message ' + type;
    if (type === 'bot') {
      div.innerHTML = '<div class="label">🤖 Gemini AI</div>' + escapeHtml(text);
    } else {
      div.textContent = text;
    }
    chatArea.appendChild(div);
    chatArea.scrollTop = chatArea.scrollHeight;
  }

  function escapeHtml(text) {
    const div = document.createElement('div');
    div.textContent = text;
    return div.innerHTML;
  }

  async function sendMessage() {
    const msg = msgInput.value.trim();
    if (!msg) return;

    addMessage(msg, 'user');
    msgInput.value = '';
    sendBtn.disabled = true;
    typing.classList.add('active');
    chatArea.scrollTop = chatArea.scrollHeight;

    try {
      const response = await fetch('/chat', {
        method: 'POST',
        headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
        body: 'msg=' + encodeURIComponent(msg)
      });
      const data = await response.json();
      
      if (data.reply) {
        addMessage(data.reply, 'bot');
      } else if (data.error) {
        addMessage('Hata: ' + data.error, 'system');
      }
    } catch (err) {
      addMessage('Bağlantı hatası. Tekrar deneyin.', 'system');
    }

    typing.classList.remove('active');
    sendBtn.disabled = false;
    msgInput.focus();
  }
</script>
</body>
</html>
)rawliteral";

// ==================== GEMINI API ÇAĞRISI ====================
String callGeminiAPI(String userMessage) {
  WiFiClientSecure client;
  client.setInsecure(); // NodeMCU'da sertifika doğrulama atla (RAM sınırlı)

  HTTPClient https;
  
  String url = "https://generativelanguage.googleapis.com/v1beta/models/";
  url += modelName;
  url += ":generateContent?key=";
  url += apiKey;

  // JSON istek gövdesi oluştur
  JsonDocument doc;
  JsonArray contents = doc["contents"].to<JsonArray>();
  JsonObject content = contents.add<JsonObject>();
  content["role"] = "user";
  JsonArray parts = content["parts"].to<JsonArray>();
  JsonObject part = parts.add<JsonObject>();
  part["text"] = userMessage;

  // Sistem yönergesi (isteğe bağlı - özelleştirebilirsiniz)
  JsonObject genConfig = doc["generationConfig"].to<JsonObject>();
  genConfig["maxOutputTokens"] = 500;  // Yanıt uzunluğu (NodeMCU RAM'i için kısa tutun)
  genConfig["temperature"] = 0.7;

  String requestBody;
  serializeJson(doc, requestBody);

  Serial.println("API'ye istek gönderiliyor...");
  Serial.println("URL: " + url.substring(0, 80) + "...");

  if (https.begin(client, url)) {
    https.addHeader("Content-Type", "application/json");
    https.setTimeout(15000); // 15 saniye timeout
    
    int httpCode = https.POST(requestBody);
    Serial.println("HTTP Kodu: " + String(httpCode));

    if (httpCode == HTTP_CODE_OK) {
      String payload = https.getString();
      
      // Yanıtı parse et
      JsonDocument responseDoc;
      DeserializationError error = deserializeJson(responseDoc, payload);
      
      if (!error) {
        const char* text = responseDoc["candidates"][0]["content"]["parts"][0]["text"];
        if (text) {
          String reply = String(text);
          Serial.println("Yanıt: " + reply.substring(0, 100) + "...");
          https.end();
          return reply;
        }
      }
      
      Serial.println("JSON parse hatası");
      https.end();
      return "Yanıt işlenemedi. Tekrar deneyin.";
      
    } else if (httpCode == 429) {
      https.end();
      return "API limit aşıldı. Lütfen biraz bekleyip tekrar deneyin. (Ücretsiz katman: ~250-1000 istek/gün)";
    } else {
      String errorPayload = https.getString();
      Serial.println("Hata yanıtı: " + errorPayload.substring(0, 200));
      https.end();
      return "API Hatası (Kod: " + String(httpCode) + "). API anahtarınızı kontrol edin.";
    }
  }
  
  return "Sunucuya bağlanılamadı. İnternet bağlantınızı kontrol edin.";
}

// ==================== SUNUCU HANDLERs ====================
void handleRoot() {
  server.send_P(200, "text/html", htmlPage);
}

void handleChat() {
  if (!server.hasArg("msg")) {
    server.send(400, "application/json", "{\"error\":\"Mesaj bulunamadı\"}");
    return;
  }

  String userMsg = server.arg("msg");
  Serial.println("\n>> Kullanıcı: " + userMsg);

  String reply = callGeminiAPI(userMsg);
  
  // JSON yanıt oluştur
  JsonDocument doc;
  doc["reply"] = reply;
  String jsonResponse;
  serializeJson(doc, jsonResponse);
  
  server.send(200, "application/json", jsonResponse);
}

void handleNotFound() {
  server.send(404, "text/plain", "404 - Sayfa bulunamadi");
}

// ==================== SETUP & LOOP ====================
void setup() {
  Serial.begin(115200);
  delay(100);
  
  Serial.println("\n\n=============================");
  Serial.println(" NodeMCU AI Chatbot Server");
  Serial.println("=============================");

  // WiFi bağlantısı
  Serial.print("WiFi'ye bağlanılıyor: ");
  Serial.println(ssid);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 40) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✓ WiFi bağlandı!");
    Serial.print("IP Adresi: ");
    Serial.println(WiFi.localIP());
    Serial.println("\nTarayıcınızda şu adrese gidin:");
    Serial.print("http://");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n✗ WiFi bağlantısı başarısız!");
    Serial.println("SSID ve şifreyi kontrol edin.");
    return;
  }

  // Web sunucu rotaları
  server.on("/", handleRoot);
  server.on("/chat", HTTP_POST, handleChat);
  server.onNotFound(handleNotFound);
  
  server.begin();
  Serial.println("\n✓ Web sunucu başlatıldı!");
  Serial.println("=============================\n");
}

void loop() {
  server.handleClient();
}
