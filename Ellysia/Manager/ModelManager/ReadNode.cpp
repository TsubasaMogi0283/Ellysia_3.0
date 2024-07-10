#include "ReadNode.h"
#include <Matrix4x4Calculation.h>

ReadNode* ReadNode::GetInstance(){
    static ReadNode instance;

    return &instance;
}

Node ReadNode::Read(aiNode* node){
    Node result = {};

    ////nodeのlocalMatrixを取得
    //aiMatrix4x4 ailocalMatrix = node->mTransformation;
    ////列ベクトル形式を行ベクトル形式に転置
    //ailocalMatrix.Transpose();
    ////他の要素も同様に
    //result.localMatrix.m[0][0] = ailocalMatrix[0][0];
    //result.localMatrix.m[0][1] = ailocalMatrix[0][1];
    //result.localMatrix.m[0][2] = ailocalMatrix[0][2];
    //result.localMatrix.m[0][3] = ailocalMatrix[0][3];
 
    //result.localMatrix.m[1][0] = ailocalMatrix[1][0];
    //result.localMatrix.m[1][1] = ailocalMatrix[1][1];
    //result.localMatrix.m[1][2] = ailocalMatrix[1][2];
    //result.localMatrix.m[1][3] = ailocalMatrix[1][3];
    //
    //result.localMatrix.m[2][0] = ailocalMatrix[2][0];
    //result.localMatrix.m[2][1] = ailocalMatrix[2][1];
    //result.localMatrix.m[2][2] = ailocalMatrix[2][2];
    //result.localMatrix.m[2][3] = ailocalMatrix[2][3];

    //result.localMatrix.m[3][0] = ailocalMatrix[3][0];
    //result.localMatrix.m[3][1] = ailocalMatrix[3][1];
    //result.localMatrix.m[3][2] = ailocalMatrix[3][2];
    //result.localMatrix.m[3][3] = ailocalMatrix[3][3];




    aiVector3D scale = {};
    aiVector3D translate = {};
    aiQuaternion rotate = {};
    //assimpの行列からSRTを抽出する関数を利用
    node->mTransformation.Decompose(scale, rotate, translate);
    //Scale
    result.transform.scale = { scale.x,scale.y,scale.z };
    //rotate
    result.transform.rotate = { rotate.x,-rotate.y,-rotate.z,rotate.w };
    //translate
    result.transform.translate = { -translate.x,translate.y,translate.z};

    Vector3 newRotate = { result.transform.rotate.x, result.transform.rotate.y, result.transform.rotate.z };
    result.localMatrix = Matrix4x4Calculation::MakeAffineMatrix(result.transform.scale, newRotate, result.transform.translate);


    //Node名を格納
    result.name = node->mName.C_Str();
    //子供の数だけ確保
    result.children.resize(node->mNumChildren);
    for (uint32_t childIndex = 0; childIndex < node->mNumChildren; ++childIndex) {
        //再帰的に読んで階層構造を作っていく
        result.children[childIndex] = Read(node->mChildren[childIndex]);
    }

    return result;
}

