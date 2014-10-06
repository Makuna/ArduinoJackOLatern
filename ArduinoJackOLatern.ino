// proximity sensor constants
#define ProximitySensorAnalogIndex 0 // note:  This is specifically the analog index, not physical pin
#define ProximityThreshold 85 // larger is closer, 65-512 usable ranges
#define ProximityConsecutiveReadings 32 // number of readings at thresholds before changing proximity state

// proximity effect constants
#define RedLedPinA 7
#define RedLedPinB 6
#define BuzzerPin 5
#define BuzzerFrequencyHertz 5000

// candle effect constants
#define CandleLedPinA 11
#define CandleLedPinB 12
#define CandleFlickerMaxInterval 400
#define CandleFlickerMinInterval 100
#define CandleFlickerMaxBrightness 255
#define CandleFlickerMinBrightness 64

// candle effect variables
unsigned long candlePreviousTime = 0;
unsigned long candleFlickerInterval = CandleFlickerMinInterval;
int candleFlickerStart = CandleFlickerMinBrightness;
int candleFlickerTarget = CandleFlickerMaxBrightness;
int candleFlickerCurrent = CandleFlickerMinBrightness;

// proximity sensor variables
int proximityInRangeReadings = 0;
bool isProximityDetected = false;

void setup() 
{
  pinMode(CandleLedPinA, OUTPUT);
  pinMode(CandleLedPinB, OUTPUT);
  pinMode(RedLedPinA, OUTPUT);
  pinMode(RedLedPinB, OUTPUT);
  pinMode(BuzzerPin, OUTPUT);

  delay(1000); // Allow the proximity sensor to initialize
}

void loop() 
{
  UpdateProximityDetection();
  if (isProximityDetected)
  {
    TurnOffCandleEffect();
    UpdateProximityEffect();
  }
  else
  {
    TurnOffProximityEffect();
    UpdateCandleEffect();
  }
}


void TurnOffProximityEffect()
{
  noTone(BuzzerPin);
  digitalWrite(RedLedPinA, LOW);
  digitalWrite(RedLedPinB, LOW);
}

void UpdateProximityEffect()
{
  digitalWrite(RedLedPinA, HIGH);
  digitalWrite(RedLedPinB, HIGH);
  tone(BuzzerPin, BuzzerFrequencyHertz);
}

void TurnOffCandleEffect()
{
  digitalWrite(CandleLedPinA, LOW);
  digitalWrite(CandleLedPinB, LOW);
}

void UpdateCandleEffect()
{
  unsigned long currentTime = millis();
  unsigned long deltaTime = currentTime - candlePreviousTime;
  if (deltaTime > candleFlickerInterval)
  {
    // reached target, pick new brightness and time
    candlePreviousTime = currentTime;
    
    candleFlickerStart = candleFlickerTarget;
    candleFlickerTarget = random(CandleFlickerMinBrightness, CandleFlickerMaxBrightness);
    candleFlickerInterval = random(CandleFlickerMinInterval, CandleFlickerMaxInterval);
  }
  
  // linear interpolation from source to target giving current time progress
  candleFlickerCurrent = candleFlickerStart + candleFlickerInterval * (candleFlickerTarget - candleFlickerStart) / deltaTime;
  
  analogWrite(CandleLedPinA, candleFlickerCurrent);
  analogWrite(CandleLedPinB, candleFlickerCurrent);
}

void UpdateProximityDetection()
{
  int proximity = analogRead(ProximitySensorAnalogIndex);
  
  // leakey bucket implementation for state change
  // this provides a debounce filter
  //
  
  // update count of in range readings
  if (proximity > ProximityThreshold)
  {
    // fill to limit
    proximityInRangeReadings = min(ProximityConsecutiveReadings, proximityInRangeReadings + 1);
  }
  else if (proximityInRangeReadings > 0)
  {
    // leak
    proximityInRangeReadings--;
  }
  
  // test thresholds and set state only if thresholds are reached
  // otherwise leave the state alone
  if (proximityInRangeReadings == ProximityConsecutiveReadings)
  {
    isProximityDetected = true;
  }
  else if (proximityInRangeReadings == 0)
  {
    isProximityDetected = false;
  }
}

