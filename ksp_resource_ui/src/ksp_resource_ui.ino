// Project Start Date 04.12.2023
// Author, tommojphillips (Tommo J. Productions) (tommojphillips)

#include <KerbalSimpit.h>
#include <PayloadStructs.h>
#include <MCUFRIEND_kbv.h>

MCUFRIEND_kbv lcd;
KerbalSimpit ksp(Serial);
resourceMessage lfMsg;
resourceMessage oxMsg;
resourceMessage abMsg;
resourceMessage ecMsg;
resourceMessage mpMsg;
flightStatusMessage flightMsg;

const int containerWidth = 50;
const int containerHeight = 215;
const int containerMargin = 5;

bool inFlight = false;

float ox = 0;
float lf = 0;
float ab = 0;
float ec = 0;
float mp = 0;

void initLcd() 
{
  Serial.begin(115200);

  lcd.begin(lcd.readID());
  lcd.setRotation(1);
  lcd.fillScreen(BLACK);

  lcd.setTextSize(2);
}
void initKsp() {

  while (!ksp.init()) {

    delay(100);
  }
  
  delay(1000);

  ksp.inboundHandler(messageHandler);

  ksp.registerChannel(OX_STAGE_MESSAGE);
  ksp.registerChannel(LF_STAGE_MESSAGE);
  ksp.registerChannel(ELECTRIC_MESSAGE);
  ksp.registerChannel(AB_MESSAGE);
  ksp.registerChannel(MONO_MESSAGE);
  ksp.registerChannel(FLIGHT_STATUS_MESSAGE);
}

void setup() {
  
  initLcd();
  
  lcd.setCursorText(5, 5);
  lcd.print("Waiting for KSP");

  initKsp();
  
  lcd.fillScreen(BLACK);
  ksp.printToKSP("Connected", PRINT_TO_SCREEN);

  // setting up containers
  drawContainer(0,   "LF", 0xfe40);
  drawContainer(55,  "OX", 0x05bf);
  drawContainer(110, "Ab", YELLOW);
  drawContainer(165, "EC", GREEN);
  drawContainer(220, "MP", 0xc638);

  // // setting initial container value.
  // changeContainerLevel(0,   0.25f, lf, GREEN);
  // changeContainerLevel(55,  1, ox, BLUE);
  // changeContainerLevel(110, 1, ab, YELLOW);
  // changeContainerLevel(165, 0.5f, ec, CYAN);  
  // changeContainerLevel(220, 0.6f, mp, WHITE);  
}

void loop() {
  
  ksp.update();

  delay(80);

  changeContainerLevel(0,   lfMsg.available / lfMsg.total, lf, 0xfea7);
  changeContainerLevel(55,  oxMsg.available / oxMsg.total, ox, 0x465f);
  changeContainerLevel(110, abMsg.available / abMsg.total, ab, 0xffe8);
  changeContainerLevel(165, ecMsg.available / ecMsg.total, ec, 0x47e8);
  changeContainerLevel(220, mpMsg.available / mpMsg.total, mp, 0xef7d);
}

void drawContainer(int left, String containerName, unsigned int color) {

  lcd.drawRect(
    left + containerMargin, // left (x)
    containerMargin,        // top (y)
    containerWidth,         // width
    containerHeight,        // height
    color);                 // color
  lcd.setCursor(left + (containerWidth / 2) - (lcd.textWidth() / 2), containerHeight + 8);
  lcd.print(containerName);
}
void fillContainer(int left, int yOffset, int heightOffset, unsigned int color) {  

  lcd.fillRect(
    left + containerMargin + 2,         // left (x)
    containerMargin + 2 + yOffset,      // top (y)
    containerWidth - 4,                 // width
    containerHeight - 4 - heightOffset, // height
    color);                             // color
}
void changeContainerLevel(int left, float currentLevel, float &previousLevel, unsigned int color) {

  if (currentLevel == previousLevel) {
    return;
  }

  if (currentLevel > 1) {
    currentLevel = 1;
  }
  else if (currentLevel < 0) {
    currentLevel = 0;
  }

  int level = calculateContainerHeight(currentLevel);
  int plevel = calculateContainerHeight(previousLevel);
  int dlevel;

  if (currentLevel < previousLevel)
  {
    dlevel = calculateContainerHeight(previousLevel - currentLevel);
    fillContainer(left, plevel, dlevel, BLACK);
  }
  else
  { 
    dlevel = calculateContainerHeight(currentLevel - previousLevel);
    fillContainer(left, level, dlevel, color);
  }
  previousLevel = currentLevel;
}
int calculateContainerHeight(float percent) {
  
    return containerHeight - ((containerHeight - 4) * percent) - 4;
}

void messageHandler(byte messageType, byte msg[], byte msgSize) {
  
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