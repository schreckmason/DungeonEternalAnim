#ifndef UI_H
#define UI_H

class UI
{
	private:
		static int printEquipmentSlots();

		static int selectNPC(NPC *);
		
	public:
		static int initColors();
		
		/* display a help page when -h is specified */
		static int help();

		static int clearRow(int row);
		static int reprint();
		
		static int mList();
		static int oList();
		static int sList(); // spell list

		static int printHP();
		static int printMP();

		static int printMonsterHP();
		static int printMonsterHP(NPC *);

		static int selectTarget();

		static int asciiDragon();/* Dragon ascii art */
		static int flameAnim();/* Flame animation */

		static int iList(); /* inventory list */
		static int wMode(); /* Wizard mode */
};

#endif
