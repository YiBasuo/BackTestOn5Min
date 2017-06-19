//------------------------------------------------------------------------
// 简称: GetNearestPeak
// 名称: 
// 类别: 用户函数
// 类型: 用户函数
// 输出: 数值型
//------------------------------------------------------------------------

Params
	String target("pressure");// "pressure", "support"
Vars
	Numeric i;// Iterator

	Numeric firstLow;
	Numeric firstHigh;
	Numeric secondLow;
	Numeric secondHigh;
Begin
	firstLow = Low;
	firstHigh = High;
	secondLow = Low;
	secondHigh = High;
	for i = 1 to 100:
	{
		if (Low[i] < Low[i + 1] && Low[i] < Low[i - 1] && firstLow == Low)
		{
			firstLow = Low[i];
			continue;
		}
		if (Low[i] < Low[i + 1] && Low[i] < Low[i - 1] && firstLow != Low)
		{
			secondLow = Low[i];
			break;
		}
	}
	
	for i = 1 to 100:
	{
		if (High[i] > High[i + 1] && High[i] > High[i - 1] && firstHigh == High)
		{
			firstHigh = High[i];
			continue;
		}
		
		if (High[i] > High[i + 1] && High[i] > High[i - 1] && firstHigh != High)
		{
			secondHigh = High[i];
			break;
		}
	}
	
	if (target == "pressure")
	{
		if (Abs(firstHigh - secondHigh) < 2)
		{
			return Max(firstHigh, secondHigh);
		}
		else 
		{
			return firstHigh;
		}
	}
	else if (target == "support")
	{
		if (Abs(firstLow - secondLow) < 2)
		{
			return Min(firstLow, secondLow);
		}
		else 
		{
			return firstLow;
		}
	}
	else 
	{
		return 0;
	}

End

//------------------------------------------------------------------------
// 编译版本	GS2015.12.25
// 用户版本	2017/06/17 10:25:57
// 版权所有	qwerrqwer
// 更改声明	TradeBlazer Software保留对TradeBlazer平台
//			每一版本的TradeBlazer公式修改和重写的权利
//------------------------------------------------------------------------