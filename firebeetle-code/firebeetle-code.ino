#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>

// Replace the next variables with your SSID/Password combination
const char *ssid = "SSID NAME";
const char *password = "SSID PASSWORD";
const char *mqttServer = "MQTT SERVER";
const int mqttPort = 1883;
const char *mqttUser = "MQTT USER";
const char *mqttPassword = "MQTT PASSWORD";
WiFiClient espClient;
PubSubClient client(espClient);

void setup()
{
  Serial.begin(115200);
  delay(100); //Take some time to open up the Serial Monitor

  //Setup WiFi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  print_wakeup_reason();

  //get wakeup bit
  uint64_t wakeupBit = esp_sleep_get_ext1_wakeup_status();
  //Serial.print("Wakeup Bit");
  //Serial.println(wakeupBit);
  //if it didnt woke because of flap or door, we dont need to do anything.
  if (esp_sleep_get_wakeup_cause() == 3)
  { // 3 is RTC_CTRL - the IOs controlled by rtc
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
      delay(1000);
      Serial.print(".");
    }

    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    client.setServer(mqttServer, mqttPort);

    while (!client.connected())
    {
      Serial.println("Connecting to MQTT...");

      if (client.connect("paesslersMailbox", mqttUser, mqttPassword))
      {
        Serial.println("mqtt connected");
      }
      else
      {

        Serial.print("failed with state ");
        Serial.print(client.state());
        delay(2000);
      }
    }

    if (wakeupBit & GPIO_SEL_25)
    {
      // GPIO 25 woke up
      Serial.println("a letter arrived");
      client.publish("mailbox/action", "arrived");
    }
    else
    {
      // must be GPIO 26 - we can't as explicitly because with unarmed flap this doesnt work.
      Serial.println("emptied mailbox");
      client.publish("mailbox/action", "emptied");
    }
  }

  //we need the internal pullups/downs to work so wee need an option to keep the power for them on during sleep.
  //note: when you use external pulldowns (10K) instead of the internal ones you can save around 10µA in deepsleep  (then comment out the next 5 lines)
  //note2: don't use gpio34-39 when using internal pullup/downs - they don't have them.
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
  gpio_pullup_dis(GPIO_NUM_25);
  gpio_pulldown_en(GPIO_NUM_25);
  gpio_pullup_dis(GPIO_NUM_26);
  gpio_pulldown_en(GPIO_NUM_26);

  /*
   * Important: Assume someone threw a magazine into our Mailbox, blocking the flap. This would cause it to
   * permanently wake up and drain our battery. So we'll only arm the mailbox door switch after a letter
   * arrived.
   */

  if (wakeupBit & GPIO_SEL_25)
  {
    //enable gpio 25 and 26 for deep sleep (using bit or) - wakeup if any of those go high.
    Serial.println("only arming door");
    esp_sleep_enable_ext1_wakeup(GPIO_SEL_26, ESP_EXT1_WAKEUP_ANY_HIGH);
  }
  else
  {
    //arm both flap and door.
    Serial.println("arming door and flap");
    esp_sleep_enable_ext1_wakeup(GPIO_SEL_25 | GPIO_SEL_26, ESP_EXT1_WAKEUP_ANY_HIGH);
  }
  //Go to sleep now
  Serial.println("Going to sleep now");
  esp_deep_sleep_start();
  Serial.println("This will never be printed");
}

void print_wakeup_reason()
{
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  Serial.print("Wakeup: ");
  Serial.println(esp_sleep_get_wakeup_cause());

  switch (wakeup_reason)
  {
  case ESP_SLEEP_WAKEUP_EXT0:
    Serial.println("Wakeup caused by external signal using RTC_IO");
    break;
  case ESP_SLEEP_WAKEUP_EXT1:
    Serial.println("Wakeup caused by external signal using RTC_CNTL");
    break;
  case ESP_SLEEP_WAKEUP_TIMER:
    Serial.println("Wakeup caused by timer");
    break;
  case ESP_SLEEP_WAKEUP_TOUCHPAD:
    Serial.println("Wakeup caused by touchpad");
    break;
  case ESP_SLEEP_WAKEUP_ULP:
    Serial.println("Wakeup caused by ULP program");
    break;
  default:
    Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason);
    break;
  }
}

void loop()
{
  //This is not going to be called
}
