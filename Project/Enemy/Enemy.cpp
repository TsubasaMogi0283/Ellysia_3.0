#include "Enemy.h"

#include "../Collider/CollisionConfig.h"
#include <VectorCalculation.h>

#include "SpotLight.h"
#include <SingleCalculation.h>
#include <numbers>
#include <ModelManager.h>

void Enemy::Initialize(uint32_t modelHandle, Vector3 position, Vector3 speed){
	

	model_.reset(Model::Create(modelHandle));
	worldTransform_.Initialize();

	worldTransform_.scale_ = { .x = SCALE_SIZE,.y = SCALE_SIZE ,.z = SCALE_SIZE };
#ifdef _DEBUG
	float DEBUG_SCALE = 1.0f;
	worldTransform_.scale_ = { .x = DEBUG_SCALE,.y = DEBUG_SCALE ,.z = DEBUG_SCALE };
#endif // _DEBUG

	//座標の代入
	worldTransform_.translate_ = position;

	//マテリアルの初期化
	material_.Initialize();
	material_.lightingKinds_ = Spot;

	//生存か死亡
	isAlive_ = true;
	deleteTime_ = 180;
	
	//追跡かどうか
	isTracking_ = false;

	
	preSpeed_ = speed;
	speed_ = speed;

	//色
	color_ = { 1.0f,1.0f,1.0f,1.0f };
	//デフォルトで右方向に向いているようにする
	direction_ = { 1.0f,0.0f,0.0f };

	//状態
	preCondition_ = EnemyCondition::NoneMove;
	condition_ = EnemyCondition::Move;


	//スピードが全て0になっていたらNoneMove
	if (speed_.x == 0.0f &&
		speed_.y == 0.0f &&
		speed_.z == 0.0f) {
		preCondition_ = EnemyCondition::NoneMove;
		condition_ = EnemyCondition::NoneMove;
	}
	
	//攻撃
	attackTime_ = 0;


#pragma region 当たり判定
	//半径
	radius_ = 1.0f;

	//AABBのmax部分に加算する縦横高さのサイズ
	upSideSize_ = {.x= radius_ ,.y= radius_ ,.z= radius_ };

	//AABBのmin部分に加算する縦横高さのサイズ
	downSideSize_ = { .x = radius_ ,.y = radius_ ,.z = radius_ };

	//判定
	//自分
	SetCollisionAttribute(COLLISION_ATTRIBUTE_ENEMY);
	//相手
	SetCollisionMask(COLLISION_ATTRIBUTE_NONE);

#pragma endregion

	uint32_t debugModelHandle = ModelManager::GetInstance()->LoadModelFile("Resources/CG3/Sphere", "Sphere.obj");
#ifdef _DEBUG
	
	debugModel_.reset(Model::Create(debugModelHandle));

	debugModelWorldTransform_.Initialize();
	const float DEBUG_MODEL_SCALE = 0.25f;
	debugModelWorldTransform_.scale_ = { .x= DEBUG_MODEL_SCALE,.y= DEBUG_MODEL_SCALE,.z= DEBUG_MODEL_SCALE };


	

#endif // _DEBUG
	attackModel_ =new EnemyAttackCollision();
	attackModel_->Initialize(debugModelHandle);
	isAttack_ = false;

	
}



void Enemy::Update(){
	
	const float SPEED_AMOUNT = 0.05f;
	//StatePatternにするよ！！
	//状態
	switch (condition_) {
		//何もしない
	case EnemyCondition::NoneMove:
		#ifdef _DEBUG
		ImGui::Begin("None");
		ImGui::End();
		#endif // DEBUG
		
		attackTime_ = 0;
		preTrackingPlayerPosition_ = {};
		preTrackingPosition_ = {};
		speed_ = { 0.0f,0.0f,0.0f };
		break;
	
		//通常の動き
	case EnemyCondition::Move:

		
		
		attackTime_ = 0;
		isAttack_ = false;



		//通常の動き
		preTrackingPlayerPosition_ = {};
		preTrackingPosition_ = {};
		if (speed_.x != 0.0f ||
			speed_.y != 0.0f ||
			speed_.z != 0.0f) {
			direction_ = VectorCalculation::Normalize(speed_);
		}
		
		worldTransform_.translate_ = VectorCalculation::Add(worldTransform_.translate_, speed_);
	
	
		break;
	
	case EnemyCondition::PreTracking:
	
		attackTime_ = 0;
		isAttack_ = false;
		#pragma region 追跡準備
	
	
		//取得したら追跡
		preTrackingPlayerPosition_ = playerPosition_;
		preTrackingPosition_ = GetWorldPosition();
		
		
		#pragma endregion


		//強制的に追跡
		preCondition_ = EnemyCondition::PreTracking;
		condition_ = EnemyCondition::Tracking;

		break;
	
		//追跡
	case EnemyCondition::Tracking:
		//追跡処理


#ifdef _DEBUG
		ImGui::Begin("Tracking");
		ImGui::End();
#endif // DEBUG



		//向きを求める
		direction_ = VectorCalculation::Subtract(preTrackingPlayerPosition_, preTrackingPosition_);
		direction_ = VectorCalculation::Normalize(direction_);

		//加算
		Vector3 speedVelocity = VectorCalculation::Multiply(direction_, SPEED_AMOUNT);
		worldTransform_.translate_ = VectorCalculation::Add(worldTransform_.translate_, speedVelocity);


		break;
	
		//攻撃
	case EnemyCondition::Attack:
		attackTime_ += 1;
		

		//2～4秒までが攻撃
		if (attackTime_ > 120 && attackTime_ <= 240) {
			if (attackTime_ == 121) {
				isAttack_ = true;
				
			}
			else {
				isAttack_ = false;
			}

			
#ifdef _DEBUG
			ImGui::Begin("Attack");
			ImGui::End();
#endif // DEBUG

		}
		else {
			isAttack_ = false;
		}
	
		//4秒経ったらまた0になる
		if (attackTime_ > 240) {
			attackTime_ = 0;

		}

		break;

	}


	

	//向きを計算しモデルを回転させる
	float directionToRotateY = std::atan2f(-direction_.z,direction_.x);

	const float ROTATE_OFFSET = -std::numbers::pi_v<float>/2.0f;
	worldTransform_.rotate_.y = directionToRotateY + ROTATE_OFFSET;

#ifdef _DEBUG
	const float DEBUG_MODEL_ROTATE_OFFSET = std::numbers::pi_v<float>;
	worldTransform_.rotate_.y = directionToRotateY + DEBUG_MODEL_ROTATE_OFFSET;
#endif // _DEBUG


	

#ifdef _DEBUG
	const float INTERVAL = 5.0f;
	debugModelWorldTransform_.translate_ = VectorCalculation::Add(GetWorldPosition(), VectorCalculation::Multiply(direction_, INTERVAL));
	debugModelWorldTransform_.Update();

	

#endif // _DEBUG
	Vector3 enemyWorldPosition = GetWorldPosition();
	attackModel_->SetEnemyPosition(enemyWorldPosition);
	attackModel_->SetEnemyDirection(direction_);
	attackModel_->Update();


	//更新
	worldTransform_.Update();
	material_.Update();

	aabb_.min = VectorCalculation::Subtract(GetWorldPosition(), downSideSize_);
	aabb_.max = VectorCalculation::Add(GetWorldPosition(), upSideSize_);



#ifdef _DEBUG
	float degreeRotateY = directionToRotateY * (180.0f / std::numbers::pi_v<float>);

	ImGui::Begin("敵");
	ImGui::InputFloat3("direction_", &direction_.x);
	ImGui::InputFloat("RotateY", &degreeRotateY);
	
	
	ImGui::InputFloat3("Speed", &direction_.x);
	if (ImGui::TreeNode("Condition")) {
		int newCondition = static_cast<int>(condition_);
		int newPreCondition = static_cast<int>(preCondition_);
		ImGui::InputInt("Current", &newCondition);
		ImGui::InputInt("Pre", &newPreCondition);
		ImGui::TreePop();
	}
	
	ImGui::Checkbox("isAttck", &isAttack_);
	if (ImGui::TreeNode("AABB")) {
		ImGui::InputFloat3("Max", &aabb_.max.x);
		ImGui::InputFloat3("Min", &aabb_.min.x);
		ImGui::TreePop();
	}

	ImGui::InputFloat3("Position", &worldTransform_.translate_.x);
	ImGui::InputFloat3("preTrackingPlayerPosition", &preTrackingPlayerPosition_.x);
	ImGui::InputFloat3("preTrackingPosition_", &preTrackingPosition_.x);
	ImGui::InputInt("AliveTive", &deleteTime_);
	ImGui::End();
#endif // _DEBUG
}



void Enemy::OnCollision() {
#ifdef _DEBUG
	ImGui::Begin("EnemyCollision");
	ImGui::InputFloat4("Color", &color_.x);
	ImGui::End();
#endif // _DEBUG

	const float COLOR_CHANGE_INTERVAL = 0.005f;
	color_.y -= COLOR_CHANGE_INTERVAL;
	color_.z -= COLOR_CHANGE_INTERVAL;

	//0になったら消す
	if (color_.y < 0.0f &&
		color_.z < 0.0f) {
		isAttack_ = false;
		isAlive_ = false;
	}


	material_.color_ = color_;
	
}



Vector3 Enemy::GetWorldPosition() {
	Vector3 result = {
		.x= worldTransform_.worldMatrix_.m[3][0],
		.y = worldTransform_.worldMatrix_.m[3][1],
		.z = worldTransform_.worldMatrix_.m[3][2],
	};

	return result;
}




void Enemy::Draw(Camera& camera,SpotLight&spotLight){
#ifdef _DEBUG
	debugModel_->Draw(debugModelWorldTransform_, camera, material_, spotLight);

#endif // _DEBUG

	//攻撃用
	if (isAttack_ == true) {
		attackModel_->Draw(camera, spotLight);
	}

	//描画
	if (isAlive_ == true) {
		model_->Draw(worldTransform_, camera,material_, spotLight);
	}
	
}

Enemy::~Enemy(){
	delete attackModel_;
}

