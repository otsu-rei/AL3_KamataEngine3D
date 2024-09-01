#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// c++
#include <cstdint>
#include <memory>
#include <list>

// engine
#include <Sprite.h>
#include <WinApp.h>
#include <WorldTransform.h>

// Geometry
#include <Vector2.h>

// Adapter
#include <Adapter/Random.h>

#include <Easing.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Background class
////////////////////////////////////////////////////////////////////////////////////////////
class Background {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init();

	void Term();

	void Update();

	void Draw();


private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// ParticleElement structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct ParticleElement {

		//* bind members *//

		Vector4f color;
		Vector2f position;
		Vector2f size;

		//* data *//

		float currentTime = 0.0f;
		bool isDelete     = false;

		//* parameter *//

		const float lifeTime = 60.0f/*_1frame*/ * Random::Generate<float>(1.0f, 2.0f)/*_sec*/; 

		//* IA *//

		std::unique_ptr<Sprite> sprite;

		//* methods *//

		void Create(uint32_t textureHandle) {

			float randomColor = Random::Generate<float>(0.5f, 0.7f);
			color = {randomColor, randomColor, randomColor, 1.0f};
			position = {
				Random::Generate<float>(0.0f, WinApp::kWindowWidth),
				Random::Generate<float>(0.0f, WinApp::kWindowHeight)
			};
			size     = {8.0f, 8.0f};

			sprite.reset(Sprite::Create(textureHandle, position, color));
			sprite->SetSize(size);
			sprite->SetAnchorPoint({0.5f, 0.5f});
		}

		void Update(Vector2f velocity) {

			if (++currentTime > lifeTime) {
				isDelete = true;
				return;
			}

			float t = currentTime / lifeTime;

			position += velocity;

			color.w = std::lerp(1.0f, 0.0f, t);

			size.x = (1.0f - t) * 8.0f;
			size.y = (1.0f - t) * 8.0f;

			BindParameter();
		}

		void Draw() const { sprite->Draw();}

	private:

		void BindParameter() {
			sprite->SetColor(color);
			sprite->SetPosition(position);
			sprite->SetSize(size);
		}

	};

	//=========================================================================================
	// private variables
	//=========================================================================================

	uint32_t white1x1TextureHandle_;
	std::unique_ptr<Sprite> backSprite_;

	//* emitter *//

	std::list<ParticleElement> particles_;

	float currentTime_ = 0.0f;
	const float emitTime_    = 10.0f/*_frame*/; 

	float velocityX_ = 0.0f;

};