#pragma once


namespace MathUtil
{
	float lerp(float a, float b, float f)
	{
		return a + f * (b - a);
	}
}