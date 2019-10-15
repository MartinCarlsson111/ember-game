struct float3
{
	float3()
	{
		x = 0;
		y = 0;
		z = 0;
	}

	float3(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	float3(float a)
	{
		x = a;
		y = a;
		z = a;
	}

	float x, y, z;
};