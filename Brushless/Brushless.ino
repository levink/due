
#define PLUS LOW
#define MINUS HIGH
const uint32_t steps[6] = {
	PIO_PC24,
	PIO_PC24 | PIO_PC25,
	PIO_PC25,
	PIO_PC25 | PIO_PC26,
	PIO_PC26,
	PIO_PC24 | PIO_PC26,
};
const uint32_t steps1[6] = {
	PIO_PC24,
	PIO_PC25,
	PIO_PC26,
	PIO_PC24,
	PIO_PC25,
	PIO_PC26
};

const uint16_t cosCount = 192; //192 //Число различных значений косинуса (должно быть кратно 2 и 3)
//const uint16_t cosCount = 96;
const uint16_t cosCountHalf = cosCount / 2;
const uint16_t pwmCos1[] = {//96
	500, 499, 498, 495, 491, 487, 481, 474, 467, 458, 448, 438, 427, 415, 402, 389, 375, 361, 346, 330, 315, 299, 283, 266, 250, 234, 217, 201, 185, 170, 154, 139, 125, 111, 98, 85, 73, 62, 52, 42, 33, 26, 19, 13, 9, 5, 2, 1,
	500, 499, 498, 495, 491, 487, 481, 474, 467, 458, 448, 438, 427, 415, 402, 389, 375, 361, 346, 330, 315, 299, 283, 266, 250, 234, 217, 201, 185, 170, 154, 139, 125, 111, 98, 85, 73, 62, 52, 42, 33, 26, 19, 13, 9, 5, 2, 1
};
const uint16_t pwmCos[] = {//192
	800, 800, 799, 798, 797, 795, 792, 790, 786, 783, 779, 774, 770, 764, 759, 753, 746, 740, 733, 725, 717, 709, 701, 692, 683, 673, 664, 654, 644, 633, 622, 611, 600, 589, 577, 565, 553, 541, 529, 516, 504, 491, 478, 465, 452, 439, 426, 413, 400, 387, 374, 361, 348, 335, 322, 309, 296, 284, 271, 259, 247, 235, 223, 211, 200, 189, 178, 167, 156, 146, 136, 127, 117, 108, 99, 91, 83, 75, 67, 60, 54, 47, 41, 36, 30, 26, 21, 17, 14, 10, 8, 5, 3, 2, 1, 0,
	800, 800, 799, 798, 797, 795, 792, 790, 786, 783, 779, 774, 770, 764, 759, 753, 746, 740, 733, 725, 717, 709, 701, 692, 683, 673, 664, 654, 644, 633, 622, 611, 600, 589, 577, 565, 553, 541, 529, 516, 504, 491, 478, 465, 452, 439, 426, 413, 400, 387, 374, 361, 348, 335, 322, 309, 296, 284, 271, 259, 247, 235, 223, 211, 200, 189, 178, 167, 156, 146, 136, 127, 117, 108, 99, 91, 83, 75, 67, 60, 54, 47, 41, 36, 30, 26, 21, 17, 14, 10, 8, 5, 3, 2, 1, 0
};


uint16_t ia, ib, ic;
volatile uint16_t a,b,c;
volatile uint32_t valA1, valA2, valB1, valB2, valC1, valC2;

//const short pwmSteps = 500;
const short pwmSteps = 800;
volatile short pwmCounter = 0;
uint32_t serialValue = 1000;
volatile int v = 0;

void TC3_Handler()
{
	pwmCounter++; //[0...pwmSteps)
	if (pwmCounter == pwmSteps) 
	{
		v++;
		pwmCounter = 0;
	}
	
	uint32_t val = 0;
	val |= (pwmCounter < a ? valA1 : valA2);
	val |= (pwmCounter < b ? valB1 : valB2);	
	val |= (pwmCounter < c ? valC1 : valC2);	
	
	PIOC->PIO_ODSR = val;
	TC_GetStatus(TC1, 0);
}
void startTimer(Tc *tc, uint32_t channel, IRQn_Type irq, uint32_t frequency) {
	pmc_set_writeprotect(false);
	pmc_enable_periph_clk((uint32_t)irq);
	TC_Configure(tc, channel, TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC | TC_CMR_TCCLKS_TIMER_CLOCK4);
	uint32_t rc = VARIANT_MCK/128/frequency;// VARIANT_MCK/128/frequency; //128 because we selected TIMER_CLOCK4 above
	TC_SetRC(tc, channel, rc);
	TC_Start(tc, channel);
	tc->TC_CHANNEL[channel].TC_IER=TC_IER_CPCS;
	tc->TC_CHANNEL[channel].TC_IDR=~TC_IER_CPCS;
	NVIC_EnableIRQ(irq);
}
void setup() {
	PIOB->PIO_PER = PIO_PB27;	//Enable PIN
	PIOB->PIO_OER = PIO_PB27;	//Set to output
	PIOB->PIO_PUDR = PIO_PB27;	//disable pull-up
	PIOB->PIO_OWER = PIO_PB27;	//unmask pin for enable control with PIO_ODSR

	PIOC->PIO_PER = PIO_PC24 | PIO_PC25 | PIO_PC26; 
	PIOC->PIO_OER = PIO_PC24 | PIO_PC25 | PIO_PC26; 
	PIOC->PIO_PUDR = PIO_PC24 | PIO_PC25 | PIO_PC26; 
	PIOC->PIO_OWER = PIO_PC24 | PIO_PC25 | PIO_PC26;
	
	ia = 0;
	ib = cosCount / 3;
	ic = 2 * cosCount / 3;
	setVals();
	
	uint32_t _300KHz = 300000;
	startTimer(TC1, 0, TC3_IRQn, _300KHz);
}
void setVals()
{
	
	if (ia >= cosCount){
		ia = 0;
		valA1 = 0;
		valA2 = 16777216; //1<<24
	}
	else if (ia >= cosCountHalf) 
	{
		valA1 = 16777216; //1<<24
		valA2 = 0;
	}

	if (ib >= cosCount){
		ib = 0;
		valB1 = 0;
		valB2 = 33554432; //1 << 25
	}
	else if (ib >= cosCountHalf) 
	{
		valB1 = 33554432; //1 << 25
		valB2 = 0;
	}

	if (ic >= cosCount){
		ic = 0;
		valC1 = 0;
		valC2 = 67108864; //1 << 26
	}
	else if (ic >= cosCountHalf) 
	{
		valC1 = 67108864; //1 << 26
		valC2 = 0;
	}
	
	a = pwmCos[ia];
	b = pwmCos[ib];
	c = pwmCos[ic];
}
void computeCos()
{
	const double p = 2 * PI / cosCount ;
	for(int i=0; i < cosCount/2; i++)
	{
		double val = pwmSteps*0.5*(1 + cos(i*p));
		if (val - (int)val > 0.5) Serial.print((int)val + 1);
		else Serial.print((int)val);
		Serial.print(", ");
	}
}

uint32_t last = millis();

bool once = true;
uint delayTime = 100000; //0.1 sec
void loop() {
	delayMicroseconds(delayTime);
	
	uint32_t cur = millis();
	if (cur - last > 1000)
	{
		last = cur;
		Serial.println(v);
		v = 0;
	}
	
	noInterrupts(); 
	ia++;
	ib++;
	ic++;
	setVals();
	interrupts();

	if (once)
	{	
		computeCos();
		once = false;
	}
}
void Rotate()
{
	uint16_t step = cosCount;
	while(step > 0)
	{
		ia++;
		ib++;
		ic++;
		setVals();
		delayMicroseconds(delayTime);
		step--;
	}
}
String bufSerial = "";
void serialEvent()
{
  while (Serial.available()) 
  {
    int c = Serial.read(); 
    if (isDigit(c)) bufSerial += (char)c;
    if (c == '\\') {
		delayTime = bufSerial.toInt();
		bufSerial = "";
    } 
  }
}