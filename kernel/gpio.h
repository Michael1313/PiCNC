#include <asm/io.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>

#define  PIN_PULL_OFF               0
#define  PIN_PULL_DOWN              1
#define  PIN_PULL_UP                2

#define GPIO_INPUT     0b000
#define GPIO_OUTPUT    0b001
#define GPIO_ALT_FUNC0 0b100
#define GPIO_ALT_FUNC1 0b101
#define GPIO_ALT_FUNC2 0b110
#define GPIO_ALT_FUNC3 0b111
#define GPIO_ALT_FUNC4 0b011
#define GPIO_ALT_FUNC5 0b010

#define PERIPH_BASE 0x3f000000
#define GPIO_BASE (PERIPH_BASE + 0x200000)
#define PWM_BASE (PERIPH_BASE + 0x20C000)
#define PWM_CLK_BASE (PERIPH_BASE + 0x101000)
#define PWMCLK_CNTL	40
#define PWMCLK_DIV	41

struct GpioRegisters
{
      uint32_t GPFSEL[6];
      uint32_t Reserved0; // Reserved byte
      uint32_t GPSET[2];
      uint32_t Reserved1;
      uint32_t GPCLR[2];
      uint32_t Reserved2;
      uint32_t GPLEV[2];
      uint32_t Reserved3;
      uint32_t GPEDS[2];
      uint32_t Reserved4;
      uint32_t GPREN[2];
      uint32_t Reserved5;
      uint32_t GPFEN[2];
      uint32_t Reserved6;
      uint32_t GPHEN[2];
      uint32_t Reserved7;
      uint32_t GPLEN[2];
      uint32_t Reserved8;
      uint32_t GPAREN[2];
      uint32_t Reserved9;
      uint32_t GPAFEN[2];
      uint32_t Reserved10;
      uint32_t GPPUD;
      uint32_t GPPUDCLK[2];
      uint32_t Reserved11[4];
} *s_pGpioRegisters;

static void SetGPIOFunction(int GPIO, int functionCode)
{
    int registerIndex = GPIO / 10;
    int bit = (GPIO % 10) * 3;

    unsigned oldValue = s_pGpioRegisters->GPFSEL[registerIndex];
    unsigned mask = 0b111 << bit;
    printk("Changing function of GPIO%d from %x to %x\n", GPIO, (oldValue >> bit) & 0b111, functionCode);
    s_pGpioRegisters->GPFSEL[registerIndex] = (oldValue & ~mask) | ((functionCode << bit) & mask);
}

static void SetGPIOOutputValue(int GPIO, bool outputValue)
{
    if (outputValue)
        s_pGpioRegisters->GPSET[GPIO / 32] = (1 << (GPIO % 32));
        //iowrite32( (1 << (GPIO % 32)), &s_pGpioRegisters->GPSET[GPIO / 32] );
    else
        s_pGpioRegisters->GPCLR[GPIO / 32] = (1 << (GPIO % 32));
        //iowrite32( (1 << (GPIO % 32)), &s_pGpioRegisters->GPCLR[GPIO / 32] );
}

static void set_pull_resistor(int GPIO, int ioctl_param){
    // Write to GPPUD to set the required control signal
    s_pGpioRegisters->GPPUD = ioctl_param;
         
    // Wait 150 cycles: the maximum frequency is 125Mhz, so that is 1.2us at most
    udelay( 2000 );
         
    // Write to GPPUDCLK to clock the control signal into the GPIO pads
    s_pGpioRegisters->GPPUDCLK[GPIO / 32] |= (1 << (GPIO % 32));
         
    // Wait 150 cycles: the maximum frequency is 125Mhz, so that is 1.2us at most
    udelay( 2000 );
         
    // Write to GPPUD to remove the control signal
    s_pGpioRegisters->GPPUD = 0;
         
    // Write to GPPUDCLK to remove the clock
    s_pGpioRegisters->GPPUDCLK[ GPIO / 32 ] = 0;
    printk("Resistor %d\n", ioctl_param);
}

static void pin_release(int GPIO){
    int registerIndex = GPIO / 10;
    int bit = (GPIO % 10) * 3;
    
    unsigned mask = 0b111 << bit;
    
    //Disable all interruptions
    s_pGpioRegisters->GPREN[ GPIO / 32 ] &= ~(1 << (GPIO % 32));    // Disable rising edge interruption
    s_pGpioRegisters->GPFEN[ GPIO / 32 ] &= ~(1 << (GPIO % 32));    // Disable falling edge interruption
    s_pGpioRegisters->GPHEN[ GPIO / 32 ] &= ~(1 << (GPIO % 32));    // Disable high detect interruption
    s_pGpioRegisters->GPLEN[ GPIO / 32 ] &= ~(1 << (GPIO % 32));    // Disable low detect interruption
    
    s_pGpioRegisters->GPCLR[GPIO / 32] |= (1 << (GPIO % 32));    //low level on GPIO
    s_pGpioRegisters->GPFSEL[registerIndex] &= ~mask;            // Set pin as input
    set_pull_resistor(GPIO, PIN_PULL_OFF);            //pull_none
    
    free_irq( gpio_to_irq(GPIO), NULL );
}

struct pwmRegisters
{
	uint32_t CTL;
	uint32_t STA;
	uint32_t DMAC;
	uint32_t reserved0;
	uint32_t RNG1;
	uint32_t DAT1;
	uint32_t FIF1;
	uint32_t reserved1;
	uint32_t RNG2;
	uint32_t DAT2;
} *s_pPwmRegisters;

struct S_PWM_CTL {
	unsigned PWEN1 : 1;
	unsigned MODE1 : 1;
	unsigned RPTL1 : 1;
	unsigned SBIT1 : 1;
	unsigned POLA1 : 1;
	unsigned USEF1 : 1;
	unsigned CLRF1 : 1;
	unsigned MSEN1 : 1;
	unsigned PWEN2 : 1;
	unsigned MODE2 : 1;
	unsigned RPTL2 : 1;
	unsigned SBIT2 : 1;
	unsigned POLA2 : 1;
	unsigned USEF2 : 1;
	unsigned Reserved1 : 1;
	unsigned MSEN2 : 1;
	unsigned Reserved2 : 16;
} *pwm_ctl;

struct S_PWM_STA {
	unsigned FULL1 : 1;
	unsigned EMPT1 : 1;
	unsigned WERR1 : 1;
	unsigned RERR1 : 1;
	unsigned GAPO1 : 1;
	unsigned GAPO2 : 1;
	unsigned GAPO3 : 1;
	unsigned GAPO4 : 1;
	unsigned BERR : 1;
	unsigned STA1 : 1;
	unsigned STA2 : 1;
	unsigned STA3 : 1;
	unsigned STA4 : 1;
	unsigned Reserved : 19;
} *pwm_sta;

volatile unsigned int *s_pPwmClkRegisters;

/*
 * Establish the PWM frequency :
 */
static void pwm_frequency(uint32_t idiv) {

	/*
	* Kill the clock :
	*/
	*(s_pPwmClkRegisters+PWMCLK_CNTL) = 0x5A000020; /* Kill clock */
	pwm_ctl->PWEN1 = 0; /* Disable PWM */
	udelay(10);

	*(s_pPwmClkRegisters+PWMCLK_DIV) = 0x5A000000 | ( idiv << 12 );

	/*
	* Set source to oscillator and enable clock :
	*/
	*(s_pPwmClkRegisters+PWMCLK_CNTL) = 0x5A000011;
	udelay(10);
	printk("Busy clk == %d\n", (*(s_pPwmClkRegisters+PWMCLK_CNTL) & 0x80) );
	/*
	* GPIO 18 is PWM, when set to Alt Func 5 :
	*/
	SetGPIOFunction(18, GPIO_INPUT); /* Set ALT = 0 */
	SetGPIOFunction(18, GPIO_ALT_FUNC5); /* Or in '5 ' */
	printk("Sbit == %d\n", (pwm_ctl->SBIT1 & 0xFF) );

	pwm_ctl->MODE1 = 0; /* PWM mode */
	pwm_ctl->RPTL1 = 0;
	pwm_ctl->SBIT1 = 0;
	pwm_ctl->POLA1 = 0;
	pwm_ctl->USEF1 = 0;
	pwm_ctl->MSEN1 = 0;
//	pwm_ctl->MSEN1 = 1;
//	pwm_ctl->POLA1 = 1;
	pwm_ctl->CLRF1 = 1;
}

static void pwm_ratio(unsigned n,unsigned m) {

	pwm_ctl->PWEN1 = 0; /* Disable */

	s_pPwmRegisters->RNG1 = m;
	s_pPwmRegisters->DAT1 = n;

	if ( !pwm_sta->STA1 ) {
		if ( pwm_sta->RERR1 ) pwm_sta->RERR1 = 1;
		if ( pwm_sta->WERR1 ) pwm_sta->WERR1 = 1;
		if ( pwm_sta->BERR ) pwm_sta->BERR = 1;
	}
	printk("Err == %d\n", s_pPwmRegisters -> STA );

	udelay(10); /* Pause */
	pwm_ctl->PWEN1 = 1; /* Enable */
}

static void remap_gpio(void){
	s_pGpioRegisters = (struct GpioRegisters *)ioremap(GPIO_BASE, sizeof(struct GpioRegisters));
}

static void remap_pwm(void){
	s_pPwmRegisters = (struct pwmRegisters *)ioremap(PWM_BASE, sizeof(struct pwmRegisters));
	pwm_ctl = (struct S_PWM_CTL *) &s_pPwmRegisters -> CTL;
	pwm_sta = (struct S_PWM_STA *) &s_pPwmRegisters -> STA;
	s_pPwmClkRegisters = ioremap(PWM_CLK_BASE, 4096);
}

static void unmap_gpio(void){
	iounmap(s_pGpioRegisters);
}

static void unmap_pwm(void){
	iounmap(s_pPwmRegisters);
	iounmap(s_pPwmClkRegisters);
}
