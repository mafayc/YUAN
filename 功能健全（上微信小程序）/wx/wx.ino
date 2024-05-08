#include <WiFi.h>
#include<oledssd1306.h>
#include <PubSubClient.h>  //建议使用PubSubClient2.7.0，最新的库不太好用
WiFiClient espClient;
PubSubClient client(espClient);



// #define ssid           "HUAWEI"              //wifi名称
// #define password       "imfy0639"    //wifi密码
#define ssid           "QKZZ"              //wifi名称
#define password       "Qkzz666888"    //wifi密码

#define mqttServer     "630297d99a.st1.iotda-device.cn-north-4.myhuaweicloud.com"       //例如iot-mqtts.cn-north-4.myhuaweicloud.com，详情参考华为云控制台
#define mqttPort        1883             //例如1883（华为MQTT地址1883，MQTTS地址8883，详情参考华为云控制台）
//三元组：三元组生成链接：https://iot-tool.obs-website.cn-north-4.myhuaweicloud.com/
#define ClientId       "6636d9a371d845632a08686e_ESP32S3_0_0_2024050501"        
#define mqttUser       "6636d9a371d845632a08686e_ESP32S3"
#define mqttPassword   "f8e714cbb3153935d6673d2b0e507dd6d2d691feb3356d8f45b42105ab0f83ac"
//注册设备的ID和密钥
#define device_id      "6636d9a371d845632a08686e_ESP32S3" 
#define secret         "imfy0639" 
//注意修改自己的服务ID   
#define Iot_link_Body_Format              "{\"services\":[{\"service_id\":\"fuwu\",\"properties\":{%s" 
//参考上报格式：{"services":[{"service_id":"Dev_data","properties":{"temp": 39}}]}
//设备属性上报
#define Iot_link_MQTT_Topic_Report        "$oc/devices/6636d9a371d845632a08686e_ESP32S3/sys/properties/report"
//接收平台下发的命令
#define Iot_link_MQTT_Topic_Commands      "$oc/devices/6636d9a371d845632a08686e_ESP32S3/sys/commands/#" 
//设备相应平台的命令
#define Iot_link_MQTT_Topic_CommandsRes   "$oc/devices/6636d9a371d845632a08686e_ESP32S3/sys/commands/response/request_id="
//获取影子设备
#define Iot_link_MQTT_Topic_ceii          "$oc/devices/6636d9a371d845632a08686e_ESP32S3/sys/properties/report"

int data_temp=0;   //模拟上报的数值


void setup() {
  Serial.begin(9600);
  
     //屏幕初始化
      OLED_int1vcc();
      OLED_int2oled();
      OLED_kill();
      OLED_String(0,0,"WiFi {int...}");
      //OLED_DisplayTurn(1);
      //屏幕初始化结束
  
  //wifi初始化
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("正在连接WiFi..");
    OLED_String(0,0,"WiFi loading...");
  }
  Serial.println("WiFi已连接");
  OLED_kill();
  OLED_String(0,0,"WiFi {ok}");
  //MQTT初始化
  MQTT_Init();
OLED_kill();
OLED_String(0,0,"MQTT ok");

}
void loop() {
  if (!client.connected()){MQTT_Init();}else client.loop();
if(data_temp==100)data_temp=0;

delay(1000);MQTT_POST();

}















void MQTT_Init()
  {
    client.setServer(mqttServer, mqttPort);
    while (!client.connected()) 
    {
      Serial.println("正在连接到 MQTT...");
      if (client.connect(ClientId, mqttUser, mqttPassword )) 
      {
        Serial.println("MQTT连接成功");
      } 
      else 
      {
        Serial.print("MQTT连接失败，状态为 ");
        Serial.print(client.state());
        delay(3000);
      }
    }

    //client.subscribe(Iot_link_MQTT_Topic_Commands); //连接成功时订阅主题commands
    //client.subscribe(Iot_link_MQTT_Topic_ceii); //连接成功时订阅主题commands
    
client.setCallback(callback);//设定回调方式，当ESP8266收到订阅消息时会调用此方法
    
  }
void MQTT_POST()//上报属性
  {
    char properties[32];
    char jsonBuf[128];
    sprintf(properties,"\"ceii\":%d}}]}",data_temp++);
    sprintf(jsonBuf,Iot_link_Body_Format,properties);     Serial.println(jsonBuf);
    client.publish(Iot_link_MQTT_Topic_Report, jsonBuf);  //Serial.println(Iot_link_MQTT_Topic_Report);
    //Serial.println("MQTT 上报 OK!");

    /*//删除上报ceii的值
    String dl="{\"services\": [{\"service_id\": \"fuwu\",\"properties\": {}}]}";
    client.publish(Iot_link_MQTT_Topic_ceii,dl.c_str());
*/
  }








void callback(char* topic, byte* payload, unsigned int length)//来源地址//数据字符指针//总长度
  {
    String MqttData="";
    Serial.printf("数据内容：");
    for(int i=0;i<length;i++)  {MqttData+=(char)payload[i];}
    Serial.println(MqttData);
OLED_kill();
OLED_String(0,0,MqttData.c_str());

    //回响
    //解析request id，设备响应时的topic需要包含命令的request id，且会动态变化
    char *p=topic;
    String request_id="";
    int i=0;
    while((*p++)!='\0')//获取topic长度
    {
      i++;
    }
    topic+=(i-36);    //移动指针位置
    for(int j=i-36;j<i;j++)//末36位为request id
      request_id+=*topic++;
    //Serial.println("request_id："+request_id);
    
    String param="{}";
    client.publish((Iot_link_MQTT_Topic_CommandsRes+request_id).c_str(),param.c_str());
    Serial.println("响应完成");


  }
