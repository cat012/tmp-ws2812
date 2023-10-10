
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FastLED.h>


#define WIFI_CONN_TIME_SEC  8

static const char *sta_ssid = "xxxxxxxxxx";
static const char *sta_password = "xxxxxxxxxx";

static const char *esp_softap_ssid = "esp32lsd";
static const char *esp_softap_pass = "11223344Lap";

static const uint8_t defaultvalue=128;

static const uint8_t red=0;
static const uint8_t yellow=42;
static const uint8_t green=85;
static const uint8_t aqua=128;
static const uint8_t blue=181;
static const uint8_t purple=213;

static uint8_t currentvalue=defaultvalue;
static uint8_t currentcolor=aqua;


#define LED_PIN     5
#define NUM_LEDS    16
#define BRIGHTNESS  255
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB

CRGB leds[NUM_LEDS];

ESP8266WebServer server(80);

static uint8_t currentpattern=0;
static uint8_t ledcolorbuff[NUM_LEDS]={0};


//-------------------------------------------------------------------------------------------------
static void wifi_init(char *ssid, char *pass)
    {
    WiFi.disconnect(true);
    WiFi.mode(WIFI_STA);

    WiFi.begin(ssid, pass);

    for(uint8_t c=0; (WiFi.status() != WL_CONNECTED) && (c<WIFI_CONN_TIME_SEC); c++)
        {
        delay(1000);
        Serial.print(".");
        }

    if(WiFi.status() == WL_CONNECTED)
        {
        Serial.println("");
        Serial.print("^^ lsd local IP: ");
        Serial.println(WiFi.localIP());
        }
    else
        {
        WiFi.disconnect(true);
        WiFi.softAP(esp_softap_ssid, esp_softap_pass);
        IPAddress myIP = WiFi.softAPIP();
        Serial.println("");
        Serial.print("^^ lsd AP IP: ");
        Serial.println(myIP);
        }
    }


//-------------------------------------------------------------------------------------------------
void handle_root(void)
    {
    String page="";
    page +="<html><head><meta charset=\"utf-8\"><title>lsd</title></head>";
    page +="<body bgcolor=\"GhostWhite\">";

    page +="Brightness: ";
    page +=currentvalue;
    page +="<form action=\"/changevalue\"method=\"POST\">";
    page +="<input type=\"text\" name=\"newvalue\"size=\"5\"placeholder=\"0-255\"></br>";
    page +="<input type=\"submit\"value=\"Save\"></form>";

    page +="Color: ";
    page +=currentcolor;
    page +="<form action=\"/changecolor\"method=\"POST\">";
    page +="<input type=\"text\" name=\"newcolor\"size=\"5\"placeholder=\"0-255\"></br>";
    page +="<input type=\"submit\"value=\"Save\"></form>";
    
    page +="Pattern: ";
    page +=currentpattern;
    page +="<form action=\"/navpatterns\"method=\"POST\">";
    page +="<input type=\"submit\"name=\"pattern0\"value=\"0\"></br>"; 
    page +="<input type=\"submit\"name=\"pattern1\"value=\"1\"></br>";
    page +="<input type=\"submit\"name=\"pattern2\"value=\"2\"></br>";
    page +="<input type=\"submit\"name=\"pattern3\"value=\"3\">";
    page +="</form>";

    page +="<form action=\"/setpattern3\"method=\"POST\">";
    page +="<input type=\"text\"name=\"newledcolor00\"size=\"4\"placeholder=\"0-255\">";
    page +="<input type=\"submit\"value=\"Save\">";
    page +="<input type=\"text\"name=\"newledcolor01\"size=\"4\"placeholder=\"0-255\">";
    page +="<input type=\"submit\"value=\"Save\">";
    page +="<input type=\"text\"name=\"newledcolor02\"size=\"4\"placeholder=\"0-255\">";
    page +="<input type=\"submit\"value=\"Save\">";
    page +="<input type=\"text\"name=\"newledcolor03\"size=\"4\"placeholder=\"0-255\">";
    page +="<input type=\"submit\"value=\"Save\"></br>";
    page +="<input type=\"text\"name=\"newledcolor04\"size=\"4\"placeholder=\"0-255\">";
    page +="<input type=\"submit\"value=\"Save\">";
    page +="<input type=\"text\"name=\"newledcolor05\"size=\"4\"placeholder=\"0-255\">";
    page +="<input type=\"submit\"value=\"Save\">";
    page +="<input type=\"text\"name=\"newledcolor06\"size=\"4\"placeholder=\"0-255\">";
    page +="<input type=\"submit\"value=\"Save\">";
    page +="<input type=\"text\"name=\"newledcolor07\"size=\"4\"placeholder=\"0-255\">";
    page +="<input type=\"submit\"value=\"Save\"></br>";
    page +="<input type=\"text\"name=\"newledcolor08\"size=\"4\"placeholder=\"0-255\">";
    page +="<input type=\"submit\"value=\"Save\">";
    page +="<input type=\"text\"name=\"newledcolor09\"size=\"4\"placeholder=\"0-255\">";
    page +="<input type=\"submit\"value=\"Save\">";
    page +="<input type=\"text\"name=\"newledcolor10\"size=\"4\"placeholder=\"0-255\">";
    page +="<input type=\"submit\"value=\"Save\">";
    page +="<input type=\"text\"name=\"newledcolor11\"size=\"4\"placeholder=\"0-255\">";
    page +="<input type=\"submit\"value=\"Save\"></br>";
    page +="<input type=\"text\"name=\"newledcolor12\"size=\"4\"placeholder=\"0-255\">";
    page +="<input type=\"submit\"value=\"Save\">";
    page +="<input type=\"text\"name=\"newledcolor13\"size=\"4\"placeholder=\"0-255\">";
    page +="<input type=\"submit\"value=\"Save\">";
    page +="<input type=\"text\"name=\"newledcolor14\"size=\"4\"placeholder=\"0-255\">";
    page +="<input type=\"submit\"value=\"Save\">";
    page +="<input type=\"text\" name=\"newledcolor15\"size=\"4\"placeholder=\"0-255\">";
    page +="<input type=\"submit\"value=\"Save\">";
    page +="</form>";

    page +="<p><a href=\"/\">Home</a></p></body></html>";

    server.send(200, "text/html", page);
    }


//-------------------------------------------------------------------------------------------------
void handle_404(void)
    {
    server.send(404, "text/html", "<p>404: Not found</p><p><a href=\"/\">Home</a></p>");
    }


//-------------------------------------------------------------------------------------------------
void server_send_invalid_request(void)
    {
    server.send(400, "text/html", "<p>400: Invalid Request</p><p><a href=\"/\">Home</a></p>");
    }


//-------------------------------------------------------------------------------------------------
void handle_changecolor(void)
    {
    if(!server.hasArg("newcolor") || server.arg("newcolor")==NULL) { server_send_invalid_request(); return; }

    String str1=server.arg("newcolor");
    char strbuff[4]="";
    str1.toCharArray(strbuff,4);
    uint8_t result=atoi(strbuff);

    currentcolor=result;
    currentpattern=0;

    handle_root();
    }


//-------------------------------------------------------------------------------------------------
void handle_changevalue(void)
    {
    if(!server.hasArg("newvalue") || server.arg("newvalue")==NULL) { server_send_invalid_request(); return; }

    String str1=server.arg("newvalue");
    char strbuff[4]="";
    str1.toCharArray(strbuff,4);
    uint8_t result=atoi(strbuff);

    currentvalue=result;

    handle_root();
    }


//-------------------------------------------------------------------------------------------------
void handle_changepattern(void)
    {
    if(server.hasArg("pattern0")) { currentpattern=0; }
    else if(server.hasArg("pattern1")) { if(currentpattern==1) currentpattern=0; else currentpattern=1; }
    else if(server.hasArg("pattern2")) { currentpattern=2; }
    else if(server.hasArg("pattern3")) { currentpattern=3; }
    else { server_send_invalid_request(); return; }

    handle_root();
    }

static const char *lednums[16]=
{
"newledcolor00",
"newledcolor01",
"newledcolor02",
"newledcolor03",
"newledcolor04",
"newledcolor05",
"newledcolor06",
"newledcolor07",
"newledcolor08",
"newledcolor09",
"newledcolor10",
"newledcolor11",
"newledcolor12",
"newledcolor13",
"newledcolor14",
"newledcolor15"
};

//-------------------------------------------------------------------------------------------------
void handle_changepattern3(void)
    {
    for(uint8_t c=0; c<16; c++)
        {
        if(server.hasArg(lednums[c]) && server.arg(lednums[c])!=NULL) 
            {
            String str1=server.arg(lednums[c]);
            char strbuff[4]="";
            str1.toCharArray(strbuff,4);
            uint16_t result=atoi(strbuff);
            if(result>255) break;
            ledcolorbuff[c]=result;
            handle_root();
            return;
            }
        }
    server_send_invalid_request();
    return;
    }


//-------------------------------------------------------------------------------------------------
void setup(void)
    {
    delay(1000);

    Serial.begin(74880);
    Serial.println("");
    Serial.println("^^ lsd start program");
    
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalPixelString );
    FastLED.setBrightness(BRIGHTNESS);
    FastLED.show();
    for(uint8_t c=0; c<NUM_LEDS; c++) leds[c] = CHSV(c*15, 255, currentvalue);
    FastLED.show();

    wifi_init((char*)sta_ssid, (char*)sta_password);

    server.on("/", handle_root);
    server.onNotFound(handle_404);
    server.on("/changecolor", handle_changecolor);
    server.on("/changevalue", handle_changevalue);
    server.on("/navpatterns", handle_changepattern);
    server.on("/setpattern3", handle_changepattern3);
    server.begin();
    }


//-------------------------------------------------------------------------------------------------
void loop(void)
    {
    server.handleClient();

    static bool ereset=false;
    static uint8_t oldpattern=0;
    if(currentpattern!=oldpattern) {oldpattern=currentpattern; ereset=true;}

    if(currentpattern==0)
        {
        for(uint8_t c=0; c<NUM_LEDS; c++) leds[c] = CHSV(currentcolor, 255, currentvalue);
        FastLED.show();
        }

    else if(currentpattern==1)
        {
        static uint32_t told=0;
        uint32_t tnew=millis();
        const uint32_t PAUSE=50;
    
        if(tnew-told>PAUSE)
            {
            told=tnew;
 
            for(uint8_t c=0; c<NUM_LEDS; c++) leds[c] = CHSV(currentcolor, 255, currentvalue);
            currentcolor++;
            FastLED.show();
            }
        }
            
    else if(currentpattern==2)
        {
        static uint8_t state=0;
        static uint8_t c=0;

        static uint32_t told=0;
        uint32_t tnew=millis();
        const uint32_t PAUSE=25;

        if(ereset) { c=0; ereset=false; }
    
        if(tnew-told>PAUSE)
            {
            told=tnew;
            
            if(state==0) leds[c] = CHSV(currentcolor, 255, currentvalue);
            else if(state==1) leds[c] = CRGB::Black;
            FastLED.show();
            c++;
            if(c>=NUM_LEDS) { c=0; state ? state=0 : state=1; }
            }
        }

    else if(currentpattern==3)
        {
        for(uint8_t c=0; c<NUM_LEDS; c++) leds[c] = CHSV(ledcolorbuff[c], 255, currentvalue);
        FastLED.show();
        }
    }


//end
