#pragma once
#include "..\..\diesel\diesel.h"

typedef uint8_t BYTE;

struct CubeConstantBuffer {
	ds::matrix viewprojectionMatrix;
	ds::matrix worldMatrix;
};

struct GridVertex {
	ds::vec3 position;
	ds::vec2 texture;
	ds::Color color;
};

// ---------------------------------------------------
// AbstractBorder
// ---------------------------------------------------
class ElasticBorder {

	struct LinePoint {
		float height;
		float targetHeight;
		float speed;
		ds::vec2 dir;
		ds::vec2 pos;
	};


public:
	ElasticBorder(float size, int numX, int numY, const ds::vec4& texture, RID textureID);// , const ds::AABBox& box);
	virtual ~ElasticBorder();
	void tick(float dt);
	void render();
	void splash(int index, float speed);
	//virtual void buildColumn(int index, v3* positions) = 0;
	//bool isInside(const v3& pos) const;
	//bool collides(const ds::Sphere& s) const;
	//virtual int getIndex(const v3& pos) const = 0;
private:
	float _size;
	int _numX;
	int _numY;
	LinePoint* _points;
	RID _squareBuffer;
	float* lDeltas;
	float* rDeltas;
	//int _num;
	//float _target;
	ds::vec4 _texture;
	RID _textureID;
	RID createStateGroup(int numVertices);
	CubeConstantBuffer _constantBuffer;
	RID _vertexBufferID;
	RID _drawItem;
	RID _orthoPass;
	GridVertex* _vertices;
	float _timer;
	//ds::AABBox _box;
};
/*
// ---------------------------------------------------
// BottomBorder
// ---------------------------------------------------
class BottomBorder : public AbstractBorder {

public:
	BottomBorder(float target, int num);
	virtual ~BottomBorder() {}
	void buildColumn(int index, v3* positions);
	int getIndex(const v3& pos) const;
};

// ---------------------------------------------------
// TopBorder
// ---------------------------------------------------
class TopBorder : public AbstractBorder {

public:
	TopBorder(float target, int num);
	virtual ~TopBorder() {}
	void buildColumn(int index, v3* positions);
	int getIndex(const v3& pos) const;
};

// ---------------------------------------------------
// LeftBorder
// ---------------------------------------------------
class LeftBorder : public AbstractBorder {

public:
	LeftBorder(float target, int num);
	virtual ~LeftBorder() {}
	void buildColumn(int index, v3* positions);
	int getIndex(const v3& pos) const;
};

// ---------------------------------------------------
// RightBorder
// ---------------------------------------------------
class RightBorder : public AbstractBorder {

public:
	RightBorder(float target, int num);
	virtual ~RightBorder() {}
	void buildColumn(int index, v3* positions);
	int getIndex(const v3& pos) const;
};

// ---------------------------------------------------
// ElasticBorder
// ---------------------------------------------------
class ElasticBorder {

public:
	ElasticBorder();
	~ElasticBorder();
	void tick(float dt);
	void render();
	void splash(const v3& pos, float speed);
	void splash(const v3& pos, float radius, float speed);
private:
	AbstractBorder* _borders[4];
};
*/