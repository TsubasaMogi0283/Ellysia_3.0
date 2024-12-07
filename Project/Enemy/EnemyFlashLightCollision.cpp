#include "EnemyFlashLightCollision.h"
#include <imgui.h>
#include <Collider/CollisionConfig.h>

void EnemyFlashLightCollision::Initialize(){

#pragma region 当たり判定

	//種類
	//点。AABBとか球にしたら計算量多くなるので止めた
	collisionType_ = ColliderType::PointType;


	//自分
	SetCollisionAttribute(COLLISION_ATTRIBUTE_ENEMY2);
	//相手
	SetCollisionMask(COLLISION_ATTRIBUTE_FLASH_LIGHT);

#pragma endregion


}

void EnemyFlashLightCollision::Update(){
	


#ifdef _DEBUG
	ImGui::Begin("敵の当たり判定(扇)");
	ImGui::InputFloat3("敵の位置", &enemyPosition_.x);
	ImGui::Checkbox("接触", &isTouch_);
	ImGui::End();
#endif // _DEBUG


}

Vector3 EnemyFlashLightCollision::GetWorldPosition(){
	return enemyPosition_;
}

void EnemyFlashLightCollision::OnCollision(){
	isTouch_ = true;

}

void EnemyFlashLightCollision::OffCollision(){
	//通常時
	isTouch_ = false;
}
