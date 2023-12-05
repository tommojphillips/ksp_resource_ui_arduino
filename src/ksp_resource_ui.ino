// Project Start Date 04.12.2023
// Author, tommojphillips (Tommo J. Productions) (tommojphillips)

#include <KerbalSimpit.h>
#include <PayloadStructs.h>
#include <MCUFRIEND_kbv.h>
#include <Container.h>

MCUFRIEND_kbv lcd;
KerbalSimpit ksp(Serial);
resourceMessage lfMsg;
resourceMessage oxMsg;
resourceMessage abMsg;
resourceMessage ecMsg;
resourceMessage mpMsg;
flightStatusMessage flightMsg;

bool inFlight = false;

ContainerSettings cSettings(50, 215, 5);

Container ox(lcd, cSettings);
Container lf(lcd, cSettings);
Container ab(lcd, cSettings);
Container ec(lcd, cSettings);
Container mp(lcd, cSettings);

void initLcd() 
{
  Serial.begin(115200);

  lcd.begin(lcd.readID());
  lcd.setRotation(1);
  lcd.fillScreen(BLACK);

  lcd.setTextSize(2);
}
void initKsp()
{
  /*while (!ksp.init()) {

    delay(100);
  }*/
  
  delay(1000);

  ksp.inboundHandler(messageHandler);

  ksp.registerChannel(OX_STAGE_MESSAGE);
  ksp.registerChannel(LF_STAGE_MESSAGE);
  ksp.registerChannel(ELECTRIC_MESSAGE);
  ksp.registerChannel(AB_MESSAGE);
  ksp.registerChannel(MONO_MESSAGE);
  ksp.registerChannel(FLIGHT_STATUS_MESSAGE);
}

void setup()
{  
  initLcd();
  
  lcd.setCursorText(5, 5);
  lcd.print("Waiting for KSP");

  initKsp();
  
  lcd.fillScreen(BLACK);
  ksp.printToKSP("Connected", PRINT_TO_SCREEN);

  // setting up containers
  lf.createContainer(0,   0, "LF", 0xfe40,  0xfea7);
  ox.createContainer(55,  0, "OX", 0x05bf,  0x465f);
  ab.createContainer(110, 0, "AB", YELLOW,  0xffe8);
  ec.createContainer(165, 0, "EC", GREEN,   0x47e8);
  mp.createContainer(220, 0, "MP", 0xc638,  0xef7d);
  
  ab.updateLevel(1);
  delay(1000);
  ab.updateLevel(0.5);

}

void loop()
{  
  ksp.update();

  delay(80);

  lf.updateLevel(lfMsg.available / lfMsg.total);
  ox.updateLevel(oxMsg.available / oxMsg.total);
  // ab.updateLevel(abMsg.available / abMsg.total);
  ec.updateLevel(ecMsg.available / ecMsg.total);
  mp.updateLevel(mpMsg.available / mpMsg.total);
}

void messageHandler(byte messageType, byte msg[], byte msgSize) 
{  
  switch(messageType) 
  {
    case OX_STAGE_MESSAGE:
    if (msgSize == sizeof(resourceMessage)) 
    {
      oxMsg = parseMessage<resourceMessage>(msg);
    }
    break;
    case LF_STAGE_MESSAGE:
    if (msgSize == sizeof(resourceMessage)) 
    {
      lfMsg = parseMessage<resourceMessage>(msg);
    }
    break;
    case ELECTRIC_MESSAGE:
    if (msgSize == sizeof(resourceMessage)) 
    {
      ecMsg = parseMessage<resourceMessage>(msg);
    }
    break;
    case AB_MESSAGE:
    if (msgSize == sizeof(resourceMessage)) 
    {
      abMsg = parseMessage<resourceMessage>(msg);
    }
    break;
    case MONO_MESSAGE:
    if (msgSize == sizeof(resourceMessage)) {

      mpMsg = parseMessage<resourceMessage>(msg);
    }
    break;
    case FLIGHT_STATUS_MESSAGE:
    if (msgSize == sizeof(flightStatusMessage)) {

      flightMsg = parseMessage<flightStatusMessage>(msg);
      inFlight = flightMsg.isInFlight();
    }
    break;
  }
}