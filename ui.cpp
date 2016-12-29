#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

#include <algorithm>

#include "core.h"
#include "character.h"
#include "debug.h"
#include "dungeon.h"
#include "move.h"
#include "npc.h"
#include "objectfactory.h"
#include "pc.h"
#include "ui.h"

static void getPositionStr(char buf[50], Character *c)
{
	int i;
	for(i=0; i<50; i++) buf[i]=' ';

	int cx, cy, pcx, pcy;
	c->getLocation(&cx, &cy);
	pc->getLocation(&pcx, &pcy);

	int dx = cx - pcx;
	int dy = cy - pcy;

	if (dx>0)
		sprintf(buf + 2, "%02d E   ",  dx);
	else
		sprintf(buf + 2, "%02d W   ", -dx);
	if (dx==0)
		sprintf(buf + 2, "00   ");
	if (dy>0)
		sprintf(buf + 8, "%02d S   ",  dy);
	else
		sprintf(buf + 8, "%02d N   ", -dy);
	if (dy==0)
		sprintf(buf + 8, "00   ");
}

static int compareNPC(const void *a, const void *b)
{
	int ax, ay, bx, by, pcx, pcy;

	(*(Character **)a)->getLocation(&ax, &ay);
	(*(Character **)b)->getLocation(&bx, &by);
	pc->getLocation(&pcx, &pcy);

	return (ABS((ax-pcx))+ABS((ay-pcy)))
		- (ABS((bx-pcx))+ABS((by-pcy)));
}

int UI::help()
{
	FILE* fp = fopen("README", "r");

	if (!fp) {
		fprintf(stderr, "README not found\n");
		return -1;
	}

	// ncurses stuff
	initscr();
	start_color();
	raw();
	noecho();
	curs_set(0);
	keypad(stdscr, TRUE);

	int r = 0, c = 0;
	char buffer[255][80];
	while(fgets(buffer[r++], 80, fp));
	fclose(fp);

	int rmax = r;

	int from = 0;

	while(1) {
		clear();

		char tmp[80];
		char *p;

		for (r=0; r<22; r++) {
			p = tmp;

			for (c=0; c<80; c++) {
				if ('\t'==buffer[from+r][c]) {
					sprintf(p, "    ");
					p+=4;
				}
				else *p++ = buffer[from+r][c];
			}
			mvprintw(r, 0, "%s", tmp);
		}

		int color;
		init_pair(color = 9, COLOR_GREEN, COLOR_BLACK);

		attron(COLOR_PAIR(color));

		mvprintw(24-1, 0, 
				"Use arrow keys to scroll up and down. Press Q to quit.");

		attroff(COLOR_PAIR(color));

		refresh();

		int ch = getch();
		if (ch=='Q' || ch=='q') break;
		if (ch==KEY_DOWN) 
			if (from+22<rmax) 
				from++;
		if (ch==KEY_UP)   
			if (from>0)
				from--;
	}

	endwin();

	return 0;
}

int UI::initColors()
{
	init_pair(COLOR_PC, COLOR_RED, COLOR_YELLOW);
	// HP
	init_pair(COLOR_HP, COLOR_WHITE, COLOR_RED);
	// MP
	init_pair(COLOR_MP, COLOR_WHITE, COLOR_BLUE);
	// Frozen
	init_pair(COLOR_FROZEN, COLOR_BLACK, COLOR_CYAN);
	// Poizen
	init_pair(COLOR_POISON, COLOR_BLACK, COLOR_GREEN);

	for (int i = 0; i < 8; i++) {
		init_pair(i, i, COLOR_BLACK);
	}

	return 0;
}

static void mListFrom(int start)
{
	int i;
	char buf[50];

	int nummon = dungeon->nummon();

	mvprintw(0, 1, "Listing Monster %02d to %02d out of %02d "
			"(press ESC or Q to quit)", 
			start+1, MIN((start+21), nummon), nummon);

	std::sort(dungeon->npcv.begin(), dungeon->npcv.end(), compareNPC);

	for (i=start; i<MIN((start+21), nummon); i++) {
		Character *c = dungeon->npcv[i];
		getPositionStr(buf, c);

		mvprintw(i-start+1, 2, "  %s", buf);

		mvaddch(i-start+1, 18, ACS_VLINE);

		int color = c->getColor();

		attron(COLOR_PAIR(color));
		mvprintw(i-start+1, 2, "%c", c->getSymb());
		attroff(COLOR_PAIR(color));

	}
	int r = MIN((start+21), nummon)-start+1;
	for (int c=1; c<18; c++) {
		mvaddch(r, c, ACS_HLINE);
	}
	mvaddch(r, 18, ACS_LRCORNER);
}

static void oListFrom(int start)
{
	int i;
	char buf[50];

	int numobj = dungeon->numobj();

	mvprintw(0, 1, "Listing Object %d to %d out of %d "
			"(press ESC or Q to quit)", 
			start+1, MIN((start+21), numobj), numobj);

	for (i=start; i<MIN((start+21), numobj); i++) {
		Item *item = dungeon->itemv[i];

		sprintf(buf, "     %-12s", 
				ObjectFactory::TYPE[item->type].c_str());

		mvprintw(i-start+1, 1, "   %s", buf);

		int color = item->color;		
		attron(COLOR_PAIR(color));
		mvprintw(i-start+1, 2, "%c", item->getSymb());
		attroff(COLOR_PAIR(color));

		mvaddch(i-start+1, 18, ACS_VLINE);
	}
	int r = MIN((start+21), numobj);
	for (int c=1; c<18; c++)
		mvaddch(r, c, ACS_HLINE);
	mvaddch(r, 18, ACS_LRCORNER);
}

int UI::printEquipmentSlots()
{
	mvprintw(23, 43, "Equipment");
	int col=54;
	char gear[12] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};

	/* This allows for different equipment slots being dynamically allocated based upon equip commands */
	for(int i=0;i<pc->equipment.size();i++)
	{
		gear[i] = pc->equipment[i]->getSymb();
	}

	for(int i=0;i<12;i++)
	{
		mvprintw(22, col+i*2, "%c", (char) 'A'+i);
		mvprintw(23,col+i*2-1,"'");
		mvprintw(23,col+i*2,"%c",gear[i]);
	}
	return 0;
}

int UI::printHP()
{
	int colored = int(16 * pc->hp/(double)pc->hpmax()+0.5);

	mvprintw(23, 1, "HP ""                ");

	char buf[50];
	sprintf(buf, "%d/%d""                ", pc->hp, pc->hpmax());
	buf[16] = 0;

	char coloredsubstr[colored + 1];
	memset(coloredsubstr, 0, sizeof(coloredsubstr));
	strncpy(coloredsubstr, buf, colored);

	attron(COLOR_PAIR(COLOR_HP));
	mvprintw(23, 4, "%s", coloredsubstr);
	attroff(COLOR_PAIR(COLOR_HP));

	if (colored < 16)
		mvprintw(23, 4 + colored, "%s", buf + colored);

	return 0;
}

int UI::printMP()
{
	int colored = int(16 * pc->mp/(double)pc->mpmax()+0.5);

	mvprintw(23, 21, "MP ""                ");

	char buf[50];
	sprintf(buf, "%d/%d""                ", pc->mp, pc->mpmax());
	buf[16] = 0;

	char coloredsubstr[colored + 1];
	memset(coloredsubstr, 0, sizeof(coloredsubstr));
	strncpy(coloredsubstr, buf, colored);

	attron(COLOR_PAIR(COLOR_MP));
	mvprintw(23, 24, "%s", coloredsubstr);
	attroff(COLOR_PAIR(COLOR_MP));

	if (colored < 16)
		mvprintw(23, 24 + colored, "%s", buf + colored);

	return 0;
}

int UI::printMonsterHP()
{
	printMonsterHP(pc->attacking);

	return 0;
}

int UI::printMonsterHP(NPC *att)
{
	if (!att) {
		clearRow(0);
		return -1;
	}

	int colored = int(16 * att->hp/(double)(att->hpmax())+0.5);

	mvprintw(0, 1, "HP ""                ");

	char buf[50];
	sprintf(buf, "%d/%d""                ", att->hp, att->hpmax());
	buf[16] = 0;

	char coloredsubstr[colored + 1];
	memset(coloredsubstr, 0, sizeof(coloredsubstr));
	strncpy(coloredsubstr, buf, colored);

	attron(COLOR_PAIR(COLOR_HP));
	mvprintw(0, 4, "%s", coloredsubstr);
	attroff(COLOR_PAIR(COLOR_HP));

	if (colored < 16)
		mvprintw(0, 4 + colored, "%s", buf + colored);

	// monster info	
	mvprintw(0, 20, " <- ");

	attron(COLOR_PAIR(att->getColor()));
	mvprintw(0, 24, "%c", att->getSymb());
	attroff(COLOR_PAIR(att->getColor()));

	mvprintw(0, 26, "targeted");

	return 0;
}

int UI::clearRow(int row)
{
	int i;
	for(i=0; i<80; i++)
		mvaddch(row, i, ' ');

	return 0;
}

int UI::reprint()
{
	dungeon->printDungeon();

	printHP();
	printMP();

	printEquipmentSlots();

	return 0;
}

int UI::mList()
{
	int from = 0;

	mListFrom(from);
	refresh();

	while (1) {
		int ch = getch();

		int quit = 0;

		switch (ch) {
			case 'Q':
			case 'q':
			case 27: // ESC
				quit = 1;
				break;
			case KEY_DOWN:
				if (from>21-nummon && from<nummon-21) from++;
				mListFrom(from);
				refresh();
				break;
			case KEY_UP:
				from = MAX((from-1), 0);
				mListFrom(from);
				refresh();
				break;
		}
		if (quit)
			break;
	}

	for (int i=0; i<80; i++) {
		mvprintw(0,  i, " ");
		mvprintw(22, i, " ");
	}

	return 0;
}

int UI::oList()
{
	int from = 0;

	oListFrom(from);
	refresh();

	while (1) {
		int ch = getch();

		int quit = 0;

		switch (ch) {
			case 'Q':
			case 'q':
			case 27: // ESC
				quit = 1;
				break;
			case KEY_DOWN:
				if (from>21-nummon && from<nummon-21) from++;
				oListFrom(from);
				refresh();
				break;
			case KEY_UP:
				from = MAX((from-1), 0);
				oListFrom(from);
				refresh();
				break;
		}
		if (quit)
			break;
	}

	clearRow(0);
	clearRow(22);

	return 0;
}

/* Implemente wizard mode menu */
char str[10];
int ex;

int UI::wMode()
{
int color = COLOR_CYAN;
int quit = 0;
char *env = getenv("HOME");
char filepath[1<<8];
attron(COLOR_PAIR(color));
mvprintw(10,29,"\'Wizard Mode\' Menu");
mvprintw(11,16,"Push ENTER key once toggles are done to take effect");
mvprintw(12,22,"Select \'f\' to toggle fog");
mvprintw(13,22,"Select \'c\' to toggle invincibility");
mvprintw(14,22,"Select \'i\' to toggle line of sight");
mvprintw(15,22,"select \'n\' to adjust number of monsters");
mvprintw(16,22,"select \'s\' to save current dungeon");
mvprintw(17,22,"select \'l\' to load the saved dungeon");
while(1)
{
int pd = getch();
switch(pd){
case '\n':
quit=1;
break;

case 'f':
if(nofog==1) { nofog=0; mvprintw(18,22,"fog on"); }
else { nofog = 1; mvprintw(18,22,"fog off"); }
refresh();
break;

case 'c':
if(cheat==1) { cheat=0; mvprintw(19,22,"invincibility off"); }
else { cheat=1; mvprintw(19,22,"invincibility on"); }
refresh();
break;

case 'i':
if(sight==1) { sight=0; mvprintw(20,22,"line of sight off"); }
else { sight=1; mvprintw(20,22, "line of sight on"); }
refresh();
break;

case 'n':
/* TODO: FIXME */
mvprintw(21,22,"enter number of monsters");
getstr(str);
ex=atoi(str);
mvprintw(22,22,"%d",ex);
nummon=ex;
refresh();
break;

case 's':
sprintf(filepath,"%s/.rlg327/dungeon",env);
savep=filepath;
dungeon->save(savep); mvprintw(20,22,"saved to %s",filepath);
refresh();
break;

case 'l':
sprintf(filepath,"%s/.rlg327/dungeon",env);
loadp=filepath;
dungeon->load(loadp); mvprintw(20,22,"loaded from %s",filepath);
refresh();
break;
}

if(quit) { break; }
}
attroff(COLOR_PAIR(color));
return 0;
}

/* Implement inventory list */
/* TODO: currently not waiting for user input for inventory after wizard mode addition */
int UI::iList()
{
	if(pc->invent->inv.size()==0)
	{
		int color = COLOR_RED;
		attron(COLOR_PAIR(color));
		mvprintw(10,29,"!!!EMPTY INVENTORY!!!");
		mvprintw(11,18,"any items picked up will be displayed here");
		mvprintw(12,27,"push any key to continue");
		attroff(COLOR_PAIR(color));
		int ch = getch();
		switch(ch)
		{
			default:
				break;
		}
	}
	else
	{
		int lineIndex=21;
		int color=COLOR_CYAN;
		attron(COLOR_PAIR(color));
		for(int i=0;i<pc->invent->inv.size();i++)
		{
			string iName = pc->invent->inv.at(i)->name;
			mvprintw(21-i, 26,"Slot %d: %s",i,iName.c_str());
			lineIndex--;
		}
		mvprintw(lineIndex,30,"INVENTORY");

		int ch = getch();
		int slot;
		int quit = 0;
		/*Wait for equip, drop, expunge, or inspect command*/
		switch (ch)
		{
			case '\n':
				quit=1;
			break;
			/*drop an item from inventory*/
			case 'd':
				mvprintw(lineIndex, 26, "choose to drop item");
				slot = getch()-'0'; //iIndex();
				Debug::log("%d",slot);

				if(slot<pc->invent->inv.size())
				{	
					if(slot==0) { pc->drop(0,pcx, pcy); }/* 0 */
					else if(slot==1){ pc->drop(1,pcx,pcy); }
					else if(slot==2){ pc->drop(2,pcx,pcy); }
					else if(slot==3){ pc->drop(3,pcx,pcy); }
					else if(slot==4){ pc->drop(4,pcx,pcy); }
					else if(slot==5){ pc->drop(5,pcx,pcy); }
					else if(slot==6){ pc->drop(6,pcx,pcy); }
					else if(slot==7){ pc->drop(7,pcx,pcy); }
					else if(slot==8){ pc->drop(8,pcx,pcy); }
					else if(slot==9){ pc->drop(9,pcx,pcy); }
				}
				else { mvprintw(lineIndex,26, "select a valid inventory index"); }
				break;
				/* expunge an item from the game */
			case 'x':
				mvprintw(lineIndex,26,"choose an item to expunge");
				slot = getch()-'0';
				if(slot<pc->invent->inv.size())
				{
					if(slot==0) { pc->expunge(0); }
					else if(slot==1) { pc->expunge(1); }
					else if(slot==2) { pc->expunge(2); }
					else if(slot==3) { pc->expunge(3); }
					else if(slot==4) { pc->expunge(4); }
					else if(slot==5) { pc->expunge(5); }
					else if(slot==6) { pc->expunge(6); }
					else if(slot==7) { pc->expunge(7); }
					else if(slot==8) { pc->expunge(8); }
					else if(slot==9) { pc->expunge(9); }
				}
				else { mvprintw(lineIndex,26, "select a valid inventory index"); }
				break;

				/* Inxpect an item from your inventory */
			case 'I':
				mvprintw(lineIndex,26,"select an item to view its description");
				slot = getch() - '0';
				if(slot<pc->invent->inv.size())
				{
					string iDesc = pc->invent->inv.at(slot)->desc;
					Debug::log("%s",iDesc.c_str());
					mvprintw(11,26,"%s",iDesc.c_str());
					color=COLOR_RED;
					attron(COLOR_PAIR(color));
					mvprintw(10,26,"push any key to continue");
					attroff(COLOR_PAIR(color));
					getch();
				}
				else { clearRow(lineIndex);mvprintw(lineIndex,26,"select a valid inventory index"); getch();}
				break;
			case 'w':
				mvprintw(lineIndex,26,"select an item to equip to player");
				slot = getch() - '0';
				if(slot<pc->invent->inv.size())
				{
					Debug::log("speed before: %d", pc->getSpeed());
					Debug::log("damage before: %d", pc->getTotalDam());
					Debug::log("defense bfore: %d", pc->getTotalDef());

					pc->equip(slot);

					Debug::log("speed after: %d", pc->getSpeed());
					Debug::log("damage after: %d", pc->getTotalDam());
					Debug::log("defends after: %d", pc->getTotalDef());
					clearRow(lineIndex);
					mvprintw(lineIndex,26, "push any key to continue");
					getch();
				}
				else {clearRow(lineIndex); mvprintw(lineIndex, 26, "select a valid inventody index"); getch(); }
				break;
		}
		attroff(COLOR_PAIR(color));
	}
	return 0;
}

int UI::sList()
{
	int color = COLOR_CYAN;	
	attron(COLOR_PAIR(color));

	mvprintw(17, 21, "B: Blast nearby area   cost: 30  ");
	mvprintw(18, 21, "F: Frozen ball         cost: 20  ");
	mvprintw(19, 21, "H: Heal PC             cost: 10  ");
	mvprintw(20, 21, "P: Poison ball         cost: 20  ");
	mvprintw(21, 21, "T: Teleport            cost: 30  ");

	attroff(COLOR_PAIR(color));

	int ch = getch(), cost, target, npcx, npcy;

	NPC *att;

	switch (ch) {
		case 'Q':
		case 'q':
		case 27: // ESC
			break;
		case 'B':
		case 'b':
			if (pc->mp < (cost = 30))
				break;
			for (int r=pcy-1; r<=pcy+1; r++) {
				for (int c=pcx-1; c<=pcx+1; c++) {
					if (r>=0 && r<21 &&
							c>=0 && c<80 && 
							dungeon->hmap[r][c]!=IMMUTABLE) {
						if (dungeon->hmap[r][c]>0) {
							dungeon->hmap[r][c]=0;
							dungeon->tmap[r][c]=CORR;
						}
						if (dungeon->cmap[r][c]&&!
								dungeon->cmap[r][c]->isPC())
							dungeon->cmap[r][c]->hp/=10;
					}
				}
			}
			pc->mp -= cost;
			pc->setLocation(pcx, pcy); // force update seen dungeon
			dungeon->printDungeon();
			refresh();
			break;
		case 'F':
		case 'f':
			if (pc->mp < (cost = 20))
				break;
			target = selectTarget();
			if (target < 0)
				break;
			att = dungeon->vnpcv[target];

			att->getLocation(&npcx, &npcy);

			// frozen ball spell
			for (int r=npcy-1; r<=npcy+1; r++) {
				for (int c=npcx-1; c<=npcx+1; c++) {
					if (r>=0 && r<21 &&
							c>=0 && c<80 && 
							dungeon->hmap[r][c]!=IMMUTABLE) {
						if (dungeon->cmap[r][c]&&!
								dungeon->cmap[r][c]->isPC()) {
							dungeon->cmap[r][c]->frozen = 25;
						}
					}
				}
			}
			pc->mp -= cost;
			pc->setLocation(pcx, pcy); // force update seen dungeon
			dungeon->printDungeon();	
			UI::printMP();
			refresh();
		case 'H':
		case 'h':
			if (pc->mp < (cost = 10))
				break;
			pc->hp += pc->hpmax() * 0.25;
			if (pc->hp > pc->hpmax())
				pc->hp = pc->hpmax();
			pc->mp -= cost;
			UI::printHP();
			UI::printMP();
			break;
		case 'P':
		case 'p':
			if (pc->mp < (cost = 20))
				break;
			target = selectTarget();
			if (target < 0)
				break;
			att = dungeon->vnpcv[target];

			att->getLocation(&npcx, &npcy);

			// poison ball spell
			for (int r=npcy-1; r<=npcy+1; r++) {
				for (int c=npcx-1; c<=npcx+1; c++) {
					if (r>=0 && r<21 &&
							c>=0 && c<80 && 
							dungeon->hmap[r][c]!=IMMUTABLE) {
						if (dungeon->cmap[r][c]&&!
								dungeon->cmap[r][c]->isPC()) {
							dungeon->cmap[r][c]->hp*=0.9;
							dungeon->cmap[r][c]->poison = 25;
						}
					}
				}
			}
			pc->mp -= cost;
			pc->setLocation(pcx, pcy); // force update seen dungeon
			dungeon->printDungeon();	
			UI::printMP();
			refresh();
			break;
		case 'T':
		case 't':
			if (pc->mp < (cost = 30))
				break;
			while (1) {
				int r = rand() % 21;
				int c = rand() % 80;
				if (!dungeon->hmap[r][c] && !dungeon->cmap[r][c]) {
					Move::move(pc, c, r);
					break;
				}
			}
			pc->mp -= cost;
			UI::printMP();
			break;
	}

	return 0;
}

int UI::selectNPC(NPC *npc)
{
	reprint();

	// mvprintw(1, 20, "  select target ");

	int x, y;
	npc->getLocation(&x, &y);

	mvprintw(y+1, x-1, "[");
	mvprintw(y+1, x+1, "]");

	printMonsterHP(npc);

	refresh();

	return 0;
}

int UI::selectTarget()
{	
	if (dungeon->vnpcv.empty())
		return -1;

	int index = 0;

	selectNPC(dungeon->vnpcv[index]);

	while (1) {
		bool quit = false;

		int ch = getch();

		switch (ch) {
			case 'Q':
			case 'q':
				quit = true;
				break;
			case ' ':
				pc->attacking = dungeon->vnpcv[index];
				return index;
			case KEY_DOWN:
			case KEY_RIGHT:
			case 'R':
			case 'r':
			case 'F':
			case 'f':
			case 'P':
			case 'p':
				index = (index + 1) 
					% (int)dungeon->vnpcv.size();
				selectNPC(dungeon->vnpcv[index]);
				break;
			case KEY_UP:
			case KEY_LEFT:
				index = (index - 1 + (int)dungeon->vnpcv.size()) 
					% (int)dungeon->vnpcv.size();
				selectNPC(dungeon->vnpcv[index]);
				break;
			default:
				break;
		}

		if (quit)
			break;
	}
	reprint();

	return -1;
}

