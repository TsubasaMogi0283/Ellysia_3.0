#include "PlayerCollisionToNormalEnemyAttack.h"
#include <Collider/CollisionConfig.h>
#include <ModelManager.h>

void PlayerCollisionToNormalEnemyAttack::Initialize(){

	uint32_t modelHandle = ModelManager::GetInstance()->LoadModelFile("Resources/CG3/Sphere", "Sphere.obj");
	model_.reset(Model::Create(modelHandle));


	//初期化
	worldTransform_.Initialize();
	worldTransform_.scale_ = { 5.0f,5.0f,5.0f };
	playerWorldPosition_ = {};


#pragma region 当たり判定

	//種類
	collisionType_ = CollisionType::SphereType;

	//半径
	radius_ = 1.0f;


	//自分
	SetCollisionAttribute(COLLISION_ATTRIBUTE_PLAYER);
	//相手
	SetCollisionMask(COLLISION_ATTRIBUTE_ENEMY_ATTACK);

#pragma endregion

	

}

void PlayerCollisionToNormalEnemyAttack::Update(){

	//プレイヤーの座標を持ってくる
	worldTransform_.translate_ = playerWorldPosition_;
	//ワールドトランスフォームの更新
	worldTransform_.Update();


#ifdef _DEBUG
	ImGui::Begin("敵からの攻撃"); 
	ImGui::Checkbox("接触", &isTouch_);
	ImGui::InputFloat3("座標", &playerWorldPosition_.x);
	ImGui::End();
#endif // _DEBUG



}

void PlayerCollisionToNormalEnemyAttack::Draw(Camera& camera, Material& material, SpotLight& spotLight){
#ifdef _DEBUG
	model_->Draw(worldTransform_, camera, material, spotLight);
#endif // _DEBUG

}

Vector3 PlayerCollisionToNormalEnemyAttack::GetWorldPosition(){
	Vector3 position = worldTransform_.GetWorldPosition();
	return position;

}

void PlayerCollisionToNormalEnemyAttack::OnCollision(){
	isTouch_ = true;
}

void PlayerCollisionToNormalEnemyAttack::OffCollision(){
	isTouch_ = false;
}