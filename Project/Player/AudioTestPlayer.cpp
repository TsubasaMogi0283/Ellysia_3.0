#include "AudioTestPlayer.h"
#include "VectorCalculation.h"
#include "ModelManager.h"

void AudioTestPlayer::Initialize(){
	//ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.scale = { .x = 0.5f,.y = 0.5f,.z = 0.5f };
	worldTransform_.translate = { .x = 0.0f,.y = 0.0f,.z = 0.0f };
	//モデルの生成
	uint32_t modelHandle = ModelManager::GetInstance()->LoadModelFile("Resources/Sample/Cube","cube.obj");
	model_.reset(Model::Create(modelHandle));
	
	//マテリアルの初期化
	material_.Initialize();
	material_.lightingKinds_ = LightingType::Directional;

	//コリジョンの初期化
	collosionToAudioObject_ = std::make_unique<PlayerCollisionToAudioObject>();
	collosionToAudioObject_->Initialize();

	collosionToStageObject_ = std::make_unique<PlayerCollisionToStageObject>();
	collosionToStageObject_->Initialize();

}

void AudioTestPlayer::Update(){

#ifdef _DEBUG
	ImGui::Begin("音確認用のプレイヤー");
	ImGui::SliderFloat3("位置", &worldTransform_.translate.x,-40.0f,40.0f);
	int moveConditionInt = static_cast<int>(moveCondition_);
	ImGui::InputInt("移動状態", &moveConditionInt);
	ImGui::End();

#endif // _DEBUG

	if (moveCondition_ == TestPlayerMoveCondition::OnTestPlayerMove) {
		//方向とスピードを計算
		const float SPEED = 0.1f;
		Vector3 newDirection = VectorCalculation::Multiply(direction_, SPEED);

		//位置の更新
		worldTransform_.translate = VectorCalculation::Add(worldTransform_.translate, newDirection);
		worldTransform_.Update();
	}
	
	material_.Update();



	//コリジョンの更新
	collosionToAudioObject_->SetPlayerGetWorldPosition(worldTransform_.GetWorldPosition());
	collosionToAudioObject_->Update();

	collosionToStageObject_->SetPlayerGetWorldPosition(worldTransform_.GetWorldPosition());
	collosionToStageObject_->Update();

}

void AudioTestPlayer::Draw(const Camera& camera, const DirectionalLight& directionalLight){
	model_->Draw(worldTransform_, camera, material_, directionalLight);

}
