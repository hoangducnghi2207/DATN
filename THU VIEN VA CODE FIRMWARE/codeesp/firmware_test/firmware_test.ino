#include <WiFi.h>
#define BOMNUOC 0
#define BOMOXI 1

#include <SocketIoClient.h>

//bool connect;

#include <ArduinoJson.h>// 6.10.0


const char* ssid     = "SIPLAB";
const char* password = "amonglab2020";

// Server Ip
const char* server = "192.168.0.108";
// Server port
int port = 3000;
//Khai báo thiêt bị
int TB1 = 0;
int TB2 = 0;
float ND = 0; //Nhiet do
float O2 = 0;  //O2
float PH=0; //PH
float NTU = 0 // Độ đục
String Data = "";
long last = 0;
#define ND_MIN 27
#define ND_MAX 35
#define O2_MIN 2.0
#define O2_MAX 5.0
#define PH_MIN 6.5
#define PH_MAX 9.0
#define NTU_MAX 80     
bool AUTO; // Dieu khien tu dong

SocketIoClient socket;
String DataMqttJson = "";
void setupNetwork()
{
  WiFi.begin(ssid, password);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.println("Wifi connected!");
}
void handleMessage(const char* message , size_t length)
{
  last = millis();
  Serial.print("Data nhận được: ");
  Serial.println(message);
  Data = message;


  ParseJson(String(Data));


  Data = "";
  last = millis();
}
void setup()
{
  Serial.begin(9600);
  pinMode(DEN, OUTPUT);
  pinMode(QUAT, OUTPUT);

  digitalWrite(DEN, LOW);
  digitalWrite(QUAT, LOW);


  setupNetwork();

  // kết nối server nodejs
  socket.begin(server, port);
  // lắng nghe sự kiện server gửi
  socket.on("ESP", handleMessage);
  last = millis();
  Serial.println("ESP Start");
}


void loop()
{
  DuytriServer();
  SendDataNodeJS();
}
void DuytriServer()
{
  socket.loop();
}
void ParseJson(String Data)
{
  // đưa dữ liệu json vào thư viện json để kiểm tra đúng hay sai , đúng thì tách dữ liệu => điều khiển

  const size_t capacity = JSON_OBJECT_SIZE(2) + 256;
  DynamicJsonDocument JSON1(capacity);
  DeserializationError error1 = deserializeJson(JSON1, Data);
  if (error1)
  {
    Serial.println("Data JSON Error!!!");
    return;
  }
  else
  {
    Serial.println();
    Serial.println("Data JSON ESP: ");
    serializeJsonPretty(JSON1, Serial);

    if (JSON1["TB1"] == "0")
    {
      // OFF TB1

      digitalWrite(DEN, LOW);
      TB1 = 0;
      Serial.println("Den OFF!!!");

    }
    else if (JSON1["TB1"] == "1")
    {
      // ON TB 1
      digitalWrite(DEN, HIGH);
      TB1 = 1;
      Serial.println("Den ON!!!");
    }
    else if (JSON1["TB2"] == "0")
    {
      // OFF TB2
      digitalWrite(QUAT, LOW);
      TB2 = 0;
      Serial.println("QUAT OFF!!!");

    }
    else if (JSON1["TB2"] == "1")
    {
      // ON TB 2

      digitalWrite(QUAT, HIGH);
      TB2 = 1;
      Serial.println("QUAT ON!!!");

    }
      else if (JSON1["AUTO"] == "1")
      {
       AUTO=1; 
      }
      else if (JSON1["AUTO"] == "0")
      {
       AUTO=0; 
      }


  }
}

void Datajson(String DataND, String DataPH,String DataO2,String Data NTU, String DataTB1, String DataTB2,  String DataAUTO)
{
  DataMqttJson  = "{\"ND\":\"" + String(DataND) + "\"," +
                  "\"DA\":\"" + String(DataPH) + "\"," +
                  "\"DA\":\"" + String(DataO2) + "\"," +
                  "\"DA\":\"" + String(DataNTU) + "\"," +
                  "\"TB1\":\"" + String(DataTB1) + "\"," +
                  "\"TB2\":\"" + String(DataTB2) + "\"," +
                  "\"AUTO\":\"" + String(DataAUTO) + "\"}";

  Serial.println();
  Serial.print("DataMqttJson: ");
  Serial.println(DataMqttJson);

  socket.emit("ESPJSON", DataMqttJson.c_str());

}
void SendDataNodeJS()
{
  if (millis() - last >= 1000)
  {

    Chuongtrinhcambien();
    Datajson(String(ND), String(PH),String(O2),String(NTU), String(TB1), String(TB2),  String(AUTO));

    last = millis();
  }
}
void DK_DEN()
{
  Serial.println("Onclick đèn!!!");
  last = millis();

  DK_DEN1();

  last = millis();

}

void DK_QUAT()
{
  Serial.println("Onclick Quạt!!!");
  last = millis();

  DK_QUAT1();


  last = millis();

}

void DK_DEN1()
{
 
  
    if (TB1 == 0)
    {
      digitalWrite(DEN, HIGH);
      TB1 = 1;
      Serial.println("Đèn ON!!!");
    }
    else if (TB1 == 1)
    {
      digitalWrite(DEN, LOW);
      TB1 = 0;
      Serial.println("Đèn OFF!!!");
    }
    // TB1 gửi lên web để hiển thị đèn or tb ON hoặc OFF ( 0 1)

  
}

void DK_QUAT1()
{

  if (TB2 == 0)
  {
    digitalWrite(QUAT, HIGH);
    TB2 = 1;
    Serial.println("quạt ON!!!");
  }
  else if (TB2 == 1)
  {
    digitalWrite(QUAT, LOW);
    TB2 = 0;
    Serial.println("quạt OFF!!!");
  }
 
}
void DK_AUTO()
{
  Serial.println("Onclick Quạt!!!");
  last = millis();

  DK_AUTO1();


  last = millis();

}
void DK_AUTO1()
{
  if (AUTO == 1)
  {
    if (ND < ND_MIN)
    {
      TB1 = 1;
      DK_DEN();
      DK_QUAT();
    }
    if (ND > ND_MAX)
    {
      
      TB1 = 0;
      DK_QUAT();
      DK_DEN();
    }
    if (DA < DA_MIN)
    {
      TB2 = 0;
       DK_DEN();
      DK_QUAT();
    }
    if (DA > DA_MAX)
    {
      TB2 = 1;
       DK_DEN();
      DK_QUAT();
    }
  }
  
  
}
void Chuongtrinhcambien()
{
  
 for(int i=0;i<35;i++){
    ND++;
  }
  for(int i=0;i<35;i++){
    DA++;
  }

}
