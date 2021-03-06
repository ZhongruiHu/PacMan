
#include "GameEngine.h"
#include <assert.h>


//----------------------------------------------------------------------------------------------
void CEventManager::pushEvent(const sf::Event& event)
{
	for (auto& sub : m_subcribes)
			sub->events(event);
}

void CEventManager::subscribe(CGameObject* object)
{
	m_subcribes.push_back(object);
}

void CEventManager::unsubcribe(CGameObject* object)
{
	auto it = std::find(m_subcribes.begin(), m_subcribes.end(), object);
	if (it != m_subcribes.end())
		m_subcribes.erase(it);

}
//-----------------------------------------------------------------------------------------------
CInputManager::CInputManager()
{
	m_keys_prev_ptr = &m_keys_prev;
	m_keys_now_ptr = &m_keys_now;

}
// Vertical, Horizontal, Left, Right, Fire1, Fire2

void CInputManager::registerKey(const sf::Keyboard::Key& key)
{
	m_keys_prev.insert(std::make_pair(key, false));
	m_keys_now.insert(std::make_pair(key, false));
}
void CInputManager::unregisterKey(const sf::Keyboard::Key& key)
{
	m_keys_prev.erase(m_keys_prev.find(key));
	m_keys_now.erase(m_keys_now.find(key));
}

bool CInputManager::isKeyJustPressed(const sf::Keyboard::Key& key)
{
	if (m_keys_prev_ptr->count(key))
		if (!(*m_keys_prev_ptr)[key] && (*m_keys_now_ptr)[key])
			return true;

	if (sf::Joystick::isConnected(0))
	{
		if (key == sf::Keyboard::Enter && sf::Joystick::isButtonPressed(0, 7))	return true;
	}

	return false;
}
bool CInputManager::isKeyJustReleased(const sf::Keyboard::Key& key)
{
	if (m_keys_prev_ptr->count(key))
		if ((*m_keys_prev_ptr)[key] && !(*m_keys_now_ptr)[key])
			return true;
	return false;
}

bool CInputManager::isKeyPressed(const sf::Keyboard::Key& key)
{
	if (m_keys_prev_ptr->count(key))
		return (*m_keys_now_ptr)[key];

	if (sf::Joystick::isConnected(0))
	{
		if (key == sf::Keyboard::Left && sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::PovX) < -10)	return true;
		if (key == sf::Keyboard::Right && sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::PovX) > 10)	return true;
		if (key == sf::Keyboard::Up && sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::PovY) > 10)	return true;
		if (key == sf::Keyboard::Down && sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::PovY) < -10)	return true;
		if (key == sf::Keyboard::Enter && sf::Joystick::isButtonPressed(0,7))	return true;
	}

	return false;
}

void CInputManager::update(int delta_time)
{
	std::swap(m_keys_now_ptr, m_keys_prev_ptr);
	for (auto& key : *m_keys_now_ptr)
		key.second = sf::Keyboard::isKeyPressed(key.first);
}
//-----------------------------------------------------------------------------------------------
const Vector& CGameObject::getPosition() const
{
	return m_pos;
}
void CGameObject::setPosition(const Vector& point)
{
	m_pos = point;
}
void CGameObject::setPosition(float x, float y)
{
	setPosition(Vector(x, y));
}

void CGameObject::move(const Vector& point)
{
	m_pos += point;
}

void CGameObject::setSize(const Vector& size)
{
	m_size = size;
	setBounds({ getPosition(), m_size });
}

Rect CGameObject::getBounds() const
{
	return Rect(m_pos,m_size);
}

void CGameObject::setBounds(const Rect& rect)
{
	m_pos = rect.leftTop();
	m_size = rect.size();
}


Vector CGameObject::getDirection()
{
	return m_direction;
}
void CGameObject::setDirection(const Vector& direction)
{
	m_direction = direction;
}
CGameObject::CGameObject()
{
	m_enable = m_visible = true;
	m_parent = NULL;
}

void CGameObject::setParent(CGameObject* game_object)
{
	m_parent = game_object;
}
CGameObject* CGameObject::getParent() const
{
	return m_parent;
}


void CGameObject::update(int delta_time)
{
	if (isEnabled())
	{
		for (auto& obj : m_objects)
				if (!obj->m_started)
				{
					obj->m_started = true;
					obj->start();
			    }
				
		for (auto& obj : m_objects)
			if (obj->isEnabled())
				obj->update(delta_time);			 
	}
}

void CGameObject::start()
{

}

void CGameObject::setName(const std::string& name)
{
	m_name = name;
}
const std::string&  CGameObject::getName() const
{
	return m_name;
} 

void CGameObject::disable()
{
	m_enable = false;
}
void CGameObject::enable()
{
	m_enable = true;
}
bool CGameObject::isEnabled() const
{
	return m_enable;
}

void CGameObject::hide()
{
	m_visible = false;
}
void CGameObject::show()
{
	m_visible = true;
}
bool CGameObject::isVisible() const
{
	return m_visible;
}

void CGameObject::turnOn()
{
	show();
	enable();
}
void CGameObject::turnOff()
{
	hide();
	disable();
}


CGameObject* CGameObject::addObject(CGameObject* object)
{
	m_objects.push_back(object);
	object->setParent(this);
	if (m_started)
	{
		object->m_started = true;
		object->start();
	}
	return object;
}

CGameObject* CGameObject::findObjectByName(const std::string& name)
{
	auto it = std::find_if(m_objects.begin(), m_objects.end(), [this, &name](const CGameObject* obj) -> bool { return obj->getName() == name;  });
	if (it != m_objects.end())
		return *it;

 
	return nullptr;
}


CGameObject::~CGameObject()
{
	for (auto& obj : m_objects)
		delete obj;
	m_objects.clear();
}

void CGameObject::draw(sf::RenderWindow* window)
{
	if (isVisible())
		for (auto& obj : m_objects)
			if (obj->isVisible())
				obj->draw(window);
}

void CGameObject::postDraw(sf::RenderWindow* window)
{
	if (isVisible())
		for (auto& obj : m_objects)
			if (obj->isVisible())
				obj->postDraw(window);
}

void CGameObject::foreachObject(std::function<void(CGameObject*)> predicate)
{
	for (auto& obj : m_objects)
		predicate(obj);
}
void CGameObject::foreachObject(std::function<void(CGameObject*, bool& )> predicate)
{
	bool need_break = false;
	for (auto& obj : m_objects)
	{
		predicate(obj, need_break);
		if (need_break)
			break;
	}
}



void CGameObject::removeObject(CGameObject* object)
{
 
	  auto action = [this, object]()
	  {
		  auto it = std::find(m_objects.begin(), m_objects.end(), object);
		  assert(it != m_objects.end());
		  m_objects.erase(it);
		  delete object;
	  };
	  m_preupdate_actions.push_back(action);
}

void CGameObject::moveToBack()
{
	if (getParent())
	{
		auto move_to_back_action = [this]()
		{
			auto list = &(getParent()->m_objects);
			auto it = std::find(list->begin(), list->end(), this);
			assert(*it == this);
			auto tmp = *it;
			it = list->erase(it);
			list->push_front(tmp);
		};
		m_preupdate_actions.push_back(move_to_back_action);
	}
}
void CGameObject::moveToFront()
{
	if (getParent())
	{
		auto move_to_front_action = [this]()
		{
			auto list = &(getParent()->m_objects);
			auto it = std::find(list->begin(), list->end(), this);
			assert(*it == this);
			auto tmp = *it;
			it = list->erase(it);
			list->push_back(tmp);
		};
		
		m_preupdate_actions.push_back(move_to_front_action);
	}
}

void CGameObject::moveUnderTo(CGameObject* obj)
{
	if (getParent())
	{
		auto move_under_action = [this,obj]()
		{
			auto list = &(getParent()->m_objects);
			auto this_obj = std::find(list->begin(), list->end(), this);
			auto other_obj = std::find(list->begin(), list->end(), obj);
			assert(this_obj != list->end() && other_obj != list->end());
   
			list->erase(this_obj);
			list->insert(other_obj,this);
		};

		m_preupdate_actions.push_back(move_under_action);
	}
}

void CGameObject::clear()
{
	for (auto object : m_objects)
		delete object;
	m_objects.clear();
}


void destroyObject(CGameObject* gameObject)
{
	if (gameObject->getParent())
		gameObject->getParent()->removeObject(gameObject);
	else
		delete gameObject;
}


std::vector<std::function<void()>> CGameObject::m_preupdate_actions = std::vector<std::function<void()>>();

void CGameObject::invokePreupdateActions()
{
	for (auto& action : m_preupdate_actions)
		action();
	m_preupdate_actions.clear();
}
//---------------------------------------------------------------------------------------------------------
void CGame::init()
{

}
 
CGame::~CGame()
	{

	}
CGame::CGame(const std::string& name, const Vector& screen_size)
	{
		m_root_object = new CGameObject();
		m_root_object->setName(name);
		m_screen_size = screen_size;
	}

	void CGame::setClearColor(const sf::Color& color)
	{
		m_clear_color = color;
	}

	void CGame::run()
	{
		m_window = new sf::RenderWindow(sf::VideoMode(m_screen_size.x, m_screen_size.y), m_root_object->getName());
		init();

		sf::Event event;
		sf::Clock clock;
		sf::Time accumulator = sf::Time::Zero;
		sf::Time ups = sf::seconds(1.f / 60.f);
		m_window->setFramerateLimit(120);

		while (true)   // game loop
		{
			while (m_window->pollEvent(event))
			{
				if (event.type == sf::Event::EventType::Closed)
				{
					m_window->close();
					exit(0);
				}

				if (event.type == sf::Event::Resized)
					m_window->setView(sf::View(sf::FloatRect(0, 0, event.size.width, event.size.height)));

				eventManager().pushEvent(event);
			}

			while (accumulator > ups)
			{
				accumulator -= ups;

				sf::sleep(sf::milliseconds(5));
				inputManager().update(ups.asMilliseconds());
				update(ups.asMilliseconds());
			}

			m_window->clear(m_clear_color);
			draw(m_window);
			m_window->display();
			accumulator += clock.restart();
		}
	}

	CGameObject*  CGame::getRootObject()
	{
		return m_root_object;
	}
	void  CGame::draw(sf::RenderWindow* render_window)
	{
		m_root_object->draw(render_window);
		m_root_object->postDraw(render_window);
	}

	void CGame::update(int delta_time)
	{
		CGameObject::invokePreupdateActions(); //remove obj, change z-oreder, etc
		m_root_object->update(delta_time);
	}
	CTextureManager&  CGame::textureManager()
	{
		return m_texture_manager;
	}

	CFontManager&  CGame::fontManager()
	{
		return m_font_manager;
	}

	CSoundManager&  CGame::soundManager()
	{
		return m_sound_manager;
	}

	CEventManager&  CGame::eventManager()
	{
		return m_event_manager;
	}

	CInputManager&  CGame::inputManager()
	{
		return m_input_manager;
	}

	void  CGame::playSound(const std::string& name)
	{
		int i = 0;
		while (m_sounds_buf[i].getStatus() == sf::Sound::Playing)
		{
			if (i >= 16)
				break;
			++i;
		}

		assert(i < 16); //sound buffer overflow

		m_sounds_buf[i].setBuffer(*soundManager().get(name));
		m_sounds_buf[i].play();
	}

	Vector  CGame::screenSize() const
	{
		return Vector((int)m_window->getSize().x, (int)m_window->getSize().y);
	}
 
//---------------------------------------------------------------------------------------------------------
CTimer::CTimer()
{
	setName("Timer");
}

void CTimer::update(int miliseconds)
{
	if (!isEnabled())
		return;

	for (auto it = m_call_back_list.begin(); it != m_call_back_list.end(); )
	{
		auto& pair = *it;
		pair.first -= sf::milliseconds(miliseconds);

		if (pair.first <= sf::Time() || !pair.second)
		{
			if (pair.second)
			 pair.second.operator()();
			it = m_call_back_list.erase(it);
		}
		else
			++it;
	}
}



void CTimer::clear()
{
	for (auto it = m_call_back_list.begin(); it != m_call_back_list.end(); ++it)
		(*it).second = NULL;
}


CTimer::~CTimer()
{
	clear();
}


//---------------------------------------------------------------------------------------------------------


CSpriteSheet::CSpriteSheet()
{
	setName("SpriteSheet");
	m_speed = 0.03f;
	m_current_sprite = NULL;
	setAnimType(AnimType::manual);
	m_index = 0;
}



void CSpriteSheet::load(const sf::Texture& texture, const std::vector<sf::IntRect>& rects)
{
	m_sprites.clear();
	for (auto& rect : rects)
		m_sprites.emplace_back(texture, rect);

	setSpriteIndex(0);
}

void CSpriteSheet::load(const sf::Texture& texture, const Vector& off_set, const Vector& size, int cols, int rows)
{
	m_sprites.clear();
	  for (int y = 0; y < rows; ++y)
		  for (int x = 0; x < cols; ++x)
		m_sprites.emplace_back(texture, sf::IntRect(x*abs(size.x) + off_set.x, y*abs(size.y) + off_set.y,size.x,size.y));
	
	setSpriteIndex(0);
	setAnimType(AnimType::forward);
}


void CSpriteSheet::reset()
{
	m_index = 0;
}

bool CSpriteSheet::empty() const
{
	return m_sprites.empty();
}

void CSpriteSheet::setSpriteIndex(int index)
{
	assert(index >= 0 && index < m_sprites.size());
	m_current_sprite = &m_sprites[index];
}
sf::Sprite* CSpriteSheet::currentSprite()
{
	return m_current_sprite;
}

void  CSpriteSheet::setAnimType(AnimType type)
{
	m_anim_type = type;
//	if (type == AnimType::forward)
		m_index = 0;
}

void CSpriteSheet::draw(sf::RenderWindow* wnd)
{
 
	switch (m_anim_type)
	{
	case(AnimType::manual):
	{
		break;
	}
	case(AnimType::forward_backward_cycle):
	{
		int size = m_sprites.size();

		int current_slide = int(m_index) % (size * 2);
		                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     
		if (current_slide > size - 1)
			current_slide = 2 * size - 1 - current_slide;
		setSpriteIndex(current_slide);
		break;
	}
	case(AnimType::forward_cycle):
	{
		int current_slide = int(m_index) % m_sprites.size();
		setSpriteIndex(current_slide);
		break;
	}
	case(AnimType::forward_stop):
	{
		int current_slide = int(m_index);
		if (current_slide < m_sprites.size())
			setSpriteIndex(current_slide);
		break;
	}
	case(AnimType::forward):
	{
		int current_slide = int(m_index);
		if (current_slide < m_sprites.size())
			setSpriteIndex(current_slide);
		else
			return;
		break;
	}

	}


	wnd->draw(*m_current_sprite);
}

void CSpriteSheet::setPosition(sf::Vector2f pos)
{
	m_position = pos;
	for (auto& sprite : m_sprites)
		sprite.setPosition(m_position + m_torigin);
}

void CSpriteSheet::setOrigin(const Vector& pos)
{
	for (auto& sprite : m_sprites)
		sprite.setOrigin(pos);
}

sf::Sprite&  CSpriteSheet::operator[](int index)
{
	assert(index >= 0 && index < m_sprites.size());
	return m_sprites[index];
}

sf::Vector2f CSpriteSheet::getPosition() const
{
	return m_position;
}

void CSpriteSheet::scale(float fX, float fY)
{
	for (auto& sprite : m_sprites)
		sprite.scale(fX, fY);
}

void CSpriteSheet::update(int delta_time)
{
	if (isEnabled() && m_speed)
	  m_index +=  m_speed*delta_time ;
}

void CSpriteSheet::setSpeed(float speed)
{
	m_speed = speed;
}
void CSpriteSheet::setColor(const sf::Color& color)
{
	for (auto& sprite : m_sprites)
		sprite.setColor(color);
}
void CSpriteSheet::setRotation(float angle)
{
	if (angle == m_sprites[0].getRotation())
		return;
	


	for (auto& sprite : m_sprites)
	{

		float w = sprite.getLocalBounds().width;
		float h = sprite.getLocalBounds().height;

		if (angle < 0) angle += 360;
		if (angle > 360) angle -= 360;

		if (angle == 0)
			sprite.setOrigin(0, 0);
		else if (angle == 90)
			sprite.setOrigin(0, h);
		else if (angle == 270)
			sprite.setOrigin(w, 0);
		else if (angle == 180)
			sprite.setOrigin(w, h);
		
		sprite.setRotation(angle);
	}

}


void CSpriteSheet::invert_h()
{
	//const auto&  rect = sprite.getTextureRect();
	//sf::IntRect new_rect(rect.width + rect.left, rect.top, -rect.width, rect.height);
	//return sf::Sprite(*texture, new_rect);
}

void CSpriteSheet::flipX(bool value)
{
	if (m_flipped != value)
	{
		m_flipped = value;

		for (auto& sprite : m_sprites)
		{
			auto rect = sprite.getTextureRect();
			rect.left += rect.width;
			rect.width = -rect.width;
			sprite.setTextureRect(rect);
		}
	}
}

void CSpriteSheet::setAnimOffset(float i)
{
	m_index += i;
}

AnimType CSpriteSheet::animType() const
{
	return m_anim_type;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------
	 Animator::~Animator()
	 {
		 for (auto anim : m_animations)
	     {
			 delete anim.second;
    	 }
	 }

void Animator::create(const std::string& name, const sf::Texture& texture, const Vector& off_set, const Vector& size, int cols, int rows, float speed, AnimType anim_type)
{
	CSpriteSheet* animation = new CSpriteSheet();
	animation->load(texture, off_set, size, cols, rows);
	animation->setAnimType(anim_type);
	animation->setSpeed(speed);
	assert(!m_animations[name]); // already exist
	m_animations[name] = animation;
	if (!m_current_animation) m_current_animation = animation;
}

void Animator::create(const std::string& name, const sf::Texture& texture, const Rect& rect)
{
	CSpriteSheet* animation = new CSpriteSheet();
	animation->load(texture, { { (int)rect.left(), (int)rect.top(), (int)rect.width(), (int)rect.height() } });
	assert(!m_animations[name]); // already exist
	m_animations[name] = animation;
	if (!m_current_animation) m_current_animation = animation;
}


void Animator::create(const std::string& name, const sf::Texture& texture, const std::vector<sf::IntRect>& rects, float _speed)
{
	CSpriteSheet* animation = new CSpriteSheet();

	animation->load(texture, rects);
	animation->setAnimType(AnimType::forward_cycle);
	animation->setSpeed(_speed);
	assert(!m_animations[name]); // already exist
	m_animations[name] = animation;
	if (!m_current_animation) m_current_animation = animation;
}


void Animator::play(const std::string& name)
{
	if (last_anim_name != name)
	{
		m_current_animation = m_animations[name];
		assert(m_current_animation); //not exist
		last_anim_name = name;
	
	//	if (m_current_animation->animType() == AnimType::forward_stop)
			m_current_animation->reset();
	}
}
void Animator::update(int delta_time)
{
	if (isEnabled())
	 m_current_animation->update(delta_time);
}
void Animator::draw(sf::RenderWindow* wnd)
{
	if (isVisible())
	{
 		m_current_animation->setPosition(getPosition());
		m_current_animation->draw(wnd);	 
	}
}

void Animator::flipX(bool value)
{
	if (value != m_flipped)
	{
		m_flipped = value;
		for (auto& animation : m_animations)
			animation.second->flipX(value);
	}
}

void Animator::setColor(const sf::Color& color)
{
	for (auto& animation : m_animations)
		animation.second->setColor(color);
}

void Animator::setAnimOffset(float index)
{
	for (auto& animation : m_animations)
		animation.second->setAnimOffset(index);
}

void Animator::setSpeed(const std::string& anim, float speed)
{
	m_animations[anim]->setSpeed(speed);
}

void Animator::setSpriteOffset(const std::string& anim_name, int sprite_index, const Vector& value)
{
	CSpriteSheet* sheet = m_animations[anim_name];
	assert(sheet);
	sheet->operator[](sprite_index).setOrigin(-value);
}


void Animator::scale(float fX, float fY)
{
	for (auto& animation : m_animations)
		animation.second->scale(fX, fY);
}

CSpriteSheet* Animator::get(const std::string& str)
{
	return m_animations[str];
}

//---------------------------------------------------------------------------------------------------------
CFlowText::CFlowText(const sf::Font& font, bool self_remove)
{
	m_text.setFont(font);
	m_text.setFillColor(sf::Color::Black);
	m_text.setCharacterSize(20);
	m_text.setStyle(sf::Text::Bold);
	m_flashing = false;
	m_self_remove = self_remove;
}

CFlowText* CFlowText::clone() const
{
	auto flow_text = new CFlowText(*m_text.getFont());
	flow_text->m_text =  m_text;
	flow_text->m_splash_vector = m_splash_vector;
	return flow_text;
}

bool CFlowText::isFlashing() const
{
	return m_flashing;
}
void CFlowText::splash(const Vector& pos, const std::string& text)
{
	m_flashing = true;
	setPosition(pos);
	m_text.setString(text);
	m_offset.x = m_offset.y = 0;
	m_time = 0;
}

void CFlowText::setSplashVector(const Vector& vector)
{
	m_splash_vector = vector;
}
void CFlowText::update(int delta_time)
{
	if (m_flashing)
	{
		m_time += delta_time;
		m_offset.x = m_time*0.03f*m_splash_vector.x;
		m_offset.y = m_time*0.03f*m_splash_vector.y;

		m_offset.x *= 2;
		m_color = m_time*0.2f;
		if (m_color >= 255)
			m_flashing = false;

		const sf::Color& color = m_text.getFillColor();

		m_text.setFillColor(sf::Color(color.r, color.g, color.b, 255 - m_color));
	}
	else if (m_self_remove)
		destroyObject(this);
}
void CFlowText::draw(sf::RenderWindow* window)
{
	if (m_flashing)
	{
		m_text.setPosition(getPosition() + m_offset);
		window->draw(m_text);
	}
}

void CFlowText::setTextColor(const sf::Color& color)
{
	m_text.setFillColor(color);
}
void CFlowText::setTextSize(int size)
{
	m_text.setCharacterSize(size);
}

//---------------------------------------------------------------------------------------------------------
CLabel::CLabel()
{
	init();
}
CLabel::CLabel(const std::string& str)
{
	init();
	setString(str);
}
void CLabel::init()
{
	setName("Label");
	setFontColor(sf::Color::Black);
}
void CLabel::setBounds(int x, int y, int w, int h)
{
	setPosition(x, y);
	m_rect = Rect(x, y, w, h);

	m_shape.setPosition(x, y);
	m_shape.setSize(sf::Vector2f(w, h));


	if (m_sprite.getTexture())
	{
		m_sprite.setPosition(sf::Vector2f(x, y));
		m_sprite.setScale((float)w / m_sprite.getTextureRect().width, (float)h / m_sprite.getTextureRect().height);
	}
}
void CLabel::setSprite(const sf::Sprite& sprite)
{
	m_sprite = sprite;
}
void CLabel::setFontColor(const sf::Color& color)
{
	m_text.setFillColor(color);
}
void CLabel::setFontSize(int size)
{
	m_text.setCharacterSize(size);
}
void CLabel::setFontName(const sf::Font& font)
{
	m_text.setFont(font);
}
void CLabel::setFontStyle(sf::Uint32 style)
{
	m_text.setStyle(style);
}
void CLabel::setTextAlign(int value)
{
	m_text_align = value;
}
void CLabel::setString(const std::string& str)
{
	m_text.setString(str);
}
void CLabel::setOutlineColor(const sf::Color& color)
{
	m_shape.setOutlineColor(color);
}
void CLabel::setFillColor(const sf::Color& color)
{
	m_shape.setFillColor(color);
}
void CLabel::setOutlineThickness(int value)
{
	m_shape.setOutlineThickness(value);
}
bool CLabel::contains(const Vector& point) const
{
	return m_rect.isContain(point);
}
void CLabel::draw(sf::RenderWindow* window)
{
	window->draw(m_shape);

	if (m_sprite.getTexture())
	{
		m_sprite.setPosition(getPosition());
		window->draw(m_sprite);
	}

	if (!m_text.getString().isEmpty())
	{
		if (m_text_align == center)
			m_text.setPosition(getPosition() + m_rect.size() / 2 - Vector(m_text.getGlobalBounds().width, m_text.getGlobalBounds().height) / 2);
		else if (m_text_align == left)
			m_text.setPosition(getPosition().x, getPosition().y);


		window->draw(m_text);
	}
}
Rect CLabel::getBounds() const
{
	return m_rect;
}

CLabel* CLabel::clone() const
{
	CLabel* new_label = new CLabel();
	
	new_label->m_text = m_text;
    new_label->m_sprite = m_sprite;
	new_label->m_rect = m_rect;
	new_label->m_shape = m_shape;
	new_label->m_text_align = m_text_align;
 
	Rect bounds = getBounds();
	new_label->setBounds(bounds.left(), bounds.top(), bounds.width(), bounds.height());

	return new_label;
}
//-------------------------------------------------------------------------------------------------------
CGameObject* WaypointSystem::getObject()
{
	setName("WaypointSystem");
	return getParent();
}
void WaypointSystem::addPath(const std::vector<Vector>& path, float speed, bool align)
{
	m_path.clear();
	m_path = path;
	m_speed = speed;
	m_length = 0;
	if (align)
		m_path.insert(m_path.begin(), getObject()->getPosition());
}
bool WaypointSystem::isMoving() const
{
	return !m_path.empty();
}
void WaypointSystem::stop()
{
	m_path.clear();
	m_length = 0;
}
void WaypointSystem::update(int delta_time)
{
	if (!m_path.empty())
	{
		float max_length = (m_path[1] - m_path[0]).length(); // can be optimized
		m_length += delta_time*m_speed;
		Vector current_pos = Vector::moveTowards(m_path[0], m_path[1], m_length);
		getObject()->setDirection((m_path[1] - m_path[0]).normalized());
		getObject()->setPosition(current_pos);
		if (m_length > max_length)
		{
			m_length = 0;
			m_path.erase(m_path.begin());
			if (m_path.size() == 1)
				m_path.clear();
		}
	}
}
