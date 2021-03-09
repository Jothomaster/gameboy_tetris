#include<gb/gb.h>
#include"tiles/tetris_tiles.c"
#include"maps/tetrisbg.c"
#include<rand.h>
#define setstructure(a,b,c,d) a->structure[b].x=c; a->structure[b].y=d;
#include<stdio.h>

/* Joypad bits
J_START      0x80U
J_SELECT     0x40U
J_B          0x20U
J_A          0x10U
J_DOWN       0x08U
J_UP         0x04U
J_LEFT       0x02U
J_RIGHT      0x01U
*/

/* GB screen
WIDTH - 20 blocks
HEIGHT - 18 blocks
*/ 

UBYTE game=0;

typedef struct
{
	INT8 x; INT8 y;
}Pair;

typedef struct
{
	UBYTE tileid;
	UINT8 x;
	UINT8 y;
	Pair structure[3];
	UINT8 tileids[4];
}Block;

void moveblock(Block *current, UINT8 x, UINT8 y)
{
	move_sprite(current->tileids[0],x,y);
	move_sprite(current->tileids[1],x+current->structure[0].x*8,y+current->structure[0].y*8);
	move_sprite(current->tileids[2],x+current->structure[1].x*8,y+current->structure[1].y*8);
	move_sprite(current->tileids[3],x+current->structure[2].x*8,y+current->structure[2].y*8);
	current->x=x;
	current->y=y;
	set_sprite_tile(current->tileids[0],current->tileid);
	set_sprite_tile(current->tileids[1],current->tileid);
	set_sprite_tile(current->tileids[2],current->tileid);
	set_sprite_tile(current->tileids[3],current->tileid);
}

UBYTE bgcollision(Block *current, INT8 x, INT8 y)
{
	INT16 indexTileX, indexTileY, indexTileMap;
	UBYTE result=0;
	indexTileX = (current->x+x-8)/8;
	indexTileY = (current->y+y-16)/8;
	indexTileMap = indexTileX + 20*indexTileY;
	if(indexTileMap>0)
	result+=Background[indexTileMap];
	for(UINT8 i = 0; i<3; i++)
	{
	indexTileX = (current->x+x-8)/8+current->structure[i].x;
	indexTileY = (current->y+y-16)/8+current->structure[i].y;
	indexTileMap = indexTileX + 20*indexTileY;
	result+=Background[indexTileMap];
	}
	//printf("%d",indexTileMap);
	return result;
}

UBYTE scrollblock(Block *current, INT8 x, INT8 y)
{
	if(!bgcollision(current,x,y))
	{
	current->x+=x;
	current->y+=y;
	scroll_sprite(current->tileids[0],x,y);
	scroll_sprite(current->tileids[1],x,y);
	scroll_sprite(current->tileids[2],x,y);
	scroll_sprite(current->tileids[3],x,y);
	return 1;
	}
	return 0;
}

void gameover()
{
	HIDE_SPRITES;
	for(UINT8 i = 0; i<17; i++)
	{
		for(UINT8 j = 1; j<11; j++)
		Background[20*i+j]=0x00;
	}
	printf("Game over\n ");
	game=0;
}

void transformtobg(Block *current)
{
	INT16 indexTileX, indexTileY, indexTileMap;
	indexTileX = (current->x-8)/8;
	indexTileY = (current->y-16)/8;
	indexTileMap = indexTileX + 20*indexTileY;
	if(indexTileMap<0)
	{
		gameover();
		return;
	}
	Background[indexTileMap] = current->tileid;
	for(UINT8 i = 0; i<3; i++)
	{
	indexTileX = (current->x-8)/8+current->structure[i].x;
	indexTileY = (current->y-16)/8+current->structure[i].y;
	indexTileMap = indexTileX + 20*indexTileY;
	if(indexTileMap<0)
	{
		gameover();
		return;
	}
	Background[indexTileMap] = current->tileid;
	}
	//printf("%d",Background[indexTileMap]);
}

void sleep(UINT8 duration)
{
	for(UINT8 i=0; i<duration; ++i)
		wait_vbl_done();
}

void checklines()
{
	for(UINT8 i = 0; i<17; i++)
	{
		UBYTE blockcounter = 0;
		for(UINT8 j = 1; j<11; j++)
		{
			if(Background[20*i+j]) ++blockcounter; 
		}
		if(blockcounter==10)
			for(UINT8 j=1; j<11; j++) Background[20*i+j]=0;
	}
	for(UINT8 k=0; k<4; ++k)
	{
	UINT8 i = 17;
		while((i--)-1)
		{
			UBYTE blockcounter = 0;
			for(UINT8 j = 1; j<11; j++)
			{
				if(Background[20*i+j]) ++blockcounter; 
			}
			if(!blockcounter)
				for(UINT8 j=1; j<11; j++)
				{ 
					Background[20*i+j]=Background[20*(i-1)+j];
					Background[20*(i-1)+j]=0;
				}
		}
	}
}

void spin_left(Block *current);

void spin_right(Block *current)
{
	for(UINT8 i=0; i<3; i++)
	{
		INT8 temp = current->structure[i].x;
		current->structure[i].x=current->structure[i].y*-1;
		current->structure[i].y=temp;
	}
	if(bgcollision(current,0,0)) spin_left(current);
	moveblock(current,current->x,current->y);
}

void spin_left(Block *current)
{
	for(UINT8 i=0; i<3; i++)
	{
		INT8 temp = current->structure[i].x;
		current->structure[i].x=current->structure[i].y;
		current->structure[i].y=temp*-1;
	}
	if(bgcollision(current,0,0)) spin_right(current);
	moveblock(current,current->x,current->y);
}

void generate(Block *next)
{
	switch(randw()%7)
		{
			case 0:
				setstructure(next,0,1,0);
				setstructure(next,1,-1,0);
				setstructure(next,2,0,-1);   
				break;
			case 1:
				setstructure(next,0,1,0);
				setstructure(next,1,0,-1);
				setstructure(next,2,1,-1);
				break;
			case 2:
				setstructure(next,0,1,0);
				setstructure(next,1,-1,0);
				setstructure(next,2,1,-1);
				break;			
			case 3:
				setstructure(next,0,1,0);
				setstructure(next,1,-1,0);
				setstructure(next,2,-1,-1);
				break;			
			case 4:
				setstructure(next,0,-1,0);
				setstructure(next,1,0,-1);
				setstructure(next,2,1,-1);
				break;			
			case 5:
				setstructure(next,0,1,0);
				setstructure(next,1,0,-1);
				setstructure(next,2,-1,-1);
				break;			
			case 6:
				setstructure(next,0,1,0);
				setstructure(next,1,2,0);
				setstructure(next,2,-1,0);
				break;			
		}
	next->tileid=(randw()%3)+8;
}

void swap(Block *b1, Block *b2)
{
	Block temp;
	temp = *b1;
	*b1 = *b2;
	*b2 = temp;
	for(UINT8 i=0; i<4; i++)
	{
		UINT8 temp2 = b1->tileids[i];
		b1->tileids[i] = b2->tileids[i];
		b2->tileids[i] = temp2;
	}
	INT8 temp2 = b1->x;
		b1->x = b2->x;
		b2->x = temp2;
		temp2 = b1->y;
		b1->y = b2->y;
		b2->y = temp2;
}

void main()
{
	while(1)
	{
	printf("TETRIS\nPress Start\n");
	waitpad(0b10000000);
	UWORD seed = DIV_REG;
	initrand(seed);
	set_sprite_data(0,11,Tileset);
	SHOW_BKG;
	DISPLAY_ON;
	set_bkg_data(0,11,Tileset);
	set_bkg_tiles(0,0,BackgroundWidth,BackgroundHeight, Background);
	SHOW_SPRITES;
	game=1;
	Block current = {8,0,0,{{0,0},{0,0},{0,0}},{0,1,2,3}};
	Block next = {8,0,0,{{0,0},{0,0},{0,0}},{0,1,2,3}};
	Block stored = {8,0,0,{{0,0},{0,0},{0,0}},{8,9,10,11}};
	generate(&current);
	generate(&next);
	while(game)
	{
		current = next;
		generate(&next);
			current.tileids[0]=0;
			current.tileids[1]=1;
			current.tileids[2]=2;
			current.tileids[3]=3;
			set_sprite_tile(0,current.tileid);
			set_sprite_tile(1,current.tileid);
			set_sprite_tile(2,current.tileid);
			set_sprite_tile(3,current.tileid);
			next.tileids[0]=4;
			next.tileids[1]=5;
			next.tileids[2]=6;
			next.tileids[3]=7;
			set_sprite_tile(4,next.tileid);
			set_sprite_tile(5,next.tileid);
			set_sprite_tile(6,next.tileid);
			set_sprite_tile(7,next.tileid);
			set_sprite_tile(8,stored.tileid);
			set_sprite_tile(9,stored.tileid);
			set_sprite_tile(10,stored.tileid);
			set_sprite_tile(11,stored.tileid);
			moveblock(&current,40,16);
			moveblock(&next,128,48);
			moveblock(&stored,128,80);
		do{
		INT8 counter=40;
		while(counter>0)
		{
			switch(joypad())
			{
				case 0b00000001:
					scrollblock(&current,8,0);
					sleep(3);
					counter-=3;
					break;
				case 0b00000010:
					scrollblock(&current,-8,0);
					sleep(3);
					counter-=3;
					break;
				case 0b00100000:
					spin_right(&current);
					sleep(6);
					counter-=6;
					break;
				case 0b00010000:
					spin_left(&current);
					sleep(6);
					counter-=6;
					break;
				case 0b00001000:
					counter=0;
					break;
				case 0b01000000:
					swap(&current,&stored);
					moveblock(&current,current.x,current.y);
					moveblock(&stored,stored.x,stored.y);
					sleep(6);
					counter-=6;
					break;
			}
			sleep(1);
			counter--;
		}
		}while(scrollblock(&current,0,8));
		transformtobg(&current);
		checklines();
		set_bkg_tiles(0,0,BackgroundWidth,BackgroundHeight, Background);
	}
	}
}
