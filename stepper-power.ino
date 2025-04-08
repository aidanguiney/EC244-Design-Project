// Define pin connections for TMC2208
#define STEP_PIN 9
#define DIR_PIN 8
#define ENABLE_PIN 5  // Enable pin for TMC2208
#define POT_PIN A0    // Potentiometer connected to A0

// Define motor parameters
#define STEPS_PER_REV 1600  // Total steps for one revolution (including microstepping)
#define DEAD_ZONE 30        // Threshold to stop motor when pot is near center

// Define rotation phases as constants
#define PHASE_DEMO_SEQUENCE 0
#define PHASE_CW_CONTINUOUS 1
#define PHASE_PAUSE_BEFORE_CCW 2
#define PHASE_CCW_TURNS 3
#define PHASE_CCW_CONTINUOUS 4
#define PHASE_POT_CONTROL 5

// Timing constants
#define CONTINUOUS_ROTATION_DURATION_MS 6000  // 6 seconds
#define PAUSE_DURATION_MS 2000  // 2 second pause
#define TURN_PAUSE_MS 1000  // 1 second pause between turns

// Speed parameters - lower values = faster rotation
#define FIXED_ROTATION_DELAY 150  // Delay for fixed rotations
#define CONTINUOUS_ROTATION_DELAY 250  // Delay for continuous rotation

// DIRECTION CORRECTION: Reversing the direction logic
// Set to LOW for clockwise and HIGH for counter-clockwise (reversed from previous code)
#define CLOCKWISE LOW
#define COUNTERCLOCKWISE HIGH

// State machine variables
int currentPhase = PHASE_DEMO_SEQUENCE;
unsigned long phaseStartTime = 0;
unsigned long lastStepTime = 0;
bool phaseInitialized = false;

// Serial debug
const bool DEBUG = true;

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
  
  // Initialize state machine
  transitionToPhase(PHASE_DEMO_SEQUENCE);
}

// Function to transition to a new phase
void transitionToPhase(int newPhase) {
  currentPhase = newPhase;
  phaseStartTime = millis();
  phaseInitialized = false;
  
  if (DEBUG) {
    Serial.print("Transitioning to phase: ");
    Serial.println(newPhase);
  }
}

// Set direction of motor rotation
void setDirection(bool direction) {
  digitalWrite(DIR_PIN, direction);
}

// Step the motor once with given delay
void stepOnce(int speedDelay) {
  digitalWrite(STEP_PIN, HIGH);
  delayMicroseconds(speedDelay);
  digitalWrite(STEP_PIN, LOW);
  delayMicroseconds(speedDelay);
}

// Function to step the motor a specific number of steps
void stepMotor(int steps, int speedDelay) {
  for (int i = 0; i < steps; i++) {
    stepOnce(speedDelay);
  }
}

// Perform specific turn (quarter, half, full)
void performTurn(bool direction, float turnFraction, String turnDescription) {
  if (DEBUG) {
    Serial.print(direction == CLOCKWISE ? "Clockwise " : "Counter-clockwise ");
    Serial.println(turnDescription);
  }
  
  setDirection(direction);
  stepMotor(STEPS_PER_REV * turnFraction, FIXED_ROTATION_DELAY);
}

// Handle the initial demo sequence phase
void handleDemoSequence() {
  if (!phaseInitialized) {
    if (DEBUG) Serial.println("Starting initial demo sequence");
    phaseInitialized = true;
    
    // Quarter turn clockwise
    performTurn(CLOCKWISE, 0.25, "quarter turn");
    delay(TURN_PAUSE_MS);
    
    // Half turn clockwise
    performTurn(CLOCKWISE, 0.5, "half turn");
    delay(TURN_PAUSE_MS);
    
    // Full turn clockwise
    performTurn(CLOCKWISE, 1.0, "full turn");
    delay(TURN_PAUSE_MS);
    
    // Move to continuous clockwise rotation
    transitionToPhase(PHASE_CW_CONTINUOUS);
  }
}

// Handle continuous clockwise rotation phase
void handleContinuousClockwise() {
  unsigned long currentTime = millis();
  
  // Check if it's time to transition to next phase
  if (currentTime - phaseStartTime >= CONTINUOUS_ROTATION_DURATION_MS) {
    transitionToPhase(PHASE_PAUSE_BEFORE_CCW);
    return;
  }
  
  // Perform continuous rotation
  setDirection(CLOCKWISE);
  stepOnce(CONTINUOUS_ROTATION_DELAY);
}

// Handle pause before counter-clockwise turns
void handlePauseBeforeCCW() {
  if (millis() - phaseStartTime >= PAUSE_DURATION_MS) {
    transitionToPhase(PHASE_CCW_TURNS);
  }
}

// Handle counter-clockwise turns phase
void handleCCWTurns() {
  if (!phaseInitialized) {
    phaseInitialized = true;
    
    if (DEBUG) Serial.println("Starting counter-clockwise turns");
    
    // Quarter turn counter-clockwise
    performTurn(COUNTERCLOCKWISE, 0.25, "quarter turn");
    delay(TURN_PAUSE_MS);
    
    // Half turn counter-clockwise
    performTurn(COUNTERCLOCKWISE, 0.5, "half turn");
    delay(TURN_PAUSE_MS);
    
    // Full turn counter-clockwise
    performTurn(COUNTERCLOCKWISE, 1.0, "full turn");
    delay(TURN_PAUSE_MS);
    
    // Move to continuous counter-clockwise rotation
    transitionToPhase(PHASE_CCW_CONTINUOUS);
  }
}

// Handle continuous counter-clockwise rotation phase
void handleContinuousCCW() {
  unsigned long currentTime = millis();
  
  // Check if it's time to transition to next phase
  if (currentTime - phaseStartTime >= CONTINUOUS_ROTATION_DURATION_MS) {
    transitionToPhase(PHASE_POT_CONTROL);
    return;
  }
  
  // Perform continuous rotation
  setDirection(COUNTERCLOCKWISE);
  stepOnce(CONTINUOUS_ROTATION_DELAY);
}

// Handle potentiometer control phase
void handlePotControl() {
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
  setDirection(potValue < 512 ? CLOCKWISE : COUNTERCLOCKWISE);
  
  // Step the motor with the calculated speed
  stepOnce(speedDelay);
}

void loop() {
  // State machine implementation
  switch (currentPhase) {
    case PHASE_DEMO_SEQUENCE:
      handleDemoSequence();
      break;
      
    case PHASE_CW_CONTINUOUS:
      handleContinuousClockwise();
      break;
      
    case PHASE_PAUSE_BEFORE_CCW:
      handlePauseBeforeCCW();
      break;
      
    case PHASE_CCW_TURNS:
      handleCCWTurns();
      break;
      
    case PHASE_CCW_CONTINUOUS:
      handleContinuousCCW();
      break;
      
    case PHASE_POT_CONTROL:
      handlePotControl();
      break;
  }
}
