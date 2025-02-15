#include "esp_camera.h"
#include <WiFi.h>

// Replace with your network credentials
const char* ssid = "Galaxy";
const char* password = "987654321";

// Select the camera model
#define CAMERA_MODEL_AI_THINKER  // Use this for ESP32-CAM AI Thinker module

// Define camera pins for AI-Thinker module
#if defined(CAMERA_MODEL_AI_THINKER)
#define PWDN_GPIO     -1
#define RESET_GPIO    -1
#define XCLK_GPIO     0
#define SIOD_GPIO     26
#define SIOC_GPIO     27

#define Y9_GPIO       35
#define Y8_GPIO       34
#define Y7_GPIO       39
#define Y6_GPIO       36
#define Y5_GPIO       21
#define Y4_GPIO       19
#define Y3_GPIO       18
#define Y2_GPIO       5
#define VSYNC_GPIO    25
#define HREF_GPIO     23
#define PCLK_GPIO     22
#endif

#include "esp_http_server.h"

camera_fb_t * fb = NULL;

static esp_err_t stream_handler(httpd_req_t *req) {
    char* part_buf[64];
    static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=frame";
    static const char* _STREAM_BOUNDARY = "\r\n--frame\r\n";
    static const char* _STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %d\r\n\r\n";

    httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
    
    while (true) {
        fb = esp_camera_fb_get();
        if (!fb) {
            Serial.println("Camera capture failed");
            continue;
        }

        size_t hlen = snprintf((char *)part_buf, 64, _STREAM_PART, fb->len);
        httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
        httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
        httpd_resp_send_chunk(req, (const char *)fb->buf, fb->len);
        esp_camera_fb_return(fb);
    }
    return ESP_OK;
}

void startCameraServer() {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;

    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_uri_t stream_uri = {
            .uri = "/",
            .method = HTTP_GET,
            .handler = stream_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &stream_uri);
    }
}

void setup() {
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    Serial.println();

    // Wi-Fi connection
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.println();
    Serial.print("Connected to WiFi, IP address: ");
    Serial.println(WiFi.localIP());

    // Camera configuration
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO;
    config.pin_d1 = Y3_GPIO;
    config.pin_d2 = Y4_GPIO;
    config.pin_d3 = Y5_GPIO;
    config.pin_d4 = Y6_GPIO;
    config.pin_d5 = Y7_GPIO;
    config.pin_d6 = Y8_GPIO;
    config.pin_d7 = Y9_GPIO;
    config.pin_xclk = XCLK_GPIO;
    config.pin_pclk = PCLK_GPIO;
    config.pin_vsync = VSYNC_GPIO;
    config.pin_href = HREF_GPIO;
    config.pin_sscb_sda = SIOD_GPIO;
    config.pin_sscb_scl = SIOC_GPIO;
    config.pin_pwdn = PWDN_GPIO;
    config.pin_reset = RESET_GPIO;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;

    if (psramFound()) {
        config.frame_size = FRAMESIZE_UXGA; // 1600x1200
        config.jpeg_quality = 10; // Higher is worse quality
        config.fb_count = 2;
    } else {
        config.frame_size = FRAMESIZE_SVGA;
        config.jpeg_quality = 12;
        config.fb_count = 1;
    }

    // Initialize the camera
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        Serial.printf("Camera init failed with error 0x%x", err);
        return;
    }

    // Start the camera server
    startCameraServer();
    Serial.println("Camera Ready! Visit the following URL:");
    Serial.print("http://");
    Serial.print(WiFi.localIP());
    Serial.println("/");
}

void loop() {
    delay(10000);
}
