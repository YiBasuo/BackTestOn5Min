//------------------------------------------------------------------------
// 简称: Find5minSignal
// 名称: 
// 类别: 用户函数
// 类型: 用户函数
// 输出: 数值型
//------------------------------------------------------------------------

Params	
	NumericSeries signalRaw;
	string signalPeriod("1HrSignal");
	string prev1HrHighTableName("Prev1HrHigh");
	string prev1HrLowTableName("Prev1HrLow");
	string triggerPriceTableName("TriggerPrice");

Vars	
	//********Timestamp***********//
	Numeric timeModifier(0);
	string strKey;
	
	NumericSeries timestamp;
	
	//************Signal Searching************//
	Numeric signal;
	Numeric triggerPrice;
	
	BoolSeries expired;
	NumericSeries onCall;
	NumericSeries triggerOnCall;
	NumericSeries abortOnCall;
	
	//
	Numeric oneHrHigh;
	Numeric oneHrLow;
	Numeric prevOneHrHigh;
	Numeric prevOneHrLow;
Begin
	//**********TimeStamp*****************//
	timeModifier = GetTimeModifier(signalPeriod);
	timestamp = Date + TimeModifier;

	//**********Read DB data**************//
	strKey = DateTimeToString(timestamp);
	
	prevOneHrHigh = ReadValueFromDB(prev1HrHighTableName, strKey);
	prevOneHrLow = ReadValueFromDB(prev1HrLowTableName, strKey);
	// Future function
	oneHrHigh = ReadValueFromDB("1HrHigh", strKey);
	oneHrLow = ReadValueFromDB("1HrLow", strKey);

	//***********Signal Searching***********//
	if (signalRaw[1] > 0 && timestamp <> timestamp[1])
	{
		// Future function
		if (oneHrLow < prevOneHrLow)
		{
			onCall = 0;
			triggerOnCall = 0;
			abortOnCall = 0;
			expired = True;
		}
		else
		{
			onCall = signalRaw[1];
			triggerOnCall = prevOneHrHigh;
			abortOnCall = prevOneHrLow;
			expired = False;
		}
	}
	else if (signalRaw[1] < 0 && timestamp <> timestamp[1])
	{
		// Future function
		if (oneHrHigh > prevOneHrHigh)
		{
			onCall = 0;
			triggerOnCall = 0;
			abortOnCall = 0;
			expired = True;
		}
		else
		{
			onCall = signalRaw[1];
			triggerOnCall = prevOneHrLow;
			abortOnCall = prevOneHrHigh;
			expired = False;
		}
	}
	else if (signalRaw[1] == 0 && timestamp <> timestamp[1])
	{
		onCall = 0;
		triggerOnCall = 0;
		abortOnCall = 0;
		expired = True;
	}
	else
	{
		onCall = onCall[1];
		triggerOnCall = triggerOnCall[1];
		abortOnCall = abortOnCall[1];
		expired = expired[1];
	}
		
	if (onCall > 0 && High > triggerOnCall && !expired)
	{
		signal = 1;
		triggerPrice = triggerOnCall; 
		expired = True;
	}
	else if (onCall < 0 && Low < triggerOnCall && !expired)
	{
		signal = -1;
		triggerPrice = triggerOnCall;
		expired = True;
	}
	else
	{
		signal = 0;
	}
	
	//*****************Save trigger and abort price to DB*********************//
	strKey = DateTimeToString(Date + Time);
	SetTBProfileString(triggerPriceTableName, strKey, Text(triggerPrice));
	
	return signal;
	
End

//------------------------------------------------------------------------
// 编译版本	GS2015.12.25
// 用户版本	2017/06/10 10:02:10
// 版权所有	qwerrqwer
// 更改声明	TradeBlazer Software保留对TradeBlazer平台
//			每一版本的TradeBlazer公式修改和重写的权利
//------------------------------------------------------------------------