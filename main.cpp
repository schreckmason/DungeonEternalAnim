#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <sys/stat.h> 
#include <sys/types.h> 

#include "core.h"
#include "character.h"
#include "debug.h"
#include "dijkstra.h"
#include "dungeon.h"
#include "monsterfactory.h"
#include "move.h"
#include "npc.h"
#include "parser.h"
#include "pc.h"
#include "room.h"
#include "turn.h"
#include "ui.h"
#include "inventory.h"

using namespace std;

static int regenerateDungeon();

void windowArt(WINDOW *popup)
{
	init_pair(5,COLOR_CYAN,0);
	init_pair(6,COLOR_RED,0);
	wattron(popup,COLOR_PAIR(5));
	mvwprintw(popup,1,1,"                              /   \\       \n");
	mvwprintw(popup,2,1," _                    )      ((   ))     (\n");
	mvwprintw(popup,3,1,"(@)                  /|\\      ))_((     /|\\\n");
	mvwprintw(popup,4,1,"|-|                 / | \\    (/\\|/\\)   / | \\                      (@)\n");
	mvwprintw(popup,5,1,"| |----------------/--|-voV-_--`|'/--Vov-|--\\---------------------|-|\n");
	mvwprintw(popup,6,1,"|-|                     '^`   (o o)  '^`                          | |\n");
	mvwprintw(popup,7,1,"| |                           `\\Y/'                               |-|\n");
	mvwprintw(popup,8,1,"|-|                                                               | |\n");
	mvwprintw(popup,9,1,"| |                                                               |-|\n");
	mvwprintw(popup,10,1,"|-|                                                               | |\n");
	mvwprintw(popup,11,1,"| |                                                               |-|\n");
	mvwprintw(popup,12,1,"|_|_______________________________________________________________| |\n");
	mvwprintw(popup,13,1,"(@)          l   /\\ /         ( (       \\ /\\   l                `\\|-|\n");
	mvwprintw(popup,14,1,"             l /   V           \\ \\       V   \\ l                  (@)\n");
	mvwprintw(popup,15,1,"             l/                _) )_          \\I\n");
	mvwprintw(popup,16,1,"                               `\\ /' \n");
	mvwprintw(popup,17,1,"                                 `\n");
	wattroff(popup,COLOR_PAIR(5));
	wattron(popup,COLOR_PAIR(6));
	mvwprintw(popup, 8, 26, "DUNGEON ETERNAL");
	mvwprintw(popup, 9, 8, "> Number of monsters (%d)",nummon);
	mvwprintw(popup, 9, 35, "> Number of objects (%d)", numobj);
	if(nofog==1) { mvwprintw(popup, 10, 14, "Fog of War Enabled [ ]"); }
	else { mvwprintw(popup, 10, 14, "Fog of War Enabled [x]"); }
	if(cheat==1) { mvwprintw(popup, 10, 40, "Invincible [x]"); }
	else { mvwprintw(popup, 10, 40, "Invincible [ ]"); }
	if(sight==1) { mvwprintw(popup, 11, 20,"Line of Sight Tracking [x]"); }
	else { mvwprintw(popup, 11, 20, "Line of sight Tracking [ ]"); }

	wattroff(popup,COLOR_PAIR(6));

}

int main(int argc, char** argv)
{
	char *env = getenv("HOME");

	char dirpath[1<<8], monpath[1<<8], objpath[1<<8];
	sprintf(dirpath, "%s/.rlg327", env);
	mkdir(dirpath, S_IRUSR | S_IWUSR | S_IXUSR);

	char filepath[1<<8]; 
	sprintf(filepath, "%s/.rlg327/dungeon", env);

	sprintf(monpath, "%s/.rlg327/monster_desc.txt", env);
	sprintf(objpath, "%s/.rlg327/object_desc.txt", env);

	// options
	if (argc > 1) {
		for (int i=1; i<argc; i++) {
			if (!strcmp("--debug", argv[i])) {
				debug = 1;
			} else if (!strcmp("--load", argv[i])) {
				if (i+1<argc && argv[i+1][0]!='-')
					loadp = argv[++i];
				else
					loadp = filepath;
			} else if (!strcmp("--save", argv[i])) {
				if (i+1<argc && argv[i+1][0]!='-')
					savep = argv[++i];
				else
					savep = filepath;
			} else if (!strcmp("--nummon", argv[i])) {
				if (i+1<argc && argv[i+1][0]!='-') {
					nummon = atoi(argv[++i]);
					int max = 1<<8;
					if (nummon > max) {
						cerr << "nummon should not exceed " << max << endl;
						return 1;
					}
				} else {
					fprintf(stderr, "usage: rlg327 --nummon <number>\n");
					return 1;
				}
			} else if (!strcmp("--cheat", argv[i])) {
				cheat = 1;
			} else if (!strcmp("--help", argv[i]) 
					|| !strcmp("-h", argv[i])) {
				// display help page (README)
				UI::help();
				return 1;
			} else if (!strcmp("--nofog", argv[i])) {
				nofog = 1;
			} else if (!strcmp("--parse", argv[i])) {
				if (i+1<argc) {
					Parser::parse(argv[++i]);
					return 0;
				} else {
					Parser::parse(monpath);
					Parser::parse(objpath);
					return 0;
				}
			} else if (!strcmp("--pc", argv[i])) {
				if (i+2<argc) {
					pcx = atoi(argv[++i]);
					pcy = atoi(argv[++i]);
				} else {
					fprintf(stderr, "usage: rlg327 --pc <x> <y>\n");
					return 1;
				}
			} else if (!strcmp("--seed", argv[i])) {
				if (i+1<argc) {
					seed = atoi(argv[++i]);
				} else {
					fprintf(stderr, "usage: rlg327 --seed <seed>\n");
					return 1;
				}
			} else if (!strcmp("--sight", argv[i])) {
				sight = 1;
			} else if (!strcmp("--unify", argv[i])) {
				if (i+1<argc && argv[i+1][0]!='-') {
					unify = argv[++i][0];

					if (!(('0' <= unify && unify <= '9')
								||('a' <= unify && unify <= 'f'))) {
						fprintf(stderr, "monster must be 0-9 a-f\n");
						return 1;
					}
				} else {
					fprintf(stderr, "usage: rlg327 --unify <monster>\n");
					return 1;
				}
			} else {
				// undefined option
				fprintf(stderr, "option %s is undefined.\n", argv[i]);
				fprintf(stderr, "see available options using -h switch.\n");
				return 1;
			}
		}
	} // end of options

	if (!seed) 
		// if seed is unset
		srand(seed = time(NULL));
	else 
		// seed is set by -s option
		srand(seed);

	Debug::log("seed: %d", seed);
	Debug::log("save path: %s", savep);
	Debug::log("load path: %s\n", loadp);

	/* Add flame animations */
	int height          = 24;
	int width           = 80;
	int size            = height * width;
	char flameChars[]   = {' ', '.', ':', '^', '*', 'x', 's', 'S', '#', '$' };
	int index[size+width+1];

	/* Init curses profiles */
	initscr();
	noecho();
	raw();
	curs_set(0);
	start_color();
	keypad(stdscr, TRUE);
	init_pair(1,0,0);
	init_pair(2,1,0);
	init_pair(3,3,0);
	init_pair(4,4,0);

	for(int i=0;i<(size+width+1);i++) { memset(index, 0, sizeof index); } //fill array with zeros

	WINDOW *popupArt=newwin(19,71,0,4);

	while(1)
	{
		for(int i = 0;i<int(width/9);i++)
		{
			index[(rand()%width)+width * (height-1)] = 65;
		}

		for(int j = 0; j < size; j++)
		{
			int color;
			index[j] = int((index[j]+index[j+1]+index[j+width]+index[j+width+1])/4);
			if(index[j]>15) { color = 4; }
			else if(index[j]<=15 && index[j]>9) { color = 3; }
			else if(index[j]<=9 && index[j]>4) { color = 2; }
			else { color = 1; }

			if(j<size-1)
			{
				/* Add characters to the screen */
				int flameInd=0;
				if(index[j]>9) { flameInd = 9; }
				else { flameInd = index[j]; }
				mvaddch(j/width, j%width, flameChars[flameInd] | COLOR_PAIR(color) | A_BOLD);
			}
		} 
		refresh();
		timeout(90);
		windowArt(popupArt);
		wrefresh(popupArt);

		/* This will cancel the animation and begin the game */
		if(getch()!=-1) { break; }
	}
	delwin(popupArt);
	endwin();

	/* define new dungeon */
	dungeon = new Dungeon;

	if (loadp) {
		if (dungeon->load(loadp)) {
			fprintf(stderr, "dungeon: failed to load dungeon.\n");
			return 1; // load may fail when file does not exist
		}
	} else {
		dungeon->generate();
	}

	pc = new PC;	

	MonsterFactory::load(monpath);
	ObjectFactory::load(objpath);

	dungeon->generateMonsters(nummon);
	dungeon->generateObjects(numobj);

	dungeon->turn->enqueue(pc);

	// if PC location is unset
	if (!pcx && !pcy) {
		// place at random location
		dungeon->placeCharacter(pc);
	} else {
		// f PC location is specified then place it at specified location
		dungeon->placeCharacter(pc, pcx, pcy);
	}

	// unify all monsters to one type
	if (unify) {
		// unify characteristics
		char unifyc = unify >= 'a' ? unify - 'a' + 10 : unify - '0'; 
		for (int i=0; i<nummon; i++)
			dungeon->npcv[i]->setAbil(unifyc);
	}

	// init distance map
	int pcx, pcy;
	pc->getLocation(&pcx, &pcy);
	Dijkstra::run(pcx, pcy, 0);
	Dijkstra::run(pcx, pcy, 1);


	UI::initColors();

	// game simulation
	while (1) {
		UI::clearRow(22);

		char buffer[80];
		if (pc->isDead())
			sprintf(buffer, "YOU LOST ! (press Q to quit)");
		else if (dungeon->nummon() == 0)
			sprintf(buffer, "YOU WON !! (press Q to quit)");
		else { 
			sprintf(buffer,"%02d MONSTERS LEFT !!",
					dungeon->nummon());

			mvprintw(22, 20, " A: AI C: Cast Spell");
		}

		mvprintw(22, 1, "%s", buffer);

		UI::reprint();

		// update attacked monster HP bar
		if (pc->attacking)
			UI::printMonsterHP();
		else
			UI::clearRow(0);

		refresh();

		if (pc->isDead() || !nummon) {
			char ch;
			while ((ch=getch())!='Q' && (ch!='q'));
			break;
		}

		int quit = 0;

		int ch = getch();

		// 1 if user does some action that should cost no turn
		int noturn = 0;

		int pcx, pcy;
		pc->getLocation(&pcx, &pcy);

		/* Call pickup every time you walk over an item, pc->pickup handles full inventory exception */
		if(dungeon->imap[pcy][pcx]) 
		{
			pc->pickup(pcx, pcy);
		}

		int slot;
		int color;

		switch (ch) {
			case 'Q':
			case 'q':
				quit = 1;
				if (!pc->isDead() && dungeon->nummon()>0 && ch=='q')
					quit = !quit; // don't quit if PC is not dead
				noturn = 1;
				break;
			case 'A':
			case 'a':
				Move::pcAI(); // automatic
				break;
			case '7':
			case 'Y':
			case 'y':
				noturn = Move::move(pc, pcx-1, pcy-1);
				break;
			case '8':
			case 'K':
			case 'k':
				noturn = Move::move(pc, pcx, pcy-1);
				break;
			case '9':
			case 'U':
			case 'u':
				noturn = Move::move(pc, pcx+1, pcy-1);
				break;
			case '6':
			case 'L':
			case 'l':
				noturn = Move::move(pc, pcx+1, pcy);
				break;
			case '3':
			case 'N':
			case 'n':
				noturn = Move::move(pc, pcx+1, pcy+1);
				break;
			case '2':
			case 'J':
			case 'j':
				noturn = Move::move(pc, pcx, pcy+1);
				break;
			case '1':
			case 'B':
			case 'b':
				noturn = Move::move(pc, pcx-1, pcy+1);
				break;
			case '4':
			case 'H':
			case 'h':
				noturn = Move::move(pc, pcx-1, pcy);
				break;
			case '>':
				if (dungeon->tmap[pcy][pcx]=='>')
					regenerateDungeon();
				else
					noturn = 1;
				break;
			case '<':
				if (dungeon->tmap[pcy][pcx]=='<')
					regenerateDungeon();
				else
					noturn = 1;
				break;
			case 'M':
			case 'm':
				UI::mList(); noturn = 1;
				break;
			case 'O':
			case 'o':
				UI::oList(); noturn = 1;
				break;
			case 'S':
			case 's':
				quit = 1;
				if (!pc->isDead() && ch=='s')
					quit = !quit; // don't quit if PC is not dead
				noturn = 1;
				break;
			case 'C':
			case 'c':
				noturn = 1;
				UI::sList();
				break;
			case 't':
				color = COLOR_RED;
				attron(COLOR_PAIR(color));
				mvprintw(10,15,"Select an index to unequip from bottom-right inventory 0-11");
				slot=getch()-'0';
				if(slot<pc->equipment.size())
				{
					pc->unequip(slot); 
				}
				else { mvprintw(11,26, "select a valid equipment index"); }
				attroff(COLOR_PAIR(color));
				getch();
				break;
			case '+':
				UI::wMode(); noturn = 1;
				break;
			case 'i':
				UI::iList(); noturn = 1;
				break;
			default:
				noturn = 1;
				break;
		}
		if (quit)
			break;
		if (ch == STAIR_UP || ch == STAIR_DN)
			continue;
		if (noturn)
			continue;

		// process monsters' turns until PC's turn
		while (1) {
			Character *c = dungeon->turn->dequeue();

			if (c->isDead()) {
				dungeon->removeMonster((NPC *)c);
				continue;
			}

			dungeon->turn->enqueue(c);

			if (c->frozen) {
				c->frozen--;
				continue;
			}
			if (c->poison) {
				c->poison--;
				c->hp *= 0.9;
			}

			if (c->isPC()) {					
				// increase HP
				pc->hp += pc->hpmax() * 0.01;
				if (pc->hp > pc->hpmax())
					pc->hp = pc->hpmax();

				// increase MP
				pc->mp += pc->mpmax() * 0.01;
				if (pc->mp > pc->mpmax())
					pc->mp = pc->mpmax();

				break;
			} else {
				Move::npc((NPC *)c);
				if (pc->isDead())
					break;
			}
		} // end of NPC turn
	} // end of game simulation

	endwin();

	if (savep) {
		dungeon->save(savep);
	}
	MonsterFactory::deleteFactories();
	ObjectFactory::deleteFactories();

	delete dungeon;

	delete pc;

	return 0;
}

static int regenerateDungeon()
{
	delete dungeon;

	dungeon = new Dungeon;

	dungeon->generate();

	dungeon->generateMonsters(nummon);
	dungeon->generateObjects(numobj);

	pc->clearSeenDungeon();

	// reset turn	
	pc->setTurn(0);

	pc->attacking = NULL;

	dungeon->turn->enqueue(pc);

	dungeon->placeCharacter(pc);

	pc->getLocation(&pcx, &pcy);

	// init distance map
	Dijkstra::run(pcx, pcy, 0);
	Dijkstra::run(pcx, pcy, 1);

	return 0;
}

