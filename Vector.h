#pragma once
class Vector2f
{
public:
	float x = 0.0f;
	float y = 0.0f;

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

	//Normalizes this vector and returns it.
	Vector2f GetNormalized()
	{
		float length = GetLength();

		if (length != 0.0f)
		{
			x = x / length;
			y = y / length;
		}
		else
		{
			x = 0.0f;
			y = 0.0f;
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