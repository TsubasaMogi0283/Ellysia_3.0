#include "GameManager.h"
#include <cassert>
#include "GameSceneFactory.h"


void Ellysia::GameManager::Initialize() {
	
	//シーンファクトリーの生成
	abstractSceneFactory_ = std::make_unique<GameSceneFactory>();
	
	//シーンごとに動作確認したいときはここを変えてね
	currentGamaScene_ = abstractSceneFactory_->CreateScene("Game");

#ifdef _DEBUG
	//デバッグ時はこっちに入れてね
	currentGamaScene_ = abstractSceneFactory_->CreateScene("Title");

#endif // _DEBUG

	//初期化
	currentGamaScene_->Initialize();


}



void Ellysia::GameManager::ChangeScene(const std::string& sceneName){
	//新しいシーンに遷移するためにPreの所に入っていたものを入れる
	preSceneName_ = currentSceneName_;
	//現在入っているシーン名を更新
	currentSceneName_ = sceneName;

	//シーンの値を取ってくる
	currentGamaScene_ = abstractSceneFactory_->CreateScene(currentSceneName_);
	//空ではない時初期化処理に入る
	assert(currentGamaScene_ != nullptr);
	//初期化
	currentGamaScene_->Initialize();
	
	
}

void Ellysia::GameManager::Update() {
	//更新
	currentGamaScene_->Update(this);
}

void Ellysia::GameManager::DrawObject3D() {
	//3Dオブジェクトの描画
	currentGamaScene_->DrawObject3D();
}

void Ellysia::GameManager::DrawSprite(){
	//スプライトの描画
	currentGamaScene_->DrawSprite();
}

void Ellysia::GameManager::PreDrawPostEffectFirst(){
	//ポストエフェクト描画処理前
	currentGamaScene_->PreDrawPostEffectFirst();
}


void Ellysia::GameManager::DrawPostEffect(){
	//ポストエフェクト描画前
	currentGamaScene_->DrawPostEffect();
}



