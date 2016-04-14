// Relay  1 > IN1 on Pin 2
// Sensor 1 > S on Pin 7
#include <SPI.h>
#include <Ethernet.h>
#include <dht11.h>

dht11 DHT11;
#define DHT11PIN 7

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED}; // MAC address for Arduino Board
IPAddress ip(192, 168, 1, 180); // Static IP address for Arduino Board
EthernetServer server(80);  // Run server on port 80

String HTTP_req;  // HTTP request
boolean L1_status = 0;  //  Light 1 Status

void setup(){
    Ethernet.begin(mac, ip);
    server.begin();
    Serial.begin(9600);
    pinMode(2, OUTPUT);
}

void loop(){
    EthernetClient client = server.available();
    if (client) {
        boolean currentLineIsBlank = true;
        while (client.connected()) {
            if (client.available()) {
                char c = client.read();
                HTTP_req += c;
                if (c == '\n' && currentLineIsBlank) {
                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-Type: text/html");
                    client.println("Connection: close");
                    client.println();
                    client.println("<!DOCTYPE html>");
                    client.println("<html>");
                    client.println("<head>");
                    client.println("<title>Home Control</title>");
                    client.println("<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.6/css/bootstrap.min.css\">");
                    client.println("<link rel=\"stylesheet\" href=\"https://cdnjs.cloudflare.com/ajax/libs/iCheck/1.0.2/skins/square/pink.css\">");
                    client.println("<style type=\"text/css\">input, label { cursor: pointer; }</style>");
                    client.println("</head>");
                    client.println("<body>");
                    client.println("<div class=\"container\">");
                    client.println("<div class=\"col-md-6 col-md-push-3\">");
                    client.println("<h1>Home Control</h1>");
                    client.println("<div class=\"panel panel-default\">");
                    client.println("<div class=\"panel-heading\">Lights</div>");
                    client.println("<div class=\"panel-body\">");
                    client.println("<form class=\"row\" id=\"ControlForm\" method=\"get\">");
                      client.println("<div class=\"col-md-8 col-md-push-2\">");
                        initLightControl(client);
                      client.println("</div>");
                    client.println("</form>");
                    client.println("</div>");
                    client.println("</div>");
                    client.println("<div class=\"panel panel-default\">");
                    client.println("<div class=\"panel-heading\">Sensors</div>");
                    client.println("<div class=\"panel-body\">");
                      client.println("<div class=\"row\">");
                      client.println("<div class=\"col-md-8 col-md-push-2\">");
                        int chk = DHT11.read(DHT11PIN);
                        switch (chk){
                          case DHTLIB_OK: 
                          break;
                          case DHTLIB_ERROR_CHECKSUM: 
                          client.println("Checksum error"); 
                          break;
                          case DHTLIB_ERROR_TIMEOUT: 
                          client.println("Time out error"); 
                          break;
                          default: 
                          client.println("Unknown error"); 
                          break;
                        }
                        client.println("Humidity: ");
                        client.println(DHT11.humidity);
                        client.print("%");
                        client.println("<br>");
                        client.println("Temperature: ");
                        client.println(DHT11.temperature);
                        client.print("&deg;C");
                       client.println("</div>");
                       client.println("</div>");
                    client.println("</div>");
                    client.println("</div>");
                    client.println("<script src=\"https://code.jquery.com/jquery-2.2.3.min.js\" crossorigin=\"anonymous\"></script>");
                    client.println("<script src=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.6/js/bootstrap.min.js\"></script>");
                    client.println("<script src=\"https://cdnjs.cloudflare.com/ajax/libs/iCheck/1.0.2/icheck.min.js\"></script>");
                    client.println("<script>");
                    client.println("$(document).ready(function(){");
                    client.println("$('input').iCheck({");
                    client.println("checkboxClass: 'icheckbox_square-pink',");
                    client.println("radioClass: 'iradio_square-pink',");
                    client.println("increaseArea: '20%'");
                    client.println("});");
                    client.println("});");
                    client.println("$('#L1Control').click(function (){ $.get('http://192.168.1.180/?Lights1=toggle'); });");
                    client.println("$('#L1ControlLabel').click(function (){ $('#L1Control').click(); });");
                    client.println("</script>");
                    client.println("</div>");
                    client.println("</body>");
                    client.println("</html>");
                    Serial.print(HTTP_req);
                    HTTP_req = "";
                    break;
                }
                if (c == '\n') {
                    currentLineIsBlank = true;
                } else if (c != '\r') {
                    currentLineIsBlank = false;
                }
            }
        }
        delay(1);
        client.stop();
    }
}

void initLightControl(EthernetClient EthCli) {
    if (HTTP_req.indexOf("Lights1=toggle") > -1) {
        if (L1_status) {
            L1_status = 0;
        } else {
            L1_status = 1;
        }
    }
    if (L1_status) {
        digitalWrite(2, HIGH);
        EthCli.println("<input id=\"L1Control\" type=\"checkbox\" checked> <label id=\"L1ControlLabel\" for=\"L1Control\">Lights 1</label>");
    } else {
        digitalWrite(2, LOW);
        EthCli.println("<input id=\"L1Control\" type=\"checkbox\"> <label id=\"L1ControlLabel\" for=\"L1Control\">Lights 1</label>");
    }
}

