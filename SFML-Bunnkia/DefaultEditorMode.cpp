#include "stdafx.h"
#include "DefaultEditorMode.h"

void DefaultEditorMode::initVariables()
{
	this->textureRect = sf::IntRect(0, 0, static_cast<int>(this->stateData->gridSize), static_cast<int>(this->stateData->gridSize));
	this->collision = false;
	this->type = TileTypes::DEFAULT;
	this->layer = 0;
	this->tileAddLock = true;
}

void DefaultEditorMode::initGui()
{
	// Cursor Text
	this->cursorText.setFont(*this->editorStateData->font);
	this->cursorText.setFillColor(sf::Color::White);
	this->cursorText.setOutlineColor(sf::Color::Black);
	this->cursorText.setOutlineThickness(1);
	this->cursorText.setCharacterSize(10);
	this->cursorText.setPosition(this->editorStateData->mousePosView->x, this->editorStateData->mousePosView->y);

	// Texture Selector Background
	this->sidebar.setSize(sf::Vector2f(64.f, static_cast<float>(this->stateData->gfxSettings->resolution.height)));
	this->sidebar.setFillColor(sf::Color(50, 50, 50, 100));
	this->sidebar.setOutlineColor(sf::Color(200, 200, 200, 150));
	this->sidebar.setOutlineThickness(1.f);

	// Selected Rect
	this->selectorRect.setSize(sf::Vector2f(this->stateData->gridSize, this->stateData->gridSize));
	this->selectorRect.setFillColor(sf::Color(255, 255, 255, 150));
	this->selectorRect.setOutlineThickness(1.f);
	this->selectorRect.setOutlineColor(sf::Color::Green);
	this->selectorRect.setTexture(this->tileMap->getTileSheet());
	this->selectorRect.setTextureRect(this->textureRect);

	// Texture Selector
	this->textureSelector = new gui::TextureSelector(
		65.f, 0.f, 1024.f, static_cast<float>(this->stateData->gfxSettings->resolution.height),
		this->stateData->gridSize, this->tileMap->getTileSheet(),
		*this->editorStateData->font, "T"
	);
}

DefaultEditorMode::DefaultEditorMode(StateData* state_data, TileMap* tile_map, EditorStateData* editor_state_data)
	: EditorMode(state_data, tile_map, editor_state_data)
{
	this->initVariables();
	this->initGui();
}

DefaultEditorMode::~DefaultEditorMode()
{
	delete this->textureSelector;
}

//Function
void DefaultEditorMode::updateInput(const float& dt)
{
	//Add a tile to the tilemap
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left)	&& this->getKeytime())
	{
		if (!this->sidebar.getGlobalBounds().contains(sf::Vector2f(*this->editorStateData->mousePosWindow)))
		{
			if (!this->textureSelector->getActive())
			{
				if (this->tileAddLock)
				{
					if (this->tileMap->tileEmpty(this->editorStateData->mousePosGrid->x, this->editorStateData->mousePosGrid->y, 0))
					{
						this->tileMap->addTile(this->editorStateData->mousePosGrid->x, this->editorStateData->mousePosGrid->y, 0, this->textureRect, this->collision, this->type);
					}
				}
				else
				{
					this->tileMap->addTile(this->editorStateData->mousePosGrid->x, this->editorStateData->mousePosGrid->y, 0, this->textureRect, this->collision, this->type);
				}

			}
			else
			{
				this->textureRect = this->textureSelector->getTextureRect();
			}
		}

	}
	//Remove a tile from the tilemap
	else if (sf::Mouse::isButtonPressed(sf::Mouse::Right) && this->getKeytime())
	{
		if (!this->sidebar.getGlobalBounds().contains(sf::Vector2f(*this->editorStateData->mousePosWindow)))
		{
			if (!this->textureSelector->getActive())
				this->tileMap->removeTile(this->editorStateData->mousePosGrid->x, this->editorStateData->mousePosGrid->y, 0);
		}

	}


	//Toggle collision
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key(this->editorStateData->keybinds->at("TOGGLE_COLLISION"))) && this->getKeytime())
	{
		if (this->collision)
			this->collision = false;
		else
			this->collision = true;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key(this->editorStateData->keybinds->at("INCREASE_TYPE"))) && this->getKeytime())
	{
		//CHANGE TO LIMIT TO MAX TYPE!
		++this->type;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key(this->editorStateData->keybinds->at("DECREASE_TYPE"))) && this->getKeytime())
	{
		if (this->type > 0)
			--this->type;
	}

	//Set lock on/off
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key(this->editorStateData->keybinds->at("TOGGLE_TILE_LOCK"))) && this->getKeytime())
	{
		if (this->tileAddLock)
			this->tileAddLock = false;
		else
			this->tileAddLock = true;
	}
}

void DefaultEditorMode::updateGui(const float& dt)
{
	this->textureSelector->update(*this->editorStateData->mousePosWindow, dt);

	if (!this->textureSelector->getActive())
	{
		this->selectorRect.setTextureRect(this->textureRect);
		this->selectorRect.setPosition(this->editorStateData->mousePosGrid->x * this->stateData->gridSize, this->editorStateData->mousePosGrid->y * this->stateData->gridSize);
	}

	this->cursorText.setPosition(this->editorStateData->mousePosView->x + 50.f, this->editorStateData->mousePosView->y);

	std::stringstream ss;
	ss << this->editorStateData->mousePosView->x << " " << this->editorStateData->mousePosView->y <<
		"\n" << this->editorStateData->mousePosGrid->x << " " << this->editorStateData->mousePosGrid->y <<
		"\n" << this->textureRect.left << " " << this->textureRect.top <<
		"\n" << "Tile Mode" <<
		"\n" << "Collision: " << this->collision <<
		"\n" << "Type: " << this->type <<
		"\n" << "Tiles: " << this->tileMap->getLayerSize(this->editorStateData->mousePosGrid->x, this->editorStateData->mousePosGrid->y, this->layer) <<
		"\n" << "Tile lock: " << this->tileAddLock;

	this->cursorText.setString(ss.str());
}

void DefaultEditorMode::update(const float& dt)
{
	this->updateInput(dt);
	this->updateGui(dt);
}

void DefaultEditorMode::renderGui(sf::RenderTarget& target)
{
	if (!this->textureSelector->getActive())
	{
		target.setView(*this->editorStateData->view);
		target.draw(this->selectorRect);
	}

	target.setView(this->stateData->window->getDefaultView());
	this->textureSelector->render(target);
	target.draw(this->sidebar);

	//DEBUG MOUSE POS
	target.setView(*this->editorStateData->view);
	target.draw(this->cursorText);
}

void DefaultEditorMode::render(sf::RenderTarget& target)
{
	this->renderGui(target);
}
