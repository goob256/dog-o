#ifndef BATTLES_H
#define BATTLES_H

#include <wedge2/inventory.h>

#include "battle_game.h"

class Battle_Enemy;

class Battle_2Puddle : public Battle_Game
{
public:
	Battle_2Puddle();
	virtual ~Battle_2Puddle();

	bool start();

private:
	wedge::Object get_found_object();
};

class Battle_Sensei1 : public Battle_Game
{
public:
	Battle_Sensei1();
	virtual ~Battle_Sensei1();

	bool start();

private:
	wedge::Object get_found_object();
};

class Battle_1Wrath1Puddle : public Battle_Game
{
public:
	Battle_1Wrath1Puddle();
	virtual ~Battle_1Wrath1Puddle();

	bool start();

private:
	wedge::Object get_found_object();
};

class Battle_2Wrath : public Battle_Game
{
public:
	Battle_2Wrath();
	virtual ~Battle_2Wrath();

	bool start();

private:
	wedge::Object get_found_object();
};

class Battle_1Coinman1Puddle : public Battle_Game
{
public:
	Battle_1Coinman1Puddle();
	virtual ~Battle_1Coinman1Puddle();

	bool start();

private:
	wedge::Object get_found_object();
};

class Battle_2Coinman : public Battle_Game
{
public:
	Battle_2Coinman();
	virtual ~Battle_2Coinman();

	bool start();

private:
	wedge::Object get_found_object();
};

class Battle_3Unicorn : public Battle_Game
{
public:
	Battle_3Unicorn();
	virtual ~Battle_3Unicorn();

	bool start();

private:
	wedge::Object get_found_object();
};

class Battle_Octo : public Battle_Game
{
public:
	Battle_Octo();
	virtual ~Battle_Octo();

	bool start();
	bool run();

	void set_tentacles(std::vector<wedge::Battle_Enemy *> tentacles);
	std::vector<wedge::Battle_Enemy *> get_tentacles();
	void delete_tentacle(wedge::Battle_Enemy *tentacle);
	
	void lost_device();
	void found_device();

private:
	wedge::Object get_found_object();
	std::vector<wedge::Battle_Enemy *> tentacles;
};

class Battle_3FlameFrog : public Battle_Game
{
public:
	Battle_3FlameFrog(std::string bgname, int delay);
	virtual ~Battle_3FlameFrog();

	bool start();

private:
	wedge::Object get_found_object();
};

class Battle_2Bot : public Battle_Game
{
public:
	Battle_2Bot(std::string bgname, int delay);
	virtual ~Battle_2Bot();

	bool start();

private:
	wedge::Object get_found_object();
};

class Battle_1FlameFrog1Bot : public Battle_Game
{
public:
	Battle_1FlameFrog1Bot(std::string bgname, int delay);
	virtual ~Battle_1FlameFrog1Bot();

	bool start();

private:
	wedge::Object get_found_object();
};

class Battle_3Goblin : public Battle_Game
{
public:
	Battle_3Goblin(std::string bgname, int delay);
	virtual ~Battle_3Goblin();

	bool start();

private:
	wedge::Object get_found_object();
};

class Battle_4Goblin : public Battle_Game
{
public:
	Battle_4Goblin(std::string bgname, int delay);
	virtual ~Battle_4Goblin();

	bool start();

private:
	wedge::Object get_found_object();
};

class Battle_2RedWasp : public Battle_Game
{
public:
	Battle_2RedWasp(std::string bgname, int delay);
	virtual ~Battle_2RedWasp();

	bool start();

private:
	wedge::Object get_found_object();
};

class Battle_1RedWasp2Goblin : public Battle_Game
{
public:
	Battle_1RedWasp2Goblin(std::string bgname, int delay);
	virtual ~Battle_1RedWasp2Goblin();

	bool start();

private:
	wedge::Object get_found_object();
};

class Battle_Chilly : public Battle_Game
{
public:
	Battle_Chilly();
	virtual ~Battle_Chilly();

	bool run();
	bool start();

private:
	wedge::Object get_found_object();
};

class Battle_6Snapper : public Battle_Game
{
public:
	Battle_6Snapper();
	virtual ~Battle_6Snapper();

	bool start();

private:
	wedge::Object get_found_object();
};

class Battle_3Bone : public Battle_Game
{
public:
	Battle_3Bone();
	virtual ~Battle_3Bone();

	bool start();

private:
	wedge::Object get_found_object();
};

class Battle_2Aura : public Battle_Game
{
public:
	Battle_2Aura(std::string bgname, int delay);
	virtual ~Battle_2Aura();

	bool start();

private:
	wedge::Object get_found_object();
};

class Battle_1Clone2Bone : public Battle_Game
{
public:
	Battle_1Clone2Bone();
	virtual ~Battle_1Clone2Bone();

	bool start();

private:
	wedge::Object get_found_object();
};

class Battle_4Snapper : public Battle_Game
{
public:
	Battle_4Snapper();
	virtual ~Battle_4Snapper();

	bool start();

private:
	wedge::Object get_found_object();
};

class Battle_1Aura2Snapper : public Battle_Game
{
public:
	Battle_1Aura2Snapper();
	virtual ~Battle_1Aura2Snapper();

	bool start();

private:
	wedge::Object get_found_object();
};

class Battle_2Bone : public Battle_Game
{
public:
	Battle_2Bone();
	virtual ~Battle_2Bone();

	bool start();

private:
	wedge::Object get_found_object();
};

class Battle_1Aura1Bone : public Battle_Game
{
public:
	Battle_1Aura1Bone();
	virtual ~Battle_1Aura1Bone();

	bool start();

private:
	wedge::Object get_found_object();
};

class Battle_1CutiePie1Bone : public Battle_Game
{
public:
	Battle_1CutiePie1Bone();
	virtual ~Battle_1CutiePie1Bone();

	bool start();

private:
	wedge::Object get_found_object();
};


class Battle_1Thirsty : public Battle_Game
{
public:
	Battle_1Thirsty();
	virtual ~Battle_1Thirsty();

	bool start();

private:
	wedge::Object get_found_object();
};

class Battle_2CutiePie : public Battle_Game
{
public:
	Battle_2CutiePie();
	virtual ~Battle_2CutiePie();

	bool start();

private:
	wedge::Object get_found_object();
};

class Battle_1CutiePie1Clone : public Battle_Game
{
public:
	Battle_1CutiePie1Clone();
	virtual ~Battle_1CutiePie1Clone();

	bool start();

private:
	wedge::Object get_found_object();
};

class Battle_Bumper : public Battle_Game
{
public:
	Battle_Bumper();
	virtual ~Battle_Bumper();

	bool run();
	bool start();

private:
	wedge::Object get_found_object();
};

class Battle_BigTuna : public Battle_Game
{
public:
	Battle_BigTuna();
	virtual ~Battle_BigTuna();

	bool start();

private:
	wedge::Object get_found_object();
};

#endif // BATTLES_H
