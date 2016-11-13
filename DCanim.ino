#include <FastLED.h>

#define LED_PIN     5
#define NUM_LEDS    33
#define BRIGHTNESS  64
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];


//static uint8_t ledLookup[] =     {   0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32};
static uint8_t ledLookup[] =       {  29, 15,  8,  2,  4,  6, 11, 13, 21,  0,  9, 10, 12,  7, 13, 14,  1, 16, 17, 18, 19, 20,  3, 22, 23, 24, 25, 26, 27, 28,  9, 31, 32};

static uint8_t ledMAX[] =          { 255,255,255,255,255,255,255,255,230,230, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80};
static uint8_t ledREST[] =         { 240,240,240,240,240,240,240,240,200,200, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40};

static uint8_t ledStartFrame[] =   {   2, 10, 16, 17, 22, 25, 28, 29, 34, 35, 36, 38, 39, 40, 49, 51, 51, 52, 55, 55, 55, 55, 58, 59, 61, 62, 70, 70, 70, 74, 78, 79, 80};
static uint8_t ledFullFrame[] =    {  18, 34, 31, 39, 42, 42, 48, 48, 54, 55, 56, 58, 59, 60, 69, 71, 71, 72, 75, 75, 75, 75, 78, 79, 81, 82, 90, 90, 90, 94, 96, 96, 96};
static uint8_t ledDeclineFrame[] = {  32, 46, 46, 54, 61, 48, 52, 49, 61, 75, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 96, 96, 96, 96, 96, 96, 96};
static uint8_t ledLastFrame[] =    {  63, 78, 74, 83, 88, 69, 81, 79, 74, 87, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 96, 96, 96, 96, 96, 96, 96};

static uint8_t ledStartAdd [] =    {   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0};
static uint8_t ledDeclineSub [] =  {   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0};

static uint8_t ledCurBright [] =   {   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0};


static uint8_t cb = 0;
static int8_t cbi = 1;

#define UPDATES_PER_SECOND 30


CRGBPalette16 currentPalette;
TBlendType    currentBlending;

unsigned long timeStart;
static int curframe = 0;

void setup() {

    Serial.begin(57600);
    Serial.println("resetting!");
  
    delay( 3000 ); // power-up safety delay
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness(  BRIGHTNESS );

    timeStart = millis();
    
    currentPalette = RainbowColors_p;
    currentBlending = LINEARBLEND;

    ResetD();


}

void ResetD()
{
    curframe = 0;
    for( int i = 0; i < NUM_LEDS; i++) 
    {
      uint8_t r = ledFullFrame[i] - ledStartFrame[i];
      uint8_t frac = ledMAX[i]/r;
      ledStartAdd[i] = frac;

      r = ledLastFrame[i] - ledDeclineFrame[i];
      frac = (ledMAX[i] - ledREST[i]) / r;
      ledDeclineSub[i] = frac;
    }
}


void loop()
{

    uint8_t secondHand = (millis() / 1000) % 60;
    static uint8_t lastSecond = 99;


    if (secondHand < 10)
    {
    
      int index = 0;
  
      int deltaTime = millis()-timeStart;
      timeStart = millis();
 
  
      curframe++;
      //Serial.println(deltaTime);
      //Serial.println(curframe);
      
      currentPalette = CloudColors_p; 
      currentBlending = LINEARBLEND;
      
      for( int i = 0; i < NUM_LEDS; i++) 
      {
          int j = ledLookup[i];
        
          if (curframe >= ledLastFrame[i])
          {
            leds[j] = CRGB( ledREST[i], ledREST[i], ledREST[i]);
          }
          else
          {
            if (curframe > ledStartFrame[i]) 
            {
              if (curframe <= ledFullFrame[i])
              { // increasing to full
                ledCurBright[j] = ledCurBright[i] + ledStartAdd[i];
                leds[j] = CRGB( ledCurBright[i], ledCurBright[i], ledCurBright[i]);               
              }
              else if (curframe <= ledDeclineFrame[i])
              { // steady at full
                leds[j] = CRGB( ledMAX[i], ledMAX[i], ledMAX[i]); 
              }
              else if (curframe <= ledLastFrame[i])
              { // declining to rest
                ledCurBright[j] = ledCurBright[i] - ledDeclineSub[i];
                leds[j] = CRGB( ledCurBright[i], ledCurBright[i], ledCurBright[i]);               
              }
            }
            else
            {
              leds[j] = CRGB( 0, 0, 0);
            }
          }
      }
    }
    else
    {

      currentPalette = CloudColors_p; 
      currentBlending = LINEARBLEND;
      static uint8_t startIndex = 0;
      startIndex = startIndex + 1; /* motion speed */      
      FillLEDsFromPaletteColors( startIndex);
      if (curframe != 0)
      {
        ResetD();
      }

    }

    
    FastLED.show();
    FastLED.delay(1000 / UPDATES_PER_SECOND);
}


void FillLEDsFromPaletteColors( uint8_t colorIndex)
{
    uint8_t brightness = cb;
    
    for( int i = 0; i < NUM_LEDS; i++) {
        leds[i] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
        colorIndex += 3;
    }

    cb = cb + cbi;
    if (cb == 0)
    {
      cb = cb - cbi;
      cbi = 0-cbi;
    }
}



