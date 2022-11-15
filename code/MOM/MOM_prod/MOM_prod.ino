#include "secrets.h"
#include "wifi_scanner.h"

#include <ArduinoJson.h>
#include <ESP32Ping.h>

#define LED_GPIO_PIN            13
#define OCCU_PUBLISH_TOPIC      "MOM/occupancy"
#define LOCATION                "Siebel 1404"
#define MIN_RSSI                -50
#define TARGET_RSSI             (MIN_RSSI + 10)


uint8_t channel = 11;
uint64_t last_transmission_to_aws;
LinkedList<mac_list_item_t> mac_list = LinkedList<mac_list_item_t>();
WiFiClientSecure net = WiFiClientSecure();
MQTTClient mqttClient = MQTTClient(256);


bool connectWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  printf("Connecting to Wi-Fi");
  uint64_t connect_time_start = esp_timer_get_time() / 1000000; // Seconds since boot
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    printf(".");
    // Enforce 5-second timeout
    if (esp_timer_get_time() / 1000000 - connect_time_start > 5) {
      printf("Timed out while connecting to %s!\n", WIFI_SSID);
      WiFi.disconnect();
      return false;
    }
  }
  Serial.println("Connected!");
  return true;
}

void pingTest() {
  bool ret = Ping.ping(AWS_IOT_ENDPOINT, 10);
  float avg_time_ms = Ping.averageTime();
  printf("Ping test finished. Average ping time: %f milliseconds\n", avg_time_ms);
}

bool connectAWS() {
  // Set creds to connect to AWS IoT
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);

  mqttClient.begin(AWS_IOT_ENDPOINT, 8883, net);
  printf("Connecting to AWS IoT");

  while (!mqttClient.connect(THINGNAME)) {
    printf(".");
    delay(10);
  }

  if (!mqttClient.connected()) {
    Serial.println("\nTimed out while connecting to AWS IoT Core!");
    return false;
  }

  Serial.println("\nConnected!");
  return true;
}

void disconnectAWS() {
  bool disconnected = mqttClient.disconnect();
  Serial.println("Disconnected from AWS IoT.");
}

void publishMessage() {
  if (connectAWS()) {
    StaticJsonDocument<200> doc;
    doc["location"] = LOCATION;
    doc["occupancy"] = mac_list.size();
    doc["battery"] = 92.21;
    char jsonBuffer[512];
    serializeJson(doc, jsonBuffer);

    mqttClient.publish(OCCU_PUBLISH_TOPIC, jsonBuffer);
    disconnectAWS();    
  }
}

bool is_ignorable_oui(uint8_t oui0, uint8_t oui1, uint8_t oui2) {
  int match_count;
  for (int i = 0; i < NUM_IGNORABLE_OUIS; i++) {
    match_count = 0;
    if (ignorable_ouis[i][0] == oui0) {
      match_count++;
    }
    if (ignorable_ouis[i][1] == oui1) {
      match_count++;
    }
    if (ignorable_ouis[i][2] == oui2) {
      match_count++;
    }
    if (match_count == BYTES_IN_OUI) {
      return true;
    }
  }
  return false;
}

void add_mac_to_list(uint8_t mac0, uint8_t mac1, uint8_t mac2, uint8_t mac3, uint8_t mac4, uint8_t mac5, uint64_t timestamp, int8_t rssi) {
  mac_list_item_t seen_mac;
  int i;
  if (is_ignorable_oui(mac0, mac1, mac2)) {
    return;
  }
  seen_mac.mac_addr[0] = mac0;
  seen_mac.mac_addr[1] = mac1;
  seen_mac.mac_addr[2] = mac2;
  seen_mac.mac_addr[3] = mac3;
  seen_mac.mac_addr[4] = mac4;
  seen_mac.mac_addr[5] = mac5;
  seen_mac.timestamp = timestamp;
  seen_mac.rssi = rssi;
  // If the OUI's local bit is set, it's likely randomized
  seen_mac.is_random = (mac0 & 0b00000010); 
  if (rssi <= TARGET_RSSI) {
    seen_mac.ttl = TTL_1_MIN;
  }
  else {
    seen_mac.ttl = TTL_10_MIN;
  }

  for (i = 0; i < mac_list.size(); i++) {
    mac_list_item_t curr = mac_list.get(i);
    if (curr.mac_addr[0] == mac0 && curr.mac_addr[1] == mac1 && curr.mac_addr[2] == mac2 && 
      curr.mac_addr[3] == mac3 && curr.mac_addr[4] == mac4 && curr.mac_addr[5] == mac5) {
      mac_list.set(i, seen_mac);
      break;
    }
  }
  if (i == mac_list.size()) {
    mac_list.add(seen_mac);
  }
}

void print_seen_macs() {
  mac_list_item_t curr;
  printf("%d seen MACs as of uptime %llu:\n", mac_list.size(), esp_timer_get_time() / 1000);
  for (int i = 0; i < mac_list.size(); i++) {
    curr = mac_list.get(i);
    printf("%02x:%02x:%02x:%02x:%02x:%02x at time = %llu with RSSI = %d and is_random = %d\n", curr.mac_addr[0], curr.mac_addr[1], curr.mac_addr[2], curr.mac_addr[3], curr.mac_addr[4], curr.mac_addr[5], curr.timestamp, curr.rssi, curr.is_random);
  }
}

void remove_departed_macs() {
  mac_list_item_t curr;
  for (int i = 0; i < mac_list.size(); i++) {
    curr = mac_list.get(i);
    // Remove if this MAC hasn't been seen for at least a minute
    if (esp_timer_get_time() / 1000 - curr.timestamp >= curr.ttl) {
      mac_list.remove(i);
      i--;
    }
  }
}

esp_err_t event_handler(void *ctx, system_event_t *event) {
  return ESP_OK;
}

void wifi_sniffer_init(void) {
  nvs_flash_init();
  tcpip_adapter_init();
  ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
  ESP_ERROR_CHECK( esp_wifi_set_country_code("US", false) );
  ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
  ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_NULL) );
  ESP_ERROR_CHECK( esp_wifi_start() );
}

void wifi_sniffer_set_channel(uint8_t channel) {
  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
}

const char * wifi_sniffer_packet_type2str(wifi_promiscuous_pkt_type_t type) {
  switch(type) {
    case WIFI_PKT_MGMT: return "MGMT";
    case WIFI_PKT_DATA: return "DATA";
    case WIFI_PKT_MISC: return "MISC";
    default:  return "";
  }
}

void wifi_sniffer_packet_handler(void* buff, wifi_promiscuous_pkt_type_t type) {

  const wifi_promiscuous_pkt_t *ppkt = (wifi_promiscuous_pkt_t *)buff;
  const wifi_ieee80211_packet_t *ipkt = (wifi_ieee80211_packet_t *)ppkt->payload;
  const wifi_ieee80211_mac_hdr_t *hdr = &ipkt->hdr;

  if (ppkt->rx_ctrl.rssi < MIN_RSSI) {
    return;
  }

  uint64_t timestamp = ppkt->rx_ctrl.timestamp / 1000;  // milliseconds since boot
  add_mac_to_list(hdr->addr1[0], hdr->addr1[1], hdr->addr1[2], hdr->addr1[3], hdr->addr1[4], hdr->addr1[5], timestamp, ppkt->rx_ctrl.rssi);
  add_mac_to_list(hdr->addr2[0], hdr->addr2[1], hdr->addr2[2], hdr->addr2[3], hdr->addr2[4], hdr->addr2[5], timestamp, ppkt->rx_ctrl.rssi);
  add_mac_to_list(hdr->addr3[0], hdr->addr3[1], hdr->addr3[2], hdr->addr3[3], hdr->addr3[4], hdr->addr3[5], timestamp, ppkt->rx_ctrl.rssi);

  // printf("PACKET TYPE=%s, CHAN=%02d, RSSI=%02d,"
  //   " ADDR1=%02x:%02x:%02x:%02x:%02x:%02x,"
  //   " ADDR2=%02x:%02x:%02x:%02x:%02x:%02x,"
  //   " ADDR3=%02x:%02x:%02x:%02x:%02x:%02x\n",
  //   wifi_sniffer_packet_type2str(type),
  //   ppkt->rx_ctrl.channel,
  //   ppkt->rx_ctrl.rssi,
  //   /* ADDR1 */
  //   hdr->addr1[0],hdr->addr1[1],hdr->addr1[2],
  //   hdr->addr1[3],hdr->addr1[4],hdr->addr1[5],
  //   /* ADDR2 */
  //   hdr->addr2[0],hdr->addr2[1],hdr->addr2[2],
  //   hdr->addr2[3],hdr->addr2[4],hdr->addr2[5],
  //   /* ADDR3 */
  //   hdr->addr3[0],hdr->addr3[1],hdr->addr3[2],
  //   hdr->addr3[3],hdr->addr3[4],hdr->addr3[5]
  // );
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  pinMode(LED_GPIO_PIN, OUTPUT);
  wifi_sniffer_init();
  delay(100);
  while (!connectWiFi()) {
    delay(100);
  }
  delay(100);
  pingTest();
  delay(100);
  WiFi.disconnect();
  digitalWrite(LED_GPIO_PIN, HIGH);
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_rx_cb(&wifi_sniffer_packet_handler);
  wifi_sniffer_set_channel(channel);
  last_transmission_to_aws = esp_timer_get_time() / 1000; // milliseconds since boot
}

void loop() {
  vTaskDelay(WIFI_CHANNEL_SWITCH_INTERVAL / portTICK_PERIOD_MS);
  remove_departed_macs();
  print_seen_macs();
  // Transmit to AWS IoT every minute
  if (esp_timer_get_time() / 1000 - last_transmission_to_aws >= 60000) {
    esp_wifi_set_promiscuous(false);
    connectWiFi();
    publishMessage();
    last_transmission_to_aws = esp_timer_get_time() / 1000;
  }
}
