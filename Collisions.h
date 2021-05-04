#pragma once
#include "pch.h"
#include "Overloads.h"

namespace Collisions
{
	bool IsPointInCircle(Vector2f point, Vector2f circlePos, float radius)
	{
		if (((point.x - circlePos.x) * (point.x - circlePos.x)) + ((point.y - circlePos.y) * (point.y - circlePos.y)) < (radius * radius))
			return true;
		else
			return false;
	}

	bool CircleInCircle(Vector2f circleOnePos, float circleOneRadius, Vector2f circleTwoPos, float circleTwoRadius)
	{
		Vector2f dist = circleOnePos - circleTwoPos;
		float sumofradii = circleOneRadius + circleTwoRadius;

		if (dist.GetLength() * dist.GetLength() < (sumofradii * sumofradii))
			return true;
		else
			 return false;
	}
}