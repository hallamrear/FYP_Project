#pragma once
class Vector2f
{
public:
	float x = 0.0f;
	float y = 0.0f;

	Vector2f operator-() const {
		Vector2f v;
		v.x = -x;
		v.y = -y;
		return v;
	}

	Vector2f()
	{

	};

	Vector2f(float x, float y)
	{
		this->x = x;
		this->y = y;
	};


	float Dot(Vector2f vector)
	{
		return ((x * vector.x) + (y * vector.y));
	}

	float GetLength()
	{
		return sqrtf((x * x) + (y * y));
	}
	
	Vector2f Scale(float scale)
	{
		Vector2f result;
		result.x = x * scale;
		result.y = y * scale;
		return result;
	}

	//returns normalized version of  this vector
	Vector2f GetNormalized()
	{
		float length = GetLength();

		Vector2f normalized;

		if (length != 0.0f)
		{
			normalized.x = x / length;
			normalized.y = y / length;
		}
		else
		{
			normalized.x = 0.0f;
			normalized.y = 0.0f;
		}

		return normalized;
	}

	Vector2f Normalize()
	{
		float length = GetLength();

		if (length != 0.0f)
		{
			x = x / length;
			y = y / length;
		}

		return *this;
	}
};

class Vector2i
{
public:
	int x = 0;
	int y = 0;

	Vector2i()
	{

	}

	Vector2i(int x, int y)
	{
		this->x = x;
		this->y = y;
	}

	float GetLength()
	{ 
		return sqrtf((x * x) + (y * y));
	}
};