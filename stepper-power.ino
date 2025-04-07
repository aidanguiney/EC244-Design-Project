// Define pin connections for TMC2208
#define STEP_PIN 9
#define DIR_PIN 8
#define ENABLE_PIN 5  // Enable pin for TMC2208
#define POT_PIN A0    // Potentiometer connected to A0

// Define motor parameters
#define STEPS_PER_REV 1600  // Total steps for one revolution (including microstepping)
#define DEAD_ZONE 30        // Threshold to stop motor when pot is near center

// Timing variables
unsigned long previousMillis = 0;
const long interval = 6000;  // 6 seconds delay (6,000 milliseconds)
int rotationPhase = 0;       // To track the current phase of the motor operation
bool initialSetupDone = false;
bool pauseCompleted = false; // Flag to track if pause before CCW turns is completed

// Speed parameters - lower values = faster rotation
#define FIXED_ROTATION_DELAY 150  // Delay for fixed rotations
#define CONTINUOUS_ROTATION_DELAY 250  // Delay for continuous rotation

// Pause duration before starting CCW turns (in milliseconds)
#define PAUSE_DURATION 2000  // 2 second pause

// Serial debug
const bool DEBUG = true;

// DIRECTION CORRECTION: Reversing the direction logic
// Set to LOW for clockwise and HIGH for counter-clockwise (reversed from previous code)
#define CLOCKWISE LOW
#define COUNTERCLOCKWISE HIGH

void setup() {
  // Initialize pins
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(ENABLE_PIN, OUTPUT);
  pinMode(POT_PIN, INPUT);

  // Enable the stepper driver (LOW = enabled for TMC2208)
  digitalWrite(ENABLE_PIN, LOW);
  
  if (DEBUG) {
    Serial.begin(9600);
    Serial.println("Stepper Motor Control System Initializing...");
  }

  // Short delay to ensure everything is ready
  delay(1000);
  
  // Enter the first phase - we'll start with a quarter, half, full turn CW
  rotationPhase = 0;
  previousMillis = millis();
}

// Function to step the motor a specific number of steps
void stepMotor(int steps, int speedDelay) {
  for (int i = 0; i < steps; i++) {
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(speedDelay);
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(speedDelay);
  }
}

// Performs the initial demonstration sequence
void performDemoSequence() {
  if (!initialSetupDone) {
    if (DEBUG) Serial.println("Starting initial demo sequence");
    initialSetupDone = true;
    
    // First do quarter, half, full clockwise rotation
    if (DEBUG) Serial.println("Clockwise quarter turn");
    digitalWrite(DIR_PIN, CLOCKWISE); // Set direction clockwise
    stepMotor(STEPS_PER_REV / 4, FIXED_ROTATION_DELAY); // Quarter turn
    delay(1000);
    
    if (DEBUG) Serial.println("Clockwise half turn");
    stepMotor(STEPS_PER_REV / 2, FIXED_ROTATION_DELAY); // Half turn
    delay(1000);
    
    if (DEBUG) Serial.println("Clockwise full turn");
    stepMotor(STEPS_PER_REV, FIXED_ROTATION_DELAY); // Full turn
    delay(1000);
    
    // Enter continuous clockwise rotation phase
    if (DEBUG) Serial.println("Starting continuous clockwise rotation for 6s");
    rotationPhase = 1;
    previousMillis = millis();
  }
}

void loop() {
  unsigned long currentMillis = millis();
  
  // Run the initial demo sequence if not already done
  if (!initialSetupDone) {
    performDemoSequence();
    return;
  }
  
  // Execute phases based on the current rotation phase
  switch (rotationPhase) {
    case 1:
      // Continuous clockwise rotation for 6 seconds
      if (currentMillis - previousMillis < interval) {
        digitalWrite(DIR_PIN, CLOCKWISE); // Clockwise
        digitalWrite(STEP_PIN, HIGH);
        delayMicroseconds(CONTINUOUS_ROTATION_DELAY);
        digitalWrite(STEP_PIN, LOW);
        delayMicroseconds(CONTINUOUS_ROTATION_DELAY);
      } else {
        // After 6 seconds, add pause before CCW turns
        if (DEBUG) Serial.println("Pausing before counter-clockwise turns");
        previousMillis = currentMillis;
        rotationPhase = 2;  // Move to pause phase
        pauseCompleted = false;  // Reset pause flag
      }
      break;
      
    case 2:
      // Pause before starting counter-clockwise turns
      if (currentMillis - previousMillis >= PAUSE_DURATION && !pauseCompleted) {
        // After pause, start CCW quarter, half, full turns
        if (DEBUG) Serial.println("Starting counter-clockwise turns");
        pauseCompleted = true;  // Mark pause as completed
        
        // Quarter, half, full counter-clockwise rotation
        digitalWrite(DIR_PIN, COUNTERCLOCKWISE); // Counter-clockwise
        
        if (DEBUG) Serial.println("Counter-clockwise quarter turn");
        stepMotor(STEPS_PER_REV / 4, FIXED_ROTATION_DELAY); // Quarter turn
        delay(1000);
        
        if (DEBUG) Serial.println("Counter-clockwise half turn");
        stepMotor(STEPS_PER_REV / 2, FIXED_ROTATION_DELAY); // Half turn
        delay(1000);
        
        if (DEBUG) Serial.println("Counter-clockwise full turn");
        stepMotor(STEPS_PER_REV, FIXED_ROTATION_DELAY); // Full turn
        delay(1000);
        
        // After CCW turns, move to continuous CCW rotation
        if (DEBUG) Serial.println("Starting continuous counter-clockwise rotation for 6s");
        rotationPhase = 3;
        previousMillis = millis();  // Reset timer for accurate CCW continuous timing
      }
      break;
      
    case 3:
      // Continuous counter-clockwise rotation for 6 seconds
      if (currentMillis - previousMillis < interval) {
        digitalWrite(DIR_PIN, COUNTERCLOCKWISE); // Counter-clockwise
        digitalWrite(STEP_PIN, HIGH);
        delayMicroseconds(CONTINUOUS_ROTATION_DELAY);
        digitalWrite(STEP_PIN, LOW);
        delayMicroseconds(CONTINUOUS_ROTATION_DELAY);
      } else {
        // After 6 seconds, move to potentiometer control mode
        if (DEBUG) Serial.println("Starting potentiometer control mode");
        rotationPhase = 4;
      }
      break;
      
    case 4:
      // Potentiometer-controlled speed and direction
      int potValue = analogRead(POT_PIN);
      
      // Map potentiometer value to motor speed
      // Lower delay value = faster speed (increased speed range)
      int speedDelay = map(abs(potValue - 512), 0, 512, 1500, 100);
      
      // Check if potentiometer is in the dead zone (near center)
      if (potValue > 512 - DEAD_ZONE && potValue < 512 + DEAD_ZONE) {
        // If in dead zone, don't step the motor
        return;
      }
      
      // Set direction based on potentiometer position
      // Note: Direction logic is now reversed to match actual motor behavior
      if (potValue < 512) {
        digitalWrite(DIR_PIN, CLOCKWISE); // Clockwise
      } else {
        digitalWrite(DIR_PIN, COUNTERCLOCKWISE);  // Counter-clockwise
      }
      
      // Step the motor with the calculated speed
      digitalWrite(STEP_PIN, HIGH);
      delayMicroseconds(speedDelay);
      digitalWrite(STEP_PIN, LOW);
      delayMicroseconds(speedDelay);
      break;
  }
}
