// Project Start Date 04.12.2023
// Author, tommojphillips (Tommo J. Productions) (tommojphillips)

#include <KerbalSimpit.h> // KerbalSimpit
#include <PayloadStructs.h> // KerbalSimpit MESSAGES
#include <MCUFRIEND_kbv.h> // Hardware Driver
#include <Container.h>

/// @brief  LCD hardware driver.
MCUFRIEND_kbv lcd;
/// @brief KerbalSimpit driver.  
KerbalSimpit ksp(Serial);

/// @brief liquid fuel stage msg
resourceMessage lfMsg;
/// @brief Oxidizer stage msg
resourceMessage oxMsg;
// Ablator in vessel msg
resourceMessage abMsg;
/// @brief Electic Charge in vessal msg
resourceMessage ecMsg;
/// @brief Monoprollent in vessel msg
resourceMessage mpMsg;
/// @brief Flight status msg (warp speed, status, crew, com)
flightStatusMessage flightMsg;

// Current Flight State (no_flight, in_flight)
bool inFlight = false;

ContainerSettings cSettings(50, 215, 5);

Container ox(lcd, cSettings);
Container lf(lcd, cSettings);
Container ab(lcd, cSettings);
Container ec(lcd, cSettings);
Container mp(lcd, cSettings);

enum FlightState { no_flight, flight, eva };
enum FlightPages { menu, resources, orbit, surface, landing };

FlightState flightState = no_flight;
FlightPages flightPage = resources;

void initLcd() 
{
  Serial.begin(115200);

  lcd.begin(lcd.readID());
  lcd.setRotation(1);
  lcd.clearScreen();

  lcd.setTextSize(2);
}
void initKsp()
{
  while (!ksp.init()) 
  {
    delay(100);
  }
  
  delay(1000);

  ksp.inboundHandler(onInboundMessage);

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
  
  centerText("Waiting for KSP", 3);

  initKsp();
  
  lcd.clearScreen();
  ksp.printToKSP("Connected", PRINT_TO_SCREEN);  

  centerText("Connected", 3);

  delay(1000);
}

void loop()
{  
  ksp.update();

  delay(80);

  flightStateCheck();

  switch (flightState)
  {
    case flight:
      drawFlight_page();
    break;
    case eva:
      drawEva_page();
    break;
  }  
}

void flightStateCheck()
{
  if (flightMsg.isInFlight() == inFlight) // NO CHANGE
  {
    return;
  }
  
  inFlight = !inFlight;
  
  lcd.clearScreen();

  if (!inFlight) // NO_FLIGHT detected
  {    
    noFlightPageEnter();
    flightState = FlightState::no_flight;     
    return;
  }

  if (flightMsg.isInEVA()) // ACTIVE_EVA detected
  {    
    evaPageEnter();
    flightState = FlightState::eva;
  }
  else // ACTIVE_FLIGHT detected
  {   
    flightPageEnter();
    flightState = FlightState::flight;
  }
}

void centerText(String text, unsigned int textSize)
{
  lcd.setTextSize(textSize);
  int x = (320 - (text.length() * lcd.textWidth())) / 2;
  int y = (240 - (lcd.textHeight())) / 2;
  lcd.setCursor(x, y);
  lcd.print(text);
  outlineText(text, x, y, 10, WHITE);
  lcd.setTextSize(2);
}
/// @brief outlines the text using the x and y coord system (PIXELS)
/// @param text the text to outline. used for to calculate the width of the rectangle.
/// @param x the coord X in Pixels.
/// @param y the coord Y in pixels.
/// @param padding // Padding in pixels between the text and the outline.
/// @param color // the color of the outline.
void outlineText(String text, int x, int y, unsigned int padding, unsigned int color)
{
  lcd.drawRect(x - padding, y - padding, (text.length() * lcd.textWidth()) + (padding * 2), lcd.textHeight() + (padding * 2), color);
}

/// @brief Occurs when entering Has Flight Page. Only stuff that needs to be updated/checked once goes here
void flightPageEnter()
{
    switch (flightPage)
    {
      default:
      case FlightPages::resources:
        setupResourcesPage();
      break;
    }
}
/// @brief Occurs when entering No Flight Page. Only stuff that needs to be updated/checked once goes here
void noFlightPageEnter()
{
  centerText("No Flight", 4);    
}
/// @brief Occurs when entering EVA Page. Only stuff that needs to be updated/checked once goes here
void evaPageEnter()
{
  centerText("EVA", 4);  
}

/// @brief Draws the flight page. Only stuff that needs to be updated/checked every loop goes here
void drawFlight_page()
{
  switch (flightPage)
  {
    default:
    case FlightPages::resources:
      drawResources_page();
    break;
  }  
}

void setupEvaPage()
{
  
}
/// @brief Draws the EVA page. Only stuff that needs to be updated/checked every loop goes here
void drawEva_page()
{
}

/// @brief Sets up the resource bars for the resources page.
void setupResourcesPage()
{
  // setting up containers
  lf.createContainer(0,   0, "LF", 0xfe40,  0xfea7);
  ox.createContainer(55,  0, "OX", 0x05bf,  0x465f);
  ab.createContainer(110, 0, "AB", YELLOW,  0xffe8);
  ec.createContainer(165, 0, "EC", GREEN,   0x47e8);
  mp.createContainer(220, 0, "MP", 0xc638,  0xef7d);
}
/// @brief Updates the resource bars UI.
void drawResources_page()
{
  lf.updateLevel(lfMsg.available / lfMsg.total);
  ox.updateLevel(oxMsg.available / oxMsg.total);
  ab.updateLevel(abMsg.available / abMsg.total);
  ec.updateLevel(ecMsg.available / ecMsg.total);
  mp.updateLevel(mpMsg.available / mpMsg.total);
}

/// @brief Handles inbound messages from Kerbal Space Program 1 using a mod called 'Simpit'.
/// @param messageType The type of message that is inbound.
/// @param msg the message in bytes.
/// @param msgSize The expected message size. used for validating that we actually got the data correctly. 
void onInboundMessage(byte messageType, byte msg[], byte msgSize) 
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
    if (msgSize == sizeof(resourceMessage)) 
    {
      mpMsg = parseMessage<resourceMessage>(msg);
    }
    break;
    case FLIGHT_STATUS_MESSAGE:
    if (msgSize == sizeof(flightStatusMessage))
    {
      flightMsg = parseMessage<flightStatusMessage>(msg);
    }
    break;
  }
}