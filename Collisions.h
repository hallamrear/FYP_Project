#pragma once
#include "pch.h"

namespace Collisions
{
	bool IsPointInCircle(XMFLOAT2 point, XMFLOAT2 circlePos, float radius)
	{
		if (((point.x - circlePos.x) * (point.x - circlePos.x)) + ((point.y - circlePos.y) * (point.y - circlePos.y)) < (radius * radius))
			return true;
		else
			return false;
	}
}