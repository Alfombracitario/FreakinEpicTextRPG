#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <ogcsys.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <math.h> 

//#include <time.h>

static u32 *xfb;
static GXRModeObj *rmode;

//variables
int level = 1;
int xpneed = 10;
int xp = 0;
int hp = 10;
int maxhp = 10;
int playerdm = 3;
int money = 0;
float defense = 0.1;

int mode = 0; //tienda, explorar, minería, etc
int selected = 0; //Opción seleccionada 
int vselect = 0; //lo mismo vertical
int maxvsel = 1; //valor máximo que se puede seleccionar verticalmente

int itemselect;

int enemylv = 1;
int enemyhp;
int enemydm;

int infight = 0;

int cansave = 1;

void Initialise() {
  
	VIDEO_Init();
	WPAD_Init();
 
	rmode = VIDEO_GetPreferredMode(NULL);

	xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	console_init(xfb,20,20,rmode->fbWidth,rmode->xfbHeight,rmode->fbWidth*VI_DISPLAY_PIX_SZ);
 
	VIDEO_Configure(rmode);
	VIDEO_SetNextFramebuffer(xfb);
	VIDEO_SetBlack(FALSE);
	VIDEO_Flush();
	VIDEO_WaitVSync();
	if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();
}
//mis funciones
void clear_screen() 
{
    // "Limpiar" la pantalla: Volver a inicializar la consola
    console_init(xfb,20,20,rmode->fbWidth,rmode->xfbHeight,rmode->fbWidth*VI_DISPLAY_PIX_SZ);
    VIDEO_Configure(rmode);
    VIDEO_SetNextFramebuffer(xfb);
    VIDEO_Flush();
    VIDEO_WaitVSync();
}
//LOS MODOS EN Sí FUNCIONANDO!!1
void do_explore()
{
	maxvsel = level;
	if(infight == 0)
	{
		 enemylv = vselect;
	} //actualizar el selector
	
	printf("\nSelect the level of the enemy");
	printf("\n(Up/Down) Level: %d", enemylv);
	printf("\n\n");
}

void addxp(int xpadded)//Completo
{
xp += xpadded;
while(xp >= xpneed)
{
	xp -= xpneed;
	xpneed = 7+round(level*3.7);
	level += 1;
	maxhp = 10+round(level*2.3);
	hp = maxhp;
	printf("LEVEL UUUUUUUUUUUUP!!!!!!!!!!!!!!");
}
}

void do_mine()
{
	printf("\nUhh, yep you can mine here");
	
}

void do_shop()
{
	int intdef = round(defense*15);
	maxvsel = 2;
	itemselect = vselect; //actualizar el selector
	
	printf("\nSelect item");
	printf("\n(Up/Down) Item: %d", itemselect);
	printf("\n\n");
	printf("0: health potion\nPrice: %d", (maxhp-hp)*2);
	printf("\n\n1: upgrade weapon\nPrice: %d", playerdm*10);
	printf("\n\n2: upgrade armor\nPrice: %d", intdef);
	
}

void draw_mode() //Terminado
{
	switch(mode)
		{
			case 0:
				printf("<Explore>\n");
				do_explore();
			break;
			
			case 1:
				printf("<Mine>\n");
				do_mine();
			break;
			
			case 2:
				printf("<Shop>\n");
				do_shop();
			break;
			default:
				printf("lmao how tf did you put this option, this is not valid, choose something else.\n");
				printf("ERROR!!!111!!1\n");
			break;
		}
}
void draw_info() //terminado
{
	clear_screen();
	printf("Freakin' epic text RPG!!!111!!\ngame made by Alfombracitarioj!!!11\n\n");
	
	printf("Level: %d", level);
	
	printf("\nXp: %d", xp);
	printf(" / %d", xpneed);
	
	printf("\nHP: %d", hp);
	printf(" / %d", maxhp);
	
	printf("\nMoney: %d", money);
	
	printf("\n\n");
	draw_mode();
}

void buy_item()
{
	switch(itemselect)
	{
		case 0:
			if(money >= (maxhp-hp)*2)
			{
				money -= (maxhp-hp)*2;
				hp = maxhp;
				draw_info();
				printf("\n\n\nWOAHHH NOW YOU HAVE FULL HP");
			}
			else
			{
				draw_info();
				printf("\n\n\nLMFAO, you're POOR!");
			}
		break;
		
		case 1:
			if(money >= playerdm*10)
			{
				money -= playerdm*10;
				playerdm++;
				draw_info();
				printf("\n\n\nYOUR WEAPON CAN DEAL MORE DAMAGE!!!!!!1111");
			}
			else
			{
				draw_info();
				printf("\n\n\nLMFAO, you're POOR!");
			}
		break;
		
		case 2:
			if(money >= round(defense*15))
			{
				money -= round(defense*15);
				defense += 0.25;
				draw_info();
				printf("\n\n\nYOUR DEFENSE IS BETTER WOOOOOOOOW!!!!!11!");
			}
			else
			{
				draw_info();
				printf("\n\n\nLMFAO, you're POOR!");
			}
		break;


		default:
			printf("\n\n\nuhhh, what are you trying to buy????");
		break;
	}

}

void do_fight()
{
	if(infight == 0)
	{
		enemylv = vselect;
		enemydm = round(enemylv*1.7); //randomize this pls
		enemyhp = floor(3+enemylv*5.5);
		printf("\nEnemy found.\n");
		infight = 1;
	}
	else
	{
		int prevhp = hp;
		if(round(enemydm-defense) > 1)
		{
			hp -= round(enemydm-defense);
		}
		else
		{
			hp--;
		}
		draw_info();
		printf("you lose %d",hp-prevhp);
		printf(" HP!\n");
		
		if(hp <= 0)
		{
			draw_info();
			printf("You lose :C\n");
			xp = 0;
			money = 0;
			hp = maxhp;
			infight = 0;
		}
		else
		{
			enemyhp -= playerdm;
			printf("The enemy now has less hp!!11!\n");
			if(enemyhp <= 0)
			{
				money += round(enemylv*14.4);
				draw_info();
				printf("YOU WON!!!11\n");
				addxp(floor(enemylv*2.4));
				infight = 0;
			}
		}
	}
}
void do_something() //Terminado lol
{
	switch(mode)
		{
			case 0:
				do_fight();
			break;
			
			case 1:
				money++; //P L A C E H O L D E R
				draw_info();
			break;
			
			case 2:
				buy_item();
			break;
			
			default:
				printf("ahhhhHHhhHhh WHAT IS HAPPENINGOFIDNGAOUSODIADLKGHAKJH\n");
			break;
		}
}
/*
key down = buttonsHeld
key pressed = buttonsDown
key up = buttonsRelease
*/
int main() {
 
	Initialise();
	
	
	while(1) {

		WPAD_ScanPads();
		u16 buttonsDown = WPAD_ButtonsDown(0);

		if( buttonsDown & WPAD_BUTTON_DOWN ) //mover selector a la derecha
		{
			vselect = 0;
			mode = (mode + 1) % 3;
			draw_info();
		}	
		
		if( buttonsDown & WPAD_BUTTON_UP ) //mover selector a la izquierda
		{
			vselect = 0;
			mode -= 1;
			if (mode < 0) 
			{
				mode = 2;
			}
			draw_info();
		}
		
		if( buttonsDown & WPAD_BUTTON_RIGHT ) //mover selector arriba
		{
			vselect += 1;
			if (vselect > maxvsel) 
			{
				vselect = 0;
			}
			draw_info();
		}	
		
		if( buttonsDown & WPAD_BUTTON_LEFT ) //mover selector abajo
		{
			vselect -= 1;
			if (vselect < 0) 
			{
				vselect = maxvsel;
			}
			draw_info();
		}
		
		
		
		if( buttonsDown & WPAD_BUTTON_HOME ) //Salir
		{
			exit(0);
		}
		
		if( buttonsDown & WPAD_BUTTON_2 ) 
		{
			do_something();
		}
		
	}
	//termina el loop while
	
 
	return 0;
}