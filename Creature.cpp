#include "commons.h"
#include "Creature.h"

using namespace game_utils;
using namespace std;
using namespace loaders;
using namespace cml;

namespace game_objects
{
	CCreature::CCreature(): CEntity()
	{
		owner = CV_PLAYER_UNDEFINED;
		currentAction = AA_WALK;
		model = NULL;
		name = "";
		moveSpeed = 1.0f;
		moveVector = vector3f(0.0f,0.0f,0.0f);
		level = 1;
		gold = 0;
		count = 0.0f;
		change = 100.0f;
	}

	CCreature::~CCreature()
	{
		if (model)
		{
			CV_GAME_MANAGER->getResourceManager()->returnModel(name,model);
		}
	}

	GLvoid CCreature::setName(string name)
	{
		this->name = name;
	}

	GLvoid CCreature::setOwner(GLubyte owner)
	{
		this->owner=owner;
	}

	GLvoid CCreature::setLevel(GLint level)
	{
		if (this->getLevel() < 10) // cant level if already level 10
		{
			this->level = level;
			CV_GAME_MANAGER->getConsole()->writeLine(this->getName()+" has leveled up to "+CConversions::intToStr(this->getLevel()));
		}
	}

	GLvoid CCreature::setGold(GLint gold)
	{
		this->gold=gold;
	}

	GLvoid CCreature::addCurrentXP(GLint CurrentXP)
	{
		// add the experiance
		this->CurrentXP = (this->getCurrentXP() + CurrentXP);

		// if the creature has enough experiance to level
		if (this->getCurrentXP() >= this->getLevel()*100) //some forumla
		{
			this->CurrentXP = this->getCurrentXP() - (this->getLevel()*100); // reset xp and add remaining xp from level
			this->setLevel(this->getLevel() + 1);// level up
		}
	}

	GLvoid CCreature::setModel(loaders::CBR5Model *model)
	{
		this->model = model;
		this->model->setInterpolate(true);
		this->model->setConnected(true);
	}

	GLvoid CCreature::setAction(GLint action, GLint startFrame, GLint endFrame)
	{
		actions[action] = model->registerAction(startFrame,endFrame);
	}

	GLvoid CCreature::useAction(GLint action)
	{
		this->currentAction = action;

		if (model)
		{
			model->doAction(actions[action]);
		}
	}

	GLvoid CCreature::Idle(GLfloat deltaTime)
	{
		if (count>=change)
		{
			GLfloat tX = (GLfloat)(rand()%100-50);
			GLfloat tZ = (GLfloat)(rand()%100-50);
			moveVector[0] = tX-position[0];
			moveVector[2] = tZ-position[2];
			moveVector.normalize();
			rotation[1] = 90.0f-(float)(atan2(moveVector[2],moveVector[0])*180.0f/M_PI);
			change=(GLfloat)((rand()%100))+300.0f;
			count = 0.0f;
		}

		GLint X = (GLint)(position[0]/CV_BLOCK_WIDTH);
		GLint Y = (GLint)(position[2]/CV_BLOCK_DEPTH);
		if(CV_GAME_MANAGER->getLevelManager()->getBlock(X,Y)->isWalkable(false))
		{
			position += moveVector*moveSpeed*deltaTime;
			count+=0.5f;
		}
		else
		{
			position -= moveVector*moveSpeed*deltaTime;
			count=change;
		}

	}

	GLvoid CCreature::draw(GLfloat deltaTime)
	{
		if (model)
		{
			CEntity::moveTo();
			CEntity::rotateTo();
			model->draw(deltaTime);
			CEntity::rotateBack();
			CEntity::moveBack();
		}
	}

	CBR5Model *CCreature::getModel()
	{
		return model;
	}

	string CCreature::getName()
	{
		return name;
	}

	GLubyte CCreature::getOwner()
	{
		return owner;
	}

	GLint CCreature::getLevel()
	{
		return level;
	}

	GLint CCreature::getCurrentXP()
	{
		return CurrentXP;
	}

	GLint CCreature::getGold()
	{
		return gold;
	}

	GLvoid CCreature::update(GLfloat deltaTime)
	{
	}
};