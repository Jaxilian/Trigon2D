#ifndef VECTOR2
#define VECTOR2



class Vector2
{
public:
	float data[2] = { 0,0 };

	float& x = data[0];
	float& y = data[1];

	Vector2() {};
	Vector2(float x, float y) { this->x = x; this->y = y; }
	float& operator []	(const int& rhs) {return data[rhs];}
	void operator	=	(const Vector2& rhs) { x = rhs.x; y = rhs.y; }
};


#endif // !VECTOR2