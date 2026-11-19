#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "entity.h"
#include "utils.h"

Entity entity_create(int x, int y, int level)
{
	Entity new_ent;
	new_ent.x = x;
	new_ent.y = y;
	new_ent.is_alive = 1;
	new_ent.defense = 0;

	float mult = 1.0f + (level - 1) * 0.3f;

	int type_rand = rand() % 100;
	if (type_rand < 40) {
		new_ent.type = ENT_TREE; //15% дерево(Откуда оно в башне?Ну, я не знаю)
		new_ent.symbol = 'T';
		new_ent.hp = new_ent.max_hp = (int)(10 * mult);
		new_ent.attack = 0;
		new_ent.drop_item_id = 1;
	}
	else if (type_rand < 80) {
		new_ent.type = ENT_ROCK; //10% камень
		new_ent.symbol = 'R';
		new_ent.hp = new_ent.max_hp = (int)(15 * mult);
		new_ent.attack = 0;
		new_ent.drop_item_id = 2;
	}
	else if (type_rand < 90) {
		new_ent.type = ENT_HEAL;
		new_ent.symbol = '+';
		new_ent.hp = new_ent.max_hp = 1;
		new_ent.attack = 0;
		new_ent.drop_item_id = 0;
	}
	else {
		new_ent.type = ENT_MONSTER;
		new_ent.symbol = 'M';
		new_ent.hp = new_ent.max_hp = (int)(20 * mult);
		new_ent.attack = (int)(5 * mult);
		new_ent.defense = (int)(2 * mult);
		new_ent.drop_item_id = 10;
	}
	return new_ent;
}

void entity_damage(Entity* e, Player* p, char* out_message)
{
	p->stamina -= 2;
	if (p->stamina < 0) p->stamina = 0;

	if (e->type == ENT_TREE || e->type == ENT_ROCK)
	{
		e->hp -= p->attack;
		if (e->hp <= 0)
		{
			e->is_alive = 0;
			int added = 0;
			for (int i = 0; i < MAX_ITEMS; i++)
			{
				if (p->inventory[i] == 0)
				{
					p->inventory[i] = e->drop_item_id;
					added = 1;
					break;
				}
			}
			if (added)
				sprintf(out_message, "Вы разрубили объект и получили: %s!", item_names[e->drop_item_id]);
			else
				sprintf(out_message, "Рюкзак полон! Ресурс пропал.");
		}
		else
		{
			sprintf(out_message, "Вы ударили %c на %d урона! (Осталось HP: %d)", e->symbol, p->attack, e->hp);
		}
	}
	else if (e->type == ENT_HEAL)
	{
		p->y = e->y;
		p->x = e->x;
		e->is_alive = 0;

		p->hp += 20;
		if (p->hp > p->max_hp) p->hp = p->max_hp;

		p->stamina += 15;
		if (p->stamina > p->max_stamina) p->stamina = p->max_stamina;

		sprintf(out_message, "Вы подобрали аптечку! +20 HP, +15 Выносливости.");
	}
	else if (e->type == ENT_MONSTER)
	{
		int hero_dmg = p->attack - (e->defense / 2);
		if (hero_dmg < 1) hero_dmg = 1;
		e->hp -= hero_dmg;

		if (e->hp <= 0)
		{
			e->is_alive = 0;
			int gold_reward = 5;
			p->gold += gold_reward;

			char loot_msg[100] = "";
			if (rand() % 100 < 25)
			{
				int added = 0;
				for (int i = 0; i < MAX_ITEMS; i++)
				{
					if (p->inventory[i] == 0)
					{
						p->inventory[i] = 10;
						added = 1;
						break;
					}
				}
				if (added) sprintf(loot_msg, " И трофей: %s!", item_names[10]);
				else sprintf(loot_msg, " Рюкзак полон, клык потерян.");
			}
			sprintf(out_message, "Монстр побеждён! +%d золота.%s", gold_reward, loot_msg);
		}
		else
		{
			int mon_dmg = e->attack - (p->defense / 2);
			if (mon_dmg < 1) mon_dmg = 1;
			p->hp -= mon_dmg;
			sprintf(out_message, "Вы ударили монстра на %d! Монстр бьёт в ответ на %d!", hero_dmg, mon_dmg);
		}
	}
}