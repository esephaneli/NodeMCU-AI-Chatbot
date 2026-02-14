/*
 * ============================================
 *  NodeMCU AI Chatbot Web Server
 *  Google Gemini API (Free Tier)
 * ============================================
 * 
 *  Author  : [GitHub kullanıcı adınız]
 *  License : MIT
 *  Repo    : https://github.com/[kullanici]/NodeMCU-AI-Chatbot
 * 
 *  Gereksinimler:
 *    - Arduino IDE 2.x
 *    - ESP8266 Board Paketi (v3.1.x+)
 *    - ArduinoJson Kütüphanesi (v7.x)
 * 
 *  Detaylı kurulum için README.md dosyasına bakın.
 */

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

// ============================================================
//  AYARLAR — Bu 3 değeri kendi bilgilerinizle doldurun
// ============================================================
const char* ssid     = "WIFI_ADINIZ";           // WiFi ağ adı
const char* password = "WIFI_SIFRENIZ";          // WiFi şifresi
const char* apiKey   = "GEMINI_API_ANAHTARINIZ"; // Gemini API Key
// ============================================================

// Model seçimi (ücretsiz katman):
//   "gemini-2.5-flash-lite"  → ~1000 istek/gün (önerilen)
//   "gemini-2.5-flash"       → ~250 istek/gün  (daha akıllı)
const char* modelName = "gemini-2.5-flash-lite";

// Yanıt ayarları
const int   maxTokens   = 500;   // Maks yanıt uzunluğu (token)
const float temperature = 0.7;   // Yaratıcılık: 0.0 (tutarlı) - 1.0 (yaratıcı)

ESP8266WebServer server(80);

// ==================== WEB ARAYÜZÜ ====================
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
    NodeMCU ESP8266 &bull; Gemini AI &bull; Ücretsiz Katman
  </div>

<script>
  const chatArea = document.getElementById('chatArea');
  const msgInput = document.getElementById('msgInput');
  const sendBtn  = document.getElementById('sendBtn');
  const typing   = document.getElementById('typing');

  // Enter tuşu ile gönder
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
  client.setInsecure(); // ESP8266 RAM sınırlı — sertifika doğrulama atlanıyor

  HTTPClient https;
  
  // API endpoint oluştur
  String url = "https://generativelanguage.googleapis.com/v1beta/models/";
  url += modelName;
  url += ":generateContent?key=";
  url += apiKey;

  // JSON istek gövdesi
  JsonDocument doc;
  JsonArray contents = doc["contents"].to<JsonArray>();
  JsonObject content = contents.add<JsonObject>();
  content["role"] = "user";
  JsonArray parts = content["parts"].to<JsonArray>();
  JsonObject part = parts.add<JsonObject>();
  part["text"] = userMessage;

  // Üretim ayarları
  JsonObject genConfig = doc["generationConfig"].to<JsonObject>();
  genConfig["maxOutputTokens"] = maxTokens;
  genConfig["temperature"] = temperature;

  String requestBody;
  serializeJson(doc, requestBody);

  Serial.println("[API] İstek gönderiliyor...");

  if (https.begin(client, url)) {
    https.addHeader("Content-Type", "application/json");
    https.setTimeout(15000);
    
    int httpCode = https.POST(requestBody);
    Serial.println("[API] HTTP Kodu: " + String(httpCode));

    if (httpCode == HTTP_CODE_OK) {
      String payload = https.getString();
      
      JsonDocument responseDoc;
      DeserializationError error = deserializeJson(responseDoc, payload);
      
      if (!error) {
        const char* text = responseDoc["candidates"][0]["content"]["parts"][0]["text"];
        if (text) {
          String reply = String(text);
          Serial.println("[API] Yanıt alındı (" + String(reply.length()) + " karakter)");
          https.end();
          return reply;
        }
      }
      
      https.end();
      return "⚠️ Yanıt işlenemedi. Tekrar deneyin.";
      
    } else if (httpCode == 429) {
      https.end();
      return "⏳ API limiti aşıldı. Biraz bekleyip tekrar deneyin.";
    } else if (httpCode == 400) {
      https.end();
      return "❌ Geçersiz istek. API anahtarınızı kontrol edin.";
    } else if (httpCode == 403) {
      https.end();
      return "🔒 API erişimi reddedildi. API anahtarınızın aktif olduğundan emin olun.";
    } else {
      String errorPayload = https.getString();
      Serial.println("[API] Hata: " + errorPayload.substring(0, 200));
      https.end();
      return "❌ API Hatası (Kod: " + String(httpCode) + ")";
    }
  }
  
  return "🌐 Sunucuya bağlanılamadı. İnternet bağlantınızı kontrol edin.";
}

// ==================== WEB SUNUCU ====================
void handleRoot() {
  server.send_P(200, "text/html", htmlPage);
}

void handleChat() {
  if (!server.hasArg("msg")) {
    server.send(400, "application/json", "{\"error\":\"Mesaj bulunamadı\"}");
    return;
  }

  String userMsg = server.arg("msg");
  userMsg.trim();
  
  if (userMsg.length() == 0) {
    server.send(400, "application/json", "{\"error\":\"Boş mesaj\"}");
    return;
  }
  
  if (userMsg.length() > 500) {
    userMsg = userMsg.substring(0, 500); // RAM koruması
  }

  Serial.println("\n>> Kullanıcı: " + userMsg);

  String reply = callGeminiAPI(userMsg);
  
  JsonDocument doc;
  doc["reply"] = reply;
  String jsonResponse;
  serializeJson(doc, jsonResponse);
  
  server.send(200, "application/json", jsonResponse);
}

// Durum bilgisi endpoint'i
void handleStatus() {
  JsonDocument doc;
  doc["status"] = "online";
  doc["model"] = modelName;
  doc["uptime"] = millis() / 1000;
  doc["freeHeap"] = ESP.getFreeHeap();
  doc["rssi"] = WiFi.RSSI();
  
  String json;
  serializeJson(doc, json);
  server.send(200, "application/json", json);
}

void handleNotFound() {
  server.send(404, "text/plain", "404 - Sayfa bulunamadi");
}

// ==================== SETUP & LOOP ====================
void setup() {
  Serial.begin(115200);
  delay(100);
  
  Serial.println("\n");
  Serial.println("╔══════════════════════════════════╗");
  Serial.println("║   NodeMCU AI Chatbot Server      ║");
  Serial.println("║   Gemini API - Free Tier         ║");
  Serial.println("╚══════════════════════════════════╝");

  // WiFi bağlantısı
  Serial.print("\n[WiFi] Bağlanılıyor: ");
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
    Serial.println("\n[WiFi] ✓ Bağlandı!");
    Serial.println("[WiFi] IP: " + WiFi.localIP().toString());
    Serial.println("[WiFi] Sinyal: " + String(WiFi.RSSI()) + " dBm");
    Serial.println("\n→ Tarayıcınızda açın: http://" + WiFi.localIP().toString());
  } else {
    Serial.println("\n[WiFi] ✗ Bağlantı başarısız!");
    Serial.println("[WiFi] SSID ve şifreyi kontrol edin.");
    Serial.println("[WiFi] Not: Sadece 2.4GHz ağlar desteklenir.");
    return;
  }

  // Sunucu rotaları
  server.on("/", handleRoot);
  server.on("/chat", HTTP_POST, handleChat);
  server.on("/status", HTTP_GET, handleStatus);
  server.onNotFound(handleNotFound);
  
  server.begin();
  Serial.println("\n[Server] ✓ Web sunucu başlatıldı (port 80)");
  Serial.println("[Server] Model: " + String(modelName));
  Serial.println("[Server] Hazır!\n");
}

void loop() {
  server.handleClient();
}
