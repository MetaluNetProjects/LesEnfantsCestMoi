/*********************************************************************
 *               analog example for Versa1.0
 *	Analog capture on connectors K1, K2, K3 and K5. 
 *********************************************************************/

#define BOARD Versa1

#include <fruit.h>
#include <analog.h>
#include <dcmotor.h>
#include <ramp.h>

t_delay mainDelay;
t_ramp speedRamp;

int motorDelay = 0;

DCMOTOR_DECLARE(B);

/*#define SET_PWM(pwm,val) do{ CCP##pwm##CONbits.DC##pwm##B1 = val&2;  CCP##pwm##CONbits.DC##pwm##B0 = val&1; CCPR##pwm##L=val>>2; } while(0)*/


void setup(void) {	
//----------- Setup ----------------
	fruitInit();
	delayStart(mainDelay, 5000); 	// init the mainDelay to 5 ms

//----------- Analog setup ----------------
	analogInit();		// init analog module
	analogSelect(0, MOTB_CURRENT);	// assign MotorB current sense to analog channel 1
	
	digitalClear(MOTORRELAY);
	digitalClear(LAMP);
	pinModeDigitalOut(MOTORRELAY);
	pinModeDigitalOut(LAMP);

    dcmotorInit(B);
	
	CCP5CON = 0b00001100; /* CCP5 = single PWM active high*/
	SET_PWM(5,0);
	
	rampInit(&speedRamp);
	rampSetPos(&speedRamp,0);
	
	EEreadMain();
}


void loop() {
// ---------- Main loop ------------
    static unsigned char on = 0;
    
	fraiseService();	// listen to Fraise events
	analogService();	// analog management routine
	
	if(delayFinished(mainDelay)) // when mainDelay triggers :
	{
		delayStart(mainDelay, 5000); 	// re-init mainDelay
		analogSend();		// send analog channels that changed
		
		rampCompute(&speedRamp);
		DCMOTOR(B).Vars.PWMConsign = rampGetPos(&speedRamp);
		
		DCMOTOR_COMPUTE(B,SYM);
		
		if(abs(DCMOTOR(B).Vars.PWMConsign) < 10) {
		    if(motorDelay != 0) motorDelay--;
		    else { on = 0;digitalClear(MOTORRELAY); }
		} else {
		    motorDelay = 400; // 400x5ms=2s
		    digitalSet(MOTORRELAY);
		    on = 1;
		}
		printf("C M %d %d\n", DCMOTOR(B).Vars.PWMConsign, on);
	}
}

// Receiving

void fraiseReceiveChar() // receive text
{
	unsigned char c=fraiseGetChar();
	
	if(c=='E') { 	// echo text (send it back to host)
		printf("C");
		c = fraiseGetLen(); 			// get length of current packet
		while(c--) printf("%c",fraiseGetChar());// send each received byte
		putchar('\n');				// end of line
	}
	else if((c=='S') && (fraiseGetChar()=='A') && (fraiseGetChar()=='V') && (fraiseGetChar()=='E'))
	    EEwriteMain();
}

void fraiseReceive() // receive raw bytes
{
 	unsigned int i;
 	unsigned char c = fraiseGetChar();
 	
	switch(c) {
	    case 20 : i = fraiseGetInt() ; SET_PWM(5,i); break;
	    case 121 : DCMOTOR_INPUT(B) ; break;
	    case 122 : rampInput(&speedRamp) ; break;
	}
}

void EEdeclareMain()
{
    rampDeclareEE(&speedRamp);
}
