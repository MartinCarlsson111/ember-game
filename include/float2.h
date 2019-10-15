struct float2
{
public:
	float2()
	{
		x = 0;
		y = 0;
	}

	float2(float x, float y)
	{
		this->x = x;
		this->y = y;
	}

	float2(float a)
	{
		x = a;
		y = a;
	}

	float x, y ;

};