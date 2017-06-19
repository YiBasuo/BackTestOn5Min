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
	
	Numeric FastLength(12);
	Numeric SlowLength(26);
	Numeric MACDLength(9);
	Numeric TrendLength(60);
	
	Numeric ShortTermHoldingThreshold(100);

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
	//***********Indicators and Filters***********************//
	NumericSeries DIF; 
	NumericSeries DEA;
	NumericSeries MACD;
	NumericSeries DIFMain;
	NumericSeries DEAMain;
	NumericSeries MidLine;
	NumericSeries MA60;
	
	//***********Filter Out Short-term Opportunities**********//
	Numeric i;// iterator;
	Numeric startingPoint;
	Numeric pressure;
	Numeric support;
	
	Numeric prevDivergentExtremeIndex;
	Numeric prevDivergentExtremeHigh;
	Numeric prevDivergentExtremeLow;
	
	//***********Gap Filter**********************//
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
		onCall = signalRaw[1];
		triggerOnCall = prevOneHrHigh;
		abortOnCall = prevOneHrLow;
		expired = False;
	}
	else if (signalRaw[1] < 0 && timestamp <> timestamp[1])
	{

		onCall = signalRaw[1];
		triggerOnCall = prevOneHrLow;
		abortOnCall = prevOneHrHigh;
		expired = False;
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
		
	// Enter
	if (onCall > 0 && High > triggerOnCall && !expired)
	{
		signal = 1;
		triggerPrice = triggerOnCall; 
		expired = True;
	}
	// Abort
/*	else if (onCall > 0 && Low < abortOnCall && !expired)
	{
		expired = True;
		signal = 0;
	}*/
	// Enter
	else if (onCall < 0 && Low < triggerOnCall && !expired)
	{
		signal = -1;
		triggerPrice = triggerOnCall;
		expired = True;
	}
	// Abort
/*	else if (onCall < 0 && High > triggerOnCall && !expired)
	{
		expired = True;
		signal = 0;
	}*/
	else
	{
		signal = 0;
	}	
	
	//**************************************************5 Min filter goes in here***************************************//
	// Calculate Indicators
	DIF = XAverage( Close, FastLength ) - XAverage( Close, SlowLength ) ;	
	DEA = XAverage(DIF, MACDLength);
	MACD = DIF - DEA;
	
	
	DIFMain = MA60 + DIF * 3.9;
	DEAMain = XAverage(DIFMain, MACDLength);
	MidLine = XAverage(XAverage(C,10),10);
	MA60 = XAverage(Close, 60);

	PlotNumeric("DIF", DIFMain);
	PlotNumeric("DEA", DEAMain);
	PlotNumeric("Mid", MidLine);
	PlotNumeric("MA60", MA60);
	
	// Direction of MA60 should be the same with signal direction
	if ((signal == 1 && MA60 < MA60[1]) || (signal == -1 && MA60 > MA60[1]))
	{
		signal = 0;
	}
	// Direction of Mid line should be the same with signal direction
	if ((signal == 1 && MidLine < MidLine[1]) || (signal == -1 && MidLine > MidLine[1]))
	{
		signal = 0;
	}/* */
	
	//*************************************************Filter Out Short-term Oppotunities*************************************//
	for i = 0 to 200:
	{
		if (signal > 0 && Low[i] < MA60[i])
		{
			startingPoint = i;
			break;
		}
		
		if (signal < 0 && High[i] > MA60[i])
		{
			startingPoint = i;
			break;
		}
	}
	
	// If price has already moved greatly towards the signal direction, then do not operate on it.
	if ((signal > 0 && Low - Low[startingPoint] > ShortTermHoldingThreshold) || (signal < 0 && High[startingPoint] - High > ShortTermHoldingThreshold))
	{
		signal = 0;
	}
	else
	{
		signal = signal;
	}
	
	// If the signal is triggered immediately on 5min chart, then it might possibly end up loss.
	/*	pressure = GetNearestPeak("pressure");
	support = GetNearestPeak("support");
	
	if (signal <> 0 && timestamp <> timestamp[1])
	{
		if (signal > 0 && Low[1] - support < 20)
		{
			signal = 0;
		}
		else if (signal < 0 && pressure - High[1] < 20)
		{
			signal = 0;
		}
		else
		{
			signal = signal;
		}
		PlotString("p", Text(pressure), High + 25);
		PlotString("s", Text(support), Low - 25);
		signal = 0;
	}
	else
	{
		signal = signal;
	}*/
	
	// Filter: derivative of MACD should be the same direction as signal
	if (signal > 0 && MACD < MACD[1])
	{
		signal = 0;
	}
	else if (signal < 0 && MACD > MACD[1])
	{
		signal = 0;
	}
	else
	{
		signal = signal;
	}
	
	// Filter: strong divergent signal:
	
	
	//*************************************************Gap adjustment****************************************************//
	if ((signal == 1 && Open > prevOneHrHigh) || (signal == -1 && Open < prevOneHrLow))
	{
		triggerPrice = Open;
	}
	
	//*****************Save trigger price to DB*********************//
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
