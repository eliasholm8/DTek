#include <pic32mx.h>
#include <stdbool.h>
#include <stdint.h>

/*Taken from canvas*/

#define DISPLAY_CHANGE_TO_COMMAND_MODE (PORTFCLR = 0x10)
#define DISPLAY_CHANGE_TO_DATA_MODE (PORTFSET = 0x10)

#define DISPLAY_ACTIVATE_RESET (PORTGCLR = 0x200)
#define DISPLAY_DO_NOT_RESET (PORTGSET = 0x200)

#define DISPLAY_ACTIVATE_VDD (PORTFCLR = 0x40)
#define DISPLAY_ACTIVATE_VBAT (PORTFCLR = 0x20)

#define DISPLAY_TURN_OFF_VDD (PORTFSET = 0x40)
#define DISPLAY_TURN_OFF_VBAT (PORTFSET = 0x20)

typedef struct {
    int x;	// Position of ball
    int y;
    bool x_dir; // true = right -> left
    bool y_dir;
} Ball;

typedef struct {
    unsigned int position;	// Implemented as a bitmask
    bool is_left;
} Paddle;

unsigned int const paddle_const = 0x00ffff00; 

Paddle left_paddle;
Paddle right_paddle;

Ball ball = {64, 16, true, true};

int left_score, right_score;

uint8_t frame[128][32];	// Frame buffer

uint8_t block[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

/*Copied from canvas*/

void quicksleep(int cyc) {
	for(int i = cyc; i > 0; i--);
}

void init() {
       /*
	  This will set the peripheral bus clock to the same frequency
	  as the sysclock. That means 80 MHz, when the microcontroller
	  is running at 80 MHz. Changed 2017, as recommended by Axel.
	*/
	SYSKEY = 0xAA996655;  /* Unlock OSCCON, step 1 */
	SYSKEY = 0x556699AA;  /* Unlock OSCCON, step 2 */
	while(OSCCON & (1 << 21)); /* Wait until PBDIV ready */
	OSCCONCLR = 0x180000; /* clear PBDIV bit <0,1> */
	while(OSCCON & (1 << 21));  /* Wait until PBDIV ready */
	SYSKEY = 0x0;  /* Lock OSCCON */
	
	/* Set up output pins */
	AD1PCFG = 0xFFFF;
	ODCE = 0x0;
	TRISECLR = 0xFF;
	PORTE = 0x0;
	
	/* Output pins for display signals */
	PORTF = 0xFFFF;
	PORTG = (1 << 9);
	ODCF = 0x0;
	ODCG = 0x0;
	TRISFCLR = 0x70;
	TRISGCLR = 0x200;
	
	/* Set up input pins */
	TRISDSET = (1 << 8);
	TRISFSET = (1 << 1);
	
	/* Set up SPI as master */
	SPI2CON = 0;
	SPI2BRG = 4;
	/* SPI2STAT bit SPIROV = 0; */
	SPI2STATCLR = 0x40;
	/* SPI2CON bit CKP = 1; */
        SPI2CONSET = 0x40;
	/* SPI2CON bit MSTEN = 1; */
	SPI2CONSET = 0x20;
	/* SPI2CON bit ON = 1; */
	SPI2CONSET = 0x8000;
}

char textbuffer[4][16];

const uint8_t const font[] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 94, 0, 0, 0, 0,
	0, 0, 4, 3, 4, 3, 0, 0,
	0, 36, 126, 36, 36, 126, 36, 0,
	0, 36, 74, 255, 82, 36, 0, 0,
	0, 70, 38, 16, 8, 100, 98, 0,
	0, 52, 74, 74, 52, 32, 80, 0,
	0, 0, 0, 4, 3, 0, 0, 0,
	0, 0, 0, 126, 129, 0, 0, 0,
	0, 0, 0, 129, 126, 0, 0, 0,
	0, 42, 28, 62, 28, 42, 0, 0,
	0, 8, 8, 62, 8, 8, 0, 0,
	0, 0, 0, 128, 96, 0, 0, 0,
	0, 8, 8, 8, 8, 8, 0, 0,
	0, 0, 0, 0, 96, 0, 0, 0,
	0, 64, 32, 16, 8, 4, 2, 0,
	0, 62, 65, 73, 65, 62, 0, 0,
	0, 0, 66, 127, 64, 0, 0, 0,
	0, 0, 98, 81, 73, 70, 0, 0,
	0, 0, 34, 73, 73, 54, 0, 0,
	0, 0, 14, 8, 127, 8, 0, 0,
	0, 0, 35, 69, 69, 57, 0, 0,
	0, 0, 62, 73, 73, 50, 0, 0,
	0, 0, 1, 97, 25, 7, 0, 0,
	0, 0, 54, 73, 73, 54, 0, 0,
	0, 0, 6, 9, 9, 126, 0, 0,
	0, 0, 0, 102, 0, 0, 0, 0,
	0, 0, 128, 102, 0, 0, 0, 0,
	0, 0, 8, 20, 34, 65, 0, 0,
	0, 0, 20, 20, 20, 20, 0, 0,
	0, 0, 65, 34, 20, 8, 0, 0,
	0, 2, 1, 81, 9, 6, 0, 0,
	0, 28, 34, 89, 89, 82, 12, 0,
	0, 0, 126, 9, 9, 126, 0, 0,
	0, 0, 127, 73, 73, 54, 0, 0,
	0, 0, 62, 65, 65, 34, 0, 0,
	0, 0, 127, 65, 65, 62, 0, 0,
	0, 0, 127, 73, 73, 65, 0, 0,
	0, 0, 127, 9, 9, 1, 0, 0,
	0, 0, 62, 65, 81, 50, 0, 0,
	0, 0, 127, 8, 8, 127, 0, 0,
	0, 0, 65, 127, 65, 0, 0, 0,
	0, 0, 32, 64, 64, 63, 0, 0,
	0, 0, 127, 8, 20, 99, 0, 0,
	0, 0, 127, 64, 64, 64, 0, 0,
	0, 127, 2, 4, 2, 127, 0, 0,
	0, 127, 6, 8, 48, 127, 0, 0,
	0, 0, 62, 65, 65, 62, 0, 0,
	0, 0, 127, 9, 9, 6, 0, 0,
	0, 0, 62, 65, 97, 126, 64, 0,
	0, 0, 127, 9, 9, 118, 0, 0,
	0, 0, 38, 73, 73, 50, 0, 0,
	0, 1, 1, 127, 1, 1, 0, 0,
	0, 0, 63, 64, 64, 63, 0, 0,
	0, 31, 32, 64, 32, 31, 0, 0,
	0, 63, 64, 48, 64, 63, 0, 0,
	0, 0, 119, 8, 8, 119, 0, 0,
	0, 3, 4, 120, 4, 3, 0, 0,
	0, 0, 113, 73, 73, 71, 0, 0,
	0, 0, 127, 65, 65, 0, 0, 0,
	0, 2, 4, 8, 16, 32, 64, 0,
	0, 0, 0, 65, 65, 127, 0, 0,
	0, 4, 2, 1, 2, 4, 0, 0,
	0, 64, 64, 64, 64, 64, 64, 0,
	0, 0, 1, 2, 4, 0, 0, 0,
	0, 0, 48, 72, 40, 120, 0, 0,
	0, 0, 127, 72, 72, 48, 0, 0,
	0, 0, 48, 72, 72, 0, 0, 0,
	0, 0, 48, 72, 72, 127, 0, 0,
	0, 0, 48, 88, 88, 16, 0, 0,
	0, 0, 126, 9, 1, 2, 0, 0,
	0, 0, 80, 152, 152, 112, 0, 0,
	0, 0, 127, 8, 8, 112, 0, 0,
	0, 0, 0, 122, 0, 0, 0, 0,
	0, 0, 64, 128, 128, 122, 0, 0,
	0, 0, 127, 16, 40, 72, 0, 0,
	0, 0, 0, 127, 0, 0, 0, 0,
	0, 120, 8, 16, 8, 112, 0, 0,
	0, 0, 120, 8, 8, 112, 0, 0,
	0, 0, 48, 72, 72, 48, 0, 0,
	0, 0, 248, 40, 40, 16, 0, 0,
	0, 0, 16, 40, 40, 248, 0, 0,
	0, 0, 112, 8, 8, 16, 0, 0,
	0, 0, 72, 84, 84, 36, 0, 0,
	0, 0, 8, 60, 72, 32, 0, 0,
	0, 0, 56, 64, 32, 120, 0, 0,
	0, 0, 56, 64, 56, 0, 0, 0,
	0, 56, 64, 32, 64, 56, 0, 0,
	0, 0, 72, 48, 48, 72, 0, 0,
	0, 0, 24, 160, 160, 120, 0, 0,
	0, 0, 100, 84, 84, 76, 0, 0,
	0, 0, 8, 28, 34, 65, 0, 0,
	0, 0, 0, 126, 0, 0, 0, 0,
	0, 0, 65, 34, 28, 8, 0, 0,
	0, 0, 4, 2, 4, 2, 0, 0,
	0, 120, 68, 66, 68, 120, 0, 0,
};

void display_string(int line, char *s) {
	int i;
	if(line < 0 || line >= 4)
		return;
	if(!s)
		return;
	
	for(i = 0; i < 16; i++)
		if(*s) {
			textbuffer[line][i] = *s;
			s++;
		} else
			textbuffer[line][i] = ' ';
}

uint8_t spi_send_recv(uint8_t data) {
	while(!(SPI2STAT & 0x08));
	SPI2BUF = data;
	while(!(SPI2STAT & 1));
	return SPI2BUF;
}

void display_init(void) {
        DISPLAY_CHANGE_TO_COMMAND_MODE;
	quicksleep(10);
	DISPLAY_ACTIVATE_VDD;
	quicksleep(1000000);
	
	spi_send_recv(0xAE);
	DISPLAY_ACTIVATE_RESET;
	quicksleep(10);
	DISPLAY_DO_NOT_RESET;
	quicksleep(10);
	
	spi_send_recv(0x8D);
	spi_send_recv(0x14);
	
	spi_send_recv(0xD9);
	spi_send_recv(0xF1);
	
	DISPLAY_ACTIVATE_VBAT;
	quicksleep(10000000);
	
	spi_send_recv(0xA1);
	spi_send_recv(0xC8);
	
	spi_send_recv(0xDA);
	spi_send_recv(0x20);
	
	spi_send_recv(0xAF);
}

void display_update(void) {
	int i, j, k;
	int c;
	for(i = 0; i < 4; i++) {
		DISPLAY_CHANGE_TO_COMMAND_MODE;
		spi_send_recv(0x22);
		spi_send_recv(i);
		
		spi_send_recv(0x0);
		spi_send_recv(0x10);
		
		DISPLAY_CHANGE_TO_DATA_MODE;
		
		for(j = 0; j < 16; j++) {
			c = textbuffer[i][j];
			if(c & 0x80)
				continue;
			
			for(k = 0; k < 8; k++)
				spi_send_recv(font[c*8 + k]);
		}
	}
}

void display_image(int x, const uint8_t *data) {
	int i, j;
	
	for(i = 0; i < 4; i++) {
		DISPLAY_CHANGE_TO_COMMAND_MODE;

		spi_send_recv(0x22);
		spi_send_recv(i);
		
		spi_send_recv(x & 0xF);
		spi_send_recv(0x10 | ((x >> 4) & 0xF));
		
		DISPLAY_CHANGE_TO_DATA_MODE;
		
		for(j = 0; j < 32; j++)
			spi_send_recv(~data[i*32 + j]);
	}
}




void update_screen() {
    const int strips = 4;          // The display is divided into 4 vertical strips of 32 columns each (128/32=4).
    const int bands = 4;           // Each strip is divided into 4 horizontal bands of 8 pixels each (32/8=4).
    const int columns_per_strip = 32;
    const int rows_per_band = 8;   // Each band represents 8 vertical pixels.
    
    // block[] will hold data for a 32x32 region (4 bands of 8 rows and 32 columns)
    // 32 columns * 4 bands = 128 bytes total.

    for (int strip = 0; strip < strips; strip++) {
        for (int band = 0; band < bands; band++) {
            for (int pixel_row = 0; pixel_row < rows_per_band; pixel_row++) {
                for (int col = 0; col < columns_per_strip; col++) {
                    // Calculate the corresponding coordinates in the frame
                    int frame_x = col + (columns_per_strip * strip);
                    int frame_y = pixel_row + (rows_per_band * band);

                    // Calculate where to store this bit in the block array
                    int block_index = col + (columns_per_strip * band);

                    // Get the pixel value from frame (0 or 1)
                    uint8_t pixel_value = frame[frame_x][frame_y];
                    uint8_t bit_mask = (1U << pixel_row);

                    // Set or clear the bit in block based on the pixel value
                    if (pixel_value == 1) {
                        block[block_index] |= bit_mask;
                    } else {
                        block[block_index] &= ~bit_mask;
                    }
                }
            }
        }

        // After filling all bands for this strip, draw it on the display
        display_image((columns_per_strip * strip), block);
    }
}



void system_init() {
	// Enable interrupts
	asm("ei");

	// Paddle setup
    left_paddle.position = ~paddle_const;
    left_paddle.is_left = true;

    right_paddle.position = ~paddle_const;
    right_paddle.is_left = false;


    for (int i = 0; i < 128; i++) {
        for (int j = 0; j < 32; j++) {
            frame[i][j] = 1;
        }       
    }   

    TRISD = 0b11100000;
    TRISF = 0b10;
    PORTD = 0;
    PORTF = 0;

    IEC(0) |= 0x100;
    IPC(2) |= 0xC;

    IFS(0) = 0;
    T2CON = 0x0;
    TMR2 = 0x0;
    PR2 = 10000;
    T2CON = 0x8070;
}

// Usally avalible from stdlib but isn't for some reason
int abs(int num) {
    return (num < 0) ? -num : num;
}

void paddle_handler(Paddle *paddle, int delta_h) {

    unsigned int new_paddle = ~(paddle->position);

    new_paddle = (delta_h < 0) ? (new_paddle >> abs(delta_h)) : (new_paddle << abs(delta_h));

    paddle->position = ~new_paddle;

    for (int i = 0; i < 32; i++) {

        if (paddle->is_left) {
            frame[0][i] = (paddle->position >> i) & 0x1;
        } else {
            frame[127][i] = (paddle->position >> i) & 0x1;
        }
    }
}

void reset_game() {
    ball.x = 64;
    ball.y = 16;
    ball.x_dir = !ball.x_dir;
    right_paddle.position = ~paddle_const;
    left_paddle.position = ~paddle_const;

    for (int i = 0; i < 32; i++) {
        frame[0][i] = (left_paddle.position >> i) & 0x1;
        frame[127][i] = (left_paddle.position >> i) & 0x1;
    }
    quicksleep(1000);
    update_screen();
}

int getbtns(){
    return (PORTD >> 4) & 0x1E | ((PORTF >> 1) & 0x1);
}

void ball_handler() {
    bool right_scored = false;
	bool left_scored = false;

    frame[ball.x][ball.y] = 1;

    // Check edge collison
    if (ball.y == 0 || ball.y == 31) {
        ball.y_dir = !ball.y_dir;
    }

    // Check paddle collison
    if (((~right_paddle.position >> ball.y) & 0b1) && (ball.x == 126)) {
        ball.x_dir = !ball.x_dir;
    }

    if (((~left_paddle.position >> ball.y) & 0b1) && (ball.x == 1)) {
        ball.x_dir = !ball.x_dir;
    }
    // Move ball
    ball.x += ball.x_dir ? 1 : -1;
    ball.y += ball.y_dir ? 1 : -1;

    if (ball.x == 0) {
        right_score++;
        right_scored = true;
    }

    if (ball.x == 127) {
        left_score++;
        left_scored = true;
    }
    
    
    frame[ball.x][ball.y] = 0;

	if (right_scored || left_scored) {
		if (left_scored) {
			display_string(1, "left wins");
			display_string(2, "press any button");
			display_string(3, "to continue");
		} else if (right_scored) {
			display_string(1, "right wins");
			display_string(2, "press any button");
			display_string(3, "to continue");
		}
		display_update();
		quicksleep(500000);	// Wait a litte until button check

		while (!getbtns()) {}
		reset_game();
	}
}

void user_isr() {
    if (IFS(0) & 0x100) {
        IFS(0) &= ~0x100;
        ball_handler();
    }
    
}


void button_handler() {
    int buttons = getbtns();

    if (buttons & 0b0001) {
        if (right_paddle.position & 0x1) {
            paddle_handler(&right_paddle, -1);
        }
    }

    if (buttons & 0b0010) {
        if (right_paddle.position & 0x80000000) {
            paddle_handler(&right_paddle, 1);
        }
    }

    if (buttons & 0b0100) {
        if (left_paddle.position & 0x1) {
            paddle_handler(&left_paddle, -1);
        }
    }
    if (buttons & 0b1000) {
        if (left_paddle.position & 0x80000000) {
            paddle_handler(&left_paddle, 1);
        }
    }
    update_screen();
}



int main(void) {
    init();
    display_init();
    display_update();
    system_init();

    paddle_handler(&left_paddle, 1);
    paddle_handler(&right_paddle, 1);
    update_screen();
	reset_game();
	display_update();
    
    while (true)
    {
        button_handler();	// Read the buttons and move the paddles
    }
    
}