#pragma once

namespace
{
#pragma region Vector2f
	Vector2f operator+(const Vector2f& left, const Vector2f& right)
	{
		Vector2f ans;
		ans.x = left.x + right.x;
		ans.y = left.y + right.y;
		return ans;
	}

	Vector2f& operator+=(Vector2f& left, const Vector2f& right)
	{
		left.x += right.x;
		left.y += right.y;
		return left;
	}

	Vector2f operator-(const Vector2f& left, const Vector2f& right)
	{
		Vector2f ans;
		ans.x = left.x - right.x;
		ans.y = left.y - right.y;
		return ans;
	}

	Vector2f& operator-=(Vector2f& left, const Vector2f& right)
	{
		left.x -= right.x;
		left.y -= right.y;
		return left;
	}

	Vector2f operator*(const Vector2f& left, const Vector2f& right)
	{
		Vector2f ans;
		ans.x = left.x * right.x;
		ans.y = left.y * right.y;
		return ans;
	}

	Vector2f operator*(const Vector2f& left, const float& right)
	{
		Vector2f ans;
		ans.x = left.x * right;
		ans.y = left.y * right;
		return ans;
	}

	Vector2f operator*(const float& left, const Vector2f& right)
	{
		Vector2f ans;
		ans.x = right.x / left;
		ans.y = right.y / left;
		return ans;
	}

	Vector2f operator/(const Vector2f& left, const Vector2f& right)
	{
		Vector2f ans;
		ans.x = left.x / right.x;
		ans.y = left.y / right.y;
		return ans;
	}

	Vector2f operator/(const Vector2f& left, const float& right)
	{
		Vector2f ans;
		ans.x = left.x / right;
		ans.y = left.y / right;
		return ans;
	}

	Vector2f operator/(const float& left, const Vector2f& right)
	{
		Vector2f ans;
		ans.x = right.x / left;
		ans.y = right.y / left;
		return ans;
	}


	bool operator==(const Vector2f& left, const Vector2f& right)
	{
		if (left.x == right.x && left.y == right.y)
			return true;
		else
			return false;
	}

	bool operator!=(const Vector2f& left, const Vector2f& right)
	{
		if (left.x == right.x && left.y == right.y)
			return false;
		else
			return true;
	}
#pragma endregion
}