#pragma once
#include "DirectXGame/engine/struct/Vector2.h"
#include "DirectXGame/engine/struct/Matrix3x3.h"

//Transform
struct Transform2D {
	Vector2 scale;
	float rotate;
	Vector2 translate;
};

//Box
struct Box {
	Vector2 min_; //!< �ŏ��_
	Vector2 max_; //!< �ő�_

	Box(Vector2 min = Vector2(), Vector2 max = Vector2()) : min_(min), max_(max) {}

	bool intersects(const Box& other) const {
		return (min_.x <= other.max_.x && max_.x >= other.min_.x &&
			min_.y <= other.max_.y && max_.y >= other.min_.y &&);
	}
};


// ��
struct Sphere2D {
	Vector2 center; //!<���S�_
	float radius;   //!<���a 
};


// ����
struct Line2D
{
	Vector2 origin; //!<�n�_
	Vector2 diff;   //!<�I�_�ւ̍����x�N�g��
};
//������
struct Ray2D
{
	Vector2 origin; //!<�n�_
	Vector2 diff;   //!<�I�_�ւ̍����x�N�g��
};

//����
struct Segment2D
{

	Vector2 origin; //!<�n�_
	Vector2 end;


	// �R���X�g���N�^
	Segment2D(const Vector2& o, const Vector2& e) : origin(o), end(e) {}

	// �����̃x�N�g��
	Vector2 diff() const {
		return end - origin;
	}

	// �����̒���
	float length() const {
		return diff().Length();
	}

	// �P�ʕ����x�N�g��
	Vector2 normalizedDirection() const {
		return diff().Normalize();
	}

	// �w�肳�ꂽ t (0.0 ~ 1.0) �̈ʒu�̓_���擾
	Vector2 pointAt(float t) const {
		return origin + diff() * t;
	}
};

struct CornerSegment2D {
	Vector2 center;
	int segment;
	float radius;
};

//�O�p�`
struct Triangle2D
{
	Vector2 vertices[3]; // !���_
	Box bounds;

	// +=�I�y���[�^�[�̃I�[�o�[���[�h 
	Triangle2D& operator+=(const Vector2& offset) {
		for (auto& vertex : vertices) {
			vertex += offset;
		}
		return *this;
	}

	Triangle2D OffsetVector2(const Vector2& offset) const {
		Triangle2D result = *this;  // �R�s�[���쐬
		for (auto& vertex : result.vertices) {
			vertex += offset;
		}
		return result;
	}



	Triangle2D(Vector2 v0, Vector2 v1, Vector2 v2) : vertices{ v0, v1, v2 } {
		bounds.min_ = Min(Min(v0, v1), v2);
		bounds.max_ = Max(Max(v0, v1), v2);
	}
};

//�΂�
struct Spring2D
{
	Vector2 anchor;      // �A���J�[�̌Œ肳�ꂽ�[�̈ʒu
	float naturalLength; // ���R��
	float stiffness;     // �����B�o�l�萔k
	float dampingCoefficient; // �����W��
};

struct Ball2D {
	Vector2 position;		//�ʒu
	Vector2 veloctiy;		//���x
	Vector2 acceleration;	//�����x
	float mass;				//����
	float radius;			//���a
	unsigned int color;		//�F
};

struct Pendulum2D {
	Vector2 anchor;				// �A���J�[�|�C���g
	float length;				// �R�̒���
	float angle;				// ���݂̊p�x
	float angularVelocity;		// �p�x��
	float angularAcceleration;	// �p�����x
};

struct ConicalPendulum2D {
	Vector2 anchor;				// �A���J�[�|�C���g
	float length;				// �R�̒���
	float halfApexAngle;		// �~���̒��p�̔���
	float angle;				// ���݂̊p�x
	float angularVelocity;		// �p�x��

};

// �J�v�Z��
struct Capsule2D
{
	Segment2D segment;
	float radius;


	// �R���X�g���N�^
	Capsule2D(const Vector2& p0, const Vector2& p1, float r) : segment(p0, p1), radius(r) {}

	// �J�v�Z���� AABB ���擾
	Box computeAABB() const {
		Vector2 minPoint = Min(segment.origin, segment.end) - Vector2(radius, radius);
		Vector2 maxPoint = Max(segment.origin, segment.end) + Vector2(radius, radius);
		return Box(minPoint, maxPoint);
	}
};

struct OBB2D {
	Vector2 center;
	Vector2 orientations[3];
	Vector2 size;
};