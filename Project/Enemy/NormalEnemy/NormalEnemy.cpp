#include "NormalEnemy.h"

#include <imgui.h>
#include <numbers>

#include "CollisionConfig.h"
#include "VectorCalculation.h"
#include "SpotLight.h"
#include "SingleCalculation.h"
#include "ModelManager.h"
#include "GlobalVariables.h"
#include "State/NormalEnemyMove.h"

NormalEnemy::NormalEnemy() {
	//通常
	typeName_ = "Normal";

	//インスタンスの取得
	//グローバル変数クラス
	globalVariables_ = Elysia::GlobalVariables::GetInstance();
}

void NormalEnemy::Initialize(const uint32_t& modelHandle, const Vector3& position, const Vector3& speed) {

	//モデルの生成
	model_.reset(Elysia::Model::Create(modelHandle));

	//ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	//スケールサイズ
	const float SCALE_SIZE = 7.0f;
	worldTransform_.scale = { .x = SCALE_SIZE,.y = SCALE_SIZE ,.z = SCALE_SIZE };
#ifdef _DEBUG
	float DEBUG_SCALE = 1.0f;
	worldTransform_.scale = { .x = DEBUG_SCALE,.y = DEBUG_SCALE ,.z = DEBUG_SCALE };
#endif // _DEBUG

	//座標の代入
	worldTransform_.translate = position;

	//マテリアルの初期化
	material_.Initialize();
	material_.lightingKinds = SpotLighting;
	//パーティクル
	particleMaterial_.Initialize();
	particleMaterial_.lightingKinds = NoneLighting;


	//生存か死亡
	isAlive_ = true;
	deleteTime_ = 180;

	//追跡かどうか
	isTracking_ = false;

	//スピードの初期化
	preSpeed_ = speed;
	speed_ = speed;

	//状態
	preCondition_ = EnemyCondition::NoneMove;
	condition_ = EnemyCondition::Move;


	//デバッグ用のモデル
	debugModelHandle = Elysia::ModelManager::GetInstance()->LoadModelFile("Resources/Model/Sample/Sphere", "Sphere.obj");

	//攻撃の当たり判定
	attackCollision_ = std::make_unique<EnemyAttackCollision>();
	attackCollision_->Initialize(debugModelHandle);

	//懐中電灯に対する当たり判定
	enemyFlashLightCollision_ = std::make_unique<EnemyFlashLightCollision>();
	enemyFlashLightCollision_->Initialize();


	//グローバル変数
	globalVariables_->CreateGroup(GROUNP_NAME_);
	//振動
	globalVariables_->AddItem(GROUNP_NAME_, "ShakeOffset", shakeOffset_);
	shakeOffset_ = globalVariables_->GetFloatValue(GROUNP_NAME_, "ShakeOffset");

	//状態
	currentState_ = std::make_unique<NormalEnemyMove>();
	currentState_->Initialize();
	currentStateName_ = currentState_->GetStateName();


}

void NormalEnemy::Update() {

	//生存している時だけ行動するよ
	if (isAlive_ == true) {
		//状態の更新
		currentState_->Update(this);

	}

	//方向を取得
	direction_ = currentState_->GetDirection();

	//向きを計算しモデルを回転させる
	float directionToRotateY = std::atan2f(-direction_.z, direction_.x);
	//回転のオフセット
	//元々のモデルの回転が変だったのでこれを足している
	const float ROTATE_OFFSET = -std::numbers::pi_v<float> / 2.0f;
	worldTransform_.rotate.y = directionToRotateY + ROTATE_OFFSET;

#ifdef _DEBUG
	const float DEBUG_MODEL_ROTATE_OFFSET = std::numbers::pi_v<float>;
	worldTransform_.rotate.y = directionToRotateY + DEBUG_MODEL_ROTATE_OFFSET;
#endif // _DEBUG

	//ワールドトランスフォームの更新
	worldTransform_.Update();
	//マテリアルの更新
	material_.Update();
	particleMaterial_.Update();


	//AABBの計算
	aabb_.max = VectorCalculation::Add(GetWorldPosition(), RADIUS_INTERVAL_);
	aabb_.min = VectorCalculation::Subtract(GetWorldPosition(), RADIUS_INTERVAL_);


	//当たり判定
	//懐中電灯
	enemyFlashLightCollision_->SetEnemyPosition(GetWorldPosition());
	enemyFlashLightCollision_->Update();

	//攻撃
	attackCollision_->SetEnemyPosition(GetWorldPosition());
	attackCollision_->SetEnemyDirection(direction_);
	attackCollision_->Update();

	//ダメージ演出
	Damaged();

	//死亡したらパーティクルを出して消える
	if (isAlive_ == false) {
		Dead();
	}

	//ImGui
	DisplayImGui();
}

void NormalEnemy::Draw(const Camera& camera, const SpotLight& spotLight) {
#ifdef _DEBUG
	//攻撃
	attackCollision_->Draw(camera, spotLight);

#endif // _DEBUG

	//本体
	model_->Draw(worldTransform_, camera, material_, spotLight);

	//パーティクル
	if (particle_ != nullptr) {
		particle_->Draw(camera, particleMaterial_);
	}
}

NormalEnemy::~NormalEnemy() {
	globalVariables_->SaveFile(GROUNP_NAME_);
}

void NormalEnemy::ChengeState(std::unique_ptr<BaseNormalEnemyState> newState){
	//前回と違った場合だけ通す
	if (currentState_->GetStateName() != newState->GetStateName()) {
		//さっきまで入っていたものを入れる
		preStateName_ = currentState_->GetStateName();
		//新しく入る
		currentStateName_ = newState->GetStateName();
		//それぞれに新しく入れる
		currentState_ = std::move(newState);
		//初期化
		currentState_->Initialize();
	}
}

void NormalEnemy::Damaged() {

	//ダメージを受ける
	if (enemyFlashLightCollision_->GetIsTouched() == true) {
		material_.color.y = 0.0f;
		material_.color.z = 0.0f;

		//生存している時だけ振動
		if (isAlive_ == true) {
			//振動演出
			//体力減っていくと同時に振動幅が大きくなっていくよ
			std::random_device seedGenerator;
			std::mt19937 randomEngine(seedGenerator());
			std::uniform_real_distribution<float> distribute(-1.0f, 1.0f);


			//現在の座標に加える
			worldTransform_.translate.x += distribute(randomEngine) * (1.0f - material_.color.y) * shakeOffset_;
			worldTransform_.translate.z += distribute(randomEngine) * (1.0f - material_.color.y) * shakeOffset_;
		}


	}

	//0になったら絶命
	if (material_.color.y <= 0.0f &&
		material_.color.z <= 0.0f) {
		isAlive_ = false;
	}
}

void NormalEnemy::Dead() {
	//消えていくよ
	const float DELETE_INTERVAL = 0.01f;
	material_.color.w -= DELETE_INTERVAL;

	//縮小
	const float SCALE_DOWN_VALUE = -0.1f;
	worldTransform_.scale.x += SCALE_DOWN_VALUE;
	worldTransform_.scale.y += SCALE_DOWN_VALUE;
	worldTransform_.scale.z += SCALE_DOWN_VALUE;

	if (worldTransform_.scale.z < 0.0f) {
		worldTransform_.scale.x = 0.0f;
		worldTransform_.scale.y = 0.0f;
		worldTransform_.scale.z = 0.0f;

	}

	//生成
	if (particle_ == nullptr) {
		//生成
		particle_ = std::move(Elysia::Particle3D::Create(Rise));
		//パーティクルの細かい設定
		particle_->SetTranslate(GetWorldPosition());
		const float SCALE_SIZE = 20.0f;
		particle_->SetScale({ .x = SCALE_SIZE,.y = SCALE_SIZE,.z = SCALE_SIZE });
		particle_->SetCount(20u);
		particle_->SetIsReleaseOnceMode(true);
		particle_->SetIsToTransparent(true);
	}

	//全て消えたら、消えたかどうかのフラグがたつ
	if (particle_->GetIsAllInvisible() == true) {
		isDeleted_ = true;
	}

}

void NormalEnemy::DisplayImGui() {
#ifdef _DEBUG

	ImGui::Begin("敵");
	ImGui::InputFloat3("方向", &direction_.x);
	ImGui::Checkbox("攻撃", &isAttack_);
	ImGui::Checkbox("生存", &isAlive_);

	if (ImGui::TreeNode("状態")) {
		int newCondition = static_cast<int>(condition_);
		int newPreCondition = static_cast<int>(preCondition_);
		ImGui::InputInt("現在", &newCondition);
		ImGui::InputInt("前", &newPreCondition);
		ImGui::TreePop();
	}


	ImGui::InputFloat4("色", &material_.color.x);
	ImGui::InputFloat3("座標", &worldTransform_.translate.x);
	ImGui::InputFloat3("追跡前の座標", &preTrackingPosition_.x);
	ImGui::InputInt("AliveTive", &deleteTime_);
	ImGui::End();
#endif // _DEBUG


}


