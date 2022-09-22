#include <wedge2/generic_callback.h>
#include <wedge2/globals.h>

#include "achievements.h"
#include "battles.h"
#include "enemies.h"
#include "general.h"
#include "inventory.h"

Battle_2Puddle::Battle_2Puddle() :
	Battle_Game("cave1-", 0)
{
}

Battle_2Puddle::~Battle_2Puddle()
{
}

bool Battle_2Puddle::start()
{
	if (Battle_Game::start() == false) {
		return false;
	}

	wedge::Battle_Enemy *puddle1 = new Enemy_Puddle();
	puddle1->start();
	puddle1->set_position(util::Point<int>(shim::tile_size*4.0f, shim::tile_size*2.5f));
	entities.push_back(puddle1);

	gold += puddle1->get_gold();

	wedge::Battle_Enemy *puddle2 = new Enemy_Puddle();
	puddle2->start();
	puddle2->set_position(util::Point<int>(shim::tile_size, shim::tile_size*2.0f));
	entities.push_back(puddle2);

	gold += puddle2->get_gold();

	return true;
}

wedge::Object Battle_2Puddle::get_found_object()
{
	if (util::rand(1, 5) == 1) {
		return OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_APPLE, 1);
	}
	else {
		wedge::Object o;
		o.type = wedge::OBJECT_NONE;
		return o;
	}
}

//--

Battle_Sensei1::Battle_Sensei1() :
	Battle_Game("cave1-", 0)
{
	boss_battle = true;
	boss_music_name = "music/sensei.mml";

	auto contents = OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_APPLE, 2);
	INSTANCE->inventory.add(contents);

	can_die = false;
}

Battle_Sensei1::~Battle_Sensei1()
{
}

bool Battle_Sensei1::start()
{
	if (Battle_Game::start() == false) {
		return false;
	}

	auto sensei = new Enemy_Sensei1();
	sensei->start();
	sensei->set_position(util::Point<int>(shim::tile_size*2.5f, shim::tile_size*2.75f));
	entities.push_back(sensei);

	return true;
}

wedge::Object Battle_Sensei1::get_found_object()
{
	util::achieve((void *)ACH_WHITEBELT);
	return OBJECT->make_object(wedge::OBJECT_ARMOUR, BELT_WHITE, 2);
}

//--

Battle_1Wrath1Puddle::Battle_1Wrath1Puddle() :
	Battle_Game("cave1-", 0)
{
}

Battle_1Wrath1Puddle::~Battle_1Wrath1Puddle()
{
}

bool Battle_1Wrath1Puddle::start()
{
	if (Battle_Game::start() == false) {
		return false;
	}

	wedge::Battle_Enemy *puddle1 = new Enemy_Puddle();
	puddle1->start();
	puddle1->set_position(util::Point<int>(shim::tile_size*5.25f, shim::tile_size*2.5f));
	entities.push_back(puddle1);

	gold += puddle1->get_gold();

	wedge::Battle_Enemy *wrath1 = new Enemy_Wrath();
	wrath1->start();
	wrath1->set_position(util::Point<int>(shim::tile_size*0.75f, shim::tile_size*2.0f));
	entities.push_back(wrath1);

	gold += wrath1->get_gold();

	return true;
}

wedge::Object Battle_1Wrath1Puddle::get_found_object()
{
	if (util::rand(1, 15) == 1) {
		return OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_APPLE, 1);
	}
	else {
		wedge::Object o;
		o.type = wedge::OBJECT_NONE;
		return o;
	}
}

//--

Battle_2Wrath::Battle_2Wrath() :
	Battle_Game("cave1-", 0)
{
}

Battle_2Wrath::~Battle_2Wrath()
{
}

bool Battle_2Wrath::start()
{
	if (Battle_Game::start() == false) {
		return false;
	}

	wedge::Battle_Enemy *wrath1 = new Enemy_Wrath();
	wrath1->start();
	wrath1->set_position(util::Point<int>(shim::tile_size*4.25f, shim::tile_size*2.5f));
	entities.push_back(wrath1);

	gold += wrath1->get_gold();

	wedge::Battle_Enemy *wrath2 = new Enemy_Wrath();
	wrath2->start();
	wrath2->set_position(util::Point<int>(shim::tile_size, shim::tile_size*2.0f));
	entities.push_back(wrath2);

	gold += wrath2->get_gold();

	return true;
}

wedge::Object Battle_2Wrath::get_found_object()
{
	if (util::rand(1, 10) == 1) {
		return OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_CARAMEL, 1);
	}
	else {
		wedge::Object o;
		o.type = wedge::OBJECT_NONE;
		return o;
	}
}

//--

Battle_1Coinman1Puddle::Battle_1Coinman1Puddle() :
	Battle_Game("cave1-", 0)
{
}

Battle_1Coinman1Puddle::~Battle_1Coinman1Puddle()
{
}

bool Battle_1Coinman1Puddle::start()
{
	if (Battle_Game::start() == false) {
		return false;
	}

	wedge::Battle_Enemy *puddle1 = new Enemy_Puddle();
	puddle1->start();
	puddle1->set_position(util::Point<int>(shim::tile_size*5.0f, shim::tile_size*2.5f));
	entities.push_back(puddle1);

	gold += puddle1->get_gold();

	wedge::Battle_Enemy *coinman1 = new Enemy_Coinman();
	coinman1->start();
	coinman1->set_position(util::Point<int>(shim::tile_size, shim::tile_size*2.0f));
	entities.push_back(coinman1);

	gold += coinman1->get_gold();

	return true;
}

wedge::Object Battle_1Coinman1Puddle::get_found_object()
{
	if (util::rand(1, 15) == 1) {
		return OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_APPLE, 1);
	}
	else {
		wedge::Object o;
		o.type = wedge::OBJECT_NONE;
		return o;
	}
}

//--

Battle_2Coinman::Battle_2Coinman() :
	Battle_Game("cave1-", 0)
{
}

Battle_2Coinman::~Battle_2Coinman()
{
}

bool Battle_2Coinman::start()
{
	if (Battle_Game::start() == false) {
		return false;
	}

	wedge::Battle_Enemy *coinman1 = new Enemy_Coinman();
	coinman1->start();
	coinman1->set_position(util::Point<int>(shim::tile_size*4.0f, shim::tile_size*1.5f));
	entities.push_back(coinman1);

	gold += coinman1->get_gold();

	wedge::Battle_Enemy *coinman2 = new Enemy_Coinman();
	coinman2->start();
	coinman2->set_position(util::Point<int>(shim::tile_size, shim::tile_size*2.0f));
	entities.push_back(coinman2);

	gold += coinman2->get_gold();

	return true;
}

wedge::Object Battle_2Coinman::get_found_object()
{
	if (util::rand(1, 15) == 1) {
		return OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_BANANA, 1);
	}
	else {
		wedge::Object o;
		o.type = wedge::OBJECT_NONE;
		return o;
	}
}

//--

Battle_3Unicorn::Battle_3Unicorn() :
	Battle_Game("cave1-", 0)
{
}

Battle_3Unicorn::~Battle_3Unicorn()
{
}

bool Battle_3Unicorn::start()
{
	if (Battle_Game::start() == false) {
		return false;
	}

	wedge::Battle_Enemy *unicorn1 = new Enemy_Unicorn();
	unicorn1->start();
	unicorn1->set_position(util::Point<int>(shim::tile_size*5.5f, shim::tile_size*2.0f));
	entities.push_back(unicorn1);

	gold += unicorn1->get_gold();

	wedge::Battle_Enemy *unicorn2 = new Enemy_Unicorn();
	unicorn2->start();
	unicorn2->set_position(util::Point<int>(shim::tile_size*3.5f, shim::tile_size*3.5f));
	entities.push_back(unicorn2);

	gold += unicorn2->get_gold();

	wedge::Battle_Enemy *unicorn3 = new Enemy_Unicorn();
	unicorn3->start();
	unicorn3->set_position(util::Point<int>(shim::tile_size*1.5f, shim::tile_size*2.25f));
	entities.push_back(unicorn3);

	gold += unicorn3->get_gold();

	return true;
}

wedge::Object Battle_3Unicorn::get_found_object()
{
	if (util::rand(1, 10) == 1) {
		return OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_APPLE, 1);
	}
	else {
		wedge::Object o;
		o.type = wedge::OBJECT_NONE;
		return o;
	}
}

//--

Battle_Octo::Battle_Octo() :
	Battle_Game("boss1-", 0)
{
	boss_battle = true;
	boss_music_name = "music/boss.mml";
	
	has_darkness = true;
	darkness_image1 = gfx::gen_plasma(1, 0.5f, 0.25f, shim::white);
	darkness_image2 = gfx::gen_plasma(2, 0.5f, 0.0f, shim::white);
	darkness_offset1 = util::Point<float>(0, 0);
	darkness_offset2 = util::Point<float>(128, 64);

	attack = 15;
	mp = 25;
}

Battle_Octo::~Battle_Octo()
{
	delete darkness_image1;
	delete darkness_image2;
}

bool Battle_Octo::start()
{
	if (Battle_Game::start() == false) {
		return false;
	}

	util::Point<float> t1_pos = {100, 20};

	wedge::Battle_Enemy *tentacle1 = new Enemy_Tentacle();
	tentacle1->start();
	tentacle1->set_position(t1_pos);
	entities.push_back(tentacle1);

	gold += tentacle1->get_gold();

	wedge::Battle_Enemy *tentacle2 = new Enemy_Tentacle();
	tentacle2->start();
	tentacle2->set_position(t1_pos + util::Point<float>(-1.0f*shim::tile_size, -0.9f*shim::tile_size));
	entities.push_back(tentacle2);

	gold += tentacle2->get_gold();

	wedge::Battle_Enemy *tentacle3 = new Enemy_Tentacle();
	tentacle3->start();
	tentacle3->set_position(t1_pos + util::Point<float>(-2.5f*shim::tile_size, -1.65f*shim::tile_size));
	entities.push_back(tentacle3);

	gold += tentacle3->get_gold();

	wedge::Battle_Enemy *tentacle4 = new Enemy_Tentacle();
	tentacle4->start();
	tentacle4->set_position(t1_pos + util::Point<float>(-4.5f*shim::tile_size, -1.5f*shim::tile_size));
	entities.push_back(tentacle4);

	gold += tentacle4->get_gold();

	wedge::Battle_Enemy *tentacle5 = new Enemy_Tentacle();
	tentacle5->start();
	tentacle5->set_position(t1_pos + util::Point<float>(-6.25f*shim::tile_size, 0.2f*shim::tile_size));
	entities.push_back(tentacle5);

	gold += tentacle5->get_gold();

	wedge::Battle_Enemy *tentacle6 = new Enemy_Tentacle();
	tentacle6->start();
	tentacle6->set_position(t1_pos + util::Point<float>(-1.1f*shim::tile_size, 1.0f*shim::tile_size));
	entities.push_back(tentacle6);


	gold += tentacle6->get_gold();

	wedge::Battle_Enemy *tentacle7 = new Enemy_Tentacle();
	tentacle7->start();
	tentacle7->set_position(t1_pos + util::Point<float>(-2.3f*shim::tile_size, 1.8f*shim::tile_size));
	entities.push_back(tentacle7);

	gold += tentacle7->get_gold();

	wedge::Battle_Enemy *tentacle8 = new Enemy_Tentacle();
	tentacle8->start();
	tentacle8->set_position(t1_pos + util::Point<float>(-4.6f*shim::tile_size, 1.6f*shim::tile_size));
	entities.push_back(tentacle8);

	gold += tentacle8->get_gold();

	wedge::Battle_Enemy *octo = new Enemy_Octo();
	octo->start();
	octo->set_position(t1_pos + util::Point<float>(-4*shim::tile_size, -0.75f*shim::tile_size));
	entities.push_back(octo);

	gold += octo->get_gold();

	/*
	// Right to left
	entities.push_back(tentacle1);
	entities.push_back(tentacle2);
	entities.push_back(tentacle6);
	entities.push_back(tentacle7);
	entities.push_back(tentacle3);
	entities.push_back(octo);
	entities.push_back(tentacle4);
	entities.push_back(tentacle8);
	entities.push_back(tentacle5);
	*/

	tentacles.push_back(tentacle1);
	tentacles.push_back(tentacle2);
	tentacles.push_back(tentacle3);
	tentacles.push_back(tentacle4);
	tentacles.push_back(tentacle5);
	tentacles.push_back(tentacle6);
	tentacles.push_back(tentacle7);
	tentacles.push_back(tentacle8);

	return true;
}

wedge::Object Battle_Octo::get_found_object()
{
	util::achieve((void *)ACH_8HANDED);
	util::achieve((void *)ACH_GOLDBELT);
	return OBJECT->make_object(wedge::OBJECT_ARMOUR, BELT_GOLD, 2);
}

void Battle_Octo::set_tentacles(std::vector<wedge::Battle_Enemy *> tentacles)
{
	this->tentacles = tentacles;
}

std::vector<wedge::Battle_Enemy *> Battle_Octo::get_tentacles()
{
	return tentacles;
}

void Battle_Octo::delete_tentacle(wedge::Battle_Enemy *tentacle)
{
	for (auto it = tentacles.begin(); it != tentacles.end(); it++) {
		wedge::Battle_Enemy *e = *it;
		if (e == tentacle) {
			it = tentacles.erase(it);
			return;
		}
	}
}

bool Battle_Octo::run()
{
	if (done == false && tentacles.size() == 0) {
		auto enemies = get_enemies();
		set_done(true);
		close_guis();
		window_shown = false;
		player_stats_shown = false;
		enemy_stats_shown = false;
		enemies[0]->get_sprite()->set_animation("scared");
		auto v = get_players();
		for (auto p : v) {
			if (p->get_stats()->hp > 0) {
				p->get_sprite()->set_animation("victory");
			}
		}
		if (music_backup == nullptr) {
			music_backup = shim::music;
		}
		else {
			delete shim::music;
		}
		shim::music = new audio::MML("music/victory.mml");
		shim::music->play(shim::music_volume, true);
	}

	darkness_offset1.x += 0.04f;
	darkness_offset1.y += 0.02f;
	darkness_offset2.x -= 0.04f;
	darkness_offset2.y -= 0.02f;

	return Battle_Game::run();
}

void Battle_Octo::lost_device()
{
	wedge::Battle_Game::lost_device();
	delete darkness_image1;
	darkness_image1 = NULL;
	delete darkness_image2;
	darkness_image2 = NULL;
}

void Battle_Octo::found_device()
{
	wedge::Battle_Game::found_device();
	darkness_image1 = gfx::gen_plasma(1, 0.5f, 0.25f, shim::white);
	darkness_image2 = gfx::gen_plasma(2, 0.5f, 0.0f, shim::white);
}

//--

Battle_3FlameFrog::Battle_3FlameFrog(std::string bgname, int delay) :
	Battle_Game(bgname, delay)
{
}

Battle_3FlameFrog::~Battle_3FlameFrog()
{
}

bool Battle_3FlameFrog::start()
{
	if (Battle_Game::start() == false) {
		return false;
	}

	wedge::Battle_Enemy *frog1 = new Enemy_Flame_Frog();
	frog1->start();
	frog1->set_position(util::Point<int>(shim::tile_size*5.0f, shim::tile_size*1.0f));
	entities.push_back(frog1);

	gold += frog1->get_gold();

	wedge::Battle_Enemy *frog2 = new Enemy_Flame_Frog();
	frog2->start();
	frog2->set_position(util::Point<int>(shim::tile_size*3.0f, shim::tile_size*2.5f));
	entities.push_back(frog2);

	gold += frog2->get_gold();

	wedge::Battle_Enemy *frog3 = new Enemy_Flame_Frog();
	frog3->start();
	frog3->set_position(util::Point<int>(shim::tile_size*1.0f, shim::tile_size*1.25f));
	entities.push_back(frog3);

	gold += frog3->get_gold();

	return true;
}

wedge::Object Battle_3FlameFrog::get_found_object()
{
	if (util::rand(1, 15) == 1) {
		return OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_ORANGE, 1);
	}
	else {
		wedge::Object o;
		o.type = wedge::OBJECT_NONE;
		return o;
	}
}

//--

Battle_2Bot::Battle_2Bot(std::string bgname, int delay) :
	Battle_Game(bgname, delay)
{
}

Battle_2Bot::~Battle_2Bot()
{
}

bool Battle_2Bot::start()
{
	if (Battle_Game::start() == false) {
		return false;
	}

	wedge::Battle_Enemy *bot1 = new Enemy_Bot();
	bot1->start();
	bot1->set_position(util::Point<int>(shim::tile_size*3.5f, shim::tile_size*1.25f));
	entities.push_back(bot1);

	gold += bot1->get_gold();

	wedge::Battle_Enemy *bot2 = new Enemy_Bot();
	bot2->start();
	bot2->set_position(util::Point<int>(shim::tile_size*0.5f, shim::tile_size*1.75f));
	entities.push_back(bot2);

	gold += bot2->get_gold();

	return true;
}

wedge::Object Battle_2Bot::get_found_object()
{
	if (util::rand(1, 15) == 1) {
		return OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_ORANGE, 1);
	}
	else {
		wedge::Object o;
		o.type = wedge::OBJECT_NONE;
		return o;
	}
}

//--

Battle_1FlameFrog1Bot::Battle_1FlameFrog1Bot(std::string bgname, int delay) :
	Battle_Game(bgname, delay)
{
}

Battle_1FlameFrog1Bot::~Battle_1FlameFrog1Bot()
{
}

bool Battle_1FlameFrog1Bot::start()
{
	if (Battle_Game::start() == false) {
		return false;
	}

	wedge::Battle_Enemy *bot1 = new Enemy_Bot();
	bot1->start();
	bot1->set_position(util::Point<int>(shim::tile_size*3.75f, shim::tile_size*1.0f));
	entities.push_back(bot1);

	gold += bot1->get_gold();

	wedge::Battle_Enemy *ff = new Enemy_Flame_Frog();
	ff->start();
	ff->set_position(util::Point<int>(shim::tile_size*1.0f, shim::tile_size*2.0f));
	entities.push_back(ff);

	gold += ff->get_gold();

	return true;
}

wedge::Object Battle_1FlameFrog1Bot::get_found_object()
{
	if (util::rand(1, 15) == 1) {
		return OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_ORANGE, 1);
	}
	else {
		wedge::Object o;
		o.type = wedge::OBJECT_NONE;
		return o;
	}
}

//--

Battle_3Goblin::Battle_3Goblin(std::string bgname, int delay) :
	Battle_Game(bgname, delay)
{
}

Battle_3Goblin::~Battle_3Goblin()
{
}

bool Battle_3Goblin::start()
{
	if (Battle_Game::start() == false) {
		return false;
	}

	wedge::Battle_Enemy *goblin1 = new Enemy_Goblin();
	goblin1->start();
	goblin1->set_position(util::Point<int>(shim::tile_size*4.5f, shim::tile_size*2.0f));
	entities.push_back(goblin1);

	gold += goblin1->get_gold();

	wedge::Battle_Enemy *goblin2 = new Enemy_Goblin();
	goblin2->start();
	goblin2->set_position(util::Point<int>(shim::tile_size*2.75f, shim::tile_size*3.5f));
	entities.push_back(goblin2);

	gold += goblin2->get_gold();

	wedge::Battle_Enemy *goblin3 = new Enemy_Goblin();
	goblin3->start();
	goblin3->set_position(util::Point<int>(shim::tile_size*1.0f, shim::tile_size*2.25f));
	entities.push_back(goblin3);

	gold += goblin3->get_gold();

	return true;
}

wedge::Object Battle_3Goblin::get_found_object()
{
	if (util::rand(1, 35) == 1) {
		return OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_DAGGER, 1);
	}
	else {
		wedge::Object o;
		o.type = wedge::OBJECT_NONE;
		return o;
	}
}

//--

Battle_4Goblin::Battle_4Goblin(std::string bgname, int delay) :
	Battle_Game(bgname, delay)
{
}

Battle_4Goblin::~Battle_4Goblin()
{
}

bool Battle_4Goblin::start()
{
	if (Battle_Game::start() == false) {
		return false;
	}

	wedge::Battle_Enemy *goblin1 = new Enemy_Goblin();
	goblin1->start();
	goblin1->set_position(util::Point<int>(shim::tile_size*5.0f, shim::tile_size*1.75f));
	entities.push_back(goblin1);

	gold += goblin1->get_gold();

	wedge::Battle_Enemy *goblin2 = new Enemy_Goblin();
	goblin2->start();
	goblin2->set_position(util::Point<int>(shim::tile_size*3.5f, shim::tile_size*3.25f));
	entities.push_back(goblin2);

	gold += goblin2->get_gold();

	wedge::Battle_Enemy *goblin3 = new Enemy_Goblin();
	goblin3->start();
	goblin3->set_position(util::Point<int>(shim::tile_size*2.5f, shim::tile_size*1.75f));
	entities.push_back(goblin3);

	gold += goblin3->get_gold();

	wedge::Battle_Enemy *goblin4 = new Enemy_Goblin();
	goblin4->start();
	goblin4->set_position(util::Point<int>(shim::tile_size*1.0f, shim::tile_size*3.25f));
	entities.push_back(goblin4);

	gold += goblin4->get_gold();

	return true;
}

wedge::Object Battle_4Goblin::get_found_object()
{
	if (util::rand(1, 25) == 1) {
		return OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_DAGGER, 1);
	}
	else {
		wedge::Object o;
		o.type = wedge::OBJECT_NONE;
		return o;
	}
}

//--

Battle_2RedWasp::Battle_2RedWasp(std::string bgname, int delay) :
	Battle_Game(bgname, delay)
{
}

Battle_2RedWasp::~Battle_2RedWasp()
{
}

bool Battle_2RedWasp::start()
{
	if (Battle_Game::start() == false) {
		return false;
	}

	wedge::Battle_Enemy *wasp1 = new Enemy_Red_Wasp();
	wasp1->start();
	wasp1->set_position(util::Point<int>(shim::tile_size*4.5f, shim::tile_size*1.25f));
	entities.push_back(wasp1);

	gold += wasp1->get_gold();

	wedge::Battle_Enemy *wasp2 = new Enemy_Red_Wasp();
	wasp2->start();
	wasp2->set_position(util::Point<int>(shim::tile_size, shim::tile_size*0.75f));
	entities.push_back(wasp2);

	gold += wasp2->get_gold();

	return true;
}

wedge::Object Battle_2RedWasp::get_found_object()
{
	if (util::rand(1, 10) == 1) {
		return OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_BANANA, 1);
	}
	else {
		wedge::Object o;
		o.type = wedge::OBJECT_NONE;
		return o;
	}
}

//--

Battle_1RedWasp2Goblin::Battle_1RedWasp2Goblin(std::string bgname, int delay) :
	Battle_Game(bgname, delay)
{
}

Battle_1RedWasp2Goblin::~Battle_1RedWasp2Goblin()
{
}

bool Battle_1RedWasp2Goblin::start()
{
	if (Battle_Game::start() == false) {
		return false;
	}

	wedge::Battle_Enemy *goblin1 = new Enemy_Goblin();
	goblin1->start();
	goblin1->set_position(util::Point<int>(shim::tile_size*5.75f, shim::tile_size*1.75f));
	entities.push_back(goblin1);

	gold += goblin1->get_gold();

	wedge::Battle_Enemy *goblin2 = new Enemy_Goblin();
	goblin2->start();
	goblin2->set_position(util::Point<int>(shim::tile_size*4.75f, shim::tile_size*3.25f));
	entities.push_back(goblin2);

	gold += goblin2->get_gold();

	wedge::Battle_Enemy *wasp = new Enemy_Red_Wasp();
	wasp->start();
	wasp->set_position(util::Point<int>(shim::tile_size, shim::tile_size*1.0f));
	entities.push_back(wasp);

	gold += wasp->get_gold();

	return true;
}

wedge::Object Battle_1RedWasp2Goblin::get_found_object()
{
	if (util::rand(1, 10) == 1) {
		return OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_BANANA, 1);
	}
	else {
		wedge::Object o;
		o.type = wedge::OBJECT_NONE;
		return o;
	}
}

//--

Battle_Chilly::Battle_Chilly() :
	Battle_Game("lavaboss-", 500)
{
	boss_battle = true;
	boss_music_name = "music/boss.mml";
	
	attack = 10;
	mp = 50;
}

Battle_Chilly::~Battle_Chilly()
{
}

bool Battle_Chilly::start()
{
	if (Battle_Game::start() == false) {
		return false;
	}

	wedge::Battle_Enemy *chilly = new Enemy_Chilly();
	chilly->start();
	chilly->set_position(util::Point<int>(shim::tile_size/3, shim::tile_size/3));
	entities.push_back(chilly);

	gold += chilly->get_gold();

	return true;
}

wedge::Object Battle_Chilly::get_found_object()
{
	//util::achieve((void *)ACH_BLUEBELT);
	return OBJECT->make_object(wedge::OBJECT_ARMOUR, BELT_BLUE, 3);
}

bool Battle_Chilly::run()
{
	auto enemies = get_enemies();

	if (done == false && enemies.size() == 1 && enemies[0]->get_stats()->hp >= 1000000) {
		set_done(true);
		close_guis();
		window_shown = false;
		player_stats_shown = false;
		enemy_stats_shown = false;
		auto v = get_players();
		for (auto p : v) {
			if (p->get_stats()->hp > 0) {
				p->get_sprite()->set_animation("victory");
			}
		}
		if (music_backup == nullptr) {
			music_backup = shim::music;
		}
		else {
			delete shim::music;
		}
		shim::music = new audio::MML("music/victory.mml");
		shim::music->play(shim::music_volume, true);
	}

	return Battle_Game::run();
}

//--

Battle_6Snapper::Battle_6Snapper() :
	Battle_Game("cave3water-", 0)
{
	water = true;
}

Battle_6Snapper::~Battle_6Snapper()
{
}

bool Battle_6Snapper::start()
{
	if (Battle_Game::start() == false) {
		return false;
	}

	wedge::Battle_Enemy *snapper4 = new Enemy_Snapper();
	snapper4->start();
	snapper4->set_position(util::Point<int>(shim::tile_size*5.25f, shim::tile_size*2.0f));
	entities.push_back(snapper4);

	gold += snapper4->get_gold();

	wedge::Battle_Enemy *snapper1 = new Enemy_Snapper();
	snapper1->start();
	snapper1->set_position(util::Point<int>(shim::tile_size*4.75f, shim::tile_size*0.25f));
	entities.push_back(snapper1);

	gold += snapper1->get_gold();

	wedge::Battle_Enemy *snapper5 = new Enemy_Snapper();
	snapper5->start();
	snapper5->set_position(util::Point<int>(shim::tile_size*3.25f, shim::tile_size*1.25f));
	entities.push_back(snapper5);

	gold += snapper5->get_gold();

	wedge::Battle_Enemy *snapper2 = new Enemy_Snapper();
	snapper2->start();
	snapper2->set_position(util::Point<int>(shim::tile_size*3.0f, shim::tile_size*-0.25f));
	entities.push_back(snapper2);

	gold += snapper2->get_gold();

	wedge::Battle_Enemy *snapper6 = new Enemy_Snapper();
	snapper6->start();
	snapper6->set_position(util::Point<int>(shim::tile_size*1.75f, shim::tile_size*2.25f));
	entities.push_back(snapper6);

	gold += snapper6->get_gold();

	wedge::Battle_Enemy *snapper3 = new Enemy_Snapper();
	snapper3->start();
	snapper3->set_position(util::Point<int>(shim::tile_size*1.0f, shim::tile_size*0.75f));
	entities.push_back(snapper3);

	gold += snapper3->get_gold();

	return true;
}

wedge::Object Battle_6Snapper::get_found_object()
{
	if (util::rand(1, 10) == 1) {
		return OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_ORANGE, 1);
	}
	else if (util::rand(1, 10) == 1) {
		return OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_HOT_PEPPER, 1);
	}
	else {
		wedge::Object o;
		o.type = wedge::OBJECT_NONE;
		return o;
	}
}

//--

Battle_3Bone::Battle_3Bone() :
	Battle_Game("cave3-", 0)
{
}

Battle_3Bone::~Battle_3Bone()
{
}

bool Battle_3Bone::start()
{
	if (Battle_Game::start() == false) {
		return false;
	}

	wedge::Battle_Enemy *bone1;
	if (util::rand(0, 1) == 0) {
		bone1 = new Enemy_Bone1();
	}
	else {
		bone1 = new Enemy_Bone2();
	}
	bone1->start();
	bone1->set_position(util::Point<int>(shim::tile_size*5.0f, shim::tile_size*1.5f));
	entities.push_back(bone1);

	gold += bone1->get_gold();

	wedge::Battle_Enemy *bone2;
	if (util::rand(0, 1) == 0) {
		bone2 = new Enemy_Bone1();
	}
	else {
		bone2 = new Enemy_Bone2();
	}
	bone2->start();
	bone2->set_position(util::Point<int>(shim::tile_size*3.0f, shim::tile_size*0.75f));
	entities.push_back(bone2);

	gold += bone2->get_gold();

	wedge::Battle_Enemy *bone3;
	if (util::rand(0, 1) == 0) {
		bone3 = new Enemy_Bone1();
	}
	else {
		bone3 = new Enemy_Bone2();
	}
	bone3->start();
	bone3->set_position(util::Point<int>(shim::tile_size*1.0f, shim::tile_size*2.0f));
	entities.push_back(bone3);

	gold += bone3->get_gold();

	return true;
}

wedge::Object Battle_3Bone::get_found_object()
{
	if (util::rand(1, 10) == 1) {
		return OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_ORANGE, 1);
	}
	else if (util::rand(1, 12) == 1) {
		return OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_CARAMEL, 1);
	}
	else {
		wedge::Object o;
		o.type = wedge::OBJECT_NONE;
		return o;
	}
}

//--

Battle_2Aura::Battle_2Aura(std::string bgname, int delay) :
	Battle_Game(bgname, delay)
{
	if (bgname == "cave3water-") {
		water = true;
	}
}

Battle_2Aura::~Battle_2Aura()
{
}

bool Battle_2Aura::start()
{
	if (Battle_Game::start() == false) {
		return false;
	}

	wedge::Battle_Enemy *aura1 = new Enemy_Aura();
	aura1->start();
	aura1->set_position(util::Point<int>(shim::tile_size*4.5f, shim::tile_size*1.25f));
	entities.push_back(aura1);

	gold += aura1->get_gold();

	wedge::Battle_Enemy *aura2 = new Enemy_Aura();
	aura2->start();
	aura2->set_position(util::Point<int>(shim::tile_size*1.0f, shim::tile_size*1.75f));
	entities.push_back(aura2);

	gold += aura2->get_gold();

	return true;
}

wedge::Object Battle_2Aura::get_found_object()
{
	if (util::rand(1, 12) == 1) {
		return OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_BANANA, 1);
	}
	else {
		wedge::Object o;
		o.type = wedge::OBJECT_NONE;
		return o;
	}
}

//--

Battle_1Clone2Bone::Battle_1Clone2Bone() :
	Battle_Game("cave3-", 0)
{
}

Battle_1Clone2Bone::~Battle_1Clone2Bone()
{
}

bool Battle_1Clone2Bone::start()
{
	if (Battle_Game::start() == false) {
		return false;
	}

	wedge::Battle_Enemy *clone = new Enemy_Clone();
	clone->start();
	clone->set_position(util::Point<int>(shim::tile_size*5.5f, shim::tile_size*2.5f));
	entities.push_back(clone);

	gold += clone->get_gold();

	wedge::Battle_Enemy *bone1;
	if (util::rand(0, 1) == 0) {
		bone1 = new Enemy_Bone1();
	}
	else {
		bone1 = new Enemy_Bone2();
	}
	bone1->start();
	bone1->set_position(util::Point<int>(shim::tile_size*3.0f, shim::tile_size*0.75f));
	entities.push_back(bone1);

	gold += bone1->get_gold();

	wedge::Battle_Enemy *bone2;
	if (util::rand(0, 1) == 0) {
		bone2 = new Enemy_Bone1();
	}
	else {
		bone2 = new Enemy_Bone2();
	}
	bone2->start();
	bone2->set_position(util::Point<int>(shim::tile_size*1.0f, shim::tile_size*2.0f));
	entities.push_back(bone2);

	gold += bone1->get_gold();

	return true;
}

wedge::Object Battle_1Clone2Bone::get_found_object()
{
	if (util::rand(1, 12) == 1) {
		return OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_HOT_PEPPER, 1);
	}
	else {
		wedge::Object o;
		o.type = wedge::OBJECT_NONE;
		return o;
	}
}

//--

Battle_4Snapper::Battle_4Snapper() :
	Battle_Game("cave3water-", 0)
{
	water = true;
}

Battle_4Snapper::~Battle_4Snapper()
{
}

bool Battle_4Snapper::start()
{
	if (Battle_Game::start() == false) {
		return false;
	}

	wedge::Battle_Enemy *snapper1 = new Enemy_Snapper();
	snapper1->start();
	snapper1->set_position(util::Point<int>(shim::tile_size*4.25f, shim::tile_size*0.75f));
	entities.push_back(snapper1);

	gold += snapper1->get_gold();

	wedge::Battle_Enemy *snapper4 = new Enemy_Snapper();
	snapper4->start();
	snapper4->set_position(util::Point<int>(shim::tile_size*3.0f, shim::tile_size*2.25f));
	entities.push_back(snapper4);

	gold += snapper4->get_gold();

	wedge::Battle_Enemy *snapper2 = new Enemy_Snapper();
	snapper2->start();
	snapper2->set_position(util::Point<int>(shim::tile_size*2.5f, shim::tile_size*0.25f));
	entities.push_back(snapper2);

	gold += snapper2->get_gold();

	wedge::Battle_Enemy *snapper3 = new Enemy_Snapper();
	snapper3->start();
	snapper3->set_position(util::Point<int>(shim::tile_size*1.0f, shim::tile_size*1.25f));
	entities.push_back(snapper3);

	gold += snapper3->get_gold();

	return true;
}

wedge::Object Battle_4Snapper::get_found_object()
{
	if (util::rand(1, 20) == 1) {
		return OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_BANANA, 1);
	}
	else {
		wedge::Object o;
		o.type = wedge::OBJECT_NONE;
		return o;
	}
}

//--

Battle_2Bone::Battle_2Bone() :
	Battle_Game("cave3-", 0)
{
}

Battle_2Bone::~Battle_2Bone()
{
}

bool Battle_2Bone::start()
{
	if (Battle_Game::start() == false) {
		return false;
	}

	wedge::Battle_Enemy *bone1;
	if (util::rand(0, 1) == 0) {
		bone1 = new Enemy_Bone1();
	}
	else {
		bone1 = new Enemy_Bone2();
	}
	bone1->start();
	bone1->set_position(util::Point<int>(shim::tile_size*4.0f, shim::tile_size*1.5f));
	entities.push_back(bone1);

	gold += bone1->get_gold();

	wedge::Battle_Enemy *bone2;
	if (util::rand(0, 1) == 0) {
		bone2 = new Enemy_Bone1();
	}
	else {
		bone2 = new Enemy_Bone2();
	}
	bone2->start();
	bone2->set_position(util::Point<int>(shim::tile_size*1.5f, shim::tile_size*0.75f));
	entities.push_back(bone2);

	gold += bone2->get_gold();

	return true;
}

wedge::Object Battle_2Bone::get_found_object()
{
	if (util::rand(1, 15) == 1) {
		return OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_BANANA, 1);
	}
	else {
		wedge::Object o;
		o.type = wedge::OBJECT_NONE;
		return o;
	}
}

//--

Battle_1Aura2Snapper::Battle_1Aura2Snapper() :
	Battle_Game("cave3water-", 0)
{
	water = true;
}

Battle_1Aura2Snapper::~Battle_1Aura2Snapper()
{
}

bool Battle_1Aura2Snapper::start()
{
	if (Battle_Game::start() == false) {
		return false;
	}

	wedge::Battle_Enemy *snapper1 = new Enemy_Snapper();
	snapper1->start();
	snapper1->set_position(util::Point<int>(shim::tile_size*5.5f, shim::tile_size*1.0f));
	entities.push_back(snapper1);

	gold += snapper1->get_gold();

	wedge::Battle_Enemy *snapper2 = new Enemy_Snapper();
	snapper2->start();
	snapper2->set_position(util::Point<int>(shim::tile_size*4.0f, shim::tile_size*-0.25f));
	entities.push_back(snapper2);

	gold += snapper2->get_gold();

	wedge::Battle_Enemy *aura = new Enemy_Aura();
	aura->start();
	aura->set_position(util::Point<int>(shim::tile_size*1.0f, shim::tile_size*1.75f));
	entities.push_back(aura);

	gold += aura->get_gold();

	return true;
}

wedge::Object Battle_1Aura2Snapper::get_found_object()
{
	if (util::rand(1, 10) == 1) {
		return OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_ORANGE, 1);
	}
	else {
		wedge::Object o;
		o.type = wedge::OBJECT_NONE;
		return o;
	}
}

//--

Battle_1Aura1Bone::Battle_1Aura1Bone() :
	Battle_Game("cave3-", 0)
{
}

Battle_1Aura1Bone::~Battle_1Aura1Bone()
{
}

bool Battle_1Aura1Bone::start()
{
	if (Battle_Game::start() == false) {
		return false;
	}

	wedge::Battle_Enemy *bone1;
	if (util::rand(0, 1) == 0) {
		bone1 = new Enemy_Bone1();
	}
	else {
		bone1 = new Enemy_Bone2();
	}
	bone1->start();
	bone1->set_position(util::Point<int>(shim::tile_size*4.75f, shim::tile_size*1.0f));
	entities.push_back(bone1);

	gold += bone1->get_gold();

	wedge::Battle_Enemy *aura = new Enemy_Aura();
	aura->start();
	aura->set_position(util::Point<int>(shim::tile_size*1.0f, shim::tile_size*1.75f));
	entities.push_back(aura);

	gold += aura->get_gold();

	return true;
}

wedge::Object Battle_1Aura1Bone::get_found_object()
{
	if (util::rand(1, 8) == 1) {
		return OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_BANANA, 1);
	}
	else {
		wedge::Object o;
		o.type = wedge::OBJECT_NONE;
		return o;
	}
}

//--

Battle_1CutiePie1Bone::Battle_1CutiePie1Bone() :
	Battle_Game("cave3-", 0)
{
}

Battle_1CutiePie1Bone::~Battle_1CutiePie1Bone()
{
}

bool Battle_1CutiePie1Bone::start()
{
	if (Battle_Game::start() == false) {
		return false;
	}

	wedge::Battle_Enemy *bone1;
	if (util::rand(0, 1) == 0) {
		bone1 = new Enemy_Bone1();
	}
	else {
		bone1 = new Enemy_Bone2();
	}
	bone1->start();
	bone1->set_position(util::Point<int>(shim::tile_size*4.75f, shim::tile_size*1.0f));
	entities.push_back(bone1);

	gold += bone1->get_gold();

	wedge::Battle_Enemy *cutie_pie = new Enemy_CutiePie();
	cutie_pie->start();
	cutie_pie->set_position(util::Point<int>(shim::tile_size*1.25f, shim::tile_size*2.75f));
	entities.push_back(cutie_pie);

	gold += cutie_pie->get_gold();

	return true;
}

wedge::Object Battle_1CutiePie1Bone::get_found_object()
{
	if (util::rand(1, 15) == 1) {
		return OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_CARAMEL, 1);
	}
	else {
		wedge::Object o;
		o.type = wedge::OBJECT_NONE;
		return o;
	}
}

//--

Battle_1Thirsty::Battle_1Thirsty() :
	Battle_Game("cave3water-", 0)
{
	water = true;
}

Battle_1Thirsty::~Battle_1Thirsty()
{
}

bool Battle_1Thirsty::start()
{
	if (Battle_Game::start() == false) {
		return false;
	}

	wedge::Battle_Enemy *thirsty = new Enemy_Thirsty();
	thirsty->start();
	thirsty->set_position(util::Point<int>(shim::tile_size*1.5f, shim::tile_size*1.75f));
	entities.push_back(thirsty);

	gold += thirsty->get_gold();

	return true;
}

wedge::Object Battle_1Thirsty::get_found_object()
{
	if (util::rand(1, 10) == 1) {
		return OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_HOT_PEPPER, 1);
	}
	else {
		wedge::Object o;
		o.type = wedge::OBJECT_NONE;
		return o;
	}
}

//--

Battle_2CutiePie::Battle_2CutiePie() :
	Battle_Game("cave3-", 0)
{
}

Battle_2CutiePie::~Battle_2CutiePie()
{
}

bool Battle_2CutiePie::start()
{
	if (Battle_Game::start() == false) {
		return false;
	}

	wedge::Battle_Enemy *cutie_pie1 = new Enemy_CutiePie();
	cutie_pie1->start();
	cutie_pie1->set_position(util::Point<int>(shim::tile_size*4.25f, shim::tile_size*2.75f));
	entities.push_back(cutie_pie1);

	gold += cutie_pie1->get_gold();

	wedge::Battle_Enemy *cutie_pie2 = new Enemy_CutiePie();
	cutie_pie2->start();
	cutie_pie2->set_position(util::Point<int>(shim::tile_size, shim::tile_size*2.0f));
	entities.push_back(cutie_pie2);

	gold += cutie_pie2->get_gold();

	return true;
}

wedge::Object Battle_2CutiePie::get_found_object()
{
	if (util::rand(1, 8) == 1) {
		return OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_PEACH, 1);
	}
	else {
		wedge::Object o;
		o.type = wedge::OBJECT_NONE;
		return o;
	}
}

//--

Battle_1CutiePie1Clone::Battle_1CutiePie1Clone() :
	Battle_Game("cave3-", 0)
{
}

Battle_1CutiePie1Clone::~Battle_1CutiePie1Clone()
{
}

bool Battle_1CutiePie1Clone::start()
{
	if (Battle_Game::start() == false) {
		return false;
	}

	wedge::Battle_Enemy *cutie_pie1 = new Enemy_CutiePie();
	cutie_pie1->start();
	cutie_pie1->set_position(util::Point<int>(shim::tile_size*3.25f, shim::tile_size*3.0f));
	entities.push_back(cutie_pie1);

	gold += cutie_pie1->get_gold();

	wedge::Battle_Enemy *clone1 = new Enemy_Clone();
	clone1->start();
	clone1->set_position(util::Point<int>(shim::tile_size*1.25f, shim::tile_size*2.0f));
	entities.push_back(clone1);

	gold += clone1->get_gold();

	return true;
}

wedge::Object Battle_1CutiePie1Clone::get_found_object()
{
	if (util::rand(1, 10) == 1) {
		return OBJECT->make_object(wedge::OBJECT_ITEM, ITEM_PEACH, 1);
	}
	else {
		wedge::Object o;
		o.type = wedge::OBJECT_NONE;
		return o;
	}
}

//--

Battle_Bumper::Battle_Bumper() :
	Battle_Game("cave3-", 0)
{
	boss_battle = true;
	boss_music_name = "music/boss.mml";
	
	attack = 15;
	mp = 50;
}

Battle_Bumper::~Battle_Bumper()
{
}

bool Battle_Bumper::start()
{
	if (Battle_Game::start() == false) {
		return false;
	}

	wedge::Battle_Enemy *bumper = new Enemy_Bumper();
	bumper->start();
	bumper->set_position(util::Point<float>(shim::tile_size, 2.0f*shim::tile_size));
	entities.push_back(bumper);

	gold += bumper->get_gold();

	wedge::Battle_Enemy *kiddie1 = new Enemy_Kiddie();
	kiddie1->start();
	kiddie1->set_position(util::Point<float>(3.5f*shim::tile_size, 0.75f*shim::tile_size));
	entities.push_back(kiddie1);

	gold += kiddie1->get_gold();

	wedge::Battle_Enemy *kiddie2 = new Enemy_Kiddie();
	kiddie2->start();
	kiddie2->set_position(util::Point<float>(5.5f*shim::tile_size, 1.25f*shim::tile_size));
	entities.push_back(kiddie2);

	gold += kiddie2->get_gold();

	wedge::Battle_Enemy *kiddie3 = new Enemy_Kiddie();
	kiddie3->start();
	kiddie3->set_position(util::Point<float>(4.5f*shim::tile_size, 2.75f*shim::tile_size));
	entities.push_back(kiddie3);

	gold += kiddie3->get_gold();

	return true;
}

wedge::Object Battle_Bumper::get_found_object()
{
	return OBJECT->make_object(wedge::OBJECT_ARMOUR, BELT_RED, 3);
}

bool Battle_Bumper::run()
{
	auto enemies = get_enemies();

	if (done == false && enemies.size() == 1 && enemies[0]->get_stats()->hp >= 1000000) {
		set_done(true);
		close_guis();
		window_shown = false;
		player_stats_shown = false;
		enemy_stats_shown = false;
		auto v = get_players();
		for (auto p : v) {
			if (p->get_stats()->hp > 0) {
				p->get_sprite()->set_animation("victory");
			}
		}
		if (music_backup == nullptr) {
			music_backup = shim::music;
		}
		else {
			delete shim::music;
		}
		shim::music = new audio::MML("music/victory.mml");
		shim::music->play(shim::music_volume, true);
	}

	return Battle_Game::run();
}

//--

Battle_BigTuna::Battle_BigTuna() :
	Battle_Game("endboss-", 500)
{
	boss_battle = true;
	boss_music_name = "music/boss.mml";
}

Battle_BigTuna::~Battle_BigTuna()
{
}

bool Battle_BigTuna::start()
{
	if (Battle_Game::start() == false) {
		return false;
	}

	wedge::Battle_Enemy *bigtuna = new Enemy_BigTuna();
	bigtuna->start();
	bigtuna->set_position(util::Point<float>(1.5f*shim::tile_size, shim::tile_size));
	entities.push_back(bigtuna);

	gold += bigtuna->get_gold();

	return true;
}

wedge::Object Battle_BigTuna::get_found_object()
{
	wedge::Object o;
	o.type = wedge::OBJECT_NONE;
	return o;
}
