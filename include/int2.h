struct int2
{
public:
	int2()
	{
		x = 0;
		y = 0;
	}

	int2(const int x, const int y)
	{
		this->x = x;
		this->y = y;
	}

	int2(const int a)
	{
		x = a;
		y = a;
	}

	int x, y;
};