#include "LoseScene.h"
#include <TextureManager.h>
#include <Input.h>
#include "TitleScene/TitleScene.h"
#include "GameScene/GameScene.h"
#include "GameManager.h"



void LoseScene::Initialize(){

	//スプライトの初期座標
	const Vector2 INITIAL_SPRITE_POSITION_ = { .x = 0.0f,.y = 0.0f };

	//メイン
	uint32_t failedTextureHandle = TextureManager::LoadTexture("Resources/Sprite/Result/Lose/EscapeFailed.png");
	failedTexture_.reset(Sprite::Create(failedTextureHandle, INITIAL_SPRITE_POSITION_));


	//Text
	uint32_t textHandle = TextureManager::GetInstance()->LoadTexture("Resources/Sprite/Result/Lose/LoseText.png");
	text_.reset(Sprite::Create(textHandle, INITIAL_SPRITE_POSITION_));

	//黒背景
	uint32_t blackTextureHandle = TextureManager::GetInstance()->LoadTexture("Resources/Sprite/Back/Black.png");
	black_.reset(Sprite::Create(blackTextureHandle, INITIAL_SPRITE_POSITION_));
	//透明度の設定
	transparency_ = 0.0f;
	black_->SetTransparency(transparency_);
	blackOutTime_ = 0;
	
	

	//点滅
	isFlash_ = true;

	//タイトルに戻る
	isReturnTitle = false;
}

void LoseScene::Update(GameManager* gameManager){

	//増える時間の値
	const uint32_t INCREASE_VALUE = 1u;
	//Bトリガーの反応する時間
	const uint32_t REACT_TIME = 1u;
	//Bトリガーの反応しない時間
	const uint32_t NO_REACT_TIME = 0u;

	//再スタート時間
	const uint32_t RESTART_TIME = 0u;


	//通常の点滅
	flashTime_ += INCREASE_VALUE;
	if (flashTime_ > FLASH_TIME_LIMIT_ * 0 &&
		flashTime_ <= FLASH_TIME_LIMIT_) {
		text_->SetInvisible(false);
	}
	if (flashTime_ > FLASH_TIME_LIMIT_ &&
		flashTime_ <= FLASH_TIME_LIMIT_ * 2) {
		text_->SetInvisible(true);

	}
	if (flashTime_ > FLASH_TIME_LIMIT_ * 2) {
		flashTime_ = RESTART_TIME;
	}

	//コントローラー接続時
	if (Input::GetInstance()->IsConnetGamePad() == true) {

		//Bボタンを押したとき
		if (Input::GetInstance()->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_B) {
			bTriggerTime_ += INCREASE_VALUE;

		}
		if ((Input::GetInstance()->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_B) == 0) {
			bTriggerTime_ = NO_REACT_TIME;
		}

		if (bTriggerTime_ == REACT_TIME) {

			isReturnTitle = true;
		}
	}
	//次のシーンへ
	if (Input::GetInstance()->IsTriggerKey(DIK_SPACE) == true) {
		isReturnTitle = true;
	}


	//カウントが増える時間
	const uint32_t INCREASE_COUNT_TIME = 0u;
	//点滅の間隔
	const uint32_t FLASH_INTERVAL = 2u;
	//表示
	const uint32_t DISPLAY = 0u;

	//タイトルへ戻る
	if (isReturnTitle == true) {

		fastFlashTime_ += INCREASE_VALUE;
		if (fastFlashTime_ % FAST_FLASH_TIME_INTERVAL_ == INCREASE_COUNT_TIME) {
			++textDisplayCount_;
		}
		//表示
		if (textDisplayCount_ % FLASH_INTERVAL == DISPLAY) {
			text_->SetInvisible(true);
		}
		else {
			text_->SetInvisible(false);
		}


		//指定した時間を超えたらタイトルへ遷移
		if (fastFlashTime_ > FAST_FLASH_TIME_LIMIT_) {
			text_->SetInvisible(true);
			//透明度の設定
			black_->SetTransparency(transparency_);

			//暗くなったらシーンチェンジ
			transparency_ += TRANSPARENCY_INTERVAL_;
		}

		const float MAX_TRANSPARENCY = 1.0f;
		if (transparency_ > MAX_TRANSPARENCY) {
			blackOutTime_ += INCREASE_VALUE;
		}


	}

	//タイトルへ
	if (blackOutTime_ > CHANGE_TO_TITLE_TIME_) {
		gameManager->ChangeScene(new TitleScene());
		return;
	}



}


void LoseScene::DrawObject3D()
{
}

void LoseScene::PreDrawPostEffectFirst()
{
}

void LoseScene::DrawPostEffect()
{
}

void LoseScene::DrawSprite(){
	failedTexture_->Draw();
	text_->Draw();


	//フェードイン
	black_->Draw();


}

LoseScene::~LoseScene()
{
}
