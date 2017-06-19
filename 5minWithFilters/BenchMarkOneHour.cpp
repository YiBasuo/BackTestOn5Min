//------------------------------------------------------------------------
// 简称: testMethod
// 名称: Method for testing
// 类别: 公式应用
// 类型: 用户应用
// 输出:
//------------------------------------------------------------------------

Params
Vars
	//***********************Signal Vars********************//
	BoolSeries SigOneBuy;
	BoolSeries SigOneSell;
	NumericSeries signal;
	NumericSeries triggeredPrice;
	NumericSeries abortPrice;
	
	//***********************Strategy Vars******************//
	Numeric Lots(1);
	Numeric BuyPrice;

	string strKey;
Begin	
	strKey = DateTimeToString(Date + Time);
	signal = ReadValueFromDB("1HrSignal", strKey);
	triggeredPrice = ReadValueFromDB("TriggerPrice", strKey);
	abortPrice = ReadValueFromDB("AbortPrice", strKey);
	
	//**********************************Run Strategy******************************************//
	if (signal[1] > 0)
	{
		// Exceed the prev bar, open buy. Exclude cornor case for simplification.
		if (High > High[1] && !(Low < Low[1]))
		{
			Buy(Lots, High[1]);
		}
	}
	
	if (signal[1] < 0)
	{
		// Exceed the prev bar, open buy
		if (Low < Low[1] && !(High > High[1]))
		{
			SellShort(Lots, Low[1]);
		}
	}
	
	if (Low < Low[1] And MarketPosition == 1)
	{
		Sell(Lots, Low[1]);
	}
	
	if (High > High[1] And MarketPosition == -1)
	{
		BuyToCover(Lots, High[1]);
	}
	//***********************************************************//
End


//------------------------------------------------------------------------
// 编译版本	GS2015.12.25
// 用户版本	2017/06/05 14:18:25
// 版权所有	qwerrqwer
// 更改声明	TradeBlazer Software保留对TradeBlazer平台
//			每一版本的TradeBlazer公式修改和重写的权利
//------------------------------------------------------------------------