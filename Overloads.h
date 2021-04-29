#pragma once

namespace
{
#pragma region XMFLOAT2
	XMFLOAT2 operator+(const XMFLOAT2& left, const XMFLOAT2& right)
	{
		XMFLOAT2 ans;
		ans.x = left.x + right.x;
		ans.y = left.y + right.y;
		return ans;
	}

	XMFLOAT2& operator+=(XMFLOAT2& left, const XMFLOAT2& right)
	{
		left.x += right.x;
		left.y += right.y;
		return left;
	}

	XMFLOAT2 operator-(const XMFLOAT2& left, const XMFLOAT2& right)
	{
		XMFLOAT2 ans;
		ans.x = left.x - right.x;
		ans.y = left.y - right.y;
		return ans;
	}

	XMFLOAT2& operator-=(XMFLOAT2& left, const XMFLOAT2& right)
	{
		left.x -= right.x;
		left.y -= right.y;
		return left;
	}

	XMFLOAT2 operator*(const XMFLOAT2& left, const XMFLOAT2& right)
	{
		XMFLOAT2 ans;
		ans.x = left.x * right.x;
		ans.y = left.y * right.y;
		return ans;
	}

	XMFLOAT2 operator*(const XMFLOAT2& left, const float& right)
	{
		XMFLOAT2 ans;
		ans.x = left.x * right;
		ans.y = left.y * right;
		return ans;
	}

	XMFLOAT2 operator*(const float& left, const XMFLOAT2& right)
	{
		XMFLOAT2 ans;
		ans.x = right.x / left;
		ans.y = right.y / left;
		return ans;
	}

	XMFLOAT2 operator/(const XMFLOAT2& left, const XMFLOAT2& right)
	{
		XMFLOAT2 ans;
		ans.x = left.x / right.x;
		ans.y = left.y / right.y;
		return ans;
	}

	XMFLOAT2 operator/(const XMFLOAT2& left, const float& right)
	{
		XMFLOAT2 ans;
		ans.x = left.x / right;
		ans.y = left.y / right;
		return ans;
	}

	XMFLOAT2 operator/(const float& left, const XMFLOAT2& right)
	{
		XMFLOAT2 ans;
		ans.x = right.x / left;
		ans.y = right.y / left;
		return ans;
	}


	bool operator==(const XMFLOAT2& left, const XMFLOAT2& right)
	{
		if (left.x == right.x && left.y == right.y)
			return true;
		else
			return false;
	}

	bool operator!=(const XMFLOAT2& left, const XMFLOAT2& right)
	{
		if (left.x == right.x && left.y == right.y)
			return false;
		else
			return true;
	}

#pragma endregion



#pragma region XMFLOAT3
	XMFLOAT3 operator+(const XMFLOAT3& left, const XMFLOAT3& right)
	{
		XMFLOAT3 ans;
		ans.x = left.x + right.x;
		ans.y = left.y + right.y;
		ans.z = left.z + right.z;
		return ans;
	}

	XMFLOAT3& operator+=(XMFLOAT3& left, const XMFLOAT3& right)
	{
		left.x += right.x;
		left.y += right.y;
		left.z += right.z;
		return left;
	}

	XMFLOAT3 operator-(const XMFLOAT3& left, const XMFLOAT3& right)
	{
		XMFLOAT3 ans;
		ans.x = left.x - right.x;
		ans.y = left.y - right.y;
		ans.z = left.z - right.z;
		return ans;
	}

	XMFLOAT3& operator-=(XMFLOAT3& left, const XMFLOAT3& right)
	{
		left.x -= right.x;
		left.y -= right.y;
		left.z -= right.z;
		return left;
	}

	XMFLOAT3 operator*(const XMFLOAT3& left, const XMFLOAT3& right)
	{
		XMFLOAT3 ans;
		ans.x = left.x * right.x;
		ans.y = left.y * right.y;
		ans.z = left.z * right.z;
		return ans;
	}

	XMFLOAT3 operator*(const XMFLOAT3& left, const float& right)
	{
		XMFLOAT3 ans;

		ans.x = left.x * right;
		ans.y = left.y * right;
		ans.z = left.z * right;

		return ans;
	}

	XMFLOAT3 operator/(const XMFLOAT3& left, const XMFLOAT3& right)
	{
		XMFLOAT3 ans;
		ans.x = left.x / right.x;
		ans.y = left.y / right.y;
		ans.z = left.z / right.z;
		return ans;
	}

	bool operator==(const XMFLOAT3& left, const XMFLOAT3& right)
	{
		if (left.x == right.x && left.y == right.y && left.z == left.z)
			return true;
		else
			return false;
	}
#pragma endregion
}